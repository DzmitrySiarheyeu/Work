using System;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DUTConfig_V3.Settings;
using DUTConfig_V3.Protocol;

namespace DUTConfig_V3.Forms
{
    public partial class FirmChange : Form
    {
        const int ERROR_OK = 0x11;
        const int ERROR_CRC = 0x22;

        struct LoadedParams
        {
            public byte ID;
            public string Port;
            public int Baudrate;
            public string FileName;
            public int Timeout;
        }
        struct LoaderResult
        {
            public enum LoaderStatus { LoaderOK, LoaderError }

            public LoaderStatus Status;
            public string Message;
        }

        Translation currentTranslation;
        LoadedParams loaderParams;

        public FirmChange(string port, int baudrate, byte id, Translation trans)
        {
            InitializeComponent();
            Text = Properties.Resources.AppName;

            loaderParams = new LoadedParams()
            {
                ID = id,
                Port = port,
                Baudrate = baudrate,
                FileName = ""
            };

            currentTranslation = trans;
            btDownloadFirm.Text = currentTranslation.btDownloadFirmText;
            btOpenFirmFile.Text = currentTranslation.btOpenFirmFileText;

            tbTout.Tag = 10;
        }

        private void btOpenFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog of = new OpenFileDialog();
            of.Filter = "(*.frm)|*.frm";
            if (of.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            lbFileName.Text = makeFileName(of.FileName, 40);

            loaderParams.FileName = of.FileName;
        }

        private void btDownload_Click(object sender, EventArgs e)
        {
            if (backgroundLoader.IsBusy)
                return;

            if (loaderParams.FileName == "")
            {
                MessageBox.Show(currentTranslation.FirmFileNotSpecifiedErrorMessage, Properties.Resources.AppName, 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            progressBar.Value = 0;
            loaderParams.Timeout = GetTouot();
            backgroundLoader.RunWorkerAsync(loaderParams);
        }

        private void backgroundLoader_DoWork(object sender, DoWorkEventArgs e)
        {
            LoadedParams param = (LoadedParams)e.Argument;
            SerialPort port = null;
            Stream inFile = null;

            try
            {
                port = new SerialPort(param.Port, /*9600*/param.Baudrate);
                port.WriteTimeout = port.ReadTimeout = 500;
                port.Open();

                byte[] buffer = new byte[0x40000];
                byte receivedByte = ERROR_CRC;		// One byte received from COM port
                byte retries = 0;		        // Number of tries so far
                int frameSize;					// Number of bytes in the current frame
                int bufferSize = 0;				// Total number of bytes in the buffer
                int bytesSent = 0;				// Number of bytes sent so far

                bool? single_wire = null;

                inFile = File.Open(param.FileName, FileMode.Open, FileAccess.Read);
                byte[] buf_temp = new byte[2];
                String str_temp = "  ";
                while (inFile.Read(buf_temp, 0, 2) > 1)
                {
                    str_temp = ((char)buf_temp[0]).ToString();
                    str_temp = str_temp.Insert(1, ((char)buf_temp[1]).ToString());
                    buffer[bufferSize] = Convert.ToByte(str_temp, 16);
                    bufferSize++;
                }

                port.BaudRate = 9600;
                port.ReadTimeout = param.Timeout;
                for (int index = 0; index < bufferSize; index += frameSize)
                {
                    frameSize = ((buffer[index] << 8) | buffer[index + 1]) + 2;

                    // Ignore any received bytes.
                    port.ReadTimeout = 1;
                    while (port.BytesToRead > 0)
                        port.ReadByte();
                    port.WriteTimeout = port.ReadTimeout = 400;

                    port.Write(buffer, index, frameSize);

                    if (!single_wire.HasValue)
                    {
                        // не знаем тип интерфейса
                        try
                        {
                            receivedByte = (byte)port.ReadByte();

                            // если первый байт, что мы приняли равен первому, что мы отослали
                            // и в порте еще есть байты, мы работаем по однопроводному интерфесу
                            if (receivedByte == buffer[0] && port.BytesToRead > 0)
                            {
                                int toRead = frameSize;
                                while (toRead-- > 0)
                                    receivedByte = (byte)port.ReadByte();

                                single_wire = true;
                            }
                            else
                                single_wire = false;

                            // иначе ответ мы уже прочитали
                        }
                        catch(TimeoutException)
                        {
                            // нет ответа
                            // надо попробовать modbus

                            try
                            {
                                // перевод в режим загрузчика
                                port.BaudRate = param.Baudrate;

                                Modbus mb = new Modbus(port, param.ID);
                                mb.WriteSingleRegisterWithCheck(0x4000, 0x3800);
                                single_wire = mb.InterfType == Modbus.InterfaceType.OneWire ? true : false;

                                port.BaudRate = 9600;

                                // передадим пакет снова
                                index -= frameSize;
                                continue;

                            }
                            catch
                            {
                                // нет ответа на пакет modbus - выход
                                e.Result = new LoaderResult()
                                {
                                    Status = LoaderResult.LoaderStatus.LoaderError,
                                    Message = currentTranslation.modbusNoAnswerError
                                };
                                return;
                            }
                        }
                        
                    }
                    else
                    {

                        // Check the response
                        if (!single_wire.Value)
                            receivedByte = (byte)port.ReadByte();
                        else
                        {
                            int toRead = frameSize + 1;
                            while (toRead-- > 0)
                                receivedByte = (byte)port.ReadByte();
                        }
                    }

                    switch (receivedByte)
                    {
                        case ERROR_OK:
                            bytesSent += frameSize;
                            retries = 0;

                            backgroundLoader.ReportProgress(100 * bytesSent / bufferSize);
                            break;
                        case ERROR_CRC:
                            if (++retries < 4)
                                index -= frameSize;
                            else
                            {
                                e.Result = new LoaderResult()
                                {
                                    Status = LoaderResult.LoaderStatus.LoaderError,
                                    Message = currentTranslation.FirmCRCErrorMessage
                                };
                                return;
                            }
                            break;
                        default:
                            e.Result = new LoaderResult()
                            {
                                Status = LoaderResult.LoaderStatus.LoaderError,
                                Message = currentTranslation.InvalidAnsErrorMessage
                            };
                            return;
                    }
                }

                backgroundLoader.ReportProgress(100);
                e.Result = new LoaderResult()
                {
                    Status = LoaderResult.LoaderStatus.LoaderOK,
                    Message = currentTranslation.WriteDoneMessage
                };
            }
            catch (TimeoutException)
            {
                e.Result = new LoaderResult()
                {
                    Status = LoaderResult.LoaderStatus.LoaderError,
                    Message = currentTranslation.modbusNoAnswerError
                };
            }
            catch (Exception ex)
            {
                e.Result = new LoaderResult()
                {
                    Status = LoaderResult.LoaderStatus.LoaderError,
                    Message = ex.Message
                };
            }
            finally
            {
                if (inFile != null)
                    inFile.Dispose();
                if (port != null)
                {
                    if(port.IsOpen)
                        port.Close();
                }
            }
        }

        private void backgroundLoader_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            LoaderResult result = (LoaderResult)e.Result;

            MessageBox.Show(result.Message, Properties.Resources.AppName, MessageBoxButtons.OK, 
                result.Status == LoaderResult.LoaderStatus.LoaderOK ? MessageBoxIcon.Information : MessageBoxIcon.Error);

            progressBar.Value = 0;

            if (result.Status == LoaderResult.LoaderStatus.LoaderOK)
                Dispose();
        }

        private void backgroundLoader_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressBar.Value = e.ProgressPercentage;
        }

