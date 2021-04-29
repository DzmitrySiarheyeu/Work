using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Xml;
using System.Xml.Serialization;
using ZedGraph;
using System.Runtime.InteropServices;

using DUTConfig_V3.Settings;
using DUTConfig_V3.Forms.etc;
using DUTConfig_V3.Protocol;

namespace DUTConfig_V3.Forms
{
    public partial class Fl_termo_window : Form
    {
        Termo_correction Termo = new Termo_correction();
        Modbus modbus;
        Points Points_table;
        Translation currentTranslation;

        public Fl_termo_window(Modbus modbus_pered, Termo_correction termo_pered, Translation trans)
        {
            modbus = modbus_pered;
            //Termo = termo_pered;
            Points_table = new Points();
            InitializeComponent();

            currentTranslation = trans;
        }

        private void BintTranslation(Translation trans)
        {
            Text = trans.ThermoFormText;
            label9.Text = trans.ThermoCalibModeText;
            label8.Text = trans.ThermoCalibRangeText;
            label3.Text = trans.ThermoCalibTableText;
            dataGridView1.Columns[0].HeaderText = trans.ThermoCalibTableColumnNumText;
            dataGridView1.Columns[2].HeaderText = trans.ThermoCalibTableColumnFreqText;
            label1.Text = trans.ThermoPolyPowText;
            label4.Text = trans.ThermoCurrentCoeffsText;
            dataGridView2.Columns[0].HeaderText = trans.ThermoCoeffsTableColumnPowText;
            dataGridView2.Columns[1].HeaderText = trans.ThermoCoeffsTableColumnCoeffText;
            button2.Text = trans.ThermoClearText;
            label5.Text = trans.ThermoPolyPowText;
            label2.Text = trans.ThermoPolyCoeffsText;
            button_load.Text = trans.ThermoLoadFileText;
            button_save.Text = trans.ThermoSaveFileText;
            button_calculate.Text = trans.ThermoCalcCoeffsText;
            button_write_polinom.Text = trans.ThermoWriteText;
            button_read_polinom.Text = trans.ThermoReadText;
            button1.Text = trans.ThermoWriteDefaultText;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                textBox_a.Clear();
                float[] temparray = rotate_array(Termo.polinom_termo_correct);
                Write_array(temparray, Termo.Adress_polinom_termo_correct);
                Write_array(Termo.polinom_t, Termo.Adress_polinom_t);
                MessageBox.Show(currentTranslation.WriteDoneMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void Write_array(float[] array, ushort adress)
        {
            try
            {
                modbus.Open();
                for (int i = 0; i < array.Length; i++)
                {
                    modbus.WriteFloat((ushort)(adress + 2 * i), array[i]);
                }
            }
            catch (Exception ex)
            {
                throw (new Exception(ex.Message));

            }
            finally
            {
                modbus.Close();
            }

            //modbus.WriteMemory(0, adress, b_buf, Convert.ToByte(Count * 4));
        }

        private void Fl_termo_window_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                Points_table.Table_baze.Clear();
                Points_table.Table_current.Clear();
                Points_table.Table_read_from_device.Clear();
                for (int i = 0; i < dataGridView1.RowCount - 1; i++)
                {
                    DUTConfig_V3.Forms.etc.Point Temp = new DUTConfig_V3.Forms.etc.Point();
                    Temp.number = i + 1;
                    Temp.x = (double)dataGridView1.Rows[i].Cells[1].Value;
                    Temp.y = (double)dataGridView1.Rows[i].Cells[2].Value;
                    Points_table.Table_baze.Add(Temp);
                }

                for (int i = 0; i < dataGridView2.RowCount; i++)
                {
                    Point_Current Temp = new Point_Current();
                    Temp.x = "X^" + i.ToString();
                    Temp.y = (double)dataGridView2.Rows[i].Cells[1].Value;
                    Points_table.Table_current.Add(Temp);
                }


                Points_table.STEP_POLINOM = Convert.ToInt32(textBox_m.Text);
                Points_table.STEP_POLINOM_CURRENT = Convert.ToInt32(textBox_mcurr.Text);

                XmlSerializer mySerializer = new XmlSerializer(typeof(Points));
                StreamWriter myWriter = new StreamWriter(AppSettings.AppFolder + "Points.xml");
                mySerializer.Serialize(myWriter, Points_table);
               
                myWriter.Close();
            }
            catch (Exception ex)
            { MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error); }
        }

