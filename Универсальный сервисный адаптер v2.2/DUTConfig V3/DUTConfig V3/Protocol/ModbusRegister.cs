using System;
using System.Runtime.InteropServices;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;

using DUTConfig_V3.Settings;
using ZedGraph;
using ModbusRTU;


namespace DUTConfig_V3.Protocol
{
    public class ModbusRegister
    {
        public class ApproxValues
        {
            public float XVal { get; set; }
            public float YVal { get; set; }
        }
        public class PolyCoefficients
        {
            public float A { get; set; }
            public float B { get; set; }
            public float C { get; set; }
            public float D { get; set; }
        }

        public ModbusRegister()
        {
            registerBuf = new ushort[REGISTER_SIZE];
            ApproxTable = new List<ApproxValues>(30);
            PolyCoeffs = new PolyCoefficients();
        }

        [XmlIgnore]
        const int REGISTER_SIZE = 61; // в ushort'ах
        [XmlIgnore]
        ushort[] registerBuf;

        [XmlIgnore]
        public const int TAR_TABLE_ADDR = 0x3680;
        [XmlIgnore]
        public const int TABLE_SIZE = 30; // во float'ах

        public List<ApproxValues> ApproxTable { get; set; }
        public PolyCoefficients PolyCoeffs { get; set; }

        [XmlIgnore]
        public bool DataDamageFlag = false;

        [XmlIgnore]
        public float Liters
        {
            get
            {
                return GetRegsFLOAT(0);
            }
        }

        [XmlIgnore]
        public string LitersStr
        {
            get
            {
                return string.Format("{0:0.0}", Liters); 
            }
        }

        [XmlIgnore]
        public float PercentL
        {
            get
            {
                return GetRegsFLOAT(2);
            }
            set
            {
                SetRegsFLOAT(value, 2);
            }
        }

        [XmlIgnore]
        public string PercentLStr
        {
            get
            {
                return string.Format("{0:0.0}", PercentL);
            }
        }

        [XmlIgnore]
        public float DOTFrequency
        {
            get
            {
                return GetRegsFLOAT(4);
            }
            set
            {
                SetRegsFLOAT(value, 4);
            }
        }

        [XmlIgnore]
        public string DOTFrequencyStr
        {
            get
            {
                return string.Format("{0:0.0}", DOTFrequency);
            }
        }

        [XmlIgnore]
        public float DOTFrequencyCore
        {
            get
            {
                return GetRegsFLOAT(6);
            }
            set
            {
                SetRegsFLOAT(value, 6);
            }
        }

        [XmlIgnore]
        public string DOTFrequencyCoreStr
        {
            get
            {
                return string.Format("{0:0.0}", DOTFrequencyCore);
            }
        }

        [XmlIgnore]
        public float DOTPeriod
        {
            get
            {
                return GetRegsFLOAT(8);
            }
        }

        [XmlIgnore]
        public string DOTPeriodStr
        {
            get
            {
                return string.Format("{0:0.0}", DOTPeriod);
            }
        }

        [XmlIgnore]
        public float DOTPeriodCore
        {
            get
            {
                return GetRegsFLOAT(10);
            }
        }

        [XmlIgnore]
        public string DOTPeriodCoreStr
        {
            get
            {
                return string.Format("{0:0.0}", DOTPeriodCore);
            }
        }

        [XmlIgnore]
        public float Ut
        {
            get
            {
                return GetRegsFLOAT(12);
            }
        }

        [XmlIgnore]
        public string UtStr
        {
            get
            {
                return string.Format("{0:0.0}", Ut);
            }
        }

        [XmlIgnore]
        public short T
        {
            get
            {
                return (short)GetRegsSHORT(14);
            }
        }

        [XmlIgnore]
        public ushort FlTermo
        {
            get
            {
                ushort flag = GetRegsSHORT(15);

                if (flag > 1)
                {
                    DataDamageFlag = true;
                    return 0;
                }

                return flag;
            }
        }