        private string makeFileName(string path, int maxLen)
        {
            string filePath = "";

            if (path.Length > maxLen)
            {
                string[] pathElems = path.Split('\\');

                int tailLen = "...\\".Length + pathElems[pathElems.Length - 1].Length;

                foreach (string elem in pathElems)
                {
                    if ((maxLen - (filePath.Length + elem.Length + 1)) >= tailLen)
                    {
                        filePath += elem + "\\";
                        continue;
                    }

                    filePath += "...\\" + pathElems[pathElems.Length - 1];
                    break;
                }
            }
            else
                filePath = path;

            return filePath;
        }

        private void FirmChange_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (backgroundLoader.IsBusy)
                e.Cancel = true;
        }

        private void tbTout_Enter(object sender, EventArgs e)
        {
            int tout = GetTouot();
            tbTout.Text = tout.ToString();
        }

        private void tbTout_Leave(object sender, EventArgs e)
        {
            int currTout = (int)tbTout.Tag;

            int tout;
            if (!int.TryParse(tbTout.Text, out tout) || (tout < 10 || tout > 90))
                tout = currTout;

            tbTout.Text = tout.ToString() + " с";
            tbTout.Tag = tout;
        }

        private int GetTouot()
        {
            try
            {
                string sTout = tbTout.Text.Split(new char[] { ' ' })[0];

                return int.Parse(sTout);
                
            }
            catch (Exception)
            {
                return 10;
            }
        }
    }
}
