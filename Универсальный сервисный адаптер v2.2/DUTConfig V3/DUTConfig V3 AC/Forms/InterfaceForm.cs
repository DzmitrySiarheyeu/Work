using System;
using System.Threading;
using System.IO.Ports;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using ZedGraph;
using ModbusRTU;
using DUTConfig_V3.Protocol;
using DUTConfig_V3.Forms;
using DUTConfig_V3.Settings;

namespace DUTConfig_V3.Forms
{
    public partial class InterfaceForm : Form
    {
        public enum ConnectionStatus { ConnectionOn, ConnectionOff, ConnectionUnknown }

        struct ConnectionParams
        {
            public ulong ConnectionBaud;
            public ulong CurrentBaud;
            public byte ConnectionID;
        }

        struct WriterParams
        {
            public ViewMode Mode;
            public AvgType AvgType;
            public AppType AppType;
            public string Port;
            public int Baudrate;
            public byte ModbusID;
            public ModbusRegister Register;
            public byte NewID;
            public int NewBaud;

            public bool flag;
        }
        struct WriterResult
        {
            public enum WriterStatus { WriterOk, WriterError }

            public string Message;
            public WriterStatus Status;
        }

        struct ReaderParams
        {
            public string Port;
            public int Baudrate;
            public byte ModbusID;
            public ModbusRegister Register;
            public bool FromSettings;
        }
        struct ReaderResult
        {
            public enum ReaderStatus { ReaderCancel, ReaderError }

            public string Message;
            public ReaderStatus Status;
            public bool FromSettings;
        }

        class ViewModeDesc
        {
            Control[] ModeControls;
            Point[] ModeLocations;
            Size MySize;
            Point MyLocation;

            public ViewModeDesc(Control[] modeControls, Point[] modeLocations, Size mySize, Point myLocation)
            {
                if(modeControls == null || modeLocations == null)
                    throw new ArgumentNullException();

                if(modeControls.Length != modeLocations.Length)
                    throw new ArgumentException();

                ModeControls = modeControls;
                ModeLocations = modeLocations;
                MySize = mySize;
                MyLocation = myLocation;
            }

            public void DisplayControls(Control control)
            {
                control.Size = MySize;

                Point shifted = new Point(MyLocation.X, MyLocation.Y - thisForm.VerticalScroll.Value + 1);
                control.Location = shifted;

                control.Controls.Clear();

                for (int i = 0; i < ModeControls.Length; i++)
                    ModeControls[i].Location = ModeLocations[i];

                control.Controls.AddRange(ModeControls);
            }
        }

        enum EditingStatus { EditOff, EditOn }
        enum ViewMode { Standart, Extended }
        enum AvgType { ExponAvg = 0, RunningAvg = 1, AdaptAvg = 2 }
        enum AppType { PieceWiseApp = 0, PolyApp = 1 }

        static Form thisForm;

        // контролы, видимость которых зависит от режима
        Control[] viewDependentControls;
        ToolStripItem[] viewDependentMenuItems;
        // клнтролы, которые разблокируются в режиме редактирования
        Control[] editDependentControls;

        ViewModeDesc stModeParamsView, stModeAdaptParamsView, exModeParamsView, exModeAdaptParamsView;
        ViewModeDesc stModeOmniView, exModeOmniView;

        // контролы для настройки параметров
        Control[] paramsControls;

        // текущий режим отображения меню
        ViewMode currentViewMode = ViewMode.Extended;

        // для считываемых данных
        ModbusRegister register;

        // для определения режима редактирования
        EditingStatus currentEditingStatus;

        // текущий перевод
        Translation currentTranslation;

        // текущие настройки
        DefaultSettings defSettings;

        // состояние поключения (есть/нет)
        ConnectionStatus connectStatus;

        // для синхронизации
        ManualResetEvent synchro;

        // параметры подключения
        ConnectionParams connectionParams;

        // флаг закрытия окна
        bool closingRequest = false;

        // флаг нажатия на кнопку НАСТРОИТЬ
        bool settingsReq = false;

        // флаг нажатия на кнопку ОК при записи настроек
        bool settingWriteReq = false;


        #region НАЧАЛО/ЗАВЕРШЕНИЕ РАБОТЫ

        public InterfaceForm(DefaultSettings defSettings, Translation trans)
        {
            InitializeComponent();
            FillViewDescriptors();

            thisForm = this;

            register = new ModbusRegister();
            readingBinding.DataSource = register;
            BindParameters(readingBinding);
            readingBinding.ResetBindings(false);

            coeffsBinding.DataSource = register.PolyCoeffs;
            BindCoeffs(coeffsBinding);
            coeffsBinding.ResetBindings(false);

            btBackToMain.Click += new EventHandler((s, e) => Close());
            miViewStandart.Click += new EventHandler((s, e) => ChangeMenuMode(ViewMode.Standart));
            miViewExtended.Click += new EventHandler((s, e) => ChangeMenuMode(ViewMode.Extended));
            dgvTarTable.DataError += new DataGridViewDataErrorEventHandler((s, e) =>
                MessageBox.Show(currentTranslation.ApprTableDataErrorMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error));

            // флаг подключить/отключить
            btConnect.Tag = true;

            cbPort_DropDown(cbPort, null);
            cbBaud.Text = "19200";

            this.Text = Properties.Resources.AppName + " " + Properties.Resources.AppVersion;
            this.defSettings = defSettings;

            currentTranslation = trans;
            if (currentTranslation != null)
                TranslateManual(currentTranslation);

            BindSettings(defSettings);

            editDependentControls = new Control[]
            {
                btTableClear,
                btTankEmpty,
                btTankFull,
                gbPoly,
                dgvTarTable
            };

            viewDependentControls = new Control[]
            {
                dgvTarTable,
                canvas,
                //btTankFull,
                //btTankEmpty,
                btTableClear
            };

            viewDependentMenuItems = new ToolStripItem[]
            {
                tsSeparator,
                miChangeFirmware,
                miThermo
            };

            paramsControls = new Control[]
            {
                cbTempSensorOn,
                cbApprType, 
                cbAvgType,
                tbAvgTimeInterval, 
                tbFullTankFreq,
                tbEmptyTankFreq,
                cbOmniAutoOn,
                tbOmniAutoPeriod,
                cbOmniNetMode,
                tbOmniMaxN,
                tbDeltaPowerSupply,
                tbAvgAdaptEngineOnTimeInterval,
                tbAvgAdaptEngineOffTimeInterval,
                tbFreqRange
            };

            ChangeMenuMode(ViewMode.Standart);

            synchro = new ManualResetEvent(true);

            connectStatus = ConnectionStatus.ConnectionUnknown;
        }

        private void InterfaceForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (backgroundReader.IsBusy)
            {
                closingRequest = e.Cancel = true;
                backgroundReader.CancelAsync();
            }

            defSettings.Port = cbPort.Text;
            defSettings.Baudrate = ulong.Parse(cbBaud.Text);
            defSettings.ModbusID = byte.Parse(tbID.Text);
        }

