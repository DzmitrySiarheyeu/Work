using System;
using System.Collections.Generic;
using System.Text;

namespace DUTConfig_V3.Protocol
{
   public partial class Modbus
    {
        /* 
        
        #define BUS_MSG_CNTR			1
        #define BUS_COM_ERR_CNTR		2
        #define SLAVE_EXSEPT_ERR_CNTR	        3
        #define SLAVE_MSG_CNTR			4
        #define SLAVE_NO_RESP_CNTR		5
        #define SLAVE_NAK_CNTR			6
        #define SLAVE_BUSY_CNTR			7
        #define BUS_CHAR_OVRRUN_CNTR	        8
        */
        public enum ModbusCodes
        {
            //коды функций
            MOD_READ_COILS = (0x01),
            MOD_READ_HOLDING_REGISTERS = (0x03),
            MOD_READ_INPUT_REGISTER = (0x04),
            MOD_WRITE_MULTIPLE_REGISTERS = (0x10),
            MOD_ERROR_CODE_FLAG = (0x80),
            MOD_DIAGNOSTIC = (0x08),
            MOD_WRITE_MULTIPLE_COILS = (0x0F),            
            MOD_READ_FILE_RECORD = (0x14),
            MOD_REPORT_SLAVE_ID = (0x11),
            MOD_WRITE_SINGLE_REGISTER = (0x06),
            MOD_READ_FLASH = (0x46),
            MOD_WRITE_OPTION = (0x47), ////////////// Моя функция записи параметров
            MOD_WRITE_FLASH = (0x45),
            MOD_RESET = (0x44),
            MOD_BOOT = (0x50),

            FRAME_ERROR = 1,
            FRAME_SIZE_ERROR = 2,
            CRC_ERROR = 3,
            CRC_OK = 0,

            ERROR_ILLEGAL_FUNCTION = 1,
            ERROR_ILLEGAL_DATA_ADRESS = 2,
            ERROR_ILLEGAL_DATA_VALUE = 3,
            ERROR_SLAVE_DEVICE_FALURE = 4,
            ERROR_PACKET_CRC = 5,
            ERROR_PACKET_TIMEOUT = 6,
            ERROR_PACKET_HOST_ID_ERROR = 7,
            ERROR_PACKET_BUFFER_SIZE = 8,

            //максимальная длина пакета
            MBCLIENT_MAX_LENGHT = 250,

