using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;

using DUTConfig_V3.Forms.etc;
using DUTConfig_V3.Protocol;
using DUTConfig_V3.Settings;
using ModbusRTU;

namespace DUTConfig_V3.Forms
{
    public partial class SMS : Form
    {
        Translation currentTranslation;
        Points Points_table;
        Modbus modbus;
        ushort Polinom_adress;
        Packet Pack;
        List<byte[]> List_byte = new List<byte[]>();
        List<double> Itog_Poly = new List<double>();

        double[] Zap_curr = new double[10];

        public SMS(Points Points_transfer, Modbus Modbus_transfer, List<double> Itog_Poly_transfer, ushort Polinom_adress_transfer, Translation trans)
        {
            Pack = new Packet();
            Points_table = Points_transfer;
            modbus = Modbus_transfer;
            Itog_Poly = Itog_Poly_transfer;
            Polinom_adress = Polinom_adress_transfer;
            Pack.BAUDRATE = modbus.comPort.BaudRate.ToString();
            Pack.NET_ADRESS = modbus.HostID;
            Pack.TIMEOUT = modbus.comPort.ReadTimeout;
            InitializeComponent();
            comboBox_Boudrate.Text = Pack.BAUDRATE.ToString();
            comboBox_Boudrate.Update();
            bindingSource_pack.Add(Pack);
            bindingSource_pack.ResetBindings(false);
            
            currentTranslation = trans;
            TranslateManual(currentTranslation);

            comboBox_Boudrate.Text = "19200";
        }

        private void TranslateManual(Translation trans)
        {
            groupBox2.Text = trans.SMSParamsText;
            label6.Text = trans.SMSIDText;
            label7.Text = trans.SMSBaudrateText;
            label8.Text = trans.SMSTimeoutText;
            groupBox3.Text = trans.SMSRequestText;
            groupBox1.Text = trans.SMSResponceText;
            button_decode_1.Text = trans.SMSCheckText;
            button_decode_2.Text = trans.SMSCheckText;
            button_decode_3.Text = trans.SMSCheckText;
            button_decode_4.Text = trans.SMSCheckText;
            button_decode_5.Text = trans.SMSCheckText;
            button_decode_6.Text = trans.SMSCheckText;
            button_decode_7.Text = trans.SMSCheckText;
            button_decode_8.Text = trans.SMSCheckText;
            button_generate.Text = trans.SMSGenerateText;
        }

        public ushort[] Get_ushort_array(float data)
        {
            float[] temp_f = new float[1];
            byte[] temp_b = new byte[4];
            ushort[] temp_us = new ushort[2];
            temp_f[0] = data;
            Buffer.BlockCopy(temp_f, 0, temp_b, 0, 4);
            memrevercy(temp_b, 4);
            memrevercy(temp_b, 2);
            Buffer.BlockCopy(temp_b, 0, temp_us, 0, 2);
            Buffer.BlockCopy(temp_b, 2, temp_b, 0, 2);
            memrevercy(temp_b, 2);
            Buffer.BlockCopy(temp_b, 0, temp_us, 2, 2);

            return temp_us;
        }

        public byte[] Get_byte_array(ushort adress, ushort data)
        {
            WRITE_SINGLE_REGISTER p = new WRITE_SINGLE_REGISTER();
            p.function_code = (byte)0x06;
            p.st_adr_hi = (byte)(adress >> 8);
            p.st_adr_lo = (byte)adress;
            p.value_hi = (byte)(data >> 8);
            p.value_lo = (byte)data;
            CreateMBSend(p);
            Thread.Sleep(50);
            return p.DataBuffer;
        }

        public static void memrevercy(byte[] data, int count)
        {
            int i;
            byte temp;
            for (i = 0; i < count / 2; i++)
            {
                temp = data[i];
                data[i] = data[count - i - 1];
                data[count - i - 1] = temp;
            }
        }

        private void form_textbox(float value)
        {
            //  byte[] FF = BitConverter.GetBytes(value);
            //   byte CRC = Finnaly_MasterCRC_2param(0x31, modbus.HostID, FF[0], FF[1], 0x0B);
            //   byte[] arr = new byte[] { 0x31, modbus.HostID, 0x0B, FF[0], FF[1], CRC };

            //    string s = FORM_SMS(6, 5, modbus.comPort.WriteTimeout, modbus.comPort.BaudRate, arr);
        }