        private void FillViewDescriptors()
        {
            // параметры в стандартном режиме
            // не адаптивное усреднение
            stModeParamsView = new ViewModeDesc(
            new Control[] 
            {
                lbImmersionLevel, 
                lbImmersionLevelVal, 
                lbPowerSupply, 
                lbPowerSupplyVal, 
                lbInnerCore,
                lbInnerCoreVal,
                lbInnerCoreAvg, 
                lbInnerCoreAvgVal,
                lbTemp, 
                lbTempVal,
                lbAvgTimeInterval,
                tbAvgTimeInterval,
                lbFullTankFreq,
                tbFullTankFreq,
                lbEmptyTankFreq,
                tbEmptyTankFreq
            },
            new Point[]
            {
                new Point(3, 22),
                new Point(339, 22),
                new Point(4, 52),
                new Point(339, 52),
                new Point(4, 78),
                new Point(339, 78),
                new Point(4, 104),
                new Point(339, 104),
                new Point(4, 130),
                new Point(339, 130),
                new Point(4, 156),
                new Point(339, 156),
                new Point(4, 182),
                new Point(339, 182),
                new Point(4, 208),
                new Point(339, 208)
            },
            new Size(480, 235),
            new Point(5, 120));

            // омникомм в стандартном режиме
            // не адаптивное усреднение
            stModeOmniView = new ViewModeDesc(
            new Control[]
            {
                lbOmniAutoOn,
                cbOmniAutoOn,
                lbOmniAutoPeriod,
                tbOmniAutoPeriod,
                lbOmniNetMode,
                cbOmniNetMode,
                lbOmniCurrN,
                lbOmniCurrNVal
            },
            new Point[]
            {
                new Point(3, 22),
                new Point(339, 22),
                new Point(4, 48),
                new Point(339, 48),
                new Point(4, 74),
                new Point(339, 74),
                new Point(3, 100),
                new Point(338, 100)
            },
            new Size(480, 130),
            new Point(490, 120));

            // параметры в расширенном режиме
            // не адаптивное усреднение
            exModeParamsView = new ViewModeDesc(
            new Control[]
            {
                lbImmersionLevel,
                lbImmersionLevelVal,
                lbSensorReadings,
                lbSensorReadingsVal,
                lbPowerSupply,
                lbPowerSupplyVal,
                lbInnerCoreAvg,
                lbInnerCoreAvgVal,
                lbInnerCore,
                lbInnerCoreVal,
                lbTempSensorOn,
                cbTempSensorOn,
                lbTemp,
                lbTempVal,
                lbApprType,
                cbApprType,
                lbApprSize,
                lbApprSizeVal,
                lbAvgType,
                cbAvgType,
                lbAvgTimeInterval,
                tbAvgTimeInterval,
                lbFullTankFreq,
                tbFullTankFreq,
                lbEmptyTankFreq,
                tbEmptyTankFreq,
                lbFreqRange,
                tbFreqRange
            },
            new Point[]
            {
                new Point(3, 22),
                new Point(339, 22),
                new Point(4, 52),
                new Point(339, 52),
                new Point(4, 78),
                new Point(339, 78),
                new Point(4, 104),
                new Point(339, 104),
                new Point(4, 130),
                new Point(339, 130),
                new Point(4, 156),
                new Point(339, 156),
                new Point(4, 182),
                new Point(339, 182),
                new Point(4, 208),
                new Point(339, 208),
                new Point(4, 234),
                new Point(339, 234),
                new Point(4, 260),
                new Point(339, 260),
                new Point(4, 286),
                new Point(339, 286),
                new Point(4, 312),
                new Point(339, 312),
                new Point(4, 338),
                new Point(339, 338),
                new Point(4, 364),
                new Point(339, 364)
            },
            new Size(480, 370 + 25),
            new Point(5, 120));

            // омникомм в расширенном режиме
            // не адаптивное усреднение
            exModeOmniView = new ViewModeDesc(
            new Control[]
            {
                lbOmniAutoOn,
                cbOmniAutoOn,
                lbOmniAutoPeriod,
                tbOmniAutoPeriod,
                lbOmniNetMode,
                cbOmniNetMode,
                lbOmniMaxN,
                tbOmniMaxN,
                lbOmniCurrN,
                lbOmniCurrNVal
            },
            new Point[]
            {
                new Point(3, 22),
                new Point(339, 22),
                new Point(4, 48),
                new Point(339, 48),
                new Point(4, 74),
                new Point(339, 74),
                new Point(4, 100),
                new Point(339, 100),
                new Point(3, 126),
                new Point(338, 126)
            },
            new Size(480, 155),
            new Point(5, 490 + 25));

            // параметры в расширенном режиме
            // адаптивное усреднение
            exModeAdaptParamsView = new ViewModeDesc(
            new Control[]
            {
                lbImmersionLevel,
                lbImmersionLevelVal,
                lbSensorReadings,
                lbSensorReadingsVal,
                lbPowerSupply,
                lbPowerSupplyVal,
                lbInnerCoreAvg,
                lbInnerCoreAvgVal,
                lbInnerCore,
                lbInnerCoreVal,
                lbTempSensorOn,
                cbTempSensorOn,
                lbTemp,
                lbTempVal,
                lbApprType,
                cbApprType,
                lbApprSize,
                lbApprSizeVal,
                lbAvgType,
                cbAvgType,
                lbEngineState,
                lbEngineStateVal,
                lbDeltaPowerSupply,
                tbDeltaPowerSupply,
                lbAvgAdaptEngineOnTimeInterval,
                tbAvgAdaptEngineOnTimeInterval,
                lbAvgAdaptEngineOffTimeInterval,
                tbAvgAdaptEngineOffTimeInterval,
                lbFullTankFreq,
                tbFullTankFreq,
                lbEmptyTankFreq,
                tbEmptyTankFreq,
                lbFreqRange,
                tbFreqRange
            },
            new Point[]
            {
                new Point(3, 22),
                new Point(339, 22),
                new Point(4, 52),
                new Point(339, 52),
                new Point(4, 78),
                new Point(339, 78),
                new Point(4, 104),
                new Point(339, 104),
                new Point(4, 130),
                new Point(339, 130),
                new Point(4, 156),
                new Point(339, 156),
                new Point(4, 182),
                new Point(339, 182),
                new Point(4, 208),
                new Point(339, 208),
                new Point(4, 234),
                new Point(339, 234),
                new Point(4, 260),
                new Point(339, 260),
                new Point(4, 286),
                new Point(339, 286),
                new Point(4, 312),
                new Point(339, 312),
                new Point(4, 338),
                new Point(339, 338),
                new Point(4, 364),
                new Point(339, 364),
                new Point(4, 390),
                new Point(339, 390),
                new Point(4, 416),
                new Point(339, 416),
                new Point(4, 442),
                new Point(339, 442)
            },
            new Size(480, 445 + 25),
            new Point(5, 120));

            // параметры в стандартном режиме
            // адаптивное усреднение
            stModeAdaptParamsView = new ViewModeDesc(
            new Control[] 
            {
                lbImmersionLevel, 
                lbImmersionLevelVal, 
                lbPowerSupply, 
                lbPowerSupplyVal, 
                lbInnerCore,
                lbInnerCoreVal,
                lbInnerCoreAvg, 
                lbInnerCoreAvgVal,
                lbTemp, 
                lbTempVal,
                lbEngineState,
                lbEngineStateVal,
                lbDeltaPowerSupply,
                tbDeltaPowerSupply,
                lbAvgAdaptEngineOnTimeInterval,
                tbAvgAdaptEngineOnTimeInterval,
                lbAvgAdaptEngineOffTimeInterval,
                tbAvgAdaptEngineOffTimeInterval,
                lbFullTankFreq,
                tbFullTankFreq,
                lbEmptyTankFreq,
                tbEmptyTankFreq
            },
            new Point[]
            {
                new Point(3, 22),
                new Point(339, 22),
                new Point(4, 52),
                new Point(339, 52),
                new Point(4, 78),
                new Point(339, 78),
                new Point(4, 104),
                new Point(339, 104),
                new Point(4, 130),
                new Point(339, 130),
                new Point(4, 156),
                new Point(339, 156),
                new Point(4, 182),
                new Point(339, 182),
                new Point(4, 208),
                new Point(339, 208),
                new Point(4, 234),
                new Point(339, 234),
                new Point(4, 260),
                new Point(339, 260),
                new Point(4, 286),
                new Point(339, 286)
            },
            new Size(480, 315),
            new Point(5, 120));
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

            if (cbBaud.Items != null)
            {
                if (cbBaud.Items.Contains(defSettings.Baudrate.ToString()))
                    cbBaud.Text = defSettings.Baudrate.ToString();
            }

            tbID.Text = defSettings.ModbusID.ToString();
        }

