using System;
using System.Threading;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO.Ports;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DUTConfig_V3.Settings;
using DUTConfig_V3.Protocol;

namespace DUTConfig_V3.Forms
{
    public partial class AnalogForm : Form
    {
        public enum AnalogSensorType { TypeAnalog, TypeFreq }
        public enum ConnectionStatus { ConnectionOn, ConnectionOff, ConnectionUnknown}

        public enum CalibStages { ConnectionStage, ShortcutStage, CalibReqStage, CalibReadyStage, CalibStage, OutControlStage, EndStage }
        private struct CalibParams
        {
            public string Port;
            public AnalogSensorType Type;
        }
        private struct CalibResult
        {
            public enum CalibStatus { CalibOK, CalibError, CalibCancelled }

            public string Message;
            public CalibStatus Status;
        }
        private class AdapterReaderWriter
        {
            public class ReadedParams
            {
                public float F;
                public float U;
            }

            Modbus _modbus;

            public AdapterReaderWriter(SerialPort port, byte id)
            {
                _modbus = new Modbus(port, id);
            }

            public ReadedParams ReadParams()
            {
                try
                {
                    Modbus.READ_INPUT_REGS_RESP resp = _modbus.ReadInputRegisters(0, 4);
                    return new ReadedParams()
                    {
                        F = resp.GetInpRegsFLOAT(0),
                        U = resp.GetInpRegsFLOAT(2)
                    };
                }
                catch (TimeoutException)
                {
                    return null;
                }
            }

            public bool SetOutFrequency(ushort freq)
            {
                try
                {
                    _modbus.WriteSingleRegisterWithCheck(0x22, 123);
                    _modbus.WriteSingleRegisterWithCheck(0x24, freq);
                    return true;
                }
                catch (TimeoutException)
                {
                    return false;
                }
            }

            public bool SetOutDiscrete(ushort outState)
            {
                try
                {
                    SetOutFrequency(0);
                    _modbus.WriteSingleRegisterWithCheck(0x22, 123);
                    _modbus.WriteSingleRegisterWithCheck(0x25, outState);
                    return true;
                }
                catch (TimeoutException)
                {
                    return false;
                }
            }
        }

        struct ReaderResult 
        {
            public enum ReaderStatus { ReaderCancel, ReaderError }

            public string Message;
            public ReaderStatus Status;
        }
        private struct ReaderParams
        {
            public string Port;
            public int Baudrate;
            public byte ModbusID;
            public AnalogSensorType Type;
        }

        Translation currentTranslation;
        DefaultSettings defSettings;
        AnalogSensorType type;
        ConnectionStatus connectStatus;
        bool closingRequest = false;
        bool calibRequest = false;

        public AnalogForm(AnalogSensorType type, DefaultSettings defSettings, Translation trans)
        {
            InitializeComponent();
            btBackToMain.Click += new EventHandler((s,e) => Close());

            // флаг подключить/отключить
            btConnect.Tag = btCalib.Tag = true;

            cbPort_DropDown(cbPort, null);

            this.Text = Properties.Resources.AppName + " " + Properties.Resources.AppVersion;
            this.defSettings = defSettings;
            this.type = type;

            currentTranslation = trans;
            if (currentTranslation != null)
                TranslateManual(currentTranslation);

            BindSettings(defSettings);

            connectStatus = ConnectionStatus.ConnectionUnknown;
        }

        private void TranslateManual(Translation trans)
        {
            btCalib.Text = trans.btDoCalibText;

            btBackToMain.Text = trans.btBackToMainText;
            btConnect.Text = trans.btConnectText;
            lbPort.Text = trans.lbPortText;
            lbValueType.Text = type == AnalogSensorType.TypeAnalog ?
                trans.lbValueTypeVText : trans.lbValueTypeFText;
        }

        private void BindSettings(DefaultSettings defSettings)
        {
            // биндинг для ToolStrip работает хреново
            // лучше сделать руками

            if (cbPort.Items != null)
            {
                if (cbPort.Items.Contains(defSettings.Port))
                    cbPort.Text = defSettings.Port;
            }
        }

        private void cbPort_DropDown(object sender, EventArgs e)
        {
            ToolStripComboBox cb = sender as ToolStripComboBox;

            string currentPort = cb.Text;
            cb.Items.Clear();
            cb.Items.AddRange(SerialPort.GetPortNames());

            if (cb.Items != null)
            {
                if (cb.Items.Contains(currentPort))
                    cb.Text = currentPort;
                else
                    cb.SelectedIndex = cb.Items.Count - 1;
            }
        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            if (calibWorker.IsBusy)
                return;

            ToolStripButton bt = sender as ToolStripButton;

            // нужно поключить
            if (true == (bool)bt.Tag)
            {
                bt.Tag = false;
                bt.Text = currentTranslation.btDisconnectText;
                btConnect.BackColor = Color.Crimson;

                backgroundReader.RunWorkerAsync(new ReaderParams()
                {
                    Port = cbPort.Text,
                    Baudrate = 19200,
                    ModbusID = 99,
                    Type = type
                });

                if (!indicatorWorker.IsBusy)
                    indicatorWorker.RunWorkerAsync();
            }
            // нужно отключить
            else
                backgroundReader.CancelAsync();
        }