        public ushort TypeAppr
        {
            get
            {
                ushort type = GetRegsSHORT(16);

                if (type > 1)
                {
                    DataDamageFlag = true;
                    return 1;
                }

                return type;
            }
            set
            {
                SetRegsSHORT(value, 16);
            }
        }

        public ushort DeltaUPow
        {
            get
            {
                return GetRegsSHORT(17);
            }
            set
            {
                SetRegsSHORT(value, 17);
            }
        }

        [XmlIgnore]
        public ushort EngineState
        {
            get
            {
                return GetRegsSHORT(18);
            }
        }

        [XmlIgnore]
        public string EngineStateStr
        {
            get
            {
                if (EngineState == 0)
                    return Translation.Current.lbEngineStateOnText;
                else
                    return Translation.Current.lbEngineStateOffText;
            }
        }

        [XmlIgnore]
        public ulong VersionSw
        {
            get
            {
                return GetRegsLONG(19);
            }
        }

        public ushort TypeAverage
        {
            get
            {
                ushort type = GetRegsSHORT(21);

                if (type > 2)
                {
                    DataDamageFlag = true;
                    return 1;
                }
                return type;
            }
            set
            {
                SetRegsSHORT(value, 21);
            }
        }

        public ushort Time
        {
            get
            {
                return GetRegsSHORT(22);
            }
            set
            {
                SetRegsSHORT(value, 22);
            }
        }

        [XmlIgnore]
        public float CalcAlpha
        {
            get
            {
                float alpha = GetRegsFLOAT(23);
                if (alpha == 0.0f)
                    return 0.0f;

                return 7.0f / 2.0f / alpha;
            }
            set
            {
                if (value < 5.0f)
                    value = 5.0f;
                value = 7.0f / 2.0f / value;
                if (value > 1.0f)
                    value = 1.0f;

                SetRegsFLOAT(value, 23);
            }
        }

        public float Alpha
        {
           /* get
            {
                return CalcAlpha; 
            }
            set
            {
                SetRegsFLOAT(value, 23);
            }*/

            get
            {
                float alpha = GetRegsFLOAT(23);
                if (alpha == 0.0f)
                    return 0.0f;

                return 7.0f / 2.0f / alpha;
            }
            set
            {
                if (value < 5.0f)
                    value = 5.0f;
                value = 7.0f / 2.0f / value;
                if (value > 1.0f)
                    value = 1.0f;

                SetRegsFLOAT(value, 23);
            }
        }

        public ushort FlAutoSend
        {
            get
            {
                ushort flag = GetRegsSHORT(25);

                if (flag > 2)
                {
                    DataDamageFlag = true;
                    return 0;
                }

                return flag;
            }
            set
            {
                SetRegsSHORT(value, 25);
            }
        }

        public ushort PeriodAuto
        {
            get
            {
                return GetRegsSHORT(26);
            }
            set
            {
                SetRegsSHORT(value, 26);
            }
        }

        public ushort OmniNetMode
        {
            get
            {
                ushort mode = GetRegsSHORT(27);

                if (mode > 1)
                {
                    DataDamageFlag = true;
                    return 0;
                }

                return mode;
            }
            set
            {
                SetRegsSHORT(value, 27);
            }
        }

        [XmlIgnore]
        public ushort OmniError
        {
            get
            {
                return GetRegsSHORT(28);
            }
        }

        public ushort MaxN
        {
            get
            {
                return GetRegsSHORT(29);
            }
            set
            {
                SetRegsSHORT(value, 29);
            }
        }

        [XmlIgnore]
        public ushort NPoints
        {
            get
            {
                ushort points = GetRegsSHORT(30);

                if (points > 32)
                    return 32;

                return points;
            }
            set
            {
                SetRegsSHORT(value, 30);
            }
        }