        private void Fl_termo_window_Load(object sender, EventArgs e)
        {
            try
            {
                XmlSerializer serializer1 = new XmlSerializer(typeof(Points));
                FileStream fs1 = new FileStream(AppSettings.AppFolder + "Points.xml", FileMode.Open);
                Points_table = (Points)serializer1.Deserialize(fs1);
                fs1.Close();
                textBox_mcurr_TextChanged(null, null);

                for (int i = 0; i < Points_table.Table_baze.Count; i++)
                {
                    bindingSource_point.Add(Points_table.Table_baze[i]);
                }

                for (int i = 0; i < Termo.polinom_termo_correct.Length; i++)
                {
                    Point_Current temp = new Point_Current();
                    temp.x = "X^" + i.ToString();
                    if (Points_table.Table_current.Count > i)
                        temp.y = Points_table.Table_current[i].y;
                    else
                        temp.y = 0.0f;
                    bindingSource_current.Add(temp);
                }
                if (Points_table.f) 
                    radioButton_f.Checked = true; 
                else 
                    radioButton_n.Checked = true;

                radioButton_f_CheckedChanged(null, null);

                bindingSource_points.Add(Points_table);
                bindingSource_points.ResetBindings(false);
                bindingSource_point.ResetBindings(false);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                BintTranslation(currentTranslation);
                TranslateMenu(currentTranslation);
            }
        }

        private void button_load_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Filter = "xml files (*.xml)|*.xml|All files (*.*)|*.*";
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    // XmlSerializer mySerializer = new XmlSerializer(typeof(Points));
                    //  StreamWriter myWriter = new StreamWriter("Points_rezerv.xml");

                    //  try
                    // {

                    //     mySerializer.Serialize(myWriter, Points_table);
                    //      myWriter.Close();


                    XmlSerializer serializer1 = new XmlSerializer(typeof(Points));
                    FileStream fs1 = new FileStream(openFileDialog1.FileName, FileMode.Open);
                    Points_table = (Points)serializer1.Deserialize(fs1);
                    fs1.Close();

                    bindingSource_point.Clear(); bindingSource_points.Clear();
                    for (int i = 0; i < Points_table.Table_baze.Count; i++)
                    {
                        bindingSource_point.Add(Points_table.Table_baze[i]);
                    }
                    bindingSource_current.Clear();
                    for (int i = 0; i < Points_table.Table_current.Count; i++)
                    {
                        bindingSource_current.Add(Points_table.Table_current[i]);
                    }
                    bindingSource_points.Add(Points_table);
                   // textBox_m.Text = Points_table.STEP_POLINOM.ToString();
                    //textBox_mcurr.Text = Points_table.STEP_POLINOM_CURRENT.ToString();
                    bindingSource_points.ResetBindings(false);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

