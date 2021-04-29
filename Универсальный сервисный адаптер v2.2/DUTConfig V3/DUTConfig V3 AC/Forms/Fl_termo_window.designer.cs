namespace DUTConfig_V3.Forms
{
    partial class Fl_termo_window
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
            this.button1 = new System.Windows.Forms.Button();
            this.zedGraph = new ZedGraph.ZedGraphControl();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.numberDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.xDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.yDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.bindingSource_point = new System.Windows.Forms.BindingSource(this.components);
            this.button_load = new System.Windows.Forms.Button();
            this.button_save = new System.Windows.Forms.Button();
            this.button_calculate = new System.Windows.Forms.Button();
            this.button_write_polinom = new System.Windows.Forms.Button();
            this.button_read_polinom = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_m = new System.Windows.Forms.TextBox();
            this.bindingSource_points = new System.Windows.Forms.BindingSource(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.textBox_a = new System.Windows.Forms.TextBox();
            this.timer_right_click = new System.Windows.Forms.Timer(this.components);
            this.dataGridView2 = new System.Windows.Forms.DataGridView();
            this.xDataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.yDataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.bindingSource_current = new System.Windows.Forms.BindingSource(this.components);
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.textBox_mcurr = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.textBox_diap = new System.Windows.Forms.TextBox();
            this.textBox_fmax = new System.Windows.Forms.TextBox();
            this.textBox_fmin = new System.Windows.Forms.TextBox();
            this.radioButton_n = new System.Windows.Forms.RadioButton();
            this.radioButton_f = new System.Windows.Forms.RadioButton();
            this.button_SMS = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource_point)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource_points)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource_current)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(417, 510);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(125, 40);
            this.button1.TabIndex = 0;
            this.button1.Text = "Установить заводские значения";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // zedGraph
            // 
            this.zedGraph.Location = new System.Drawing.Point(302, 6);
            this.zedGraph.Name = "zedGraph";
            this.zedGraph.ScrollGrace = 0D;
            this.zedGraph.ScrollMaxX = 0D;
            this.zedGraph.ScrollMaxY = 0D;
            this.zedGraph.ScrollMaxY2 = 0D;
            this.zedGraph.ScrollMinX = 0D;
            this.zedGraph.ScrollMinY = 0D;
            this.zedGraph.ScrollMinY2 = 0D;
            this.zedGraph.Size = new System.Drawing.Size(500, 476);
            this.zedGraph.TabIndex = 1;
            // 
            // dataGridView1
            // 
            this.dataGridView1.AutoGenerateColumns = false;
            this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.dataGridView1.BackgroundColor = System.Drawing.Color.Gainsboro;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.numberDataGridViewTextBoxColumn,
            this.xDataGridViewTextBoxColumn,
            this.yDataGridViewTextBoxColumn});
            this.dataGridView1.DataSource = this.bindingSource_point;
            this.dataGridView1.Location = new System.Drawing.Point(13, 84);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(285, 165);
            this.dataGridView1.TabIndex = 2;
            this.dataGridView1.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView1_CellEndEdit);
            this.dataGridView1.DataError += new System.Windows.Forms.DataGridViewDataErrorEventHandler(this.dataGridView1_DataError);
            this.dataGridView1.RowsRemoved += new System.Windows.Forms.DataGridViewRowsRemovedEventHandler(this.dataGridView1_RowsRemoved);
            // 
            // numberDataGridViewTextBoxColumn
            // 
            this.numberDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.numberDataGridViewTextBoxColumn.DataPropertyName = "number";
            this.numberDataGridViewTextBoxColumn.HeaderText = "№";
            this.numberDataGridViewTextBoxColumn.Name = "numberDataGridViewTextBoxColumn";
            // 
            // xDataGridViewTextBoxColumn
            // 
            this.xDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.xDataGridViewTextBoxColumn.DataPropertyName = "x";
            this.xDataGridViewTextBoxColumn.HeaderText = "T,⁰ C";
            this.xDataGridViewTextBoxColumn.Name = "xDataGridViewTextBoxColumn";
            // 
            // yDataGridViewTextBoxColumn
            // 
            this.yDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.yDataGridViewTextBoxColumn.DataPropertyName = "y";
            this.yDataGridViewTextBoxColumn.HeaderText = "F, Гц";
            this.yDataGridViewTextBoxColumn.Name = "yDataGridViewTextBoxColumn";
            // 
            // bindingSource_point
            // 
            this.bindingSource_point.DataSource = typeof(DUTConfig_V3.Forms.etc.Point);
            // 
            // button_load
            // 
            this.button_load.Location = new System.Drawing.Point(19, 510);
            this.button_load.Name = "button_load";
            this.button_load.Size = new System.Drawing.Size(75, 40);
            this.button_load.TabIndex = 3;
            this.button_load.Text = "Загрузить ";
            this.button_load.UseVisualStyleBackColor = true;
            this.button_load.Click += new System.EventHandler(this.button_load_Click);
            // 
            // button_save
            // 
            this.button_save.Location = new System.Drawing.Point(96, 510);
            this.button_save.Name = "button_save";
            this.button_save.Size = new System.Drawing.Size(75, 40);
            this.button_save.TabIndex = 4;
            this.button_save.Text = "Сохранить";
            this.button_save.UseVisualStyleBackColor = true;
            this.button_save.Click += new System.EventHandler(this.button_save_Click);
            // 
            // button_calculate
            // 
            this.button_calculate.Location = new System.Drawing.Point(174, 510);
            this.button_calculate.Name = "button_calculate";
            this.button_calculate.Size = new System.Drawing.Size(75, 40);
            this.button_calculate.TabIndex = 5;
            this.button_calculate.Text = "Рассчитать";
            this.button_calculate.UseVisualStyleBackColor = true;
            this.button_calculate.Click += new System.EventHandler(this.button_calculate_Click);
            // 
            // button_write_polinom
            // 
            this.button_write_polinom.Location = new System.Drawing.Point(255, 510);
            this.button_write_polinom.Name = "button_write_polinom";
            this.button_write_polinom.Size = new System.Drawing.Size(75, 40);
            this.button_write_polinom.TabIndex = 6;
            this.button_write_polinom.Text = "Записать в датчик";
            this.button_write_polinom.UseVisualStyleBackColor = true;
            this.button_write_polinom.Click += new System.EventHandler(this.button_write_polinom_Click);
            // 
            // button_read_polinom
            // 
            this.button_read_polinom.Location = new System.Drawing.Point(336, 510);
            this.button_read_polinom.Name = "button_read_polinom";
            this.button_read_polinom.Size = new System.Drawing.Size(75, 40);
            this.button_read_polinom.TabIndex = 7;
            this.button_read_polinom.Text = "Прочитать из датчика";
            this.button_read_polinom.UseVisualStyleBackColor = true;
            this.button_read_polinom.Click += new System.EventHandler(this.button_read_polinom_Click);
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(21, 486);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(171, 17);
            this.label2.TabIndex = 12;
            this.label2.Text = "Коэфф-ты полинома:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBox_m
            // 
            this.textBox_m.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.bindingSource_points, "STEP_POLINOM", true));
            this.textBox_m.Location = new System.Drawing.Point(196, 251);
            this.textBox_m.Name = "textBox_m";
            this.textBox_m.Size = new System.Drawing.Size(101, 20);
            this.textBox_m.TabIndex = 10;
            // 
            // bindingSource_points
            // 
            this.bindingSource_points.DataSource = typeof(DUTConfig_V3.Forms.etc.Points);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(13, 254);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(182, 13);
            this.label1.TabIndex = 9;
            this.label1.Text = "Степень полинома:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.RestoreDirectory = true;
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.RestoreDirectory = true;
            // 
            // textBox_a
            // 
            this.textBox_a.Location = new System.Drawing.Point(197, 485);
            this.textBox_a.Name = "textBox_a";
            this.textBox_a.Size = new System.Drawing.Size(605, 20);
            this.textBox_a.TabIndex = 13;
            // 
            // timer_right_click
            // 
            this.timer_right_click.Interval = 1000;
            // 
            // dataGridView2
            // 
            this.dataGridView2.AllowUserToAddRows = false;
            this.dataGridView2.AllowUserToDeleteRows = false;
            this.dataGridView2.AllowUserToResizeColumns = false;
            this.dataGridView2.AutoGenerateColumns = false;
            this.dataGridView2.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.dataGridView2.BackgroundColor = System.Drawing.Color.Gainsboro;
            this.dataGridView2.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView2.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.xDataGridViewTextBoxColumn1,
            this.yDataGridViewTextBoxColumn1});
            this.dataGridView2.DataSource = this.bindingSource_current;
            this.dataGridView2.Location = new System.Drawing.Point(13, 294);
            this.dataGridView2.Name = "dataGridView2";
            this.dataGridView2.Size = new System.Drawing.Size(285, 165);
            this.dataGridView2.TabIndex = 14;
            this.dataGridView2.DataError += new System.Windows.Forms.DataGridViewDataErrorEventHandler(this.dataGridView2_DataError);
            // 
            // xDataGridViewTextBoxColumn1
            // 
            this.xDataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.xDataGridViewTextBoxColumn1.DataPropertyName = "x";
            this.xDataGridViewTextBoxColumn1.HeaderText = "Степень";
            this.xDataGridViewTextBoxColumn1.Name = "xDataGridViewTextBoxColumn1";
            // 
            // yDataGridViewTextBoxColumn1
            // 
            this.yDataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.yDataGridViewTextBoxColumn1.DataPropertyName = "y";
            this.yDataGridViewTextBoxColumn1.HeaderText = "Коэф-т";
            this.yDataGridViewTextBoxColumn1.Name = "yDataGridViewTextBoxColumn1";
            // 
            // bindingSource_current
            // 
            this.bindingSource_current.DataSource = typeof(DUTConfig_V3.Forms.etc.Point_Current);
            // 
            // label3
            // 
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label3.Location = new System.Drawing.Point(13, 65);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(282, 16);
            this.label3.TabIndex = 16;
            this.label3.Text = "Данные пользователя:";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label4.Location = new System.Drawing.Point(13, 274);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(285, 17);
            this.label4.TabIndex = 17;
            this.label4.Text = "Текущие коэффициенты:";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(13, 459);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(68, 23);
            this.button2.TabIndex = 18;
            this.button2.Text = "сброс";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // textBox_mcurr
            // 
            this.textBox_mcurr.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.bindingSource_points, "STEP_POLINOM_CURRENT", true));
            this.textBox_mcurr.Location = new System.Drawing.Point(197, 461);
            this.textBox_mcurr.Name = "textBox_mcurr";
            this.textBox_mcurr.Size = new System.Drawing.Size(101, 20);
            this.textBox_mcurr.TabIndex = 20;
            this.textBox_mcurr.TextChanged += new System.EventHandler(this.textBox_mcurr_TextChanged);
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(86, 464);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(105, 13);
            this.label5.TabIndex = 19;
            this.label5.Text = "Степень полинома:";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label9);
            this.panel1.Controls.Add(this.label8);
            this.panel1.Controls.Add(this.label7);
            this.panel1.Controls.Add(this.label6);
            this.panel1.Controls.Add(this.textBox_diap);
            this.panel1.Controls.Add(this.textBox_fmax);
            this.panel1.Controls.Add(this.textBox_fmin);
            this.panel1.Enabled = false;
            this.panel1.Location = new System.Drawing.Point(10, 6);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(288, 58);
            this.panel1.TabIndex = 21;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(10, 9);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(45, 13);
            this.label9.TabIndex = 25;
            this.label9.Text = "Режим:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(10, 35);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(61, 13);
            this.label8.TabIndex = 24;
            this.label8.Text = "Диапазон:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(150, 35);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 13);
            this.label7.TabIndex = 23;
            this.label7.Text = "Fmax:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(150, 9);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 13);
            this.label6.TabIndex = 22;
            this.label6.Text = "Fmin:";
            // 
            // textBox_diap
            // 
            this.textBox_diap.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.bindingSource_points, "Diap", true));
            this.textBox_diap.Location = new System.Drawing.Point(77, 30);
            this.textBox_diap.Name = "textBox_diap";
            this.textBox_diap.Size = new System.Drawing.Size(67, 20);
            this.textBox_diap.TabIndex = 4;
            // 
            // textBox_fmax
            // 
            this.textBox_fmax.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.bindingSource_points, "Fmax", true));
            this.textBox_fmax.Location = new System.Drawing.Point(186, 30);
            this.textBox_fmax.Name = "textBox_fmax";
            this.textBox_fmax.Size = new System.Drawing.Size(99, 20);
            this.textBox_fmax.TabIndex = 3;
            // 
            // textBox_fmin
            // 
            this.textBox_fmin.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.bindingSource_points, "Fmin", true));
            this.textBox_fmin.Location = new System.Drawing.Point(186, 6);
            this.textBox_fmin.Name = "textBox_fmin";
            this.textBox_fmin.Size = new System.Drawing.Size(99, 20);
            this.textBox_fmin.TabIndex = 2;
            // 
            // radioButton_n
            // 
            this.radioButton_n.AutoSize = true;
            this.radioButton_n.Location = new System.Drawing.Point(118, 13);
            this.radioButton_n.Name = "radioButton_n";
            this.radioButton_n.Size = new System.Drawing.Size(33, 17);
            this.radioButton_n.TabIndex = 1;
            this.radioButton_n.TabStop = true;
            this.radioButton_n.Text = "N";
            this.radioButton_n.UseVisualStyleBackColor = true;
            // 
            // radioButton_f
            // 
            this.radioButton_f.AutoSize = true;
            this.radioButton_f.DataBindings.Add(new System.Windows.Forms.Binding("Checked", this.bindingSource_points, "f", true));
            this.radioButton_f.Location = new System.Drawing.Point(87, 13);
            this.radioButton_f.Name = "radioButton_f";
            this.radioButton_f.Size = new System.Drawing.Size(31, 17);
            this.radioButton_f.TabIndex = 0;
            this.radioButton_f.TabStop = true;
            this.radioButton_f.Text = "F";
            this.radioButton_f.UseVisualStyleBackColor = true;
            this.radioButton_f.CheckedChanged += new System.EventHandler(this.radioButton_f_CheckedChanged);
            // 
            // button_SMS
            // 
            this.button_SMS.Location = new System.Drawing.Point(727, 511);
            this.button_SMS.Name = "button_SMS";
            this.button_SMS.Size = new System.Drawing.Size(75, 40);
            this.button_SMS.TabIndex = 7;
            this.button_SMS.Text = "SMS";
            this.button_SMS.UseVisualStyleBackColor = true;
            this.button_SMS.Click += new System.EventHandler(this.button_SMS_Click);
            // 
            // Fl_termo_window
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Silver;
            this.ClientSize = new System.Drawing.Size(814, 554);
            this.Controls.Add(this.radioButton_n);
            this.Controls.Add(this.radioButton_f);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.dataGridView2);
            this.Controls.Add(this.textBox_a);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_m);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button_SMS);
            this.Controls.Add(this.button_read_polinom);
            this.Controls.Add(this.button_write_polinom);
            this.Controls.Add(this.button_calculate);
            this.Controls.Add(this.button_save);
            this.Controls.Add(this.button_load);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.zedGraph);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.textBox_mcurr);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "Fl_termo_window";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Термокомпенсация";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Fl_termo_window_FormClosing);
            this.Load += new System.EventHandler(this.Fl_termo_window_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource_point)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource_points)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource_current)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private ZedGraph.ZedGraphControl zedGraph;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.BindingSource bindingSource_point;
        private System.Windows.Forms.Button button_load;
        private System.Windows.Forms.Button button_save;
        private System.Windows.Forms.Button button_calculate;
        private System.Windows.Forms.Button button_write_polinom;
        private System.Windows.Forms.Button button_read_polinom;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_m;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.BindingSource bindingSource_points;
        private System.Windows.Forms.TextBox textBox_a;
        private System.Windows.Forms.Timer timer_right_click;
        private System.Windows.Forms.BindingSource bindingSource_current;
        private System.Windows.Forms.DataGridView dataGridView2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox textBox_mcurr;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textBox_diap;
        private System.Windows.Forms.TextBox textBox_fmax;
        private System.Windows.Forms.TextBox textBox_fmin;
        private System.Windows.Forms.RadioButton radioButton_n;
        private System.Windows.Forms.RadioButton radioButton_f;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.DataGridViewTextBoxColumn numberDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn xDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn yDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn xDataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn yDataGridViewTextBoxColumn1;
        private System.Windows.Forms.Button button_SMS;
    }
}