        private void BindParameters(BindingSource binding)
        {
            lbPowerSupplyVal.DataBindings.Add(new Binding("Text", binding, "UPowStr"));
            lbSensorReadingsVal.DataBindings.Add(new Binding("Text", binding, "LitersStr"));
            lbImmersionLevelVal.DataBindings.Add(new Binding("Text", binding, "PercentLStr"));
            lbInnerCoreAvgVal.DataBindings.Add(new Binding("Text", binding, "DOTFrequencyStr"));
            lbInnerCoreVal.DataBindings.Add(new Binding("Text", binding, "DOTFrequencyCoreStr"));
            lbTempVal.DataBindings.Add(new Binding("Text", binding, "T"));
            lbApprSizeVal.DataBindings.Add(new Binding("Text", binding, "NPoints"));

            tbFullTankFreq.DataBindings.Add(new Binding("Text", binding, "FMin", false, DataSourceUpdateMode.OnPropertyChanged));
            tbEmptyTankFreq.DataBindings.Add(new Binding("Text", binding, "FMax", false, DataSourceUpdateMode.OnPropertyChanged));
            tbFreqRange.DataBindings.Add(new Binding("Text", binding, "DeltaFOut", false, DataSourceUpdateMode.OnPropertyChanged));
            lbSWVersionVal.DataBindings.Add(new Binding("Text", binding, "VersionSw"));
            tbOmniAutoPeriod.DataBindings.Add(new Binding("Text", binding, "PeriodAuto", false, DataSourceUpdateMode.OnPropertyChanged));
            tbOmniMaxN.DataBindings.Add(new Binding("Text", binding, "MaxN", false, DataSourceUpdateMode.OnPropertyChanged));

            cbTempSensorOn.DataBindings.Add(new Binding("SelectedIndex", binding, "FlTermoCorrect", false, DataSourceUpdateMode.OnPropertyChanged));
            cbApprType.DataBindings.Add(new Binding("SelectedIndex", binding, "TypeAppr", false, DataSourceUpdateMode.OnPropertyChanged));
            cbAvgType.DataBindings.Add(new Binding("SelectedIndex", binding, "TypeAverage", false, DataSourceUpdateMode.OnPropertyChanged));
            lbEngineStateVal.DataBindings.Add(new Binding("Text", binding, "EngineStateStr"));
            tbDeltaPowerSupply.DataBindings.Add(new Binding("Text", binding, "DeltaUPow", false, DataSourceUpdateMode.OnPropertyChanged));
            tbAvgAdaptEngineOnTimeInterval.DataBindings.Add(new Binding("Text", binding, "Time", false, DataSourceUpdateMode.OnPropertyChanged));
            tbAvgAdaptEngineOffTimeInterval.DataBindings.Add(new Binding("Text", binding, "T_Avg_window_stop", false, DataSourceUpdateMode.OnPropertyChanged));
            cbOmniAutoOn.DataBindings.Add(new Binding("SelectedIndex", binding, "FlAutoSend", false, DataSourceUpdateMode.OnPropertyChanged));
            cbOmniNetMode.DataBindings.Add(new Binding("SelectedIndex", binding, "OmniNetMode", false, DataSourceUpdateMode.OnPropertyChanged));
        }

        private void BindCoeffs(BindingSource binding)
        {
            tbPolyCoeffA.DataBindings.Add(new Binding("Text", binding, "A", false, DataSourceUpdateMode.OnPropertyChanged));
            tbPolyCoeffB.DataBindings.Add(new Binding("Text", binding, "B", false, DataSourceUpdateMode.OnPropertyChanged));
            tbPolyCoeffC.DataBindings.Add(new Binding("Text", binding, "C", false, DataSourceUpdateMode.OnPropertyChanged));
            tbPolyCoeffD.DataBindings.Add(new Binding("Text", binding, "D", false, DataSourceUpdateMode.OnPropertyChanged));
        }

        private void TranslateManual(Translation trans)
        {
            miFile.Text = trans.miFileText;
            miFileSave.Text = trans.miFileSaveText;
            miFileLoad.Text = trans.miFileLoadText;
            miView.Text = trans.miViewText;
            miViewStandart.Text = trans.miViewStandartText;
            miViewExtended.Text = trans.miViewExtendedText;
            miChangeFirmware.Text = trans.miChangeFirmwareText;
            miThermo.Text = trans.miThermoText;

            btBackToMain.Text = trans.btBackToMainText;
            btConnect.Text = trans.btConnectText;
            lbPort.Text = trans.lbPortText;
            lbModbusID.Text = trans.lbModbusIDText;
            lbBaudrate.Text = trans.lbBaudrateText;

            gbParams.Text = trans.gbParamsText;
            lbPowerSupply.Text = trans.lbPowerSupplyText;
            lbSensorReadings.Text = trans.lbSensorReadingsText;
            lbImmersionLevel.Text = trans.lbImmersionLevelText;
            lbInnerCoreAvg.Text = trans.lbInnerCoreAvgText;
            lbInnerCore.Text = trans.lbInnerCoreText;
            lbTempSensorOn.Text = trans.lbTempSensorOnText;
            lbEngineState.Text = trans.lbEngineStateText;
            cbTempSensorOn.Items.AddRange(new object[] { trans.TempSensorOffText, trans.TempSensorOnText });
            cbTempSensorOn.SelectedIndex = 0;
            lbTemp.Text = trans.lbTempText + ", °C";
            lbApprType.Text = trans.lbApprTypeText;
            cbApprType.Items.AddRange(new object[] { trans.ApprTypeLinearText, trans.ApprTypePolyText });
            cbApprType.SelectedIndex = 0;
            lbApprSize.Text = trans.lbApprSizeText;
            lbAvgType.Text = trans.lbAvgTypeText;
            cbAvgType.Items.AddRange(new object[] { trans.AvgTypeExpText, trans.AvgTypeRunText, trans.AvgTypeAdaptText });
            cbAvgType.SelectedIndex = 0;
            lbAvgTimeInterval.Text = trans.lbAvgIntervalText;
            lbDeltaPowerSupply.Text = trans.lbDeltaPowerSupplyText;
            lbAvgAdaptEngineOnTimeInterval.Text = trans.lbAvgAdaptEngineOnTimeIntervalText;
            lbAvgAdaptEngineOffTimeInterval.Text = trans.lbAvgAdaptEngineOffTimeIntervalText;
            lbFullTankFreq.Text = trans.lbFullTankFreqText;
            lbEmptyTankFreq.Text = trans.lbEmptyTankFreqText;
            lbFreqRange.Text = trans.lbFreqRangeText;

            lbSWVersion.Text = trans.lbSWVersionText;
            lbErrorCode.Text = trans.lbErrorCodeText;
            lbErrorCodeVal.Text = "0 - " + trans.Error0;
            gbOmnicomm.Text = trans.gbOmnicommText;
            lbOmniAutoOn.Text = trans.lbOmniAutoOnText;
            cbOmniAutoOn.Items.AddRange(new object[] { trans.OmniAutoNoneText, trans.OmniAutoASCIIText, trans.OmniAutoBinText });
            cbOmniAutoOn.SelectedIndex = 0;
            lbOmniAutoPeriod.Text = trans.lbOmniAutoPeriodText;
            lbOmniNetMode.Text = trans.lbOmniNetModeText;
            cbOmniNetMode.Items.AddRange(new object[] { trans.OmniNetOfflineText, trans.OmniNetOnlineText });
            cbOmniNetMode.SelectedIndex = 0;
            lbOmniMaxN.Text = trans.lbOmniMaxNText;
            lbOmniCurrN.Text = trans.lbOmniCurrNText;
            gbPoly.Text = trans.gbPolyText;

            dgvTarTable.Columns[0].HeaderText = trans.TarImmersHeaderText;
            dgvTarTable.Columns[1].HeaderText = trans.TarVolumeHeaderText;

            canvas.XAxisLabel = trans.TarImmersHeaderText;
            canvas.YAxisLabel = trans.TarVolumeHeaderText;
            canvas.TranslateMenu(trans);

            btEdit.Text = trans.btEditText;
            btEditCancel.Text = trans.btEditCancelText;
            btEditOk.Text = trans.btEditOkText;

            btTableClear.Text = trans.btTableClearText;
            btTankEmpty.Text = trans.btTankEmptyText;
            btTankFull.Text = trans.btTankFullText;
        }

        #endregion

        #region ЧТЕНИЕ

