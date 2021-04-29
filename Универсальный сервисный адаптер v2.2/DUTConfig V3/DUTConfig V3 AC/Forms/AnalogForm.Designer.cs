namespace DUTConfig_V3.Forms
{
    partial class AnalogForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AnalogForm));
            this.lbValueType = new System.Windows.Forms.Label();
            this.lbValue = new System.Windows.Forms.Label();
            this.toolStrip = new System.Windows.Forms.ToolStrip();
            this.btBackToMain = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.lbPort = new System.Windows.Forms.ToolStripLabel();
            this.cbPort = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.btConnect = new System.Windows.Forms.ToolStripButton();
            this.lbConnectStatus = new System.Windows.Forms.ToolStripLabel();
            this.backgroundReader = new System.ComponentModel.BackgroundWorker();
            this.indicatorWorker = new System.ComponentModel.BackgroundWorker();
            this.btCalib = new System.Windows.Forms.Button();
            this.calibWorker = new System.ComponentModel.BackgroundWorker();
            this.lbCalibMessages = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.lbCurrent = new System.Windows.Forms.Label();
            this.toolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // lbValueType
            // 
            this.lbValueType.Font = new System.Drawing.Font("Segoe Print", 60F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbValueType.Location = new System.Drawing.Point(11, 57);
            this.lbValueType.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lbValueType.Name = "lbValueType";
            this.lbValueType.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.lbValueType.Size = new System.Drawing.Size(762, 140);
            this.lbValueType.TabIndex = 0;
            this.lbValueType.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lbValue
            // 
            this.lbValue.Font = new System.Drawing.Font("Segoe Print", 60F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbValue.Location = new System.Drawing.Point(11, 197);
            this.lbValue.Margin = new System.Windows.Forms.Padding(0);
            this.lbValue.Name = "lbValue";
            this.lbValue.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.lbValue.Size = new System.Drawing.Size(762, 116);
            this.lbValue.TabIndex = 0;
            this.lbValue.Text = "0.00";
            this.lbValue.TextAlign = System.Drawing.ContentAlignment.TopCenter;
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
            this.toolStripSeparator2,
            this.btConnect,
            this.lbConnectStatus});
            this.toolStrip.Location = new System.Drawing.Point(0, 0);
            this.toolStrip.Name = "toolStrip";
            this.toolStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.toolStrip.Size = new System.Drawing.Size(784, 45);
            this.toolStrip.TabIndex = 1;
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
            // lbConnectStatus
            // 
            this.lbConnectStatus.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.lbConnectStatus.BackColor = System.Drawing.Color.LimeGreen;
            this.lbConnectStatus.ForeColor = System.Drawing.SystemColors.ControlText;
            this.lbConnectStatus.Name = "lbConnectStatus";
            this.lbConnectStatus.Size = new System.Drawing.Size(0, 42);
            // 
            // backgroundReader
            // 
            this.backgroundReader.WorkerSupportsCancellation = true;
            this.backgroundReader.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundReader_DoWork);
            this.backgroundReader.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundReader_RunWorkerCompleted);
            // 
            // indicatorWorker
            // 
            this.indicatorWorker.WorkerSupportsCancellation = true;
            this.indicatorWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.indicatorWorker_DoWork);
            this.indicatorWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.indicatorWorker_RunWorkerCompleted);
            // 
            // btCalib
            // 
            this.btCalib.BackColor = System.Drawing.Color.LimeGreen;
            this.btCalib.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btCalib.Font = new System.Drawing.Font("Segoe UI", 9.75F);
            this.btCalib.Location = new System.Drawing.Point(661, 358);
            this.btCalib.Name = "btCalib";
            this.btCalib.Size = new System.Drawing.Size(111, 42);
            this.btCalib.TabIndex = 2;
            this.btCalib.Text = "Калибровать";
            this.btCalib.UseVisualStyleBackColor = false;
            this.btCalib.Click += new System.EventHandler(this.btCalib_Click);
            // 
            // calibWorker
            // 
            this.calibWorker.WorkerSupportsCancellation = true;
            this.calibWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.calibWorker_DoWork);
            this.calibWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.calibWorker_RunWorkerCompleted);
            // 
            // lbCalibMessages
            // 
            this.lbCalibMessages.AutoSize = true;
            this.lbCalibMessages.Font = new System.Drawing.Font("Segoe UI", 20.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbCalibMessages.Location = new System.Drawing.Point(28, 360);
            this.lbCalibMessages.Name = "lbCalibMessages";
            this.lbCalibMessages.Size = new System.Drawing.Size(0, 37);
            this.lbCalibMessages.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Segoe Print", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(29, 57);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(262, 36);
            this.label1.TabIndex = 4;
            this.label1.Text = "Ток потребления, мА:";
            // 
            // lbCurrent
            // 
            this.lbCurrent.AutoSize = true;
            this.lbCurrent.Font = new System.Drawing.Font("Segoe Print", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbCurrent.Location = new System.Drawing.Point(297, 57);
            this.lbCurrent.Name = "lbCurrent";
            this.lbCurrent.Size = new System.Drawing.Size(30, 36);
            this.lbCurrent.TabIndex = 4;
            this.lbCurrent.Text = "0";
            // 
            // AnalogForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(5F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(784, 412);
            this.Controls.Add(this.lbCurrent);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lbCalibMessages);
            this.Controls.Add(this.btCalib);
            this.Controls.Add(this.toolStrip);
            this.Controls.Add(this.lbValue);
            this.Controls.Add(this.lbValueType);
            this.Font = new System.Drawing.Font("Arial Narrow", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2, 3, 2, 3);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(800, 450);
            this.MinimumSize = new System.Drawing.Size(800, 450);
            this.Name = "AnalogForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.AnalogForm_FormClosing);
            this.toolStrip.ResumeLayout(false);
            this.toolStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbValueType;
        private System.Windows.Forms.Label lbValue;
        private System.Windows.Forms.ToolStrip toolStrip;
        private System.Windows.Forms.ToolStripButton btBackToMain;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripLabel lbPort;
        private System.Windows.Forms.ToolStripComboBox cbPort;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton btConnect;
        private System.ComponentModel.BackgroundWorker backgroundReader;
        private System.Windows.Forms.ToolStripLabel lbConnectStatus;
        private System.ComponentModel.BackgroundWorker indicatorWorker;
        private System.Windows.Forms.Button btCalib;
        private System.ComponentModel.BackgroundWorker calibWorker;
        private System.Windows.Forms.Label lbCalibMessages;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lbCurrent;
    }
}