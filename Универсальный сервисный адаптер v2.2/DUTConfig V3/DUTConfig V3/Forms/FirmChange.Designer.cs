namespace DUTConfig_V3.Forms
{
    partial class FirmChange
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
            this.btDownloadFirm = new System.Windows.Forms.Button();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.btOpenFirmFile = new System.Windows.Forms.Button();
            this.lbFileName = new System.Windows.Forms.Label();
            this.backgroundLoader = new System.ComponentModel.BackgroundWorker();
            this.tbTout = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btDownloadFirm
            // 
            this.btDownloadFirm.Location = new System.Drawing.Point(163, 64);
            this.btDownloadFirm.Name = "btDownloadFirm";
            this.btDownloadFirm.Size = new System.Drawing.Size(108, 23);
            this.btDownloadFirm.TabIndex = 0;
            this.btDownloadFirm.Text = "Прошить";
            this.btDownloadFirm.UseVisualStyleBackColor = true;
            this.btDownloadFirm.Click += new System.EventHandler(this.btDownload_Click);
            // 
            // progressBar
            // 
            this.progressBar.Location = new System.Drawing.Point(12, 93);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(259, 23);
            this.progressBar.TabIndex = 1;
            // 
            // btOpenFirmFile
            // 
            this.btOpenFirmFile.Location = new System.Drawing.Point(12, 12);
            this.btOpenFirmFile.Name = "btOpenFirmFile";
            this.btOpenFirmFile.Size = new System.Drawing.Size(259, 23);
            this.btOpenFirmFile.TabIndex = 2;
            this.btOpenFirmFile.Text = "Открыть файл прошивки";
            this.btOpenFirmFile.UseVisualStyleBackColor = true;
            this.btOpenFirmFile.Click += new System.EventHandler(this.btOpenFile_Click);
            // 
            // lbFileName
            // 
            this.lbFileName.Location = new System.Drawing.Point(12, 38);
            this.lbFileName.Name = "lbFileName";
            this.lbFileName.Size = new System.Drawing.Size(259, 23);
            this.lbFileName.TabIndex = 3;
            this.lbFileName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // backgroundLoader
            // 
            this.backgroundLoader.WorkerReportsProgress = true;
            this.backgroundLoader.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundLoader_DoWork);
            this.backgroundLoader.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.backgroundLoader_ProgressChanged);
            this.backgroundLoader.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundLoader_RunWorkerCompleted);
            // 
            // tbTout
            // 
            this.tbTout.Location = new System.Drawing.Point(65, 67);
            this.tbTout.Name = "tbTout";
            this.tbTout.Size = new System.Drawing.Size(69, 20);
            this.tbTout.TabIndex = 4;
            this.tbTout.Text = "10 c";
            this.tbTout.Enter += new System.EventHandler(this.tbTout_Enter);
            this.tbTout.Leave += new System.EventHandler(this.tbTout_Leave);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 70);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(50, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Таймаут";
            // 
            // FirmChange
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(283, 123);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tbTout);
            this.Controls.Add(this.lbFileName);
            this.Controls.Add(this.btOpenFirmFile);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.btDownloadFirm);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "FirmChange";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Изменение прошивки";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FirmChange_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btDownloadFirm;
        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.Button btOpenFirmFile;
        private System.Windows.Forms.Label lbFileName;
        private System.ComponentModel.BackgroundWorker backgroundLoader;
        private System.Windows.Forms.TextBox tbTout;
        private System.Windows.Forms.Label label1;
    }
}