        private void FillReadings(bool not_used)
        {
          /*  register.ApproxTable.Sort((ModbusRegister.ApproxValues v1, ModbusRegister.ApproxValues v2) =>
            {
                if (v1.XVal < v2.XVal)
                    return -1;
                else if (v1.XVal > v2.YVal)
                    return 1;
                else
                    return 0;
            });*/

            try
            {
                readingBinding.ResetBindings(false);
            }
            catch
            {
                throw new Exception(Translation.Current.DUTParamsError);
            }

            /*if (register.DataDamageFlag)
            {
                Invoke(new Action<string>(s => MessageBox.Show(s, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Warning)), Translation.Current.DUTParamsError);
                register.DataDamageFlag = false;
            }*/

            string errorText = currentTranslation.Error0;

            switch (register.Error)
            {
                case 0:
                    break;
                case 300:
                    errorText = currentTranslation.Error300;
                    break;
                case 320:
                    errorText = currentTranslation.Error320;
                    break;
                case 340:
                    errorText = currentTranslation.Error340;
                    break;
                case 360:
                    errorText = currentTranslation.Error360;
                    break;
                case 380:
                    errorText = currentTranslation.Error380;
                    break;
                case 400:
                    errorText = currentTranslation.Error400;
                    break;
                case 420:
                    errorText = currentTranslation.Error420;
                    break;
                case 440:
                    errorText = currentTranslation.Error440;
                    break;
                default:
                    errorText = currentTranslation.ErrorX;
                    break;
            }

            lbErrorCodeVal.Text = String.Format("{0} - {1}", register.Error, errorText);

            if (currentViewMode == ViewMode.Standart) 
                cbAvgType.SelectedIndex = register.TypeAverage <= 2 ? register.TypeAverage : 0;
                
            if (register.TypeAverage == 0)
                tbAvgTimeInterval.Text = register.Alpha.ToString();
        }

        private void FillReadingsOmnicomm(ushort N)
        {
            lbOmniCurrNVal.Text = N.ToString();
        }

