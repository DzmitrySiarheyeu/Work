namespace DUTConfig_V3.Forms
{
    partial class InterfaceForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(InterfaceForm));
            this.menuStrip = new System.Windows.Forms.ToolStrip();
            this.miFile = new System.Windows.Forms.ToolStripDropDownButton();
            this.miFileSave = new System.Windows.Forms.ToolStripMenuItem();
            this.miFileLoad = new System.Windows.Forms.ToolStripMenuItem();
            this.miView = new System.Windows.Forms.ToolStripDropDownButton();
            this.miViewStandart = new System.Windows.Forms.ToolStripMenuItem();
            this.miViewExtended = new System.Windows.Forms.ToolStripMenuItem();
            this.tsSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.miChangeFirmware = new System.Windows.Forms.ToolStripButton();
            this.miThermo = new System.Windows.Forms.ToolStripButton();
            this.progressBar = new System.Windows.Forms.ToolStripProgressBar();
            this.toolStrip = new System.Windows.Forms.ToolStrip();
            this.btBackToMain = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.lbPort = new System.Windows.Forms.ToolStripLabel();
            this.cbPort = new System.Windows.Forms.ToolStripComboBox();
            this.lbBaudrate = new System.Windows.Forms.ToolStripLabel();
            this.cbBaud = new System.Windows.Forms.ToolStripComboBox();
            this.lbModbusID = new System.Windows.Forms.ToolStripLabel();
            this.tbID = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.btConnect = new System.Windows.Forms.ToolStripButton();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.lbConnectStatus = new System.Windows.Forms.ToolStripLabel();
            this.btTestButton = new System.Windows.Forms.ToolStripButton();
            this.lbEmptyTankFreq = new System.Windows.Forms.Label();
            this.lbTemp = new System.Windows.Forms.Label();
            this.lbFullTankFreq = new System.Windows.Forms.Label();
            this.lbInnerCoreAvg = new System.Windows.Forms.Label();
            this.lbImmersionLevel = new System.Windows.Forms.Label();
            this.gbOmnicomm = new System.Windows.Forms.GroupBox();
            this.tbOmniMaxN = new System.Windows.Forms.TextBox();
            this.lbOmniMaxN = new System.Windows.Forms.Label();
            this.tbOmniAutoPeriod = new System.Windows.Forms.TextBox();
            this.cbOmniNetMode = new System.Windows.Forms.ComboBox();
            this.lbOmniCurrNVal = new System.Windows.Forms.Label();
            this.lbOmniAutoOn = new System.Windows.Forms.Label();
            this.lbOmniAutoPeriod = new System.Windows.Forms.Label();
            this.lbOmniNetMode = new System.Windows.Forms.Label();
            this.lbOmniCurrN = new System.Windows.Forms.Label();
            this.cbOmniAutoOn = new System.Windows.Forms.ComboBox();
            this.lbImmersionLevelVal = new System.Windows.Forms.Label();
            this.lbInnerCoreAvgVal = new System.Windows.Forms.Label();
            this.lbTempVal = new System.Windows.Forms.Label();
            this.tbFullTankFreq = new System.Windows.Forms.TextBox();
            this.tbEmptyTankFreq = new System.Windows.Forms.TextBox();
            this.backgroundReader = new System.ComponentModel.BackgroundWorker();
            this.lbSWVersion = new System.Windows.Forms.Label();
            this.gbParams = new System.Windows.Forms.GroupBox();
            this.cbAvgType = new System.Windows.Forms.ComboBox();
            this.tbDeltaPowerSupply = new System.Windows.Forms.TextBox();
            this.tbAvgAdaptEngineOnTimeInterval = new System.Windows.Forms.TextBox();
            this.lbAvgType = new System.Windows.Forms.Label();
            this.lbApprSizeVal = new System.Windows.Forms.Label();
            this.tbAvgAdaptEngineOffTimeInterval = new System.Windows.Forms.TextBox();
            this.cbApprType = new System.Windows.Forms.ComboBox();
            this.lbEngineStateVal = new System.Windows.Forms.Label();
            this.lbEngineState = new System.Windows.Forms.Label();
            this.lbApprSize = new System.Windows.Forms.Label();
            this.lbDeltaPowerSupply = new System.Windows.Forms.Label();
            this.lbApprType = new System.Windows.Forms.Label();
            this.lbPowerSupplyVal = new System.Windows.Forms.Label();
            this.cbTempSensorOn = new System.Windows.Forms.ComboBox();
            this.lbAvgAdaptEngineOnTimeInterval = new System.Windows.Forms.Label();
            this.lbInnerCoreVal = new System.Windows.Forms.Label();
            this.lbAvgAdaptEngineOffTimeInterval = new System.Windows.Forms.Label();
            this.tbAvgTimeInterval = new System.Windows.Forms.TextBox();
            this.lbTempSensorOn = new System.Windows.Forms.Label();
            this.lbSensorReadingsVal = new System.Windows.Forms.Label();
            this.lbPowerSupply = new System.Windows.Forms.Label();
            this.lbSensorReadings = new System.Windows.Forms.Label();
            this.lbInnerCore = new System.Windows.Forms.Label();
            this.lbAvgTimeInterval = new System.Windows.Forms.Label();
            this.lbSWVersionVal = new System.Windows.Forms.Label();
            this.btEdit = new System.Windows.Forms.Button();
            this.fpEdit = new System.Windows.Forms.FlowLayoutPanel();
            this.btEditOk = new System.Windows.Forms.Button();
            this.btEditCancel = new System.Windows.Forms.Button();
            this.dgvTarTable = new System.Windows.Forms.DataGridView();
            this.xValDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.yValDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.readingBinding = new System.Windows.Forms.BindingSource(this.components);
            this.backgroundWriter = new System.ComponentModel.BackgroundWorker();
            this.btTableClear = new System.Windows.Forms.Button();
            this.btTankEmpty = new System.Windows.Forms.Button();
            this.btTankFull = new System.Windows.Forms.Button();
            this.pbPolyEquation = new System.Windows.Forms.PictureBox();
            this.tbPolyCoeffA = new System.Windows.Forms.TextBox();
            this.tbPolyCoeffB = new System.Windows.Forms.TextBox();
            this.tbPolyCoeffC = new System.Windows.Forms.TextBox();
            this.tbPolyCoeffD = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.gbPoly = new System.Windows.Forms.GroupBox();
            this.indicatorWorker = new System.ComponentModel.BackgroundWorker();
            this.lbErrorCode = new System.Windows.Forms.Label();
            this.lbErrorCodeVal = new System.Windows.Forms.Label();
            this.lbFreqRange = new System.Windows.Forms.Label();
            this.tbFreqRange = new System.Windows.Forms.TextBox();
            this.coeffsBinding = new System.Windows.Forms.BindingSource(this.components);
            this.canvas = new DUTConfig_V3.Forms.GraphControl();
            this.menuStrip.SuspendLayout();
            this.toolStrip.SuspendLayout();
            this.fpEdit.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvTarTable)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.readingBinding)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbPolyEquation)).BeginInit();
            this.gbPoly.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.coeffsBinding)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip
            // 
            this.menuStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miFile,
            this.miView,
            this.tsSeparator,
            this.miChangeFirmware,
            this.miThermo,
            this.progressBar});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.menuStrip.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.menuStrip.Size = new System.Drawing.Size(894, 25);
            this.menuStrip.TabIndex = 3;
            this.menuStrip.Text = "menuStrip";
            // 
            // miFile
            // 
            this.miFile.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.miFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miFileSave,
            this.miFileLoad});
            this.miFile.Image = ((System.Drawing.Image)(resources.GetObject("miFile.Image")));
            this.miFile.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.miFile.Name = "miFile";
            this.miFile.Size = new System.Drawing.Size(49, 22);
            this.miFile.Text = "Файл";
            // 
            // miFileSave
            // 
            this.miFileSave.Name = "miFileSave";
            this.miFileSave.Size = new System.Drawing.Size(132, 22);
            this.miFileSave.Text = "Сохранить";
            this.miFileSave.Click += new System.EventHandler(this.miFileSave_Click);
            // 
            // miFileLoad
            // 
            this.miFileLoad.Name = "miFileLoad";
            this.miFileLoad.Size = new System.Drawing.Size(132, 22);
            this.miFileLoad.Text = "Загрузить";
            this.miFileLoad.Click += new System.EventHandler(this.miFileLoad_Click);
            // 
            // miView
            // 
            this.miView.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.miView.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miViewStandart,
            this.miViewExtended});
            this.miView.Image = ((System.Drawing.Image)(resources.GetObject("miView.Image")));
            this.miView.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.miView.Name = "miView";
            this.miView.Size = new System.Drawing.Size(58, 22);
            this.miView.Text = "Режим";
            // 
            // miViewStandart
            // 
            this.miViewStandart.Checked = true;
            this.miViewStandart.CheckState = System.Windows.Forms.CheckState.Checked;
            this.miViewStandart.Name = "miViewStandart";
            this.miViewStandart.Size = new System.Drawing.Size(154, 22);
            this.miViewStandart.Text = "Обычный";
            // 
            // miViewExtended
            // 
            this.miViewExtended.Name = "miViewExtended";
            this.miViewExtended.Size = new System.Drawing.Size(154, 22);
            this.miViewExtended.Text = "Расширенный";
            // 
            // tsSeparator
            // 
            this.tsSeparator.Name = "tsSeparator";
            this.tsSeparator.Size = new System.Drawing.Size(6, 25);
            this.tsSeparator.Visible = false;
            // 
            // miChangeFirmware
            // 
            this.miChangeFirmware.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.miChangeFirmware.Image = ((System.Drawing.Image)(resources.GetObject("miChangeFirmware.Image")));
            this.miChangeFirmware.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.miChangeFirmware.Name = "miChangeFirmware";
            this.miChangeFirmware.Size = new System.Drawing.Size(119, 22);
            this.miChangeFirmware.Text = "Сменить прошивку";
            this.miChangeFirmware.Visible = false;
            this.miChangeFirmware.Click += new System.EventHandler(this.miChangeFirmware_Click);
            // 
            // miThermo
            // 
            this.miThermo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.miThermo.Image = ((System.Drawing.Image)(resources.GetObject("miThermo.Image")));
            this.miThermo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.miThermo.Name = "miThermo";
            this.miThermo.Size = new System.Drawing.Size(121, 22);
            this.miThermo.Text = "Термокомненсация";
            this.miThermo.Visible = false;
            this.miThermo.Click += new System.EventHandler(this.miThermo_Click);
            // 
            // progressBar
            // 
            this.progressBar.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(280, 22);
            this.progressBar.Visible = false;
            // 
            // toolStrip
            // 
            this.toolStrip.AutoSize = false;
            this.toolStrip.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.toolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btBackToMain,
            this.toolStripSeparator1,
            this.lbPort,
            this.cbPort,
            this.lbBaudrate,
            this.cbBaud,
            this.lbModbusID,
            this.tbID,
            this.toolStripSeparator2,
            this.btConnect,
            this.toolStripLabel1,
            this.lbConnectStatus,
            this.btTestButton});
            this.toolStrip.Location = new System.Drawing.Point(0, 25);
            this.toolStrip.Name = "toolStrip";
            this.toolStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.toolStrip.Size = new System.Drawing.Size(894, 45);
            this.toolStrip.TabIndex = 4;
            // 
            // btBackToMain
            // 
            this.btBackToMain.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btBackToMain.Image = ((System.Drawing.Image)(resources.GetObject("btBackToMain.Image")));
            this.btBackToMain.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btBackToMain.Name = "btBackToMain";
            this.btBackToMain.Size = new System.Drawing.Size(48, 42);
            this.btBackToMain.Text = "Назад";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 45);
            // 
            // lbPort
            // 
            this.lbPort.Name = "lbPort";
            this.lbPort.Size = new System.Drawing.Size(41, 42);
            this.lbPort.Text = "Порт:";
            // 
            // cbPort
            // 
            this.cbPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbPort.Font = new System.Drawing.Font("Segoe UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.cbPort.Name = "cbPort";
            this.cbPort.Size = new System.Drawing.Size(90, 45);
            this.cbPort.DropDown += new System.EventHandler(this.cbPort_DropDown);
            // 
            // lbBaudrate
            // 
            this.lbBaudrate.Name = "lbBaudrate";
            this.lbBaudrate.Size = new System.Drawing.Size(117, 42);
            this.lbBaudrate.Text = "Скорость обмена:";
            // 
            // cbBaud
            // 
            this.cbBaud.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBaud.Font = new System.Drawing.Font("Segoe UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.cbBaud.Items.AddRange(new object[] {
            "9600",
            "14440",
            "19200",
            "38400",
            "57600",
            "115200"});
            this.cbBaud.Name = "cbBaud";
            this.cbBaud.Size = new System.Drawing.Size(90, 45);
            this.cbBaud.SelectedIndexChanged += new System.EventHandler(this.cbBaud_SelectedIndexChanged);
            // 
            // lbModbusID
            // 
            this.lbModbusID.Name = "lbModbusID";
            this.lbModbusID.Size = new System.Drawing.Size(76, 42);
            this.lbModbusID.Text = "Modbus ID:";
            // 
            // tbID
            // 
            this.tbID.Font = new System.Drawing.Font("Segoe UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbID.MaxLength = 3;
            this.tbID.Name = "tbID";
            this.tbID.Size = new System.Drawing.Size(50, 45);
            this.tbID.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 45);
            // 
            // btConnect
            // 
            this.btConnect.BackColor = System.Drawing.Color.LimeGreen;
            this.btConnect.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btConnect.Image = ((System.Drawing.Image)(resources.GetObject("btConnect.Image")));
            this.btConnect.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btConnect.Name = "btConnect";
            this.btConnect.Size = new System.Drawing.Size(86, 42);
            this.btConnect.Text = "Подключить";
            this.btConnect.Click += new System.EventHandler(this.btConnect_Click);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(0, 42);
            // 
            // lbConnectStatus
            // 
            this.lbConnectStatus.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.lbConnectStatus.Name = "lbConnectStatus";
            this.lbConnectStatus.Size = new System.Drawing.Size(0, 42);
            // 
            // btTestButton
            // 
            this.btTestButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btTestButton.Image = ((System.Drawing.Image)(resources.GetObject("btTestButton.Image")));
            this.btTestButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btTestButton.Name = "btTestButton";
            this.btTestButton.Size = new System.Drawing.Size(33, 42);
            this.btTestButton.Text = "test";
            this.btTestButton.Visible = false;
            this.btTestButton.Click += new System.EventHandler(this.toolStripButton1_Click);
            // 
            // lbEmptyTankFreq
            // 
            this.lbEmptyTankFreq.AutoSize = true;
            this.lbEmptyTankFreq.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbEmptyTankFreq.Location = new System.Drawing.Point(4, 260);
            this.lbEmptyTankFreq.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbEmptyTankFreq.Name = "lbEmptyTankFreq";
            this.lbEmptyTankFreq.Size = new System.Drawing.Size(198, 16);
            this.lbEmptyTankFreq.TabIndex = 2;
            this.lbEmptyTankFreq.Text = "Частота для пустого бака, Гц";
            // 
            // lbTemp
            // 
            this.lbTemp.AutoSize = true;
            this.lbTemp.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbTemp.Location = new System.Drawing.Point(4, 104);
            this.lbTemp.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbTemp.Name = "lbTemp";
            this.lbTemp.Size = new System.Drawing.Size(166, 16);
            this.lbTemp.TabIndex = 2;
            this.lbTemp.Text = "Температура головы, °C";
            // 
            // lbFullTankFreq
            // 
            this.lbFullTankFreq.AutoSize = true;
            this.lbFullTankFreq.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbFullTankFreq.Location = new System.Drawing.Point(4, 234);
            this.lbFullTankFreq.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbFullTankFreq.Name = "lbFullTankFreq";
            this.lbFullTankFreq.Size = new System.Drawing.Size(200, 16);
            this.lbFullTankFreq.TabIndex = 1;
            this.lbFullTankFreq.Text = "Частота для полного бака, Гц";
            // 
            // lbInnerCoreAvg
            // 
            this.lbInnerCoreAvg.AutoSize = true;
            this.lbInnerCoreAvg.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbInnerCoreAvg.Location = new System.Drawing.Point(4, 78);
            this.lbInnerCoreAvg.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbInnerCoreAvg.Name = "lbInnerCoreAvg";
            this.lbInnerCoreAvg.Size = new System.Drawing.Size(280, 16);
            this.lbInnerCoreAvg.TabIndex = 1;
            this.lbInnerCoreAvg.Text = "Уср. частота внутреннего генератора, Гц";
            // 
            // lbImmersionLevel
            // 
            this.lbImmersionLevel.AutoSize = true;
            this.lbImmersionLevel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbImmersionLevel.Location = new System.Drawing.Point(3, 22);
            this.lbImmersionLevel.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbImmersionLevel.Name = "lbImmersionLevel";
            this.lbImmersionLevel.Size = new System.Drawing.Size(208, 20);
            this.lbImmersionLevel.TabIndex = 0;
            this.lbImmersionLevel.Text = "Уровень погружения, %";
            // 
            // gbOmnicomm
            // 
            this.gbOmnicomm.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F);
            this.gbOmnicomm.Location = new System.Drawing.Point(5, 490);
            this.gbOmnicomm.Name = "gbOmnicomm";
            this.gbOmnicomm.Size = new System.Drawing.Size(480, 155);
            this.gbOmnicomm.TabIndex = 5;
            this.gbOmnicomm.TabStop = false;
            this.gbOmnicomm.Text = "Омникомм";
            // 
            // tbOmniMaxN
            // 
            this.tbOmniMaxN.Enabled = false;
            this.tbOmniMaxN.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbOmniMaxN.Location = new System.Drawing.Point(342, 97);
            this.tbOmniMaxN.Margin = new System.Windows.Forms.Padding(2, 2, 0, 2);
            this.tbOmniMaxN.Name = "tbOmniMaxN";
            this.tbOmniMaxN.Size = new System.Drawing.Size(100, 22);
            this.tbOmniMaxN.TabIndex = 7;
            // 
            // lbOmniMaxN
            // 
            this.lbOmniMaxN.AutoSize = true;
            this.lbOmniMaxN.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbOmniMaxN.Location = new System.Drawing.Point(4, 100);
            this.lbOmniMaxN.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbOmniMaxN.Name = "lbOmniMaxN";
            this.lbOmniMaxN.Size = new System.Drawing.Size(185, 16);
            this.lbOmniMaxN.TabIndex = 6;
            this.lbOmniMaxN.Text = "Максимальное значение N";
            // 
            // tbOmniAutoPeriod
            // 
            this.tbOmniAutoPeriod.Enabled = false;
            this.tbOmniAutoPeriod.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbOmniAutoPeriod.Location = new System.Drawing.Point(342, 45);
            this.tbOmniAutoPeriod.Margin = new System.Windows.Forms.Padding(2, 2, 0, 2);
            this.tbOmniAutoPeriod.Name = "tbOmniAutoPeriod";
            this.tbOmniAutoPeriod.Size = new System.Drawing.Size(100, 22);
            this.tbOmniAutoPeriod.TabIndex = 5;
            // 
            // cbOmniNetMode
            // 
            this.cbOmniNetMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbOmniNetMode.Enabled = false;
            this.cbOmniNetMode.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.cbOmniNetMode.FormattingEnabled = true;
            this.cbOmniNetMode.Location = new System.Drawing.Point(342, 71);
            this.cbOmniNetMode.Margin = new System.Windows.Forms.Padding(2, 2, 0, 2);
            this.cbOmniNetMode.Name = "cbOmniNetMode";
            this.cbOmniNetMode.Size = new System.Drawing.Size(121, 24);
            this.cbOmniNetMode.TabIndex = 4;
            // 
            // lbOmniCurrNVal
            // 
            this.lbOmniCurrNVal.AutoSize = true;
            this.lbOmniCurrNVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbOmniCurrNVal.Location = new System.Drawing.Point(338, 126);
            this.lbOmniCurrNVal.Margin = new System.Windows.Forms.Padding(1, 2, 0, 2);
            this.lbOmniCurrNVal.Name = "lbOmniCurrNVal";
            this.lbOmniCurrNVal.Size = new System.Drawing.Size(34, 20);
            this.lbOmniCurrNVal.TabIndex = 3;
            this.lbOmniCurrNVal.Text = "0.0";
            // 
            // lbOmniAutoOn
            // 
            this.lbOmniAutoOn.AutoSize = true;
            this.lbOmniAutoOn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbOmniAutoOn.Location = new System.Drawing.Point(4, 22);
            this.lbOmniAutoOn.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbOmniAutoOn.Name = "lbOmniAutoOn";
            this.lbOmniAutoOn.Size = new System.Drawing.Size(166, 16);
            this.lbOmniAutoOn.TabIndex = 3;
            this.lbOmniAutoOn.Text = "Тип автовыдачи данных";
            // 
            // lbOmniAutoPeriod
            // 
            this.lbOmniAutoPeriod.AutoSize = true;
            this.lbOmniAutoPeriod.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbOmniAutoPeriod.Location = new System.Drawing.Point(4, 48);
            this.lbOmniAutoPeriod.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbOmniAutoPeriod.Name = "lbOmniAutoPeriod";
            this.lbOmniAutoPeriod.Size = new System.Drawing.Size(173, 16);
            this.lbOmniAutoPeriod.TabIndex = 3;
            this.lbOmniAutoPeriod.Text = "Период выдачи данных, с";
            // 
            // lbOmniNetMode
            // 
            this.lbOmniNetMode.AutoSize = true;
            this.lbOmniNetMode.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbOmniNetMode.Location = new System.Drawing.Point(4, 74);
            this.lbOmniNetMode.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbOmniNetMode.Name = "lbOmniNetMode";
            this.lbOmniNetMode.Size = new System.Drawing.Size(159, 16);
            this.lbOmniNetMode.TabIndex = 3;
            this.lbOmniNetMode.Text = "Режим сетевой работы";
            // 
            // lbOmniCurrN
            // 
            this.lbOmniCurrN.AutoSize = true;
            this.lbOmniCurrN.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbOmniCurrN.Location = new System.Drawing.Point(3, 126);
            this.lbOmniCurrN.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbOmniCurrN.Name = "lbOmniCurrN";
            this.lbOmniCurrN.Size = new System.Drawing.Size(181, 20);
            this.lbOmniCurrN.TabIndex = 3;
            this.lbOmniCurrN.Text = "Текущее значение N";
            // 
            // cbOmniAutoOn
            // 
            this.cbOmniAutoOn.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbOmniAutoOn.Enabled = false;
            this.cbOmniAutoOn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.cbOmniAutoOn.FormattingEnabled = true;
            this.cbOmniAutoOn.Location = new System.Drawing.Point(342, 19);
            this.cbOmniAutoOn.Margin = new System.Windows.Forms.Padding(2, 2, 0, 2);
            this.cbOmniAutoOn.Name = "cbOmniAutoOn";
            this.cbOmniAutoOn.Size = new System.Drawing.Size(121, 24);
            this.cbOmniAutoOn.TabIndex = 4;
            // 
            // lbImmersionLevelVal
            // 
            this.lbImmersionLevelVal.AutoSize = true;
            this.lbImmersionLevelVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbImmersionLevelVal.Location = new System.Drawing.Point(338, 23);
            this.lbImmersionLevelVal.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.lbImmersionLevelVal.Name = "lbImmersionLevelVal";
            this.lbImmersionLevelVal.Size = new System.Drawing.Size(44, 20);
            this.lbImmersionLevelVal.TabIndex = 3;
            this.lbImmersionLevelVal.Text = "0.00";
            // 
            // lbInnerCoreAvgVal
            // 
            this.lbInnerCoreAvgVal.AutoSize = true;
            this.lbInnerCoreAvgVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbInnerCoreAvgVal.Location = new System.Drawing.Point(339, 78);
            this.lbInnerCoreAvgVal.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.lbInnerCoreAvgVal.Name = "lbInnerCoreAvgVal";
            this.lbInnerCoreAvgVal.Size = new System.Drawing.Size(25, 16);
            this.lbInnerCoreAvgVal.TabIndex = 3;
            this.lbInnerCoreAvgVal.Text = "0.0";
            // 
            // lbTempVal
            // 
            this.lbTempVal.AutoSize = true;
            this.lbTempVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbTempVal.Location = new System.Drawing.Point(339, 104);
            this.lbTempVal.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.lbTempVal.Name = "lbTempVal";
            this.lbTempVal.Size = new System.Drawing.Size(25, 16);
            this.lbTempVal.TabIndex = 3;
            this.lbTempVal.Text = "0.0";
            // 
            // tbFullTankFreq
            // 
            this.tbFullTankFreq.Enabled = false;
            this.tbFullTankFreq.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbFullTankFreq.Location = new System.Drawing.Point(342, 231);
            this.tbFullTankFreq.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.tbFullTankFreq.Name = "tbFullTankFreq";
            this.tbFullTankFreq.Size = new System.Drawing.Size(100, 22);
            this.tbFullTankFreq.TabIndex = 5;
            // 
            // tbEmptyTankFreq
            // 
            this.tbEmptyTankFreq.Enabled = false;
            this.tbEmptyTankFreq.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbEmptyTankFreq.Location = new System.Drawing.Point(342, 257);
            this.tbEmptyTankFreq.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.tbEmptyTankFreq.Name = "tbEmptyTankFreq";
            this.tbEmptyTankFreq.Size = new System.Drawing.Size(100, 22);
            this.tbEmptyTankFreq.TabIndex = 5;
            // 
            // backgroundReader
            // 
            this.backgroundReader.WorkerReportsProgress = true;
            this.backgroundReader.WorkerSupportsCancellation = true;
            this.backgroundReader.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroudReader_DoWork);
            this.backgroundReader.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroudReader_RunWorkerCompleted);
            // 
            // lbSWVersion
            // 
            this.lbSWVersion.AutoSize = true;
            this.lbSWVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbSWVersion.Location = new System.Drawing.Point(12, 77);
            this.lbSWVersion.Name = "lbSWVersion";
            this.lbSWVersion.Size = new System.Drawing.Size(78, 16);
            this.lbSWVersion.TabIndex = 8;
            this.lbSWVersion.Text = "Версия ПО";
            // 
            // gbParams
            // 
            this.gbParams.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F);
            this.gbParams.Location = new System.Drawing.Point(5, 120);
            this.gbParams.Name = "gbParams";
            this.gbParams.Size = new System.Drawing.Size(480, 370);
            this.gbParams.TabIndex = 9;
            this.gbParams.TabStop = false;
            this.gbParams.Text = "Параметры ДУТ";
            // 
            // cbAvgType
            // 
            this.cbAvgType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbAvgType.Enabled = false;
            this.cbAvgType.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.cbAvgType.FormattingEnabled = true;
            this.cbAvgType.Location = new System.Drawing.Point(342, 437);
            this.cbAvgType.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.cbAvgType.Name = "cbAvgType";
            this.cbAvgType.Size = new System.Drawing.Size(121, 24);
            this.cbAvgType.TabIndex = 26;
            this.cbAvgType.TextChanged += new System.EventHandler(this.ChangeAvgType);
            // 
            // tbDeltaPowerSupply
            // 
            this.tbDeltaPowerSupply.Enabled = false;
            this.tbDeltaPowerSupply.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbDeltaPowerSupply.Location = new System.Drawing.Point(342, 153);
            this.tbDeltaPowerSupply.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.tbDeltaPowerSupply.Name = "tbDeltaPowerSupply";
            this.tbDeltaPowerSupply.Size = new System.Drawing.Size(100, 22);
            this.tbDeltaPowerSupply.TabIndex = 5;
            // 
            // tbAvgAdaptEngineOnTimeInterval
            // 
            this.tbAvgAdaptEngineOnTimeInterval.Enabled = false;
            this.tbAvgAdaptEngineOnTimeInterval.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbAvgAdaptEngineOnTimeInterval.Location = new System.Drawing.Point(342, 179);
            this.tbAvgAdaptEngineOnTimeInterval.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.tbAvgAdaptEngineOnTimeInterval.Name = "tbAvgAdaptEngineOnTimeInterval";
            this.tbAvgAdaptEngineOnTimeInterval.Size = new System.Drawing.Size(100, 22);
            this.tbAvgAdaptEngineOnTimeInterval.TabIndex = 5;
            // 
            // lbAvgType
            // 
            this.lbAvgType.AutoSize = true;
            this.lbAvgType.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbAvgType.Location = new System.Drawing.Point(4, 440);
            this.lbAvgType.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbAvgType.Name = "lbAvgType";
            this.lbAvgType.Size = new System.Drawing.Size(114, 16);
            this.lbAvgType.TabIndex = 25;
            this.lbAvgType.Text = "Тип усреднения";
            // 
            // lbApprSizeVal
            // 
            this.lbApprSizeVal.AutoSize = true;
            this.lbApprSizeVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbApprSizeVal.Location = new System.Drawing.Point(339, 414);
            this.lbApprSizeVal.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.lbApprSizeVal.Name = "lbApprSizeVal";
            this.lbApprSizeVal.Size = new System.Drawing.Size(15, 16);
            this.lbApprSizeVal.TabIndex = 24;
            this.lbApprSizeVal.Text = "0";
            // 
            // tbAvgAdaptEngineOffTimeInterval
            // 
            this.tbAvgAdaptEngineOffTimeInterval.Enabled = false;
            this.tbAvgAdaptEngineOffTimeInterval.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbAvgAdaptEngineOffTimeInterval.Location = new System.Drawing.Point(342, 205);
            this.tbAvgAdaptEngineOffTimeInterval.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.tbAvgAdaptEngineOffTimeInterval.Name = "tbAvgAdaptEngineOffTimeInterval";
            this.tbAvgAdaptEngineOffTimeInterval.Size = new System.Drawing.Size(100, 22);
            this.tbAvgAdaptEngineOffTimeInterval.TabIndex = 5;
            // 
            // cbApprType
            // 
            this.cbApprType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbApprType.Enabled = false;
            this.cbApprType.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.cbApprType.FormattingEnabled = true;
            this.cbApprType.Location = new System.Drawing.Point(342, 359);
            this.cbApprType.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.cbApprType.Name = "cbApprType";
            this.cbApprType.Size = new System.Drawing.Size(121, 24);
            this.cbApprType.TabIndex = 22;
            this.cbApprType.TextChanged += new System.EventHandler(this.ChangeAppType);
            // 
            // lbEngineStateVal
            // 
            this.lbEngineStateVal.AutoSize = true;
            this.lbEngineStateVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbEngineStateVal.Location = new System.Drawing.Point(339, 130);
            this.lbEngineStateVal.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbEngineStateVal.Name = "lbEngineStateVal";
            this.lbEngineStateVal.Size = new System.Drawing.Size(15, 16);
            this.lbEngineStateVal.TabIndex = 2;
            this.lbEngineStateVal.Text = "?";
            // 
            // lbEngineState
            // 
            this.lbEngineState.AutoSize = true;
            this.lbEngineState.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbEngineState.Location = new System.Drawing.Point(4, 130);
            this.lbEngineState.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbEngineState.Name = "lbEngineState";
            this.lbEngineState.Size = new System.Drawing.Size(149, 16);
            this.lbEngineState.TabIndex = 2;
            this.lbEngineState.Text = "Состояние двигателя";
            // 
            // lbApprSize
            // 
            this.lbApprSize.AutoSize = true;
            this.lbApprSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbApprSize.Location = new System.Drawing.Point(4, 414);
            this.lbApprSize.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbApprSize.Name = "lbApprSize";
            this.lbApprSize.Size = new System.Drawing.Size(195, 16);
            this.lbApprSize.TabIndex = 23;
            this.lbApprSize.Text = "Число точек аппроксимации";
            // 
            // lbDeltaPowerSupply
            // 
            this.lbDeltaPowerSupply.AutoSize = true;
            this.lbDeltaPowerSupply.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbDeltaPowerSupply.Location = new System.Drawing.Point(4, 156);
            this.lbDeltaPowerSupply.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbDeltaPowerSupply.Name = "lbDeltaPowerSupply";
            this.lbDeltaPowerSupply.Size = new System.Drawing.Size(262, 16);
            this.lbDeltaPowerSupply.TabIndex = 2;
            this.lbDeltaPowerSupply.Text = "Перепад напряжения бортовой сети, В";
            // 
            // lbApprType
            // 
            this.lbApprType.AutoSize = true;
            this.lbApprType.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbApprType.Location = new System.Drawing.Point(4, 362);
            this.lbApprType.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbApprType.Name = "lbApprType";
            this.lbApprType.Size = new System.Drawing.Size(139, 16);
            this.lbApprType.TabIndex = 21;
            this.lbApprType.Text = "Тип аппроксимации";
            // 
            // lbPowerSupplyVal
            // 
            this.lbPowerSupplyVal.AutoSize = true;
            this.lbPowerSupplyVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbPowerSupplyVal.Location = new System.Drawing.Point(339, 52);
            this.lbPowerSupplyVal.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.lbPowerSupplyVal.Name = "lbPowerSupplyVal";
            this.lbPowerSupplyVal.Size = new System.Drawing.Size(25, 16);
            this.lbPowerSupplyVal.TabIndex = 16;
            this.lbPowerSupplyVal.Text = "0.0";
            // 
            // cbTempSensorOn
            // 
            this.cbTempSensorOn.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbTempSensorOn.Enabled = false;
            this.cbTempSensorOn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.cbTempSensorOn.FormattingEnabled = true;
            this.cbTempSensorOn.Location = new System.Drawing.Point(342, 385);
            this.cbTempSensorOn.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.cbTempSensorOn.Name = "cbTempSensorOn";
            this.cbTempSensorOn.Size = new System.Drawing.Size(121, 24);
            this.cbTempSensorOn.TabIndex = 20;
            // 
            // lbAvgAdaptEngineOnTimeInterval
            // 
            this.lbAvgAdaptEngineOnTimeInterval.AutoSize = true;
            this.lbAvgAdaptEngineOnTimeInterval.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbAvgAdaptEngineOnTimeInterval.Location = new System.Drawing.Point(4, 182);
            this.lbAvgAdaptEngineOnTimeInterval.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbAvgAdaptEngineOnTimeInterval.Name = "lbAvgAdaptEngineOnTimeInterval";
            this.lbAvgAdaptEngineOnTimeInterval.Size = new System.Drawing.Size(292, 16);
            this.lbAvgAdaptEngineOnTimeInterval.TabIndex = 2;
            this.lbAvgAdaptEngineOnTimeInterval.Text = "Интервал уср. с работающим двигателем, с";
            // 
            // lbInnerCoreVal
            // 
            this.lbInnerCoreVal.AutoSize = true;
            this.lbInnerCoreVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbInnerCoreVal.Location = new System.Drawing.Point(339, 310);
            this.lbInnerCoreVal.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.lbInnerCoreVal.Name = "lbInnerCoreVal";
            this.lbInnerCoreVal.Size = new System.Drawing.Size(25, 16);
            this.lbInnerCoreVal.TabIndex = 17;
            this.lbInnerCoreVal.Text = "0.0";
            // 
            // lbAvgAdaptEngineOffTimeInterval
            // 
            this.lbAvgAdaptEngineOffTimeInterval.AutoSize = true;
            this.lbAvgAdaptEngineOffTimeInterval.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbAvgAdaptEngineOffTimeInterval.Location = new System.Drawing.Point(4, 208);
            this.lbAvgAdaptEngineOffTimeInterval.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbAvgAdaptEngineOffTimeInterval.Name = "lbAvgAdaptEngineOffTimeInterval";
            this.lbAvgAdaptEngineOffTimeInterval.Size = new System.Drawing.Size(298, 16);
            this.lbAvgAdaptEngineOffTimeInterval.TabIndex = 2;
            this.lbAvgAdaptEngineOffTimeInterval.Text = "Интервал уср. с заглушенным двигателем, с";
            // 
            // tbAvgTimeInterval
            // 
            this.tbAvgTimeInterval.Enabled = false;
            this.tbAvgTimeInterval.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbAvgTimeInterval.Location = new System.Drawing.Point(342, 333);
            this.tbAvgTimeInterval.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.tbAvgTimeInterval.Name = "tbAvgTimeInterval";
            this.tbAvgTimeInterval.Size = new System.Drawing.Size(100, 22);
            this.tbAvgTimeInterval.TabIndex = 5;
            this.tbAvgTimeInterval.Leave += new System.EventHandler(this.tbAvgTimeInterval_Leave);
            // 
            // lbTempSensorOn
            // 
            this.lbTempSensorOn.AutoSize = true;
            this.lbTempSensorOn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbTempSensorOn.Location = new System.Drawing.Point(4, 388);
            this.lbTempSensorOn.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbTempSensorOn.Name = "lbTempSensorOn";
            this.lbTempSensorOn.Size = new System.Drawing.Size(136, 16);
            this.lbTempSensorOn.TabIndex = 19;
            this.lbTempSensorOn.Text = "Термокомпенсация";
            // 
            // lbSensorReadingsVal
            // 
            this.lbSensorReadingsVal.AutoSize = true;
            this.lbSensorReadingsVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbSensorReadingsVal.Location = new System.Drawing.Point(339, 286);
            this.lbSensorReadingsVal.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.lbSensorReadingsVal.Name = "lbSensorReadingsVal";
            this.lbSensorReadingsVal.Size = new System.Drawing.Size(25, 16);
            this.lbSensorReadingsVal.TabIndex = 3;
            this.lbSensorReadingsVal.Text = "0.0";
            // 
            // lbPowerSupply
            // 
            this.lbPowerSupply.AutoSize = true;
            this.lbPowerSupply.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbPowerSupply.Location = new System.Drawing.Point(4, 52);
            this.lbPowerSupply.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbPowerSupply.Name = "lbPowerSupply";
            this.lbPowerSupply.Size = new System.Drawing.Size(162, 16);
            this.lbPowerSupply.TabIndex = 3;
            this.lbPowerSupply.Text = "Напряжение питания, В";
            // 
            // lbSensorReadings
            // 
            this.lbSensorReadings.AutoSize = true;
            this.lbSensorReadings.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbSensorReadings.Location = new System.Drawing.Point(4, 286);
            this.lbSensorReadings.Margin = new System.Windows.Forms.Padding(1, 5, 0, 5);
            this.lbSensorReadings.Name = "lbSensorReadings";
            this.lbSensorReadings.Size = new System.Drawing.Size(151, 16);
            this.lbSensorReadings.TabIndex = 3;
            this.lbSensorReadings.Text = "Показания датчика, л";
            // 
            // lbInnerCore
            // 
            this.lbInnerCore.AutoSize = true;
            this.lbInnerCore.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbInnerCore.Location = new System.Drawing.Point(4, 310);
            this.lbInnerCore.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbInnerCore.Name = "lbInnerCore";
            this.lbInnerCore.Size = new System.Drawing.Size(281, 16);
            this.lbInnerCore.TabIndex = 18;
            this.lbInnerCore.Text = "Мгн. частота внутреннего генератора, Гц";
            // 
            // lbAvgTimeInterval
            // 
            this.lbAvgTimeInterval.AutoSize = true;
            this.lbAvgTimeInterval.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbAvgTimeInterval.Location = new System.Drawing.Point(4, 336);
            this.lbAvgTimeInterval.Margin = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.lbAvgTimeInterval.Name = "lbAvgTimeInterval";
            this.lbAvgTimeInterval.Size = new System.Drawing.Size(167, 16);
            this.lbAvgTimeInterval.TabIndex = 0;
            this.lbAvgTimeInterval.Text = "Интервал усреднения, с";
            // 
            // lbSWVersionVal
            // 
            this.lbSWVersionVal.AutoSize = true;
            this.lbSWVersionVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbSWVersionVal.Location = new System.Drawing.Point(127, 77);
            this.lbSWVersionVal.Name = "lbSWVersionVal";
            this.lbSWVersionVal.Size = new System.Drawing.Size(0, 16);
            this.lbSWVersionVal.TabIndex = 8;
            // 
            // btEdit
            // 
            this.btEdit.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.fpEdit.SetFlowBreak(this.btEdit, true);
            this.btEdit.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btEdit.Location = new System.Drawing.Point(0, 0);
            this.btEdit.Margin = new System.Windows.Forms.Padding(0);
            this.btEdit.Name = "btEdit";
            this.btEdit.Size = new System.Drawing.Size(117, 37);
            this.btEdit.TabIndex = 12;
            this.btEdit.Text = "Настроить";
            this.btEdit.UseVisualStyleBackColor = true;
            this.btEdit.Click += new System.EventHandler(this.btEdit_Click);
            // 
            // fpEdit
            // 
            this.fpEdit.Controls.Add(this.btEdit);
            this.fpEdit.Controls.Add(this.btEditOk);
            this.fpEdit.Controls.Add(this.btEditCancel);
            this.fpEdit.Location = new System.Drawing.Point(366, 648);
            this.fpEdit.Margin = new System.Windows.Forms.Padding(0);
            this.fpEdit.Name = "fpEdit";
            this.fpEdit.Size = new System.Drawing.Size(119, 38);
            this.fpEdit.TabIndex = 13;
            // 
            // btEditOk
            // 
            this.btEditOk.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btEditOk.Location = new System.Drawing.Point(0, 37);
            this.btEditOk.Margin = new System.Windows.Forms.Padding(0);
            this.btEditOk.Name = "btEditOk";
            this.btEditOk.Size = new System.Drawing.Size(42, 37);
            this.btEditOk.TabIndex = 12;
            this.btEditOk.Text = "OK";
            this.btEditOk.UseVisualStyleBackColor = true;
            this.btEditOk.Visible = false;
            this.btEditOk.Click += new System.EventHandler(this.btEditOk_Click);
            // 
            // btEditCancel
            // 
            this.btEditCancel.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btEditCancel.Location = new System.Drawing.Point(42, 37);
            this.btEditCancel.Margin = new System.Windows.Forms.Padding(0);
            this.btEditCancel.Name = "btEditCancel";
            this.btEditCancel.Size = new System.Drawing.Size(75, 37);
            this.btEditCancel.TabIndex = 12;
            this.btEditCancel.Text = "Отмена";
            this.btEditCancel.UseVisualStyleBackColor = true;
            this.btEditCancel.Visible = false;
            this.btEditCancel.Click += new System.EventHandler(this.btEditCancel_Click);
            // 
            // dgvTarTable
            // 
            this.dgvTarTable.AutoGenerateColumns = false;
            this.dgvTarTable.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvTarTable.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.xValDataGridViewTextBoxColumn,
            this.yValDataGridViewTextBoxColumn});
            this.dgvTarTable.DataMember = "ApproxTable";
            this.dgvTarTable.DataSource = this.readingBinding;
            this.dgvTarTable.Enabled = false;
            this.dgvTarTable.Location = new System.Drawing.Point(491, 77);
            this.dgvTarTable.Name = "dgvTarTable";
            this.dgvTarTable.Size = new System.Drawing.Size(395, 200);
            this.dgvTarTable.TabIndex = 10;
            this.dgvTarTable.Visible = false;
            this.dgvTarTable.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.dgvTarTable_CellEndEdit);
            this.dgvTarTable.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler(this.dgvTarTable_RowsAdded);
            this.dgvTarTable.UserDeletedRow += new System.Windows.Forms.DataGridViewRowEventHandler(this.dgvTarTable_UserDeletedRow);
            // 
            // xValDataGridViewTextBoxColumn
            // 
            this.xValDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.xValDataGridViewTextBoxColumn.DataPropertyName = "XVal";
            this.xValDataGridViewTextBoxColumn.HeaderText = "Уровеь погружения, %";
            this.xValDataGridViewTextBoxColumn.Name = "xValDataGridViewTextBoxColumn";
            // 
            // yValDataGridViewTextBoxColumn
            // 
            this.yValDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.yValDataGridViewTextBoxColumn.DataPropertyName = "YVal";
            this.yValDataGridViewTextBoxColumn.HeaderText = "Объем топлива, л";
            this.yValDataGridViewTextBoxColumn.Name = "yValDataGridViewTextBoxColumn";
            // 
            // readingBinding
            // 
            this.readingBinding.DataSource = typeof(DUTConfig_V3.Protocol.ModbusRegister);
            // 
            // backgroundWriter
            // 
            this.backgroundWriter.WorkerReportsProgress = true;
            this.backgroundWriter.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWriter_DoWork);
            this.backgroundWriter.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.backgroundWriter_ProgressChanged);
            this.backgroundWriter.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWriter_RunWorkerCompleted);
            // 
            // btTableClear
            // 
            this.btTableClear.Enabled = false;
            this.btTableClear.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btTableClear.Location = new System.Drawing.Point(491, 281);
            this.btTableClear.Margin = new System.Windows.Forms.Padding(0);
            this.btTableClear.Name = "btTableClear";
            this.btTableClear.Size = new System.Drawing.Size(115, 30);
            this.btTableClear.TabIndex = 12;
            this.btTableClear.Text = "Очистить";
            this.btTableClear.UseVisualStyleBackColor = true;
            this.btTableClear.Visible = false;
            this.btTableClear.Click += new System.EventHandler(this.btTableClear_Click);
            // 
            // btTankEmpty
            // 
            this.btTankEmpty.Enabled = false;
            this.btTankEmpty.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btTankEmpty.Location = new System.Drawing.Point(631, 280);
            this.btTankEmpty.Margin = new System.Windows.Forms.Padding(0);
            this.btTankEmpty.Name = "btTankEmpty";
            this.btTankEmpty.Size = new System.Drawing.Size(115, 30);
            this.btTankEmpty.TabIndex = 12;
            this.btTankEmpty.Text = "Пустой бак";
            this.btTankEmpty.UseVisualStyleBackColor = true;
            this.btTankEmpty.Click += new System.EventHandler(this.btTankEmpty_Click);
            // 
            // btTankFull
            // 
            this.btTankFull.Enabled = false;
            this.btTankFull.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btTankFull.Location = new System.Drawing.Point(771, 280);
            this.btTankFull.Margin = new System.Windows.Forms.Padding(0);
            this.btTankFull.Name = "btTankFull";
            this.btTankFull.Size = new System.Drawing.Size(115, 30);
            this.btTankFull.TabIndex = 12;
            this.btTankFull.Text = "Полный бак";
            this.btTankFull.UseVisualStyleBackColor = true;
            this.btTankFull.Click += new System.EventHandler(this.btTankFull_Click);
            // 
            // pbPolyEquation
            // 
            this.pbPolyEquation.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pbPolyEquation.BackgroundImage")));
            this.pbPolyEquation.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pbPolyEquation.ErrorImage = null;
            this.pbPolyEquation.InitialImage = null;
            this.pbPolyEquation.Location = new System.Drawing.Point(6, 19);
            this.pbPolyEquation.Name = "pbPolyEquation";
            this.pbPolyEquation.Size = new System.Drawing.Size(375, 37);
            this.pbPolyEquation.TabIndex = 11;
            this.pbPolyEquation.TabStop = false;
            // 
            // tbPolyCoeffA
            // 
            this.tbPolyCoeffA.Location = new System.Drawing.Point(33, 64);
            this.tbPolyCoeffA.Name = "tbPolyCoeffA";
            this.tbPolyCoeffA.Size = new System.Drawing.Size(135, 22);
            this.tbPolyCoeffA.TabIndex = 12;
            // 
            // tbPolyCoeffB
            // 
            this.tbPolyCoeffB.Location = new System.Drawing.Point(246, 64);
            this.tbPolyCoeffB.Name = "tbPolyCoeffB";
            this.tbPolyCoeffB.Size = new System.Drawing.Size(135, 22);
            this.tbPolyCoeffB.TabIndex = 12;
            // 
            // tbPolyCoeffC
            // 
            this.tbPolyCoeffC.Location = new System.Drawing.Point(33, 92);
            this.tbPolyCoeffC.Name = "tbPolyCoeffC";
            this.tbPolyCoeffC.Size = new System.Drawing.Size(135, 22);
            this.tbPolyCoeffC.TabIndex = 12;
            // 
            // tbPolyCoeffD
            // 
            this.tbPolyCoeffD.Location = new System.Drawing.Point(246, 92);
            this.tbPolyCoeffD.Name = "tbPolyCoeffD";
            this.tbPolyCoeffD.Size = new System.Drawing.Size(135, 22);
            this.tbPolyCoeffD.TabIndex = 12;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(3, 67);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(24, 16);
            this.label1.TabIndex = 13;
            this.label1.Text = "A=";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label2.Location = new System.Drawing.Point(217, 67);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(24, 16);
            this.label2.TabIndex = 13;
            this.label2.Text = "B=";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label3.Location = new System.Drawing.Point(4, 95);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(24, 16);
            this.label3.TabIndex = 13;
            this.label3.Text = "C=";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label4.Location = new System.Drawing.Point(217, 95);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(25, 16);
            this.label4.TabIndex = 13;
            this.label4.Text = "D=";
            // 
            // gbPoly
            // 
            this.gbPoly.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.gbPoly.Controls.Add(this.label4);
            this.gbPoly.Controls.Add(this.label3);
            this.gbPoly.Controls.Add(this.label2);
            this.gbPoly.Controls.Add(this.label1);
            this.gbPoly.Controls.Add(this.tbPolyCoeffD);
            this.gbPoly.Controls.Add(this.tbPolyCoeffC);
            this.gbPoly.Controls.Add(this.tbPolyCoeffB);
            this.gbPoly.Controls.Add(this.tbPolyCoeffA);
            this.gbPoly.Controls.Add(this.pbPolyEquation);
            this.gbPoly.Enabled = false;
            this.gbPoly.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.gbPoly.Location = new System.Drawing.Point(491, 565);
            this.gbPoly.Name = "gbPoly";
            this.gbPoly.Size = new System.Drawing.Size(393, 121);
            this.gbPoly.TabIndex = 15;
            this.gbPoly.TabStop = false;
            this.gbPoly.Text = "Полином";
            this.gbPoly.Visible = false;
            // 
            // indicatorWorker
            // 
            this.indicatorWorker.WorkerSupportsCancellation = true;
            this.indicatorWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.indicatorWorker_DoWork);
            this.indicatorWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.indicatorWorker_RunWorkerCompleted);
            // 
            // lbErrorCode
            // 
            this.lbErrorCode.AutoSize = true;
            this.lbErrorCode.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbErrorCode.Location = new System.Drawing.Point(12, 99);
            this.lbErrorCode.Name = "lbErrorCode";
            this.lbErrorCode.Size = new System.Drawing.Size(83, 16);
            this.lbErrorCode.TabIndex = 8;
            this.lbErrorCode.Text = "Код ошибки";
            // 
            // lbErrorCodeVal
            // 
            this.lbErrorCodeVal.AutoSize = true;
            this.lbErrorCodeVal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbErrorCodeVal.Location = new System.Drawing.Point(127, 99);
            this.lbErrorCodeVal.Name = "lbErrorCodeVal";
            this.lbErrorCodeVal.Size = new System.Drawing.Size(99, 16);
            this.lbErrorCodeVal.TabIndex = 8;
            this.lbErrorCodeVal.Text = "0 - ошибок нет";
            // 
            // lbFreqRange
            // 
            this.lbFreqRange.AutoSize = true;
            this.lbFreqRange.Location = new System.Drawing.Point(7, 18);
            this.lbFreqRange.Name = "lbFreqRange";
            this.lbFreqRange.Size = new System.Drawing.Size(224, 16);
            this.lbFreqRange.TabIndex = 0;
            this.lbFreqRange.Text = "Диапазон генерации частоты, Гц";
            // 
            // tbFreqRange
            // 
            this.tbFreqRange.Enabled = false;
            this.tbFreqRange.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbFreqRange.Location = new System.Drawing.Point(361, 15);
            this.tbFreqRange.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.tbFreqRange.Name = "tbFreqRange";
            this.tbFreqRange.Size = new System.Drawing.Size(100, 22);
            this.tbFreqRange.TabIndex = 1;
            // 
            // coeffsBinding
            // 
            this.coeffsBinding.DataSource = typeof(DUTConfig_V3.Protocol.ModbusRegister.PolyCoefficients);
            // 
            // canvas
            // 
            this.canvas.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.canvas.Color1 = System.Drawing.Color.Green;
            this.canvas.Color2 = System.Drawing.Color.Red;
            this.canvas.Location = new System.Drawing.Point(491, 314);
            this.canvas.Name = "canvas";
            this.canvas.NodeSymbol1 = ZedGraph.SymbolType.Circle;
            this.canvas.NodeSymbol2 = ZedGraph.SymbolType.Circle;
            this.canvas.Size = new System.Drawing.Size(393, 245);
            this.canvas.TabIndex = 16;
            this.canvas.Title = "";
            this.canvas.UseSmoothing = false;
            this.canvas.Width1 = 2;
            this.canvas.Width2 = 3;
            this.canvas.XAxisLabel = "";
            this.canvas.YAxisLabel = "";
            // 
            // InterfaceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(894, 692);
            this.Controls.Add(this.canvas);
            this.Controls.Add(this.btTankFull);
            this.Controls.Add(this.btTankEmpty);
            this.Controls.Add(this.btTableClear);
            this.Controls.Add(this.gbPoly);
            this.Controls.Add(this.fpEdit);
            this.Controls.Add(this.dgvTarTable);
            this.Controls.Add(this.gbParams);
            this.Controls.Add(this.lbSWVersionVal);
            this.Controls.Add(this.lbErrorCodeVal);
            this.Controls.Add(this.lbErrorCode);
            this.Controls.Add(this.lbSWVersion);
            this.Controls.Add(this.gbOmnicomm);
            this.Controls.Add(this.toolStrip);
            this.Controls.Add(this.menuStrip);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "InterfaceForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.InterfaceForm_FormClosing);
            this.SizeChanged += new System.EventHandler(this.InterfaceForm_Resize);
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            this.toolStrip.ResumeLayout(false);
            this.toolStrip.PerformLayout();
            this.fpEdit.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgvTarTable)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.readingBinding)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbPolyEquation)).EndInit();
            this.gbPoly.ResumeLayout(false);
            this.gbPoly.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.coeffsBinding)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion



        private System.Windows.Forms.ToolStrip menuStrip;
        private System.Windows.Forms.ToolStrip toolStrip;
        private System.Windows.Forms.ToolStripButton btBackToMain;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripLabel lbPort;
        private System.Windows.Forms.ToolStripComboBox cbPort;
        private System.Windows.Forms.ToolStripLabel lbBaudrate;
        private System.Windows.Forms.ToolStripComboBox cbBaud;
        private System.Windows.Forms.ToolStripLabel lbModbusID;
        private System.Windows.Forms.ToolStripTextBox tbID;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton btConnect;
        private System.Windows.Forms.ToolStripDropDownButton miFile;
        private System.Windows.Forms.ToolStripDropDownButton miView;
        private System.Windows.Forms.ToolStripMenuItem miViewStandart;
        private System.Windows.Forms.ToolStripMenuItem miViewExtended;
        private System.Windows.Forms.GroupBox gbOmnicomm;
        private System.Windows.Forms.ToolStripMenuItem miFileSave;
        private System.Windows.Forms.ToolStripMenuItem miFileLoad;
        private System.Windows.Forms.ToolStripSeparator tsSeparator;
        private System.Windows.Forms.ToolStripButton miChangeFirmware;
        private System.Windows.Forms.ToolStripButton miThermo;
        private System.Windows.Forms.Label lbEmptyTankFreq;
        private System.Windows.Forms.Label lbTemp;
        private System.Windows.Forms.Label lbFullTankFreq;
        private System.Windows.Forms.Label lbInnerCoreAvg;
        private System.Windows.Forms.Label lbImmersionLevel;
        private System.Windows.Forms.Label lbImmersionLevelVal;
        private System.Windows.Forms.Label lbInnerCoreAvgVal;
        private System.Windows.Forms.Label lbTempVal;
        private System.Windows.Forms.TextBox tbFullTankFreq;
        private System.Windows.Forms.TextBox tbEmptyTankFreq;
        private System.ComponentModel.BackgroundWorker backgroundReader;
        private System.Windows.Forms.Label lbSWVersion;
        private System.Windows.Forms.Label lbOmniAutoOn;
        private System.Windows.Forms.Label lbOmniAutoPeriod;
        private System.Windows.Forms.Label lbOmniNetMode;
        private System.Windows.Forms.Label lbOmniCurrN;
        private System.Windows.Forms.TextBox tbOmniAutoPeriod;
        private System.Windows.Forms.ComboBox cbOmniNetMode;
        private System.Windows.Forms.GroupBox gbParams;
        private System.Windows.Forms.Label lbOmniCurrNVal;
        private System.Windows.Forms.Label lbSWVersionVal;
        private System.Windows.Forms.BindingSource readingBinding;
        private System.Windows.Forms.Button btEdit;
        private System.Windows.Forms.FlowLayoutPanel fpEdit;
        private System.Windows.Forms.Button btEditOk;
        private System.Windows.Forms.Button btEditCancel;
        private System.Windows.Forms.Label lbDeltaPowerSupply;
        private System.Windows.Forms.Label lbAvgAdaptEngineOnTimeInterval;
        private System.Windows.Forms.Label lbAvgAdaptEngineOffTimeInterval;
        private System.Windows.Forms.TextBox tbDeltaPowerSupply;
        private System.Windows.Forms.TextBox tbAvgAdaptEngineOnTimeInterval;
        private System.Windows.Forms.TextBox tbAvgAdaptEngineOffTimeInterval;
        private System.Windows.Forms.Label lbSensorReadings;
        private System.Windows.Forms.Label lbAvgTimeInterval;
        private System.Windows.Forms.Label lbSensorReadingsVal;
        private System.Windows.Forms.TextBox tbAvgTimeInterval;
        private System.Windows.Forms.DataGridView dgvTarTable;
        private System.Windows.Forms.DataGridViewTextBoxColumn xValDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn yValDataGridViewTextBoxColumn;
        private System.Windows.Forms.ToolStripProgressBar progressBar;
        private System.ComponentModel.BackgroundWorker backgroundWriter;
        private System.Windows.Forms.BindingSource coeffsBinding;
        private System.Windows.Forms.Button btTableClear;
        private System.Windows.Forms.Button btTankEmpty;
        private System.Windows.Forms.Button btTankFull;
        private System.Windows.Forms.PictureBox pbPolyEquation;
        private System.Windows.Forms.TextBox tbPolyCoeffA;
        private System.Windows.Forms.TextBox tbPolyCoeffB;
        private System.Windows.Forms.TextBox tbPolyCoeffC;
        private System.Windows.Forms.TextBox tbPolyCoeffD;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox gbPoly;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripLabel lbConnectStatus;
        private System.ComponentModel.BackgroundWorker indicatorWorker;
        private System.Windows.Forms.Label lbErrorCode;
        private System.Windows.Forms.Label lbErrorCodeVal;
        private System.Windows.Forms.Label lbPowerSupply;
        private System.Windows.Forms.Label lbPowerSupplyVal;
        private System.Windows.Forms.ComboBox cbOmniAutoOn;
        private System.Windows.Forms.ComboBox cbTempSensorOn;
        private System.Windows.Forms.Label lbTempSensorOn;
        private System.Windows.Forms.Label lbInnerCoreVal;
        private System.Windows.Forms.Label lbInnerCore;
        private System.Windows.Forms.TextBox tbOmniMaxN;
        private System.Windows.Forms.Label lbOmniMaxN;
        private System.Windows.Forms.ComboBox cbAvgType;
        private System.Windows.Forms.Label lbAvgType;
        private System.Windows.Forms.Label lbApprSizeVal;
        private System.Windows.Forms.Label lbApprSize;
        private System.Windows.Forms.ComboBox cbApprType;
        private System.Windows.Forms.Label lbApprType;
        private System.Windows.Forms.Label lbEngineStateVal;
        private System.Windows.Forms.Label lbEngineState;
        private System.Windows.Forms.Label lbFreqRange;
        private System.Windows.Forms.TextBox tbFreqRange;
        private GraphControl canvas;
        private System.Windows.Forms.ToolStripButton btTestButton;
    }
}