        public ushort ModId
        {
            get
            {
                return GetRegsSHORT(31);
            }
            set
            {
                SetRegsSHORT(value, 31);
            }
        }

        public ulong Baudrate
        {
            get
            {
                return GetRegsLONG(32);
            }
            set
            {
                SetRegsLONG((uint)value, 32);
            }
        }

        [XmlIgnore]
        public ushort Error
        {
            get
            {
                return GetRegsSHORT(34);
            }
        }

        public ushort Password
        {
            get
            {
                return GetRegsSHORT(35);
            }
            set
            {
                SetRegsSHORT(value, 35);
            }
        }

        public float FMin
        {
            get
            {
                return GetRegsFLOAT(36);
            }
            set
            {
                SetRegsFLOAT(value, 36);
            }
        }

        public float FMax
        {
            get
            {
                return GetRegsFLOAT(38);
            }
            set
            {
                SetRegsFLOAT(value, 38);
            }
        }

        // старая прошивка закончилась

        [XmlIgnore]
        public float UPow
        {
            get
            {
                return ((float)GetRegsSHORT(40) / 1000.0f);
            }
        }

        [XmlIgnore]
        public string UPowStr
        {
            get
            {
                return string.Format("{0:0.0}", UPow);
            }
        }

        public ushort T_Avg_window_stop
        {
            get
            {
                return GetRegsSHORT(41);
            }
            set
            {
                SetRegsSHORT(value, 41);
            }
        }

        public ushort DeltaFOut
        {
            get
            {
                return GetRegsSHORT(42);
            }
            set
            {
                SetRegsSHORT(value, 42);
            }
        }

        /*public ulong ID
        {
            get
            {
                return GetRegsLONG(42);
            }
            set
            {
                SetRegsLONG((uint)value, 42);
            }
        }*/

        public ushort FlTermoCorrect
        {
            get
            {
                return (ushort)(GetRegsSHORT(44) > 0 ? 1 : 0);
            }
            set
            {
                SetRegsSHORT(value, 44);
            }
        }

        public float[] PolinomTermoCorrect
        {
            get
            {
                float[] buf = new float[5];

                buf[0] = GetRegsFLOAT(45);
                buf[1] = GetRegsFLOAT(47);
                buf[2] = GetRegsFLOAT(49);
                buf[3] = GetRegsFLOAT(51);
                buf[4] = GetRegsFLOAT(53);

                return buf;
            }
            set
            {
                if (value.Length != 5)
                    return;

                SetRegsFLOAT(value[0], 45);
                SetRegsFLOAT(value[1], 47);
                SetRegsFLOAT(value[2], 49);
                SetRegsFLOAT(value[3], 51);
                SetRegsFLOAT(value[4], 53);
            }
        }

        public float[] PolinomT
        {
            get
            {
                float[] buf = new float[3];

                buf[0] = GetRegsFLOAT(55);
                buf[1] = GetRegsFLOAT(57);
                buf[2] = GetRegsFLOAT(59);

                return buf;
            }
            set
            {
                if (value.Length != 3)
                    return;

                SetRegsFLOAT(value[0], 55);
                SetRegsFLOAT(value[1], 57);
                SetRegsFLOAT(value[2], 59);
            }
        }

        public byte[] GetAppTableBuf()
        {
            byte[] buf = new byte[TABLE_SIZE * 2 * sizeof(float)];
            int size = ApproxTable.Count > 30 ? 30 : ApproxTable.Count;

            for (int i = 0; i < size; i++)
            {
                byte[] valBuf = BitConverter.GetBytes(ApproxTable[i].XVal);
                Array.Copy(valBuf, 0, buf, i * sizeof(float), valBuf.Length);

                valBuf = BitConverter.GetBytes(ApproxTable[i].YVal);
                Array.Copy(valBuf, 0, buf, buf.Length / 2 + i * sizeof(float), valBuf.Length);
            }

            return buf;
        }