        private void FillReadingsManual(ModbusRegister register)
        {
            lbInnerCoreAvgVal.Text = register.DOTFrequencyStr;
            lbInnerCoreVal.Text = register.DOTFrequencyCoreStr;
            lbImmersionLevelVal.Text = register.PercentLStr;

            /*if (register.EngineState != 0)
                lbEngineStateVal.Text = currentTranslation.lbEngineStateOnText;
            else
                lbEngineStateVal.Text = currentTranslation.lbEngineStateOffText;*/
        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            if (backgroundWriter.IsBusy)
            {
                MessageBox.Show(currentTranslation.WritingActiveMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            ToolStripButton bt = sender as ToolStripButton;

            // нужно поключить
            if (true == (bool)bt.Tag)
            {
                bt.Tag = false;
                bt.Text = currentTranslation.btDisconnectText;
                btConnect.BackColor = Color.Crimson;

                // binding работает хреново для ToolStripItem, поэтому делаем руками
                byte id;
                if (!byte.TryParse(tbID.Text, out id))
                {
                    tbID.Text = defSettings.ModbusID.ToString();
                    id = defSettings.ModbusID;
                } 

                backgroundReader.RunWorkerAsync(new ReaderParams()
                {
                    Port = cbPort.Text,
                    Baudrate = 
                        (int)(currentEditingStatus == EditingStatus.EditOn ? connectionParams.ConnectionBaud : connectionParams.CurrentBaud),
                    ModbusID = currentEditingStatus == EditingStatus.EditOn ? connectionParams.ConnectionID : id,
                    Register = register
                });

                indicatorWorker.RunWorkerAsync();
            }
            // нужно отключить
            else
                backgroundReader.CancelAsync();
        }

        private void GetTarTable(Modbus modbus, ModbusRegister register)
        {
            byte[] bytesX, bytesY;

            if ((AppType)register.TypeAppr == AppType.PieceWiseApp)
            {
                int count = register.NPoints * 4;
                if (count == 0)
                    return;

                bytesX = modbus.ReadMemory(0, ModbusRegister.TAR_TABLE_ADDR, (byte)count);
                bytesY = modbus.ReadMemory(0, ModbusRegister.TAR_TABLE_ADDR + 60 * 2, (byte)count);

                register.SetTableBuf(bytesX, bytesY, count / 4);
            }
            else
            {
                // 4 коэффициента
                bytesX = modbus.ReadMemory(0, ModbusRegister.TAR_TABLE_ADDR, 16);

                if (bytesX == null || bytesX.Length != 16)
                    return;

                float A = register.PolyCoeffs.A = BitConverter.ToSingle(bytesX, 0);
                float B = register.PolyCoeffs.B = BitConverter.ToSingle(bytesX, 4);
                float C = register.PolyCoeffs.C = BitConverter.ToSingle(bytesX, 8);
                float D = register.PolyCoeffs.D = BitConverter.ToSingle(bytesX, 12);

                register.ApproxTable.Clear();
                for (int i = 0; i <= 100; i += 10)
                {
                    register.ApproxTable.Add(new ModbusRegister.ApproxValues()
                    {
                        XVal = i,
                        YVal = (float)(A * Math.Pow(i, 3.0) + B * Math.Pow(i, 2.0) + C * i + D)
                    });
                }
            }
        }

        private void HideSettingsButton(object notUsed)
        {
            btEdit.Visible = false;
            btEditOk.Visible = btEditCancel.Visible = true;

            currentEditingStatus = EditingStatus.EditOn;

            foreach (Control c in paramsControls)
                c.Enabled = true;

            foreach (Control c in editDependentControls)
                c.Enabled = true;
        }

        private void ShowSettingsButton(object notUsed)
        {
            btEdit.Visible = true;
            btEditOk.Visible = btEditCancel.Visible = false;

            currentEditingStatus = EditingStatus.EditOff;

            foreach (Control c in paramsControls)
                c.Enabled = false;

            foreach (Control c in editDependentControls)
                c.Enabled = false;
        }

        private void backgroudReader_DoWork(object sender, DoWorkEventArgs e)
        {
            //Thread.Sleep(1000);

            synchro.WaitOne();
            synchro.Reset();

            ReaderParams param = (ReaderParams)e.Argument;
            ModbusRegister register = param.Register;
            ModbusRegister tempReg = new ModbusRegister();
            ConnectionStatus prevStatus = ConnectionStatus.ConnectionUnknown;
            SerialPort port = null;

            // флаг использования широковещательного пакета
            bool wideRangeUsed = false;
            // флаг чтения табличных данных
            bool apprReaded = false;
            // флаг читали/не читали
            bool firstRead = false;
            // флаг показывает, что надо прочитать все параметры
            bool readAll = false;

            try
            {
                port = new SerialPort(param.Port, param.Baudrate, Parity.None, 8, StopBits.One);
                port.WriteTimeout = port.ReadTimeout = 250;
                port.Open();

                while (!backgroundReader.CancellationPending)
                {
                    Modbus modbus = new Modbus(port, param.ModbusID);

                    try
                    {
                        // читаем регистры датчиков старых версий
                        Modbus.READ_INPUT_REGS_RESP resp = modbus.ReadInputRegisters(0, 45);
                        byte[] ansBus = new byte[resp.count];
                        Array.Copy(resp.DataBuffer, 3, ansBus, 0, resp.count);
                        
                        Omnicomm.OmnicommResponce omniResp =
                            Omnicomm.ReadOmnicommData(port, param.ModbusID == 0 ? (byte)0xff : param.ModbusID, 
                            modbus.InterfType == Modbus.InterfaceType.OneWire ? true : false);

                        // если на прошлой итерации соединения не было
                        if (prevStatus == ConnectionStatus.ConnectionOff)
                            // установим флаг, чтобы прочитать все параметры
                            readAll = true;

                        // есть соединение с девайсом
                        prevStatus = connectStatus = ConnectionStatus.ConnectionOn;

                        if (currentEditingStatus == EditingStatus.EditOn && firstRead && !readAll)
                            tempReg.SetDataBuf(ansBus);
                        else
                            register.SetDataBuf(ansBus);

                        if (settingsReq)
                        {
                            Invoke(new Action<object>(o => HideSettingsButton(o)), 0);
                            settingsReq = false;
                        }

                        if (!apprReaded || readAll)
                        {
                            // дошли до сюда - соединение есть
                            if (param.FromSettings)
                            {
                                Invoke(new Action<object>(o => HideSettingsButton(o)), 0);
                                settingsReq = false;
                            }

                            GetTarTable(modbus, register);
                            apprReaded = true;
                        }

                        // если был ответ на широоквещательный пакет
                        // последующие запросы будем слать на адрес из ответа 
                        if (param.ModbusID == 0)
                        {
                            wideRangeUsed = true;
                            param.ModbusID = resp.HostID;
                            connectionParams.ConnectionID = resp.HostID;
                            Invoke(new Action<string>(a => tbID.Text = a), param.ModbusID.ToString());
                        }

                        if (currentEditingStatus == EditingStatus.EditOn && !readAll)
                        {
                            // режим чтения в режиме редактирования параметров
                            if (!firstRead)
                            {
                                // все параметры устанавливаем только один раз
                                firstRead = true;
                                Invoke(new Action<ushort>(n => FillReadingsOmnicomm(n)), omniResp.N);
                                Invoke(new Action<bool>(a => FillReadings(a)), true);
                            }
                            else
                            {
                                register.DOTFrequency = tempReg.DOTFrequency;
                                register.DOTFrequencyCore = tempReg.DOTFrequencyCore;
                                register.PercentL = tempReg.PercentL;

                                // потом вручную устанавливаем только пару параметров
                                Invoke(new Action<ModbusRegister>(r => FillReadingsManual(r)), register);
                            }
                        }
                        else
                        {
                            readAll = false;

                            // обычный режим чтения
                            Invoke(new Action<ushort>(n => FillReadingsOmnicomm(n)), omniResp.N);
                            Invoke(new Action<bool>(a => FillReadings(a)), true);
                        }

                        
                        Thread.Sleep(400);
                    }
                    catch (TimeoutException)
                    {
                        if (param.ModbusID == 0 || wideRangeUsed)
                        {
                            // нет ответа на широковещательный пакет - выходим
                            if (param.FromSettings || settingsReq)
                            {
                                e.Result = new ReaderResult()
                                {
                                    Status = ReaderResult.ReaderStatus.ReaderError,
                                    Message = currentTranslation.modbusNoAnswerError,
                                    FromSettings = param.FromSettings
                                };

                                return;
                            }

                            if (connectStatus != ConnectionStatus.ConnectionOff)
                            {
                                prevStatus = connectStatus = ConnectionStatus.ConnectionOff;

                                register.ClearDataBuf();
                                register.ClearTables();
                                Invoke(new Action<bool>(a => FillReadings(a)), true);
                                Invoke(new Action<ushort>(n => FillReadingsOmnicomm(n)), (ushort)0);
                            }
                        }

                        // нужно попробовать широковещательный пакет
                        param.ModbusID = 0;
                        Thread.Sleep(100);
                        continue;
                    }
                    catch (Omnicomm.OmnicommException ex)
                    {
                        string message = ex.Message;

                        switch (ex.Reason)
                        {
                            case Omnicomm.OmnicommException.ExceptionReason.CRCError:
                                message = currentTranslation.omniCRCError;
                                break;
                            case Omnicomm.OmnicommException.ExceptionReason.IDError:
                                message = currentTranslation.omniHostIDError;
                                break;
                            case Omnicomm.OmnicommException.ExceptionReason.LengthError:
                                message = currentTranslation.omniBufferLenError;
                                break;
                            case Omnicomm.OmnicommException.ExceptionReason.MasterError:
                                message = currentTranslation.omniMasterError;
                                break;
                        }

                        e.Result = new ReaderResult()
                        {
                            Status = ReaderResult.ReaderStatus.ReaderError,
                            Message = message,
                            FromSettings = param.FromSettings
                        };
                        return;
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
                            Message = message,
                            FromSettings = param.FromSettings
                        };
                        return;
                    }
                    catch (Exception ex)
                    {
                        e.Result = new ReaderResult()
                        {
                            Status = ReaderResult.ReaderStatus.ReaderError,
                            Message = ex.Message,
                            FromSettings = param.FromSettings
                        };
                        return;
                    }
                }

                if (param.FromSettings && !settingWriteReq)
                {
                    try
                    {
                        Modbus modbus = new Modbus(port, param.ModbusID);

                        // читаем регистры датчиков старых версий
                        Modbus.READ_INPUT_REGS_RESP resp = modbus.ReadInputRegisters(0, 45);
                        byte[] ansBus = new byte[resp.count];
                        Array.Copy(resp.DataBuffer, 3, ansBus, 0, resp.count);
                        
                        Omnicomm.OmnicommResponce omniResp =
                            Omnicomm.ReadOmnicommData(port, param.ModbusID == 0 ? (byte)0xff : param.ModbusID, 
                            modbus.InterfType == Modbus.InterfaceType.OneWire ? true : false);

                        register.SetDataBuf(ansBus);
                        GetTarTable(modbus, register);
                        Invoke(new Action<ushort>(n => FillReadingsOmnicomm(n)), omniResp.N);
                        Invoke(new Action<bool>(a => FillReadings(a)), true);
                    }
                    catch
                    {
                        
                    }
                }

                e.Result = new ReaderResult()
                {
                    Status = ReaderResult.ReaderStatus.ReaderCancel,
                    FromSettings = param.FromSettings
                };
                return;
            }
            catch (Exception ex)
            {
                e.Result = new ReaderResult()
                {
                    Status = ReaderResult.ReaderStatus.ReaderError,
                    Message = ex.Message,
                    FromSettings = param.FromSettings
                };
                return;
            }
            finally
            {
                if (port != null)
                {
                    if(port.IsOpen)
                        port.Close();
                }

                synchro.Set();
            }
        }

        private void backgroudReader_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            ReaderResult result = (ReaderResult)e.Result;

            //if (result.FromSettings)
                ShowSettingsButton(null);

            if (result.Status == ReaderResult.ReaderStatus.ReaderError)
                MessageBox.Show(result.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);

            btConnect.Tag = true;
            btConnect.Text = currentTranslation.btConnectText;
            btConnect.BackColor = Color.LimeGreen;

            indicatorWorker.CancelAsync();

            settingsReq = false;
            /*if (closingRequest)
                Close();*/
        }

        #endregion

        #region ЗАПИСЬ