        private string FORM_SMS(byte com_length, byte resp_length, int timeout, string Baudrate, byte[] command)
        {
            bool error = false; string s = "";
            try
            {
                string s1 = "";
                for (int i = 0; i < command.Length; i++)
                {
                    s1 += command[i].ToString("X2");
                }
                string baudrate = Baudrate.ToString();
               // baudrate = baudrate.Remove(baudrate.LastIndexOf(" "), 1);
                s = "S485 = " + com_length + ":" + resp_length + ":" + timeout + ":" + baudrate + ":" + s1 + ";";
            }
            catch (Exception ex)
            {
                error = true;
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            if (error) return ""; else return s;
        }

        #region EX



        public class MODBUS_PACKET
        {
            public byte[] DataBuffer;
            public int BufferSize;
            public byte function_code
            {
                get
                {
                    return DataBuffer[1];
                }
                set
                {
                    DataBuffer[1] = value;
                }

            }
            public MODBUS_PACKET()
            {
                this.DataBuffer = new byte[8];
            }
            public MODBUS_PACKET(MODBUS_PACKET packet)
            {
                this.DataBuffer = new byte[8];
                this.DataBuffer = packet.DataBuffer;
            }
        }
        public class WRITE_SINGLE_REGISTER : MODBUS_PACKET
        {
            public byte st_adr_hi
            {
                get { return DataBuffer[2]; }
                set { DataBuffer[2] = value; }
            }
            public byte st_adr_lo
            {
                get
                {
                    return DataBuffer[3];
                }
                set
                {
                    DataBuffer[3] = value;
                }
            }
            public byte value_hi
            {
                get
                {
                    return DataBuffer[4];
                }
                set
                {
                    DataBuffer[4] = value;
                }
            }
            public byte value_lo
            {
                get
                {
                    return DataBuffer[5];
                }
                set
                {
                    DataBuffer[5] = value;
                }
            }
            public WRITE_SINGLE_REGISTER()
            {
                BufferSize = 6 + 2;
            }
        };
        public void CreateMBSend(MODBUS_PACKET packet)
        {
            ushort crc;
            if (packet.BufferSize > 252)
            {
                return;
            }
            packet.DataBuffer[0] = modbus.HostID;	//первый эл-т буфера - номер хоста
            //дописываем CRC
            crc = CRC16.GetCRC16(packet.DataBuffer, packet.BufferSize - 2, 0xFFFF);
            packet.DataBuffer[packet.BufferSize - 2] = (byte)(crc & 0xFF);
            packet.DataBuffer[packet.BufferSize - 1] = (byte)(crc >> 8);
        }
        #endregion

        private void button_generate_Click(object sender, EventArgs e)
        {
            float[] ARR = new float[5];
            for (int i = 0; i < 5; i++)
            {
                ARR[i] = Convert.ToSingle(Itog_Poly[i]);
            }
            ARR = rotate_array(ARR);

            for (int i = 0; i < 5; i++)
            {
                float t = ARR[i];
                ushort[] temp_us = new ushort[2];
                temp_us = Get_ushort_array(t);

                byte[] temp_by, temp_by2 = new byte[8];
                temp_by = Get_byte_array(Convert.ToUInt16(Polinom_adress + i*2), temp_us[0]);
                temp_by2 = Get_byte_array(Convert.ToUInt16(Polinom_adress + i*2 + 1), temp_us[1]);
                List_byte.Add(temp_by); List_byte.Add(temp_by2);
            }
           textBox1.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[0]);
           textBox2.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[1]);
           textBox3.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[2]);
           textBox4.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[3]);
           textBox5.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[4]);
           textBox6.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[5]);
           textBox7.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[6]);
           textBox8.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[7]);
           textBox9.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[8]);
           textBox10.Text = FORM_SMS(8, 8, Pack.TIMEOUT, Pack.BAUDRATE, List_byte[9]);
        }

        private void Check_SMS(TextBox T1, TextBox T2)
        {
            try
            {
                string s1, s2;
                s1 = T1.Text;
                s2 = T2.Text;
                s1 = s1.Substring(s1.LastIndexOf(':') + 1, 16);
                s2 = s2.Substring(s2.LastIndexOf(':') + 1, 16);
                if (s1 == s2) T2.BackColor = Color.SpringGreen;
                else T2.BackColor = Color.IndianRed;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button_decode_1_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox1, textBox_decode_1);
        }

        private void button_decode_2_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox2, textBox_decode_2);
        }

        private void button_decode_3_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox3, textBox_decode_3);
        }

        private void button_decode_4_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox4, textBox_decode_4);
        }

        private void button_decode_5_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox5, textBox_decode_5);
        }

        private void button_decode_6_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox6, textBox_decode_6);
        }

        private void button_decode_7_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox7, textBox_decode_7);
        }

        private void button_decode_8_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox8, textBox_decode_8);
        }

        private void button_decode_9_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox9, textBox_decode_9);
        }

        private void button_decode_10_Click(object sender, EventArgs e)
        {
            Check_SMS(textBox10, textBox_decode_10);
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
    }

    public class Packet
    {

        private byte Net_adress;
        public byte NET_ADRESS
        {
            get
            {
                return Net_adress;
            }
            set
            {
                Net_adress = value;
            }
        }

        private string Baudrate;
        public string BAUDRATE
        {
            get
            {
                return Baudrate;
            }
            set
            {
                Baudrate = value;
            }
        }

        private int Timeout;
        public int TIMEOUT
        {
            get
            {
                return Timeout;
            }
            set
            {
                Timeout = value;
            }
        }

        public Packet()
        {
        }
    }
}
