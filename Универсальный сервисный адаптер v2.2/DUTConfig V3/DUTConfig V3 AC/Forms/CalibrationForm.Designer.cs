namespace DUTConfig_V3.Forms
{
    partial class CalibrationForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CalibrationForm));
            this.toolStrip = new System.Windows.Forms.ToolStrip();
            this.btBackToMain = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.lbPort = new System.Windows.Forms.ToolStripLabel();
            this.cbPort = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.btConnect = new System.Windows.Forms.ToolStripButton();
            this.lbConnectStatus = new System.Windows.Forms.ToolStripLabel();
            this.lbCalibMessages = new System.Windows.Forms.ListBox();
            this.calibWorker = new System.ComponentModel.BackgroundWorker();
            this.indicatorWorker = new System.ComponentModel.BackgroundWorker();
            this.toolStrip.SuspendLayout();
            this.SuspendLayout();
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
            this.toolStrip.Size = new System.Drawing.Size(624, 45);
            this.toolStrip.TabIndex = 2;
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
            this.btConnect.Size = new System.Drawing.Size(91, 42);
            this.btConnect.Tag = "";
            this.btConnect.Text = "Калибровать";
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
            // lbCalibMessages
            // 
            this.lbCalibMessages.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lbCalibMessages.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbCalibMessages.FormattingEnabled = true;
            this.lbCalibMessages.ItemHeight = 16;
            this.lbCalibMessages.Location = new System.Drawing.Point(12, 48);
            this.lbCalibMessages.Name = "lbCalibMessages";
            this.lbCalibMessages.Size = new System.Drawing.Size(600, 388);
            this.lbCalibMessages.TabIndex = 3;
            // 
            // calibWorker
            // 
            this.calibWorker.WorkerSupportsCancellation = true;
            this.calibWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.calibWorker_DoWork);
            this.calibWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.calibWorker_RunWorkerCompleted);
            // 
            // indicatorWorker
            // 
            this.indicatorWorker.WorkerSupportsCancellation = true;
            this.indicatorWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.indicatorWorker_DoWork);
            this.indicatorWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.indicatorWorker_RunWorkerCompleted);
            // 
            // CalibrationForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(624, 442);
            this.Controls.Add(this.lbCalibMessages);
            this.Controls.Add(this.toolStrip);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(640, 480);
            this.Name = "CalibrationForm";
            this.Text = "CalibrationForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.CalibrationForm_FormClosing);
            this.toolStrip.ResumeLayout(false);
            this.toolStrip.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip;
        private System.Windows.Forms.ToolStripButton btBackToMain;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripLabel lbPort;
        private System.Windows.Forms.ToolStripComboBox cbPort;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton btConnect;
        private System.Windows.Forms.ToolStripLabel lbConnectStatus;
        private System.Windows.Forms.ListBox lbCalibMessages;
        private System.ComponentModel.BackgroundWorker calibWorker;
        private System.ComponentModel.BackgroundWorker indicatorWorker;
    }
}