        private void btEditOk_Click(object sender, EventArgs e)
        {
            if (backgroundWriter.IsBusy)
                return;

            settingWriteReq = true;
            backgroundReader.CancelAsync();

            if ((AppType)cbApprType.SelectedIndex == AppType.PolyApp)
            {
                if (register.ApproxTable.Count < 2)
                {
                    if (MessageBox.Show(currentTranslation.WritePolyRequest, Properties.Resources.AppName, MessageBoxButtons.YesNo, MessageBoxIcon.Question) 
                        != System.Windows.Forms.DialogResult.Yes)
                        return;
                }
            }
            else
            {
                if (register.ApproxTable.Count < 2)
                {
                    MessageBox.Show(currentTranslation.ApprTableTooShortMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            byte id;
            if (!byte.TryParse(tbID.Text, out id))
            {
                tbID.Text = connectionParams.ConnectionID.ToString();
                id = connectionParams.ConnectionID;
            }

           // register.ModId = id;
           // register.Baudrate = connectionParams.CurrentBaud;

            progressBar.Visible = true;

            register.ApproxTable.Sort((ModbusRegister.ApproxValues v1, ModbusRegister.ApproxValues v2) =>
            {
                if (v1.XVal < v2.XVal)
                    return -1;
                else if (v1.XVal > v2.YVal)
                    return 1;
                else
                    return 0;
            });
            dgvTarTable.Refresh();

            backgroundWriter.RunWorkerAsync(new WriterParams()
            {
                Mode = currentViewMode,
                AvgType = (AvgType)cbAvgType.SelectedIndex,
                AppType = (AppType)cbApprType.SelectedIndex,
                Port = cbPort.Text,
                Baudrate = (int)connectionParams.ConnectionBaud,
                Register = register,
                ModbusID = connectionParams.ConnectionID,
                NewID = id,
                NewBaud = (int)connectionParams.CurrentBaud,
                flag = sender == null,
            });
        }

        private void backgroundWriter_DoWork(object sender, DoWorkEventArgs e)
        {
            synchro.WaitOne();
            synchro.Reset();

            WriterParams param = (WriterParams)e.Argument;
            ModbusRegister register = param.Register;
            SerialPort port = null;

            register.ModId = param.NewID;
            register.Baudrate = (ulong)param.NewBaud;
            try
            {
                port = new SerialPort(param.Port, param.Baudrate, Parity.None, 8, StopBits.One);
                port.WriteTimeout = port.ReadTimeout = 250;
                port.Open();

                Modbus modbus = new Modbus(port, param.ModbusID);

                ushort[] addrsBuf = null;

                //if (!param.flag)
                {

                    if (param.Mode == ViewMode.Standart)
                    {
                        if (param.AvgType == AvgType.RunningAvg)
                            addrsBuf = new ushort[] { 0x16, 0x19, 0x1A, 0x1B, 0x24, 0x25, 0x26, 0x27 };
                        else if (param.AvgType == AvgType.ExponAvg)
                            addrsBuf = new ushort[] { 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x24, 0x25, 0x26, 0x27 };
                        else
                            addrsBuf = new ushort[] { 0x11, 0x16/*, 0x17, 0x18*/, 0x19, 0x1A, 0x1B, 0x24, 0x25, 0x26, 0x27, 0x29 };
                    }
                    else
                    {
                        register.NPoints = param.AppType == AppType.PieceWiseApp ? (ushort)register.ApproxTable.Count : (ushort)4;

                        if (param.AvgType == AvgType.RunningAvg)
                            addrsBuf = new ushort[] { 0x10, 0x15, 0x16, 0x19, 0x1A, 0x1B, 0x1D, 0x1E, 0x24, 0x25, 0x26, 0x27, 0x2a, 0x2C };
                        else if (param.AvgType == AvgType.ExponAvg)
                            addrsBuf = new ushort[] { 0x10, 0x15, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1E, 0x1D, 0x24, 0x25, 0x26, 0x27, 0x2a, 0x2C };
                        else
                            addrsBuf = new ushort[] { 0x10, 0x11, 0x15, 0x16, 0x19, 0x1A, 0x1B, 0x1D, 0x1E, 0x24, 0x25, 0x26, 0x27, 0x29, 0x2a, 0x2C };
                    }
                }
                /*else
                    addrsBuf = new ushort[] { 0x10, 0x11, 0x15, 0x16, 0x19, 0x1A, 0x1B, 0x1D, 0x1E, 0x24, 0x25, 0x26, 0x27, 0x29, 0x2a, 0x2C };*/

                int writingLen = 0;
                //if(!param.flag)
                     writingLen = addrsBuf.Length + (param.Mode == ViewMode.Extended ? 2 : 0);
                /*else
                    writingLen = addrsBuf.Length + 2;*/

                for (int i = 0; i < addrsBuf.Length; i++)
                {
                    modbus.WriteSingleRegisterWithCheck(addrsBuf[i], (ushort)(/*!param.flag */ register.GetRegsSHORT(addrsBuf[i]) /*: 0xFFFF*/));
                    backgroundWriter.ReportProgress((int)(((double)i / (double)writingLen) * 100));
                }

                if (param.Mode == ViewMode.Extended)
                {
                    byte[] buf = param.AppType == AppType.PieceWiseApp ? register.GetAppTableBuf() : register.GetPolyBuf();

                    // нужно писать именно такими буферами
                    // по другому не работает
                    byte[] part1 = new byte[128];
                    byte[] part2 = new byte[112];

                    //if (!param.flag)
                    {
                        Array.Copy(buf, 0, part1, 0, part1.Length);
                        Array.Copy(buf, part1.Length, part2, 0, part2.Length);
                    }
                   /* else
                    {
                        for (int i = 0; i < part1.Length; i++)
                            part1[i] = 0xff;
                        for (int i = 0; i < part2.Length; i++)
                            part2[i] = 0xff;
                    }*/

                    modbus.WriteMemory(0, (uint)ModbusRegister.TAR_TABLE_ADDR, part1, (byte)part1.Length);
                    modbus.WriteMemory(0, (uint)ModbusRegister.TAR_TABLE_ADDR + (uint)part1.Length, part2, (byte)part2.Length);
                }

                if (register.ModId != 0)
                {
                    // записываем id
                    modbus.WriteSingleRegisterWithCheck(0x1F, (ushort)(/*!param.flag ? */register.GetRegsSHORT(0x1F)/* : 0xFFFF*/));
                    modbus.HostID = (byte)register.ModId;
                    Invoke(new Action<string>(a => tbID.Text = a), register.ModId.ToString());
                }

                // в самую последнюю очередь пишем baudrate
                modbus.WriteSingleRegisterWithCheck(0x20, (ushort)(/*!param.flag ? */register.GetRegsSHORT(0x20)/* : 0xFFFF*/));
                modbus.WriteSingleRegisterWithCheck(0x21, (ushort)(/*!param.flag ? */register.GetRegsSHORT(0x21)/* : 0xFFFF*/));

                backgroundWriter.ReportProgress(100);
                e.Result = new WriterResult()
                {
                    Status = WriterResult.WriterStatus.WriterOk,
                    Message = currentTranslation.WriteDoneMessage
                };
                return;
            }
            catch (TimeoutException)
            {
                /*if (param.ModbusID == 0 || wideRageUsed)
                {
                    // нет ответа на широковещательный пакет - выходим
                    e.Result = new ReaderResult()
                    {
                        Status = ReaderResult.ReaderStatus.ReaderError,
                        Message = currentTranslation.modbusNoAnswerError
                    };

                    return;
                }

                // нужно попробовать широковещательный пакет
                param.ModbusID = 0;
                Thread.Sleep(100);
                continue;*/
                e.Result = new WriterResult()
                {
                    Status = WriterResult.WriterStatus.WriterError,
                    Message = currentTranslation.modbusNoAnswerError
                };
                return;
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
            catch (Exception ex)
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
                    if(port.IsOpen)
                        port.Close();
                }

                synchro.Set();
            }
        }

        private void backgroundWriter_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressBar.Value = e.ProgressPercentage;
        }

        private void backgroundWriter_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            WriterResult result = (WriterResult)e.Result;
            MessageBox.Show(result.Message, Properties.Resources.AppName, MessageBoxButtons.OK, 
                result.Status == WriterResult.WriterStatus.WriterOk ? MessageBoxIcon.Information : MessageBoxIcon.Error);

            // выставляем baudrate, который записали
            cbBaud.Text = register.Baudrate.ToString();

            if (result.Status == WriterResult.WriterStatus.WriterOk)
            {
                btEditCancel.PerformClick();
                btConnect.PerformClick();
            }

            progressBar.Value = 0;
            progressBar.Visible = false;
            settingWriteReq = false;
        }

        #endregion

        #region ТАРИРОВКА

        private void CalcPoly()
        {
            int count = register.ApproxTable.Count;

            if (count < 2)
                return;

            double[] xVals = new double[count];
            double[] yVals = new double[count];
            double[] coeffs;

            for (int i = 0; i < count; i++)
            {
                xVals[i] = register.ApproxTable[i].XVal;
                yVals[i] = register.ApproxTable[i].YVal;
            }

            alglib.barycentricinterpolant p = new alglib.barycentricinterpolant();
            alglib.polynomialfitreport rep = new alglib.polynomialfitreport();
            int info;

            alglib.polynomialfit(xVals, yVals, 4, out info, out p, out rep);
            alglib.polynomialbar2pow(p, out coeffs);

            tbPolyCoeffA.Text = coeffs[3].ToString();
            tbPolyCoeffB.Text = coeffs[2].ToString();
            tbPolyCoeffC.Text = coeffs[1].ToString();
            tbPolyCoeffD.Text = coeffs[0].ToString();

            PointPairList list = new PointPairList();

            for (int i = 0; i < count; i++)
            {
                yVals[i] = Math.Pow(xVals[i], 3.0) * coeffs[3] +
                            Math.Pow(xVals[i], 2.0) * coeffs[2] +
                            xVals[i] * coeffs[1] +
                            coeffs[0];

                list.Add(new PointPair()
                {
                    X = xVals[i],
                    Y = yVals[i]
                });
            }

            canvas.DrawCurve2(currentTranslation.GraphApproxChartLabel, list);
        }
        