        private void backgroundReader_DoWork(object sender, DoWorkEventArgs e)
        {
            ReaderParams param = (ReaderParams)e.Argument;
            SerialPort port = null;

            try
            {
                port = new SerialPort(param.Port, param.Baudrate, Parity.None, 8, StopBits.One);
                port.WriteTimeout = port.ReadTimeout = 250;
                port.Open();

                Modbus modbus = new Modbus(port, param.ModbusID);

                modbus.WriteSingleRegister(34, 123);
                modbus.WriteSingleRegister(35, (ushort)(param.Type == AnalogSensorType.TypeAnalog ? 0 : 1));

                while (!backgroundReader.CancellationPending)
                {
                    try
                    {
                        Modbus.READ_INPUT_REGS_RESP resp = modbus.ReadInputRegisters(0, 6);

                        float val = 0.0f;
                        if (param.Type == AnalogSensorType.TypeAnalog)
                            val = resp.GetInpRegsFLOAT(2);
                        else
                            val = resp.GetInpRegsFLOAT(0);

                        float curr = resp.GetInpRegsFLOAT(4) * 1000;

                        Invoke(new Action<string>(a => lbValue.Text = a), string.Format("{0:0.00}", val));
                        Invoke(new Action<string>(a => lbCurrent.Text = a), ((int)(curr)).ToString());
                        connectStatus = ConnectionStatus.ConnectionOn;

                        Thread.Sleep(400);
                    }
                    catch (TimeoutException)
                    {
                        //выходим
                        /*e.Result = new ReaderResult()
                        {
                            Status = ReaderResult.ReaderStatus.ReaderError,
                            Message = currentTranslation.modbusNoAnswerError
                        };

                        return;*/

                        Invoke(new Action<string>(a => lbValue.Text = a), string.Format("{0:0.00}", 0.0f));
                        Invoke(new Action<string>(a => lbCurrent.Text = a), "0");
                        connectStatus = ConnectionStatus.ConnectionOff;
                    }
                    catch (Modbus.ModbusException ex)
                    {
                        string message = ex.Message;

                        switch (ex.Reason)
                        {
                            case Modbus.ModbusException.ExceptionReason.IllegalAddrException:
                            case Modbus.ModbusException.ExceptionReason.IllegalFuncException:
                            case Modbus.ModbusException.ExceptionReason.IllegalValueException:
                                message = currentTranslation.modbusIllegalError;
                                break;
                            case Modbus.ModbusException.ExceptionReason.CRCException:
                                message = currentTranslation.modbusCRCError;
                                break;
                            case Modbus.ModbusException.ExceptionReason.IDException:
                                message = currentTranslation.modbusHostIDError;
                                break;
                            case Modbus.ModbusException.ExceptionReason.LengthException:
                                message = currentTranslation.modbusBufferLenError;
                                break;
                            case Modbus.ModbusException.ExceptionReason.UnknownException:
                                message = currentTranslation.modbusUnknownError;
                                break;
                        }

                        e.Result = new ReaderResult()
                        {
                            Status = ReaderResult.ReaderStatus.ReaderError,
                            Message = message
                        };
                        return;
                    }
                }

                e.Result = new ReaderResult()
                {
                    Status = ReaderResult.ReaderStatus.ReaderCancel
                };
                return;
            }
            catch(Exception ex)
            {
                e.Result = new ReaderResult()
                {
                    Status = ReaderResult.ReaderStatus.ReaderError,
                    Message = ex.Message
                };
                return;
            }
            finally
            {
                if (port != null)
                {
                    if (port.IsOpen)
                        port.Close();
                }
            }
        }

        private void backgroundReader_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            ReaderResult result = (ReaderResult)e.Result;

            if (result.Status == ReaderResult.ReaderStatus.ReaderError)
                MessageBox.Show(result.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);

            btConnect.Tag = true;
            btConnect.Text = currentTranslation.btConnectText;
            btConnect.BackColor = Color.LimeGreen;

            if (calibRequest)
            {
                calibRequest = false;
                btCalib.PerformClick();
            }
            else
                indicatorWorker.CancelAsync();
        }