            RETURN_QUERY_DATA = 0x00,
            CLEAR_COUNTERS = 0x0A,
            BUS_MESSAGE_COUNT = 0x0B,
            BUS_COM_ERROR_COUNT = 0x0C,
            SLAVE_EXCEPT_ERROR_COUNT = 0x0D,
            SLAVE_MESSAGE_COUNT = 0x0E,
            SLAVE_NO_RESPONCE_COUNT = 0x0F,
            SLAVE_NAK_COUNT = 0x10,
            SLAVE_BUSY_COUNT = 0x11,
            BUS_CHARRACTER_OVERRUN_COUNT = 0x12
        }

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
                this.DataBuffer = new byte[255];
            }
            public MODBUS_PACKET(MODBUS_PACKET packet)
            {
                this.DataBuffer = new byte[255];
                this.DataBuffer = packet.DataBuffer;
            }
        }

        public class MEMORY_OPERATE : MODBUS_PACKET
        {
            public byte device
            {
                get { return DataBuffer[2]; }
                set { DataBuffer[2] = value; }
            }
            public UInt32 adress
            {
                get { return (UInt32)DataBuffer[3] + (UInt32)(DataBuffer[4] << 8) + (UInt32)(DataBuffer[5] << 16) + (UInt32)(DataBuffer[6] << 24); }
                set { DataBuffer[3] = (byte)value; DataBuffer[4] = (byte)(value >> 8); DataBuffer[5] = (byte)(value >> 16); DataBuffer[6] = (byte)(value >> 24); }
            }
            public byte count
            {
                get { return DataBuffer[7]; }
                set { BufferSize = 8 + value + 2; DataBuffer[7] = value; }
            }
            public byte count_read
            {
                get { return DataBuffer[7]; }
                set { BufferSize = 8 + 2; DataBuffer[7] = value; }
            }
            public byte[] data
            {
                get 
                { 
                    byte[] temp = new byte[count]; 
                    Array.Copy(DataBuffer, 8, temp, 0, count); 
                    return temp; 
                }
                set { Array.Copy(value, 0, DataBuffer, 8, count); }
            }
            public MEMORY_OPERATE()
            {
                count = 0;
                BufferSize = 8 + 2;
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

        public class WRITE_MULTIPLE_REGISTERS_REQ : MODBUS_PACKET
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
            public byte kol_vo_hi
            {
                get { return DataBuffer[4]; }
                set { DataBuffer[4] = value; }
            }

            public byte kol_vo_lo
            {
                get { return DataBuffer[5]; }
                set { DataBuffer[5] = value; }
            }

            public byte kol_vo_b
            {
                get { return DataBuffer[6]; }
                set { DataBuffer[6] = value; }
            }  


            public ushort[] value
            {
                set
                {
                    int n = value.GetLength(0);
                    BufferSize = 9+n*2;
                    Buffer.BlockCopy(value, 0, DataBuffer, 7, n*2);
                }
            }

            public WRITE_MULTIPLE_REGISTERS_REQ()
            {
                BufferSize = 9;
            }
        }





        public class WRITE_MULTIPLE_REGISTERS_RESP : MODBUS_PACKET
        {

            public WRITE_MULTIPLE_REGISTERS_RESP()
            {
                BufferSize = 6 + 2;
            }
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
                
            public byte kol_vo_hi
            {
                get { return DataBuffer[4]; }
                set { DataBuffer[4] = value; }
            }

            public byte kol_vo_lo
            {
                get { return DataBuffer[5]; }
                set { DataBuffer[5] = value; }
            }


            
        }




        public class READ_INPUT_REGS_REQ : MODBUS_PACKET
        {
            public READ_INPUT_REGS_REQ()
            {
                BufferSize = 6 + 2;
            }
            public byte st_adr_hi
            {
                get
                {
                    return DataBuffer[2];
                }
                set
                {
                    DataBuffer[2] = value;
                }
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
            public byte quantity_hi
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
            public byte quantity_lo
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
        };

        public class READ_INPUT_REGS_RESP : MODBUS_PACKET
        {
            public READ_INPUT_REGS_RESP(int CountRegisters)
            {
                BufferSize = 3 + 2 + CountRegisters * 2;
            }

            public ushort[] GetInpRegisters()
            {
                ushort[] regs = new ushort[count / 2];
                Buffer.BlockCopy(DataBuffer, 3 + 0 * 2, regs, 0, count);
                return regs;
            }

            public byte HostID
            {
                get { return DataBuffer[0]; }
            }

            public byte count
            {
                get
                {
                    return DataBuffer[2];
                }
                set
                {
                    DataBuffer[2] = value;
                }
            }

            public ushort GetInpRegsUSHORT(int index)
            {
                int hi;
                hi = DataBuffer[3 + index * 2] << 8;
                return (ushort)(hi + DataBuffer[4 + index * 2]);
            }

            public float GetInpRegsFLOAT(int index)
            {
                float[] in_reg = new float[1];
                byte[] in_data = new byte[4];
                Buffer.BlockCopy(DataBuffer, 3 + index * 2, in_data, 0, 4);
                memrevercy(in_data, 4);
                Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
                return in_reg[0];
            }
            public UInt32 GetInpRegsUINT32(int index)
            {
                UInt32[] in_reg = new UInt32[1];
                byte[] in_data = new byte[4];
                Buffer.BlockCopy(DataBuffer, 3 + index * 2, in_data, 0, 4);
                memrevercy(in_data, 4);
                Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
                return in_reg[0];
            }

            public ulong GetInpRegsULONG(int index)
            {
                ulong[] in_reg = new ulong[1];
                byte[] in_data = new byte[4];
                Buffer.BlockCopy(DataBuffer, 3 + index * 2, in_data, 0, 4);
                memrevercy(in_data, 4);
                Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
                return in_reg[0];
            }


        };

        public class READ_HOLDING_REGS_REQ : MODBUS_PACKET
        {
            public READ_HOLDING_REGS_REQ()
            {
                BufferSize = 6 + 2;
            }
            public byte st_adr_hi
            {
                get
                {
                    return DataBuffer[2];
                }
                set
                {
                    DataBuffer[2] = value;
                }
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
            public byte quantity_hi
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
            public byte quantity_lo
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
        };

        public class READ_HOLDING_REGS_RESP : MODBUS_PACKET
        {
            public READ_HOLDING_REGS_RESP(int CountRegisters)
            {
                BufferSize = 3 + 2 + CountRegisters * 2;
            }

            public ushort[] GetInpRegisters()
            {
                ushort[] regs = new ushort[count / 2];
                Buffer.BlockCopy(DataBuffer, 3 + 0 * 2, regs, 0, count);
                return regs;
            }

            public byte count
            {
                get
                {
                    return DataBuffer[2];
                }
                set
                {
                    DataBuffer[2] = value;
                }
            }

            public ushort GetInpRegsUSHORT(int index)
            {
                int hi;
                hi = DataBuffer[3 + index * 2] << 8;
                return (ushort)(hi + DataBuffer[4 + index * 2]);
            }

            public float GetInpRegsFLOAT(int index)
            {
                float[] in_reg = new float[1];
                byte[] in_data = new byte[4];
                Buffer.BlockCopy(DataBuffer, 3 + index * 2, in_data, 0, 4);
                memrevercy(in_data, 4);
                Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
                return in_reg[0];
            }
            public UInt32 GetInpRegsUINT32(int index)
            {
                UInt32[] in_reg = new UInt32[1];
                byte[] in_data = new byte[4];
                Buffer.BlockCopy(DataBuffer, 3 + index * 2, in_data, 0, 4);
                memrevercy(in_data, 4);
                Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
                return in_reg[0];
            }

            public ulong GetInpRegsULONG(int index)
            {
                ulong[] in_reg = new ulong[1];
                byte[] in_data = new byte[4];
                Buffer.BlockCopy(DataBuffer, 3 + index * 2, in_data, 0, 4);
                memrevercy(in_data, 4);
                Buffer.BlockCopy(in_data, 0, in_reg, 0, 4);
                return in_reg[0];
            }


        };












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

        public static void memrevercy_word(ushort[] data, int count)
        {
            
            ushort temp;
            ushort zn = 8;
            for (int i = 0; i < count; i++)
            {
                temp = (ushort)(data[i] >>  zn);
                temp += (ushort)(data[i] << zn);
                data[i] = temp;
            }
        }

        // Запись параметров устройства
        public class WRITE_OPTION : MODBUS_PACKET
        {
          public  byte mode
            {
                get
                {
                    return DataBuffer[2];
                }
                set
                {
                    DataBuffer[2] = value;
                }
            }
          public byte ID0
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
          public byte ID1
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
          public uint boudrate0
            {
                get
                {
                    uint hi_hi, hi_lo, lo_hi, lo_lo;
                    hi_hi = (uint)DataBuffer[5] << 24;
                    hi_lo = (uint)DataBuffer[6] << 16;
                    lo_hi = (uint)DataBuffer[7] << 8;
                    lo_lo = (uint)DataBuffer[8];
                    return hi_hi + hi_lo + lo_hi + lo_lo;
                }
                set
                {
                    DataBuffer[5] = (byte)(value >> 24);
                    DataBuffer[6] = (byte)(value >> 16);
                    DataBuffer[7] = (byte)(value >> 8);
                    DataBuffer[8] = (byte)value;
                }
            }
          public uint boudrate1
            {
                get
                {
                    uint hi_hi, hi_lo, lo_hi, lo_lo;
                    hi_hi = (uint)DataBuffer[9] << 24;
                    hi_lo = (uint)DataBuffer[10] << 16;
                    lo_hi = (uint)DataBuffer[11] << 8;
                    lo_lo = (uint)DataBuffer[12];
                    return hi_hi + hi_lo + lo_hi + lo_lo;
                }
                set
                {
                    DataBuffer[9] = (byte)(value >> 24);
                    DataBuffer[10] = (byte)(value >> 16);
                    DataBuffer[11] = (byte)(value >> 8);
                    DataBuffer[12] = (byte)value;
                }
            }
          public WRITE_OPTION()
          {
              BufferSize = 11 + 2 + 2;
          }
        };

       
    }
}

