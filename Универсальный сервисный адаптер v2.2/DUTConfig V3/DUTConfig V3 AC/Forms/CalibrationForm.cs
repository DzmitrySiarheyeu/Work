using System;
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
    public partial class CalibrationForm : Form
    {
        public enum AnalogSensorType { TypeAnalog, TypeFreq }
        public enum ConnectionStatus { ConnectionOn, ConnectionOff, ConnectionUnknown }
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
                catch(TimeoutException)
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

        ConnectionStatus connectStatus = ConnectionStatus.ConnectionUnknown;
        AnalogSensorType currentType;
        Settings.DefaultSettings defaultSettings;

        #region НАЧАЛО/ЗАВЕРШЕНИЕ РАБОТЫ

        public CalibrationForm(AnalogSensorType type, Settings.DefaultSettings defSettings)
        {
            InitializeComponent();

            currentType = type;
            defaultSettings = defSettings;
            btConnect.Tag = true;

            btBackToMain.Tag = false;
            btBackToMain.Click += new EventHandler((s,e) => Close());

            cbPort_DropDown(null, null);
            ApplyDefaultSettings();
            TranslateManual();
        }

        private void CalibrationForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (calibWorker.IsBusy)
            {
                calibWorker.CancelAsync();
                btBackToMain.Tag = e.Cancel = true;
                return;
            }

            defaultSettings.Port = cbPort.Text;
        }

        private void ApplyDefaultSettings()
        {
            cbPort.Text = defaultSettings.Port;
        }

        private void TranslateManual()
        {
            Text = Translation.Current.CalibFormText;
            btBackToMain.Text = Translation.Current.btBackToMainText;
            btConnect.Text = Translation.Current.btDoCalibText;
        }

        #endregion

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
                    lbCalibMessages.Items.Add(s);
                    lbCalibMessages.SelectedIndex = lbCalibMessages.Items.Count - 1;
                    lbCalibMessages.SelectedIndex = -1;
                }), message);
        }

        private void ShowTimeSpan(int seconds)
        {
            string message = lbCalibMessages.Items[lbCalibMessages.Items.Count - 1] as string;
            string[] msg = message.Split(new string[] { "..." }, 10, StringSplitOptions.None);
            message = msg[0];

            lbCalibMessages.Items[lbCalibMessages.Items.Count - 1] = message + "..." + seconds.ToString();
        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            if (true == (bool)btConnect.Tag)
            {
                btConnect.Tag = false;

                btConnect.Text = Translation.Current.btCancelCalibText;
                btConnect.BackColor = Color.Crimson;
                lbCalibMessages.Items.Clear();

                calibWorker.RunWorkerAsync(new CalibParams()
                {
                    Port = cbPort.Text,
                    Type = currentType
                });

                indicatorWorker.RunWorkerAsync();
            }
            else
            {
                calibWorker.CancelAsync();
                btConnect.Text = Translation.Current.btDoCalibText;
                btConnect.BackColor = Color.LimeGreen;
            }
        }

        private void calibWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            //const int TOUT_INTERVALS_CNT = 60;
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
                    switch(stage)
                    {
                        case CalibStages.ConnectionStage:
                            SendCalibMessage(Translation.Current.calibStage1Message);
                            break;
                        case CalibStages.ShortcutStage:
                            reader.SetOutDiscrete(1);
                            SendCalibMessage(Translation.Current.calibStage2Message);
                            Invoke(new Action<string>(s => ShowStageMessage(s)), Translation.Current.calibShortcutReqMessage);
                            break;
                        case CalibStages.CalibReqStage:
                            SendCalibMessage(Translation.Current.calibStage3Message);
                            break;
                        case CalibStages.CalibReadyStage:
                            SendCalibMessage(Translation.Current.calibStage4Message);
                            break;
                        case CalibStages.CalibStage:
                            SendCalibMessage(Translation.Current.calibStage5Message);
                            Invoke(new Action<string>(s => ShowStageMessage(s)), Translation.Current.calibShortcutRemoveMessage);
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

                        if (stage == CalibStages.ConnectionStage)
                        {
                            readedParams = reader.ReadParams();
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ? 
                                    readedParams.U > (0 - DELTA_U) && readedParams.U < (15 + DELTA_U):
                                    readedParams.F > (300 - DELTA_F) && readedParams.U < (1500 + DELTA_F))
                                {
                                    SendCalibMessage("OK!");
                                    stage = CalibStages.ShortcutStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.ShortcutStage)
                        {
                            readedParams = reader.ReadParams();
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(1.4f, readedParams.U, DELTA_U) :
                                    CheckValue(340f, readedParams.F, DELTA_F) ||
                                    CheckValue(1500f, readedParams.F, DELTA_F))
                                {
                                    SendCalibMessage("OK!");
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

                                SendCalibMessage("OK!");
                                stage = CalibStages.CalibReadyStage;
                                break;
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.CalibReadyStage)
                        {
                            readedParams = reader.ReadParams();
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(1.3f, readedParams.U, DELTA_U) :
                                    CheckValue(330f, readedParams.F, DELTA_F))
                                {
                                    SendCalibMessage("OK!");
                                    stage = CalibStages.CalibStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.CalibStage)
                        {
                            readedParams = reader.ReadParams();
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(2.6f, readedParams.U, DELTA_U) :
                                    CheckValue(460f, readedParams.F, DELTA_F))
                                {
                                    SendCalibMessage("OK!");
                                    stage = CalibStages.OutControlStage;
                                    break;
                                }
                            }
                            else
                                connectStatus = ConnectionStatus.ConnectionOff;
                        }
                        else if (stage == CalibStages.OutControlStage)
                        {
                            readedParams = reader.ReadParams();
                            if (readedParams != null)
                            {
                                connectStatus = ConnectionStatus.ConnectionOn;

                                if (param.Type == AnalogSensorType.TypeAnalog ?
                                    CheckValue(0.0f, readedParams.U, DELTA_U) ||
                                    CheckValue(0.5f, readedParams.U, DELTA_U) ||
                                    CheckValue(1.0f, readedParams.U, DELTA_U) :
                                    CheckValue(500f, readedParams.F, DELTA_F))
                                {
                                    SendCalibMessage("OK!");
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
            catch(Exception ex)
            {
                e.Result = new CalibResult()
                {
                    Message = ex.Message,
                    Status = CalibResult.CalibStatus.CalibError
                };
            }
            finally
            {
                port.Dispose();
            }
        }

        private void calibWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            CalibResult result = (CalibResult)e.Result;

            if (result.Status != CalibResult.CalibStatus.CalibCancelled)
                MessageBox.Show(result.Message, Properties.Resources.AppName, MessageBoxButtons.OK, 
                    result.Status == CalibResult.CalibStatus.CalibError ? MessageBoxIcon.Error : MessageBoxIcon.Information);

            btConnect.Text = Translation.Current.btDoCalibText;
            btConnect.BackColor = Color.LimeGreen;
            btConnect.Tag = true;

            indicatorWorker.CancelAsync();
        }

        #endregion

        #region ИНДИКАТОР СТАТУСА

        private void SwitchConnectionStatus(bool status)
        {
            lbConnectStatus.Text = status ? Translation.Current.StatusLabelConnectOnText : Translation.Current.StatusLabelConnectOffText;
        }

        private void indicatorWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            const int INTERVALS_CNT = 5;
            bool isVisible = false;

            while (true)
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

            if (true == (bool)btBackToMain.Tag)
                Close();
        }

        #endregion

        #region ОСТАЛЬНОЕ

        private void cbPort_DropDown(object sender, EventArgs e)
        {
            string port = cbPort.Text;

            cbPort.Items.Clear();
            string[] ports = SerialPort.GetPortNames();
            if (ports != null && ports.Length > 0)
            {
                cbPort.Items.AddRange(ports);
                if (cbPort.Items.Contains(port))
                    cbPort.Text = port;
                else
                    cbPort.SelectedIndex = cbPort.Items.Count - 1;
            }
        }

        #endregion
    }
}