            bindingSource_point.ResetBindings(false);
        }

        private void button_save_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Filter = "xml files (*.xml)|*.xml|All files (*.*)|*.*";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                //  Points_table.Table_start_graph.Clear();
                Points_table.Table_restore.Clear();
                Points_table.Table_baze.Clear();
                Points_table.Table_yitog.Clear();
                //  Points_table.Table_yitog_calc.Clear();
                for (int i = 0; i < dataGridView1.RowCount - 1; i++)
                {
                    DUTConfig_V3.Forms.etc.Point Temp = new DUTConfig_V3.Forms.etc.Point();
                    Temp.number = i + 1;
                    Temp.x = (double)dataGridView1.Rows[i].Cells[1].Value;
                    Temp.y = (double)dataGridView1.Rows[i].Cells[2].Value;
                    Points_table.Table_baze.Add(Temp);
                }

                Points_table.Table_current.Clear();
                for (int i = 0; i < dataGridView2.RowCount; i++)
                {
                    Point_Current Temp = new Point_Current();
                    Temp.x = "X^" + i.ToString();
                    Temp.y = (double)dataGridView2.Rows[i].Cells[1].Value;
                    Points_table.Table_current.Add(Temp);
                }

                XmlSerializer mySerializer = new XmlSerializer(typeof(Points));
                StreamWriter myWriter = new StreamWriter(saveFileDialog1.FileName);

                try
                {
                    mySerializer.Serialize(myWriter, Points_table);
                    myWriter.Close();
                }
                catch (Exception)
                { }
            }
        }

        List<double> itog_poly = new List<double>();

        private void button_calculate_Click(object sender, EventArgs e)
        {
            Reading = false;

            alglib.barycentricinterpolant p;
            alglib.polynomialfitreport rep;
            p = new alglib.barycentricinterpolant();
            rep = new alglib.polynomialfitreport();
            double[] a_start, a_rest, a_itog;
            int m = Points_table.STEP_POLINOM;
            int m_cur = Points_table.STEP_POLINOM_CURRENT;
            // double t = 0.1;
            int info = 0;
            // double v = 0;
            double[] x = new double[dataGridView1.RowCount - 1];
            double[] y = new double[dataGridView1.RowCount - 1];

            int koeff = (Termo.high + Math.Abs(Termo.low)) / Termo.step + 1;


            double[] Y_user = new double[koeff]; // пользовательские
            double[] Y_restore = new double[koeff]; // восстановленные
            double[] Y_norm = new double[koeff]; // нормированные по p25
            double[] Y_itog = new double[koeff]; // итоговые
            //double[] yitog_calc = new double[koeff];

            // массив Stock_koeff - c заводскими значениями, переведёнными в double
            double[] Current_koeff = new double[Termo.polinom_termo_correct.Length];
            for (int i = 0; i < Current_koeff.Length; i++)
            {
                Current_koeff[i] = Convert.ToDouble(dataGridView2.Rows[i].Cells[1].Value);
            }

            Points_table.Table_yitog.Clear();
            Points_table.Table_restore.Clear();
            itog_poly.Clear();
            try
            {

                for (int i = 0; i < dataGridView1.RowCount - 1; i++)
                {
                    x[i] = (double)dataGridView1.Rows[i].Cells[1].Value;
                    y[i] = (double)dataGridView1.Rows[i].Cells[2].Value;
                }


                // 1. Приводим F в период Р = 1 / F и сохраняем в массиве
                double[] P = new double[dataGridView1.RowCount - 1];
                for (int i = 0; i < P.Length; i++)
                {
                    if (Points_table.f) P[i] = 14745600 / y[i];
                    else
                    {
                        P[i] =
                        (14745600 / Points_table.Fmax) + ((y[i] / Points_table.Diap)
                        * (14745600 / Points_table.Fmin - 14745600 / Points_table.Fmax));
                        P[i] = P[i] / polinom_calc(x[i], Current_koeff);
                    }
                }
                // 2. Получаем коэффициенты полинома а_start
                alglib.polynomialfit(x, P, m, out info, out p, out rep);
                alglib.polynomialbar2pow(p, out a_start);

                // 3. Вычисляем y_user по пользовательским данным и заносим значения в таблицу 
                int itest = Termo.low;
                for (int i = 0; itest <= Termo.high; i++)
                {
                    Y_user[i] = polinom_calc(itest, a_start);
                    itest += Termo.step;
                }
                itest = Termo.low;

                // 4. Получаем Pвосстановленное = (Pi * полином(Aтек, Ti))

                double P25 = polinom_calc(25, a_start);

                // 5. Нормируем Y : Y_norm = P25 / Y_user если в диапазоне пользователя, иначе Y_norm по полиному текущих значений          
                // 6. Получаем Y восстановленное: Y_restore = Y_user если в диапазоне пользователя, иначе Y_restore = P25 / значение по полиному текущих значений

                for (int i = 0; itest <= Termo.high; i++)
                {
                    DUTConfig_V3.Forms.etc.Point temp = new DUTConfig_V3.Forms.etc.Point();

                    if (itest >= x[0] && itest <= x[x.Length - 1])
                    {
                        Y_restore[i] = Y_user[i];
                        //Y_norm[i] = P25 / Y_user[i];
                    }
                    else
                    {
                        Y_restore[i] = P25 / polinom_calc(itest, Current_koeff);
                        //Y_norm[i] = polinom_calc(itest, Current_koeff);
                    }
                    temp.number = i + 1;
                    temp.x = itest;
                    temp.y = Y_restore[i];
                    Points_table.Table_restore.Add(temp);
                    itest += Termo.step;
                }
                itest = Termo.low;
                // массив значений X на интервале от Termo.low до Termo.high c шагом Termo.step
                double[] Xes = new double[koeff];
                for (int i = 0; i < Xes.Length; i++)
                {
                    Xes[i] = Termo.low + i * Termo.step;
                }

                // 7. Получаем коэффициенты восстановленного полинома, прогоняем калькуляцией полинома Y_restore для уточнения данных
                alglib.polynomialfit(Xes, Y_restore, m, out info, out p, out rep);
                alglib.polynomialbar2pow(p, out a_rest);
                for (int i = 0; itest <= Termo.high; i++)
                {
                    Y_restore[i] = polinom_calc(itest, a_rest);
                    Y_norm[i] = polinom_calc(25, a_rest) / Y_restore[i];
                    itest += Termo.step;
                    Points_table.Table_restore[i].y = 14745600 / Y_restore[i];
                }
                itest = Termo.low;

                // 8. Получаем коэффициенты итогового полинома
                alglib.polynomialfit(Xes, Y_norm, m_cur, out info, out p, out rep);
                alglib.polynomialbar2pow(p, out a_itog);

                // 9. Получаем массив Y_itog, с помощью его и массива Y_restore получаем точки графика 
                for (int i = 0; itest <= Termo.high; i++)
                {
                    Y_itog[i] = polinom_calc(itest, a_itog);
                    DUTConfig_V3.Forms.etc.Point temp2 = new DUTConfig_V3.Forms.etc.Point();
                    temp2.number = i + 1;
                    temp2.x = itest;
                    temp2.y = Y_itog[i] * Y_restore[i];
                    temp2.y = 14745600 / temp2.y;
                    //Y_itog[i] = temp2.y;
                    Points_table.Table_yitog.Add(temp2);
                    itest += Termo.step;
                }
                itest = Termo.low;

                // 10. показываем коэффициенты итогового полинома в текстбоксе на форме
                string s = "";
                for (int i = 0; i < a_itog.Length; i++)
                {
                    s += i + 1 + ": " + a_itog[i] + ";  ";
                    itog_poly.Add(a_itog[i]);
                }
                textBox_a.Text = s;

                // 11. Рисуем графики
                DrawGraph(Points_table);
                button_SMS.Enabled = true;
            }
            catch (Exception)
            {
                MessageBox.Show(currentTranslation.ThermoCalcFailMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        public double polinom_calc(double x, double[] a)
        {
            double v = 0;
            for (int i = 0; i < a.Length; i++)
            {
                v += a[i] * Math.Pow(x, i);
            }
            return v;
        }

        private void DrawGraph(Points table)
        {
            // Получим панель для рисования
            GraphPane pane = zedGraph.GraphPane;
            pane.XAxis.Title.Text = currentTranslation.ThermoChartXLabelText + ", ⁰С";
            if (!Reading)
                pane.YAxis.Title.Text = currentTranslation.ThermoChartYReadingLabelText;               
            else
                pane.YAxis.Title.Text = currentTranslation.ThermoChartYLabelText;
            pane.Title.Text = currentTranslation.ThermoChartTitleText;
            // Очистим список кривых на тот случай, если до этого сигналы уже были нарисованы
            pane.CurveList.Clear();

            // Установим масштаб по умолчанию для оси Y
            //pane.YAxis.Scale.MinAuto = true;
            //pane.YAxis.Scale.MaxAuto = true;

            // Создадим список точек
            //  PointPairList list1 = new PointPairList();
            PointPairList list2 = new PointPairList();
            PointPairList list3 = new PointPairList();
            // PointPairList list4 = new PointPairList();
            if (Reading)
            {
                PointPairList list5 = new PointPairList();

                for (int i = 0; i < table.Table_read_from_device.Count; i++)
                {
                    list5.Add(table.Table_read_from_device[i].x, table.Table_read_from_device[i].y);
                }

                if (list5.Count != 0) { LineItem Curve5 = pane.AddCurve(currentTranslation.ThermoChartReadedText, list5, Color.Red, SymbolType.None); }
            }
            //   double xmin = 0;
            //   double xmax = 500;

            // Заполняем список точек
            for (int i = 0; i < table.Table_yitog.Count; i++)
            {
                // list1.Add(table.Table_start_graph[i].x, table.Table_start_graph[i].y);

                list2.Add(table.Table_restore[i].x, table.Table_restore[i].y);

                list3.Add(table.Table_yitog[i].x, table.Table_yitog[i].y);

                //  list4.Add(table.Table_yitog_calc[i].x, table.Table_yitog_calc[i].y);             
            }


            // Опорные точки выделяться не будут (SymbolType.None)
            //   if (list1.Count != 0 && Termo.norm) { LineItem Curve1 = pane.AddCurve("Пользовательские", list1, Color.Blue, SymbolType.None); }

            if (list2.Count != 0 && Termo.stock) { LineItem Curve2 = pane.AddCurve(currentTranslation.ThermoChartRestoredText, list2, Color.Red, SymbolType.None); }

            if (list3.Count != 0 && Termo.itog) { LineItem Curve3 = pane.AddCurve(currentTranslation.ThermoChartResultText, list3, Color.Green, SymbolType.None); }

            //  if (list4.Count != 0 && Termo.itog_calc) {  LineItem Curve4 = pane.AddCurve("Итоговые расчётные", list4, Color.Firebrick, SymbolType.None);}



            // Вызываем метод AxisChange (), чтобы обновить данные об осях. 
            // В противном случае на рисунке будет показана только часть графика, 
            // которая умещается в интервалы по осям, установленные по умолчанию
            zedGraph.AxisChange();


            // Включаем отображение сетки напротив крупных рисок по оси X
            pane.XAxis.MajorGrid.IsVisible = true;
            // Задаем вид пунктирной линии для крупных рисок по оси X:
            // Длина штрихов равна 10 пикселям, ... 
            pane.XAxis.MajorGrid.DashOn = 1;
            // затем 5 пикселей - пропуск
            pane.XAxis.MajorGrid.DashOff = 2;

            // Включаем отображение сетки напротив крупных рисок по оси Y
            pane.YAxis.MajorGrid.IsVisible = true;
            // Аналогично задаем вид пунктирной линии для крупных рисок по оси Y
            pane.YAxis.MajorGrid.DashOn = 1;
            pane.YAxis.MajorGrid.DashOff = 2;


            // Включаем отображение сетки напротив мелких рисок по оси X
            pane.YAxis.MinorGrid.IsVisible = true;
            // Задаем вид пунктирной линии для крупных рисок по оси Y: 
            // Длина штрихов равна одному пикселю, ... 
            pane.YAxis.MinorGrid.DashOn = 1;
            // затем 2 пикселя - пропуск
            pane.YAxis.MinorGrid.DashOff = 2;

            // Включаем отображение сетки напротив мелких рисок по оси Y
            pane.XAxis.MinorGrid.IsVisible = true;
            // Аналогично задаем вид пунктирной линии для крупных рисок по оси Y
            pane.XAxis.MinorGrid.DashOn = 1;
            pane.XAxis.MinorGrid.DashOff = 2;

            pane.XAxis.Scale.MajorStepAuto = true;
            pane.YAxis.Scale.MajorStepAuto = true;
            // !!! Установим значение параметра IsBoundedRanges как true.
            // !!! Это означает, что при автоматическом подборе масштаба 
            // !!! нужно учитывать только видимый интервал графика
            pane.IsBoundedRanges = true;

            //   pane.YAxis.Scale.Min = 0;
            // pane.YAxis.Scale.MaxAuto = true;




            zedGraph.AxisChange();
            // zedGraph.ZoomOutAll(pane);
            // Обновляем график
            zedGraph.Invalidate();



        }

        public void TranslateMenu(Translation trans)
        {
            zedGraph.ContextMenuBuilder += new ZedGraphControl.ContextMenuBuilderEventHandler((s, ms, mp, o) =>
            {
                ms.Items.RemoveAt(6);  // undo all zoom/pan
                ms.Items.RemoveAt(0);  // copy

                ms.Items[0].Text = trans.GraphMenuSaveAs;
                ms.Items[1].Text = trans.GraphMenuPageSetup;
                ms.Items[2].Text = trans.GraphMenuPrint;
                ms.Items[3].Text = trans.GraphMenuPointValues;
                ms.Items[4].Text = trans.GraphMenuUnZoom;
                ms.Items[5].Text = trans.GraphMenuDefaultScale;
            });
        }

        private float[] rotate_array(float[] array)
        {
            float[] temparray = new float[array.Length];
            Array.Copy(array, temparray, array.Length);

            int half = temparray.Length / 2;

            for (int i = 0; i < half; i++)
            {
                float temp = temparray[i];
                temparray[i] = temparray[temparray.Length - i - 1];
                temparray[temparray.Length - i - 1] = temp;
            }
            return temparray;
        }

        private void button_write_polinom_Click(object sender, EventArgs e)
        {
            try
            {
                if (itog_poly.Count != 0)
                {
                    float[] arr = new float[5];

                    for (int i = 0; i < 5; i++)
                    {
                        if (i < itog_poly.Count) arr[i] = Convert.ToSingle(itog_poly[i]);
                        else arr[i] = 0;
                    }
                    arr = rotate_array(arr);
                    Write_array(arr, Termo.Adress_polinom_termo_correct);
                    MessageBox.Show(currentTranslation.WriteDoneMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                modbus.Close();
            }
        }

        bool Reading = false; // поможет прятать 3й график

        private void button_read_polinom_Click(object sender, EventArgs e)
        {
            try
            {
                Reading = true;

                bindingSource_current.Clear();

                modbus.Open(); //открываем порт
                float[] rez = modbus.ReadFloat(Termo.Adress_polinom_termo_correct, Termo.polinom_termo_correct.Length); // считываем в массив коэффициенты полинома из датчика
                rez = rotate_array(rez); // разворачиваем массив
                double[] rez_double = new double[rez.Length]; // создаём массив размером с первый, чтобы перевести числа в double (для аппроксимации)                

                for (int i = 0; i < rez_double.Length; i++)
                {
                    rez_double[i] = Convert.ToDouble(rez[i]); // переводим коэффициенты в double
                }

                for (int i = 0; i < rez_double.Length; i++)
                {
                    Point_Current Temp = new Point_Current();
                    Temp.x = "X^" + i.ToString();
                    Temp.y = rez_double[i];
                    bindingSource_current.Add(Temp);
                }

                Points_table.Table_read_from_device.Clear(); // очищаем таблицу, в которой будут хранится значения для графика

                int koeff = (Termo.high + Math.Abs(Termo.low)) / Termo.step + 1; // рассчитываем количество точек на графике, основываясь на настройках

                double[] Xes = new double[koeff]; // Массив со значениями оси Х графика
                for (int i = 0; i < Xes.Length; i++)
                {
                    Xes[i] = Termo.low + i * Termo.step;
                }

                for (int i = 0; i < koeff; i++)
                {
                    DUTConfig_V3.Forms.etc.Point temp = new DUTConfig_V3.Forms.etc.Point(); // локальная переменная, нужна для корректной передачи данных на отрисовку
                    temp.number = i + 1;
                    temp.x = Xes[i];
                    temp.y = polinom_calc(Xes[i], rez_double); // рассчитываем частоту
                    Points_table.Table_read_from_device.Add(temp);
                }
                Points_table.Table_yitog.Clear();
                // Points_table.Table_yitog_calc.Clear();
                //Points_table.Table_start_graph.Clear();
                Points_table.Table_restore.Clear();
                DrawGraph(Points_table);

                string s = "";
                for (int i = 0; i < rez.Length; i++)
                {
                    s += i + 1 + ": " + rez[i] + ";  ";
                    itog_poly.Add(rez[i]);
                }
                textBox_a.Text = s;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                modbus.Close();
            }
        }


        private void button_SMS_Click(object sender, EventArgs e)
        {
            if (itog_poly.Count == 0)
            {
                MessageBox.Show(currentTranslation.ThermoPolyFailMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            SMS f = new SMS(Points_table, modbus, itog_poly, Termo.Adress_polinom_termo_correct, currentTranslation);
            f.ShowDialog();
        }

        private void dataGridView2_DataError(object sender, DataGridViewDataErrorEventArgs e)
        {
            MessageBox.Show(currentTranslation.ApprTableDataErrorMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            bindingSource_current.Clear();

            for (int i = 0; i < Termo.polinom_termo_correct.Length; i++)
            {
                Point_Current temp = new Point_Current();
                temp.x = "X^" + i.ToString();
                temp.y = Termo.polinom_termo_correct[i];
                bindingSource_current.Add(temp);
            }
        }


        private void Sort_massiv(DUTConfig_V3.Forms.etc.Point[] massiv, int count)
        {
            //сортировка классическим методом пузырька, выполняется для кусочо-линейной аппроксимации
            double tempX; double tempY;
            for (int y = 0; y < count; y++)
            {
                for (int i = 0; i < count - 1; i++)
                {
                    if (massiv[i].x > massiv[i + 1].x)
                    {
                        tempX = massiv[i].x; tempY = massiv[i].y;
                        massiv[i].x = massiv[i + 1].x;
                        massiv[i].y = massiv[i + 1].y;
                        massiv[i].number = i + 1;
                        massiv[i + 1].x = tempX;
                        massiv[i + 1].y = tempY;
                        massiv[i + 1].number = i + 2;
                    }
                }
            }
        }

        private void Sort_numbers(DUTConfig_V3.Forms.etc.Point[] massiv, int count)
        {
            for (int y = 0; y < count; y++)
            {
                massiv[y].number = y + 1;
            }
        }

        private void dataGridView1_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (dataGridView1.RowCount != 0)
                {
                    DUTConfig_V3.Forms.etc.Point[] temparray = new DUTConfig_V3.Forms.etc.Point[dataGridView1.RowCount - 1];
                    bindingSource_point.CopyTo(temparray, 0);

                    if (temparray[temparray.Length - 1].y != 0)
                    {
                        Sort_massiv(temparray, temparray.Length);
                        bindingSource_point.Clear();

                        for (int i = 0; i < temparray.Length; i++)
                        {
                            bindingSource_point.Add(temparray[i]);
                        }
                    }
                }
            }
            catch (Exception ex) { MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error); }
        }

        private void dataGridView1_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
        {
            try
            {
                if (dataGridView1.RowCount != 0)
                {
                    DUTConfig_V3.Forms.etc.Point[] temparray = new DUTConfig_V3.Forms.etc.Point[dataGridView1.RowCount - 1];
                    bindingSource_point.CopyTo(temparray, 0);

                    if (temparray[temparray.Length - 1].y != 0)
                    {
                        Sort_numbers(temparray, temparray.Length);
                        bindingSource_point.Clear();

                        for (int i = 0; i < temparray.Length; i++)
                        {
                            bindingSource_point.Add(temparray[i]);
                        }
                    }
                }
            }
            catch (Exception ex) { MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error); }
        }

        private void textBox_mcurr_TextChanged(object sender, EventArgs e)
        {
            try
            {
                int i = Convert.ToInt32(textBox_mcurr.Text);
                if (i > 5)
                    textBox_mcurr.Text = "5";
                if (i == 0) textBox_mcurr.Text = "1";
            }
            catch (Exception)
            {
                textBox_mcurr.Text = "1";
            }
        }

        private void radioButton_f_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (radioButton_f.Checked)
                {
                    Points_table.f = true;
                    panel1.Enabled = false;
                    dataGridView1.Columns[2].HeaderText = currentTranslation.ThermoChartYReadingLabelText;
                }
                else
                {
                    Points_table.f = false;
                    panel1.Enabled = true;
                    dataGridView1.Columns[2].HeaderText = "N, %";
                }
            }
            catch (Exception ex) { MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error); }
        }

        private void dataGridView1_DataError(object sender, DataGridViewDataErrorEventArgs e)
        {
            MessageBox.Show(currentTranslation.ApprTableDataErrorMessage, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
}
