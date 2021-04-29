namespace DUTConfig_V3.Forms
{
    partial class MainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.btAnalogSensor = new System.Windows.Forms.Button();
            this.btFreqSensor = new System.Windows.Forms.Button();
            this.lbChooseSensor = new System.Windows.Forms.Label();
            this.toolStrip = new System.Windows.Forms.ToolStrip();
            this.ddbLanguage = new System.Windows.Forms.ToolStripDropDownButton();
            this.ddbCalibration = new System.Windows.Forms.ToolStripDropDownButton();
            this.miCalibAnalogSensor = new System.Windows.Forms.ToolStripMenuItem();
            this.miCalibFreqSensor = new System.Windows.Forms.ToolStripMenuItem();
            this.btInterfaceSensor = new System.Windows.Forms.Button();
            this.toolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // btAnalogSensor
            // 
            this.btAnalogSensor.BackColor = System.Drawing.Color.Gold;
            this.btAnalogSensor.Font = new System.Drawing.Font("Segoe UI", 14.25F);
            this.btAnalogSensor.Location = new System.Drawing.Point(209, 104);
            this.btAnalogSensor.Margin = new System.Windows.Forms.Padding(15);
            this.btAnalogSensor.Name = "btAnalogSensor";
            this.btAnalogSensor.Size = new System.Drawing.Size(155, 90);
            this.btAnalogSensor.TabIndex = 0;
            this.btAnalogSensor.Text = "Аналоговый датчик";
            this.btAnalogSensor.UseVisualStyleBackColor = false;
            this.btAnalogSensor.Click += new System.EventHandler(this.btAnalogSensor_Click);
            // 
            // btFreqSensor
            // 
            this.btFreqSensor.BackColor = System.Drawing.Color.Gold;
            this.btFreqSensor.Font = new System.Drawing.Font("Segoe UI", 14.25F);
            this.btFreqSensor.Location = new System.Drawing.Point(394, 104);
            this.btFreqSensor.Margin = new System.Windows.Forms.Padding(15);
            this.btFreqSensor.Name = "btFreqSensor";
            this.btFreqSensor.Size = new System.Drawing.Size(155, 90);
            this.btFreqSensor.TabIndex = 0;
            this.btFreqSensor.Text = "Частотный датчик";
            this.btFreqSensor.UseVisualStyleBackColor = false;
            this.btFreqSensor.Click += new System.EventHandler(this.btFreqSensor_Click);
            // 
            // lbChooseSensor
            // 
            this.lbChooseSensor.Font = new System.Drawing.Font("Segoe Print", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbChooseSensor.Location = new System.Drawing.Point(24, 50);
            this.lbChooseSensor.Name = "lbChooseSensor";
            this.lbChooseSensor.Size = new System.Drawing.Size(525, 39);
            this.lbChooseSensor.TabIndex = 1;
            this.lbChooseSensor.Text = "Выберите тип датчика";
            this.lbChooseSensor.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // toolStrip
            // 
            this.toolStrip.AutoSize = false;
            this.toolStrip.Font = new System.Drawing.Font("Segoe UI", 12.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.toolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ddbLanguage,
            this.ddbCalibration});
            this.toolStrip.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow;
            this.toolStrip.Location = new System.Drawing.Point(0, 0);
            this.toolStrip.Name = "toolStrip";
            this.toolStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.toolStrip.Size = new System.Drawing.Size(582, 36);
            this.toolStrip.TabIndex = 2;
            // 
            // ddbLanguage
            // 
            this.ddbLanguage.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.ddbLanguage.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.ddbLanguage.Name = "ddbLanguage";
            this.ddbLanguage.Size = new System.Drawing.Size(61, 33);
            this.ddbLanguage.Text = "Язык";
            this.ddbLanguage.DropDownItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.ddbLanguage_DropDownItemClicked);
            // 
            // ddbCalibration
            // 
            this.ddbCalibration.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.ddbCalibration.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miCalibAnalogSensor,
            this.miCalibFreqSensor});
            this.ddbCalibration.Image = ((System.Drawing.Image)(resources.GetObject("ddbCalibration.Image")));
            this.ddbCalibration.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.ddbCalibration.Name = "ddbCalibration";
            this.ddbCalibration.Size = new System.Drawing.Size(117, 33);
            this.ddbCalibration.Text = "Калибровка";
            this.ddbCalibration.Visible = false;
            // 
            // miCalibAnalogSensor
            // 
            this.miCalibAnalogSensor.Name = "miCalibAnalogSensor";
            this.miCalibAnalogSensor.Size = new System.Drawing.Size(235, 28);
            this.miCalibAnalogSensor.Text = "Аналоговый датчик";
            // 
            // miCalibFreqSensor
            // 
            this.miCalibFreqSensor.Name = "miCalibFreqSensor";
            this.miCalibFreqSensor.Size = new System.Drawing.Size(235, 28);
            this.miCalibFreqSensor.Text = "Частотный датчик";
            // 
            // btInterfaceSensor
            // 
            this.btInterfaceSensor.BackColor = System.Drawing.Color.Gold;
            this.btInterfaceSensor.Font = new System.Drawing.Font("Segoe UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btInterfaceSensor.Location = new System.Drawing.Point(24, 104);
            this.btInterfaceSensor.Margin = new System.Windows.Forms.Padding(15);
            this.btInterfaceSensor.Name = "btInterfaceSensor";
            this.btInterfaceSensor.Size = new System.Drawing.Size(155, 90);
            this.btInterfaceSensor.TabIndex = 0;
            this.btInterfaceSensor.Text = "Интерфейсный датчик";
            this.btInterfaceSensor.UseVisualStyleBackColor = false;
            this.btInterfaceSensor.Click += new System.EventHandler(this.btInterfaceSensor_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(582, 242);
            this.Controls.Add(this.toolStrip);
            this.Controls.Add(this.lbChooseSensor);
            this.Controls.Add(this.btFreqSensor);
            this.Controls.Add(this.btAnalogSensor);
            this.Controls.Add(this.btInterfaceSensor);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(590, 275);
            this.MinimumSize = new System.Drawing.Size(590, 275);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.toolStrip.ResumeLayout(false);
            this.toolStrip.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btInterfaceSensor;
        private System.Windows.Forms.Button btAnalogSensor;
        private System.Windows.Forms.Button btFreqSensor;
        private System.Windows.Forms.Label lbChooseSensor;
        private System.Windows.Forms.ToolStrip toolStrip;
        private System.Windows.Forms.ToolStripDropDownButton ddbLanguage;
        private System.Windows.Forms.ToolStripDropDownButton ddbCalibration;
        private System.Windows.Forms.ToolStripMenuItem miCalibAnalogSensor;
        private System.Windows.Forms.ToolStripMenuItem miCalibFreqSensor;
    }
}