        private void AnalogForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (backgroundReader.IsBusy || calibWorker.IsBusy)
            {
                closingRequest = e.Cancel = true;

                backgroundReader.CancelAsync();
                calibWorker.CancelAsync();
            }

            defSettings.Port = cbPort.Text;
        }

        private void SwitchConnectionStatus(bool status)
        {
            lbConnectStatus.Text = status ? currentTranslation.StatusLabelConnectOnText : currentTranslation.StatusLabelConnectOffText;
        }

        private void indicatorWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            const int INTERVALS_CNT = 5;
            bool isVisible = false;

            while(true)
            {
                if (connectStatus == ConnectionStatus.ConnectionUnknown && !indicatorWorker.CancellationPending)
                    continue;

                for (int i = 0; i < INTERVALS_CNT; i++)
                {
                    if (indicatorWorker.CancellationPending)
                        return;

                    Thread.Sleep(100);
                }

                Color color = default(Color);
                if (connectStatus == ConnectionStatus.ConnectionOn)
                {
                    Invoke(new Action<bool>(b => SwitchConnectionStatus(b)), true);
                    color = Color.LimeGreen;
                }
                else if (connectStatus == ConnectionStatus.ConnectionOff)
                {
                    Invoke(new Action<bool>(b => SwitchConnectionStatus(b)), false);
                    color = Color.Crimson;
                }

                isVisible = !isVisible;

                Invoke(new Action<Color>(c => lbConnectStatus.BackColor = c), isVisible ? color : SystemColors.Control);
            }
        }

        private void indicatorWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            lbConnectStatus.Text = "";
            lbConnectStatus.BackColor = SystemColors.Control;
            connectStatus = ConnectionStatus.ConnectionUnknown;

            if (closingRequest)
                Close();
        }

        #region КАЛИБРОВКА

        private void ShowStageMessage(string message)
        {
            MessageBox.Show(message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private bool CheckValue(float val1, float val2, float delta)
        {
            return ((val1 - delta) < val2 && val2 < (val1 + delta));
        }

        private void SendCalibMessage(string message)
        {
            Invoke(new Action<string>(s =>
            {
                /*lbCalibMessages.Items.Add(s);
                lbCalibMessages.SelectedIndex = lbCalibMessages.Items.Count - 1;
                lbCalibMessages.SelectedIndex = -1;*/
                lbCalibMessages.Text = s;
            }), message);
        }

        private void ShowTimeSpan(int seconds)
        {
            string message = lbCalibMessages./*Items[lbCalibMessages.Items.Count - 1] as string*/Text;
            string[] msg = message.Split(new string[] { "..." }, 10, StringSplitOptions.None);
            message = msg[0];

            lbCalibMessages./*Items[lbCalibMessages.Items.Count - 1]*/Text = message + "..." + seconds.ToString();
        }

        private void btCalib_Click(object sender, EventArgs e)
        {
            if (backgroundReader.IsBusy)
            {
                calibRequest = true;
                backgroundReader.CancelAsync();
                return;
            }

            if (true == (bool)btCalib.Tag)
            {
                btCalib.Tag = false;

                btCalib.Text = Translation.Current.btCancelCalibText;
                btCalib.BackColor = Color.Crimson;
                lbCalibMessages./*Items.Clear()*/Text = "";

                calibWorker.RunWorkerAsync(new CalibParams()
                {
                    Port = cbPort.Text,
                    Type = type
                });

                if (!indicatorWorker.IsBusy)
                    indicatorWorker.RunWorkerAsync();
            }
            else
            {
                calibWorker.CancelAsync();
                btCalib.Text = Translation.Current.btDoCalibText;
                btCalib.BackColor = Color.LimeGreen;
            }
        }

        private void calibWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            const float DELTA_F = 1;
            const float DELTA_U = 0.06f;

            CalibParams param = (CalibParams)e.Argument;
            SerialPort port = new SerialPort(param.Port, 19200);
            port.WriteTimeout = port.ReadTimeout = 150;

            try
            {
                port.Open();

                CalibStages stage = CalibStages.ConnectionStage;
                AdapterReaderWriter reader = new AdapterReaderWriter(port, 99);
                AdapterReaderWriter.ReadedParams readedParams;

                while (stage != CalibStages.EndStage)
                {
                    switch (stage)
                    {
                        case CalibStages.ConnectionStage:
                            SendCalibMessage(Translation.Current.calibStage1Message);
                            break;
                        case CalibStages.ShortcutStage:
                            reader.SetOutDiscrete(1);
                            SendCalibMessage(Translation.Current.calibStage2Message);
                            //Invoke(new Action<string>(s => ShowStageMessage(s)), Translation.Current.calibShortcutReqMessage);
                            break;
                        case CalibStages.CalibReqStage:
                            SendCalibMessage(Translation.Current.calibStage3Message);
                            break;
                        case CalibStages.CalibReadyStage:
                            SendCalibMessage(Translation.Current.calibStage4Message);
                            break;
                        case CalibStages.CalibStage:
                            SendCalibMessage(Translation.Current.calibStage5Message);
                            //Invoke(new Action<string>(s => ShowStageMessage(s)), Translation.Current.calibShortcutRemoveMessage);
                            break;
                        case CalibStages.OutControlStage:
                            SendCalibMessage(Translation.Current.calibStage6Message);
                            break;
                    }

                    DateTime timeBegin = DateTime.Now;
                    TimeSpan timeSpent = new TimeSpan();

                    while (timeSpent.TotalSeconds < 60)
                    {
                        if (calibWorker.CancellationPending)
                        {
                            e.Result = new CalibResult()
                            {
                                Status = CalibResult.CalibStatus.CalibCancelled
                            };
                            return;
                        }

                        readedParams = reader.ReadParams();
                        Invoke(new Action<string>(a => lbValue.Text = a), string.Format("{0:0.00}",
                            readedParams == null ? 0.00f : (param.Type == AnalogSensorType.TypeAnalog ? readedParams.U : readedParams.F)));

                        if (stage == CalibStages.ConnectionStage)
                        {
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    readedParams.U > (0 - DELTA_U) && readedParams.U < (15 + DELTA_U) :
                                    readedParams.F > (300 - DELTA_F) && readedParams.U < (1500 + DELTA_F))
                                {
                                    //SendCalibMessage("OK!");
                                    stage = CalibStages.ShortcutStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.ShortcutStage)
                        {
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(1.4f, readedParams.U, DELTA_U) :
                                    CheckValue(340f, readedParams.F, DELTA_F) ||
                                    CheckValue(1500f, readedParams.F, DELTA_F))
                                {
                                    //SendCalibMessage("OK!");
                                    stage = CalibStages.CalibReqStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.CalibReqStage)
                        {
                            if (reader.SetOutFrequency(430))
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                //SendCalibMessage("OK!");
                                stage = CalibStages.CalibReadyStage;
                                break;
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.CalibReadyStage)
                        {
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(1.3f, readedParams.U, DELTA_U) :
                                    CheckValue(330f, readedParams.F, DELTA_F))
                                {
                                    //SendCalibMessage("OK!");
                                    stage = CalibStages.CalibStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.CalibStage)
                        {
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(2.6f, readedParams.U, DELTA_U) :
                                    CheckValue(460f, readedParams.F, DELTA_F))
                                {
                                    //SendCalibMessage("OK!");
                                    stage = CalibStages.OutControlStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.OutControlStage)
                        {
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(0.0f, readedParams.U, DELTA_U) ||
                                    CheckValue(0.5f, readedParams.U, DELTA_U) ||
                                    CheckValue(1.0f, readedParams.U, DELTA_U) :
                                    CheckValue(500f, readedParams.F, DELTA_F))
                                {
                                    //SendCalibMessage("OK!");
                                    stage = CalibStages.EndStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }

                        Thread.Sleep(200);
                        timeSpent = DateTime.Now - timeBegin;

                        Invoke(new Action<int>(i => ShowTimeSpan(i)), (int)timeSpent.TotalSeconds);
                    }

                    if (timeSpent.TotalSeconds >= 60)
                    {
                        e.Result = new CalibResult()
                        {
                            Message = Translation.Current.calibStageTimeoutMessage,
                            Status = CalibResult.CalibStatus.CalibError
                        };
                        return;
                    }
                }

                e.Result = new CalibResult()
                {
                    Message = Translation.Current.WriteDoneMessage,
                    Status = CalibResult.CalibStatus.CalibOK
                };
            }
            catch (Exception ex)
            {
                e.Result = new CalibResult()
                {
                    Message = ex.Message,
                    Status = CalibResult.CalibStatus.CalibError
                };
            }
            finally
            {
                if(port.IsOpen)
                    port.Close();
            }
        }

        private void calibWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            CalibResult result = (CalibResult)e.Result;

            if (result.Status != CalibResult.CalibStatus.CalibCancelled)
                MessageBox.Show(result.Message, Properties.Resources.AppName, MessageBoxButtons.OK,
                    result.Status == CalibResult.CalibStatus.CalibError ? MessageBoxIcon.Error : MessageBoxIcon.Information);

            lbCalibMessages.Text = "";
            btCalib.Text = Translation.Current.btDoCalibText;
            btCalib.BackColor = Color.LimeGreen;
            btCalib.Tag = true;

            if (!closingRequest && result.Status != CalibResult.CalibStatus.CalibError)
                btConnect.PerformClick();
            else
                indicatorWorker.CancelAsync();
        }

        #endregion
    }
}