        public byte[] GetPolyBuf()
        {
            byte[] buf = new byte[TABLE_SIZE * 2 * sizeof(float)];

            byte[] valBuf = BitConverter.GetBytes(PolyCoeffs.A);
            Array.Copy(valBuf, 0, buf, 0, valBuf.Length);
            valBuf = BitConverter.GetBytes(PolyCoeffs.B);
            Array.Copy(valBuf, 0, buf, 4, valBuf.Length);
            valBuf = BitConverter.GetBytes(PolyCoeffs.C);
            Array.Copy(valBuf, 0, buf, 8, valBuf.Length);
            valBuf = BitConverter.GetBytes(PolyCoeffs.D);
            Array.Copy(valBuf, 0, buf, 12, valBuf.Length);

            return buf;
        }

        public void ClearDataBuf()
        {
            Array.Clear(registerBuf, 0, registerBuf.Length);
        }

        public void ClearTables()
        {
            ApproxTable.Clear();

        }

        public void SetDataBuf(byte[] buf)
        {
            /*if(buf.Length != registerBuf.Length * 2)
                return;*/

            for (int i = 0; i < buf.Length / 2; i++)
            {
                registerBuf[i] = BitConverter.ToUInt16(buf, i * 2);
            }
        }

        public void SetTableBuf(byte[] bytesX, byte[] bytesY, int count)
        {
            ApproxTable.Clear();
            for (int i = 0; i < count; i++)
            {
                ApproxTable.Add(new ApproxValues()
                {
                    XVal = BitConverter.ToSingle(bytesX, i * 4),
                    YVal = BitConverter.ToSingle(bytesY, i * 4)
                });
            }
        }

        public float GetRegsFLOAT(int index)
        {
            float[] in_reg = new float[1];
            byte[] in_data = new byte[4];
            Buffer.BlockCopy(registerBuf, index * 2, in_data, 0, 4);
            Modbus.memrevercy(in_data, 4);
            Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
            return in_reg[0];
        }
        public void SetRegsFLOAT(float data, int index)
        {
            float[] in_reg = new float[1]; ;
            byte[] in_data = new byte[4];
            in_reg[0] = data;
            Buffer.BlockCopy(in_reg, 0, in_data, 0, 4);
            Modbus.memrevercy(in_data, 4);
            Buffer.BlockCopy(in_data, 0, registerBuf, index * 2, 4);
        }

        public ushort GetRegsSHORT(int index)
        {
            ushort[] in_reg = new ushort[1];
            byte[] in_data = new byte[2];
            Buffer.BlockCopy(registerBuf, index * 2, in_data, 0, 2);
            Modbus.memrevercy(in_data, 2);
            Buffer.BlockCopy(in_data, 0, in_reg, 0, 2);
            return in_reg[0];
        }
        public void SetRegsSHORT(ushort data, int index)
        {
            ushort[] in_reg = new ushort[1];
            byte[] in_data = new byte[2];
            in_reg[0] = data;
            Buffer.BlockCopy(in_reg, 0, in_data, 0, 2);
            Modbus.memrevercy(in_data, 2);
            Buffer.BlockCopy(in_data, 0, registerBuf, index * 2, 2);
        }

        public UInt32 GetRegsLONG(int index)
        {
            UInt32[] in_reg = new UInt32[1];
            byte[] in_data = new byte[4];
            Buffer.BlockCopy(registerBuf, index * 2, in_data, 0, 4);
            Modbus.memrevercy(in_data, 4);
            Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
            return in_reg[0];
        }

        public void SetRegsLONG(UInt32 data, int index)
        {
            UInt32[] in_reg = new UInt32[1]; ;
            byte[] in_data = new byte[4];
            in_reg[0] = data;
            Buffer.BlockCopy(in_reg, 0, in_data, 0, 4);
            Modbus.memrevercy(in_data, 4);
            Buffer.BlockCopy(in_data, 0, registerBuf, index * 2, 4);
        }
    }
}