        private void PieceWiceChanged()
        {
            PointPairList list = new PointPairList();

            foreach (ModbusRegister.ApproxValues v in register.ApproxTable)
                list.Add(new PointPair(v.XVal, v.YVal));

            canvas.Clear();
            string label = "";
            if ((AppType)register.TypeAppr == AppType.PolyApp)
                label = currentTranslation.GraphUserChartLabel;

            canvas.DrawCurve1(label, list);

            if ((AppType)register.TypeAppr == AppType.PolyApp)
                CalcPoly();
        }

        private void dgvTarTable_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            PieceWiceChanged();
        }

        private void dgvTarTable_UserDeletedRow(object sender, DataGridViewRowEventArgs e)
        {
            PieceWiceChanged();
        }

        private void dgvTarTable_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            object val = dgvTarTable.Rows[e.RowIndex].Cells[e.ColumnIndex].Value;
            if (val == null)
                return;

            register.ApproxTable.Sort((ModbusRegister.ApproxValues v1, ModbusRegister.ApproxValues v2) =>
            {
                if (v1.XVal < v2.XVal)
                    return -1;
                else if (v1.XVal > v2.YVal)
                    return 1;
                else
                    return 0;
            });
            dgvTarTable.Refresh();

            PieceWiceChanged();
        }

        private void btTableClear_Click(object sender, EventArgs e)
        {
            dgvTarTable.Rows.Clear();
        }

        private void btTankEmpty_Click(object sender, EventArgs e)
        {
            register.FMax = register.DOTFrequencyCore;
            tbEmptyTankFreq.Text = register.FMax.ToString();
        }

        private void btTankFull_Click(object sender, EventArgs e)
        {
            register.FMin = register.DOTFrequencyCore;
            tbFullTankFreq.Text = register.FMin.ToString();
        }

        #endregion

        #region ИНДИКАТОР СТАТУСА ПОДКЛЮЧЕНИЯ

        private void SwitchConnectionStatus(bool status)
        {
            lbConnectStatus.Text = status ? currentTranslation.StatusLabelConnectOnText : currentTranslation.StatusLabelConnectOffText;
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
            lbConnectStatus.BackColor = SystemColors.Control;
            connectStatus = ConnectionStatus.ConnectionUnknown;

            if (closingRequest)
                Close();
        }

        #endregion

        #region ПРОЧИЕ ОБРАБОТЧИКИ

        private void miChangeFirmware_Click(object sender, EventArgs e)
        {
            if (backgroundReader.IsBusy)
                backgroundReader.CancelAsync();

            if (backgroundWriter.IsBusy)
            {
                MessageBox.Show(currentTranslation.WritingActiveMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            FirmChange firm = new FirmChange(cbPort.Text, int.Parse(cbBaud.Text), byte.Parse(tbID.Text), currentTranslation);
            firm.ShowDialog();
        }

        private void miThermo_Click(object sender, EventArgs e)
        {
            byte id;
            if(!byte.TryParse(tbID.Text, out id))
                id = defSettings.ModbusID;

            SerialPort serial = null;
            try
            {
                serial = new SerialPort(cbPort.Text, int.Parse(cbBaud.Text));
                serial.ReadTimeout = 250;

                Termo_correction tCorrection = null;
                try
                {
                    tCorrection = AppSettings.GetSettingsFromXML<Termo_correction>(AppSettings.AppFolder + "thermo_stock.xml");
                }
                catch
                {
                    MessageBox.Show(currentTranslation.ThermoFileOpenFailMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    tCorrection = new Termo_correction();

                    unchecked
                    {
                        AppSettings.SaveSettingsToXML<Termo_correction>(AppSettings.AppFolder + "thermo_stock.xml", tCorrection);
                    }
                }

                Modbus modbus = new Modbus(serial, id);
                Fl_termo_window form = new Fl_termo_window(modbus, tCorrection, currentTranslation);
                form.ShowDialog();

                /*try
                {
                    AppSettings.SaveSettingsToXML<Termo_correction>(AppSettings.AppFolder + "thermo_stock.xml", tCorrection);
                }
                catch
                {
                    MessageBox.Show(currentTranslation.ThermoFileSaveFailMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }*/
            }
            catch
            {
                if (serial != null)
                    serial.Dispose();
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

        private void ChangeAvgType(object sender, EventArgs e)
        {
            SetMenuViewMode(currentViewMode, (AvgType)cbAvgType.SelectedIndex);

            if (cbAvgType.SelectedIndex == 0 || cbAvgType.SelectedIndex == 1)
            {
                lbDeltaPowerSupply.Visible = false;
                lbAvgAdaptEngineOnTimeInterval.Visible = false;
                lbAvgAdaptEngineOffTimeInterval.Visible = false;

                tbDeltaPowerSupply.Visible = false;
                tbAvgAdaptEngineOnTimeInterval.Visible = false;
                tbAvgAdaptEngineOffTimeInterval.Visible = false;

                tbAvgTimeInterval.Visible = true;
                lbAvgTimeInterval.Visible = true;
            }
            else
            {
                lbDeltaPowerSupply.Visible = true;
                lbAvgAdaptEngineOnTimeInterval.Visible = true;
                lbAvgAdaptEngineOffTimeInterval.Visible = true;

                tbDeltaPowerSupply.Visible = true;
                tbAvgAdaptEngineOnTimeInterval.Visible = true;
                tbAvgAdaptEngineOffTimeInterval.Visible = true;

                tbAvgTimeInterval.Visible = false;
                lbAvgTimeInterval.Visible = false;
            }

            if (cbAvgType.SelectedIndex == 0)
            {
                tbAvgTimeInterval.Tag = true;
                tbAvgTimeInterval.DataBindings.Clear();
                tbAvgTimeInterval.Text = register.Alpha.ToString();
                //tbAvgTimeInterval.DataBindings.Add(new Binding("Text", readingBinding, "Alpha", false/*, DataSourceUpdateMode.OnValidation*/));
            }
            else if (cbAvgType.SelectedIndex == 1)
            {
                tbAvgTimeInterval.Tag = false;
                tbAvgTimeInterval.DataBindings.Clear();
                tbAvgTimeInterval.DataBindings.Add(new Binding("Text", readingBinding, "Time", false, DataSourceUpdateMode.OnPropertyChanged));
            }
        }

        private void tbAvgTimeInterval_Leave(object sender, EventArgs e)
        {
            bool isAlpha = (bool)tbAvgTimeInterval.Tag == true;
            if (!isAlpha)
                return;

            string curr = tbAvgTimeInterval.Text;
            float val = 0f;
            if (!float.TryParse(tbAvgTimeInterval.Text, out val))
            {
                MessageBox.Show("Недопустимое значение интервала усреднения", Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                register.Alpha = 0;
                tbAvgTimeInterval.Text = register.Alpha.ToString();
                return;
            }

            register.Alpha = val;
            tbAvgTimeInterval.Text = register.Alpha.ToString();
            if (curr != tbAvgTimeInterval.Text)
            {
                MessageBox.Show("Недопустимое значение интервала усреднения", Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ChangeAppType(object sender, EventArgs e)
        {
            ComboBox cb = sender as ComboBox;

            gbPoly.Visible = cb.SelectedIndex == 0 ? false : true;
        }

        private void SetMenuViewMode(ViewMode mode, AvgType avg)
        {
            this.SizeChanged += InterfaceForm_Resize;

            if (mode == ViewMode.Standart)
            {
                WindowState = FormWindowState.Normal;
                AutoScroll = MaximizeBox = false;

                if (avg != AvgType.AdaptAvg)
                {
                    MaximumSize = MinimumSize = Size = new System.Drawing.Size(995, 400);
                }
                else
                {
                    MaximumSize = MinimumSize = Size = new System.Drawing.Size(995, 480);
                }
            }
            else
            {
                MaximumSize = MinimumSize = new System.Drawing.Size(0, 0);

                if (avg != AvgType.AdaptAvg)
                {
                    MinimumSize = new System.Drawing.Size(400, 300);
                    Size = new System.Drawing.Size(910, 760);
                }
                else
                {
                    MinimumSize = new System.Drawing.Size(400, 300);
                    Size = new System.Drawing.Size(910, 810 + 25);
                }
            }
        }

        private void InterfaceForm_Resize(object sender, EventArgs e)
        {
            // нужно убрать ивенты
            cbApprType.TextChanged -= ChangeAppType;
            cbAvgType.TextChanged -= ChangeAvgType;
            // нужно их сохранить и установить
            // после изменения вида меню
            int appType = cbApprType.SelectedIndex;
            int avgType = cbAvgType.SelectedIndex;

            AvgType avg = (AvgType)cbAvgType.SelectedIndex;

            if (miViewStandart.Checked == true)
            {
                btTableClear.Visible = false;

                if (avg != AvgType.AdaptAvg)
                {
                    fpEdit.Location = new Point(490, 315);

                    stModeParamsView.DisplayControls(gbParams);
                    stModeOmniView.DisplayControls(gbOmnicomm);
                }
                else
                {
                    fpEdit.Location = new Point(490, 395);

                    stModeAdaptParamsView.DisplayControls(gbParams);
                    stModeOmniView.DisplayControls(gbOmnicomm);
                }

                btTankEmpty.Location = new Point(490, 255);
                btTankFull.Location = new Point(615, 255);
            }
            else
            {
                if (avg != AvgType.AdaptAvg)
                {
                    exModeParamsView.DisplayControls(gbParams);
                    exModeOmniView.DisplayControls(gbOmnicomm);          

                    fpEdit.Location = new Point(370, 675/* - this.VerticalScroll.Value + 1*/);
                }
                else
                {
                    exModeAdaptParamsView.DisplayControls(gbParams);
                    exModeOmniView.DisplayControls(gbOmnicomm);

                    gbOmnicomm.Location = new Point(5, 565 + 25/* - this.VerticalScroll.Value + 1*/);

                    fpEdit.Location = new Point(370, 725 + 25/* - this.VerticalScroll.Value + 1*/);
                }

                this.Invalidate();

                btTankEmpty.Location = new Point(630, 280/* - this.VerticalScroll.Value + 1*/);
                btTankFull.Location = new Point(770, 280/* - this.VerticalScroll.Value + 1*/);
                btTableClear.Location = new Point(490, 280/* - this.VerticalScroll.Value + 1*/);
                btTableClear.Visible = true;

                dgvTarTable.Location = new Point(491, 77/* - this.VerticalScroll.Value + 1*/);
            }

            cbApprType.SelectedIndex = appType;
            cbAvgType.SelectedIndex = avgType;
            cbApprType.TextChanged += ChangeAppType;
            cbAvgType.TextChanged += ChangeAvgType;

            AutoScroll = true;

            this.SizeChanged -= InterfaceForm_Resize;

            MaximizeBox = miViewStandart.Checked == true;
        }

        private void ChangeMenuMode(ViewMode mode)
        {
            if (currentViewMode == mode)
                return;

            currentViewMode = mode;

            if (currentViewMode == ViewMode.Standart)
            {
                

                miViewStandart.Checked = true;
                miViewExtended.Checked = false;

                gbPoly.Visible = false;
            }
            else
            {
                miViewStandart.Checked = false;
                miViewExtended.Checked = true;

                if ((AppType)cbApprType.SelectedIndex == AppType.PolyApp)
                    gbPoly.Visible = true;
            }

            SetMenuViewMode(currentViewMode, (AvgType)cbAvgType.SelectedIndex);

            bool visibility = mode == ViewMode.Standart ? false : true;

            foreach (Control c in viewDependentControls)
                c.Visible = visibility;

            foreach (ToolStripItem i in viewDependentMenuItems)
                i.Visible = visibility;

            if (this.Location.Y + this.Height > Screen.PrimaryScreen.WorkingArea.Height)
            {
                this.Location = new Point(this.Location.X, Screen.PrimaryScreen.WorkingArea.Height - this.Height);
            }

            if (this.Height > Screen.PrimaryScreen.WorkingArea.Height)
            {
                this.Location = new Point(this.Location.X, 0);
                this.Height = Screen.PrimaryScreen.WorkingArea.Height;
            }
        }

        private void btEdit_Click(object sender, EventArgs e)
        {
            byte currId;
            if (!byte.TryParse(tbID.Text, out currId))
            {
                tbID.Text = defSettings.ModbusID.ToString();
                // id, к коротому будем подключаться
                connectionParams.ConnectionID = defSettings.ModbusID;
            }
            else
                // id, к коротому будем подключаться
                connectionParams.ConnectionID = currId;

            // baudrate, с которым мы будем подключаться
            connectionParams.ConnectionBaud = connectionParams.CurrentBaud = ulong.Parse(cbBaud.Text);

            if (!backgroundReader.IsBusy)
            {
                if (backgroundWriter.IsBusy)
                {
                    MessageBox.Show(currentTranslation.WritingActiveMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                // нужно поключить
                if (true == (bool)btConnect.Tag)
                {
                    btConnect.Tag = false;
                    btConnect.Text = currentTranslation.btDisconnectText;
                    btConnect.BackColor = Color.Crimson;

                    // binding работает хреново для ToolStripItem, поэтому делаем руками
                    byte id;
                    if (!byte.TryParse(tbID.Text, out id))
                    {
                        tbID.Text = defSettings.ModbusID.ToString();
                        id = defSettings.ModbusID;
                    }

                    backgroundReader.RunWorkerAsync(new ReaderParams()
                    {
                        Port = cbPort.Text,
                        Baudrate =
                            (int)(currentEditingStatus == EditingStatus.EditOn ? connectionParams.ConnectionBaud : connectionParams.CurrentBaud),
                        ModbusID = currentEditingStatus == EditingStatus.EditOn ? connectionParams.ConnectionID : id,
                        Register = register,
                        FromSettings = true
                    });

                    indicatorWorker.RunWorkerAsync();
                }
                // нужно отключить
                else
                    backgroundReader.CancelAsync();
            }
            else
            {
                settingsReq = true;
            }
            /* else
             {
                 currentEditingStatus = EditingStatus.EditOn;

                 btEdit.Visible = false;
                 btEditOk.Visible = btEditCancel.Visible = true;

                 foreach (Control c in paramsControls)
                     c.Enabled = true;

                 foreach (Control c in editDependentControls)
                     c.Enabled = true;
             }*/
        }

        private void btEditCancel_Click(object sender, EventArgs e)
        {
            currentEditingStatus = EditingStatus.EditOff;

            btEdit.Visible = true;
            btEditOk.Visible = btEditCancel.Visible = false;

            foreach (Control c in paramsControls)
                c.Enabled = false;

            foreach (Control c in editDependentControls)
                c.Enabled = false;
        }

        private void miFileSave_Click(object sender, EventArgs e)
        {
            SaveFileDialog sf = new SaveFileDialog();
            sf.FileName = "params.xml";
            sf.Filter = "XML|*.xml";

            if (sf.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            try
            {
                AppSettings.SaveSettingsToXML<ModbusRegister>(sf.FileName, register);
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void miFileLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog of = new OpenFileDialog();
            of.FileName = "params.xml";
            of.Filter = "XML|*.xml";

            if (of.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            try
            {
                ModbusRegister loadedReg = AppSettings.GetSettingsFromXML<ModbusRegister>(of.FileName);

                if (loadedReg != null)
                {
                    readingBinding.DataSource = register = loadedReg;
                    readingBinding.ResetBindings(false);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void cbBaud_SelectedIndexChanged(object sender, EventArgs e)
        {
            // текущий baudrate
            connectionParams.CurrentBaud = ulong.Parse(cbBaud.Text);
        }

        #endregion 

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            //btEditOk_Click(null, null);
        }
    }
}
