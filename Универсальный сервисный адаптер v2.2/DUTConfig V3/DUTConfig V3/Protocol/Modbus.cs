using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;
using System.IO.Ports;
using System.Drawing;
using ModbusRTU;

namespace DUTConfig_V3.Protocol
{
    public partial class Modbus
    {
        public enum InterfaceType { NotDefined, OneWire, TwoWire }
        public class ModbusException : Exception
        {
            public enum ExceptionReason 
            { 
                IDException, 
                CRCException, 
                LengthException, 
                IllegalAddrException,
                IllegalValueException,
                IllegalFuncException, 
                UnknownException
            }

            public ExceptionReason Reason;

            public ModbusException(ExceptionReason reason)
                : base()
            {
                Reason = reason;
            }
        }

        public void Open()
        {
            comPort.Open();
        }
        public void Close()
        {
            comPort.Close();
        }

        public Modbus(SerialPort Port, byte ID)
        {
            InterfType = Modbus.InterfaceType.NotDefined;
            comPort = Port;
            HostID = ID;
        }

        public byte HostID;
        public int ErrorCode = 0;
        public InterfaceType InterfType { get; private set; }
        public SerialPort comPort;

        public InterfaceType TestInterfaceType()
        {
            // запоминаем таймаут
            int tOut = comPort.ReadTimeout;
            comPort.ReadTimeout = 50;

            // в качестве теста считываем данные omnicomm
            byte[] req = new byte[] { 0x31, HostID == 0 ? (byte)255 : HostID, 0x06, 0 };
            req[3] = Omnicomm.DoCRC8(req, 3);
            comPort.Write(req, 0, 4);

            // нужно прочитать 4 байта и проверить их
            int toRead = 4, cnt = 0;
            byte[] ans = new byte[toRead];

            try
            {
                while (toRead > 0)
                {
                    int n = comPort.Read(ans, cnt, toRead);
                    toRead -= n;
                    cnt += n;
                }

                for (int i = 0; i < 4; i++)
                {
                    if (ans[i] != req[i])
                        // приняли не запрос
                        return InterfaceType.TwoWire;
                }

                // приняли свой запрос
                return InterfaceType.OneWire;
            }
            catch
            {
                // не приняли
                // значит нет связи
                throw;
            }
            finally
            {
                comPort.ReadTimeout = tOut;
            }
        }
        
        public ushort getUSHORTDataBuffer(MODBUS_PACKET packet)
        {
            return (ushort)((ushort)(packet.DataBuffer[packet.BufferSize - 2]) + ((ushort)(packet.DataBuffer[packet.BufferSize - 1]) << 8));
        }
        public ModbusCodes CheckCRC(MODBUS_PACKET packet)
        {
            //Проверяем, совпадает ли пришедшее CRC с вычисленным
            if (CRC16.GetCRC16(packet.DataBuffer, (byte)(packet.BufferSize - 2), (ushort)0xFFFF) != getUSHORTDataBuffer(packet))
                return ModbusCodes.CRC_ERROR; //Если нет - вернуть ошибку
            return ModbusCodes.CRC_OK;
        }

        private byte[] Read1W(MODBUS_PACKET packetReq, MODBUS_PACKET packetAns)
        {
            // читаем свой запрос
            Read2W(packetReq);

            return Read2W(packetAns);
        }

        private byte[] Read2W(MODBUS_PACKET packet)
        {
            int bytesToRead = packet.BufferSize;
            int bytesRead = 0;

            byte[] inBuf = new byte[bytesToRead];

            while (bytesToRead > 0)
            {
                int n = comPort.Read(inBuf, bytesRead, bytesToRead);
                bytesRead += n;
                bytesToRead -= n;
            }

            return inBuf;
        }

        /*public void Read(MODBUS_PACKET packetReq, MODBUS_PACKET packetAns)
        {
            // просто пропускаем время
            // скорее всего ответ уже придет
            Thread.Sleep(250);

            // максимальный размер для чтения из порта
            // если работаеи по двупроводному интерфейсу, читаем только ответ девайса
            // если работаем по однопроводному интерфейсу, читаем свой запрос и ответ девайса
            int totalBytesToRead = packetReq.BufferSize + packetAns.BufferSize;
            // счетчик прочитанных байт
            int bytesRead = 0;

            byte[] inBuf = new byte[totalBytesToRead];
            // флаг принятого ответа
            bool gotReq = false;

            try
            {
                while (totalBytesToRead > 0)
                {
                    int b = comPort.ReadByte();
                    inBuf[bytesRead++] = (byte)b;
                    totalBytesToRead--;

                    // если приняли количество байт, равное длине запроса
                    // нужно проверить не приняли ли мы свой запрос
                    if (bytesRead == packetReq.BufferSize && !gotReq)
                    {
                        gotReq = true;

                        // проверяем массивы на равенство
                        for (int i = 0; i < packetReq.BufferSize; i++)
                        {
                            if (packetReq.DataBuffer[i] != inBuf[i])
                            {
                                gotReq = false;
                                break;
                            }
                        }

                        // массивы равны, значит приняли свой запрос
                        if (gotReq)
                        {
                            InterfType = InterfaceType.OneWire;

                            // сбрасываем счетчик принятого
                            bytesRead = 0;
                            // осталось принять только ответ
                            totalBytesToRead = packetAns.BufferSize;
                        }
                        // запроса нет
                        // все, что мы приняли - это кусок ответа
                        // нужно дочитать ответ
                        else
                        {
                            totalBytesToRead = packetAns.BufferSize - bytesRead;
                        }
                    }

                    // закончились байты в порте
                    // такое может слущиться по трем причинам
                    // - просто прочитали все, что надо
                    // - длина ответа меньше длины запроса
                    // - ошибка в девайсе и возвращен код ошибки
                    if (comPort.BytesToRead == 0)
                    {
                        // просто выходим и смотрим пакет
                        break;
                    }
                }
            }
            catch (TimeoutException)
            {
                throw;
            }

            Array.Copy(inBuf, 0, packetAns.DataBuffer, 0, bytesRead);

            if (bytesRead > 3 && (packetAns.DataBuffer[1] & 0x80) != 0)
            {
                ErrorCode = packetAns.DataBuffer[2];
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_FUNCTION)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalFuncException);
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_DATA_VALUE)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalValueException);
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_DATA_ADRESS)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalAddrException);
                throw new ModbusException(ModbusException.ExceptionReason.UnknownException);
            }

            //контроль размера полученных данных
            if (!((packetAns.BufferSize < 4) || (packetAns.BufferSize > (int)ModbusCodes.MBCLIENT_MAX_LENGHT)))
            {
                //проверяем CRC
                if (CheckCRC(packetAns) == ModbusCodes.CRC_OK)
                {
                    //проверяем, нам ли предназначен пакет

                    if (packetAns.DataBuffer[0] == HostID || HostID == 0)
                    {
                        if (HostID == 0) HostID = packetAns.DataBuffer[0];
                        return;
                    }
                    ErrorCode = (int)ModbusCodes.ERROR_PACKET_HOST_ID_ERROR;
                    throw new ModbusException(ModbusException.ExceptionReason.IDException);
                }
                ErrorCode = (int)ModbusCodes.ERROR_PACKET_CRC;
                throw new ModbusException(ModbusException.ExceptionReason.CRCException);
            }
            ErrorCode = (int)ModbusCodes.ERROR_PACKET_BUFFER_SIZE;
            throw new ModbusException(ModbusException.ExceptionReason.LengthException);
        }*/

        public void Read(MODBUS_PACKET packetReq, MODBUS_PACKET packetAns)
        {
            // просто пропускаем время
            // скорее всего ответ уже придет
            Thread.Sleep(50);

            // разиер первой порции
            int toRead = packetReq.BufferSize > packetAns.BufferSize ? packetAns.BufferSize : packetReq.BufferSize;
            // счетчик прочитанных байт
            int bytesRead = 0;
            // количество байт для пропуска после первого этапа
            int toSkip = 0;

            byte[] inBuf = new byte[packetAns.BufferSize + packetReq.BufferSize];
            try
            {
                while (toRead-- > 0)
                {
                    int b = comPort.ReadByte();
                    inBuf[bytesRead++] = (byte)b;
                }

                // приняли некорое количество байт
                // проверим наличие запроса
                bool isReq = true;
                for (int i = 0; i < bytesRead; i++)
                {
                    if(packetReq.DataBuffer[i] != inBuf[i])
                    {
                        isReq = false;
                        break;
                    }
                }

                InterfType = isReq ? InterfaceType.OneWire : InterfaceType.TwoWire;

                if (isReq)
                {
                    // приняли свой запрос

                    // если запрос длинее, чем ответ, то мы не дочитали его
                    // и должны пропустить несколько байт, прежде чем читать 
                    // сам ответ
                    if (packetReq.BufferSize > packetAns.BufferSize)
                        toSkip = packetReq.BufferSize - bytesRead;

                    bytesRead = 0;
                    toRead = packetAns.BufferSize;
                }
                else
                    // не приняли свой запрос
                    // просто дочитываем ответ
                    toRead = packetAns.BufferSize - bytesRead;

                // пропускаем недочитанный запрос
                while (toSkip-- > 0)
                    comPort.ReadByte();

                // дочитываем ответ
                while (toRead > 0)
                {
                    int n = comPort.Read(inBuf, bytesRead, toRead);
                    bytesRead += n;
                    toRead -= n;
                }
            }
            catch (TimeoutException)
            {
                throw;
            }

            Array.Copy(inBuf, 0, packetAns.DataBuffer, 0, bytesRead);

            if (bytesRead > 3 && (packetAns.DataBuffer[1] & 0x80) != 0)
            {
                ErrorCode = packetAns.DataBuffer[2];
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_FUNCTION)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalFuncException);
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_DATA_VALUE)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalValueException);
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_DATA_ADRESS)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalAddrException);
                throw new ModbusException(ModbusException.ExceptionReason.UnknownException);
            }

            //контроль размера полученных данных
            if (!((packetAns.BufferSize < 4) || (packetAns.BufferSize > (int)ModbusCodes.MBCLIENT_MAX_LENGHT)))
            {
                //проверяем CRC
                if (CheckCRC(packetAns) == ModbusCodes.CRC_OK)
                {
                    //проверяем, нам ли предназначен пакет

                    if (packetAns.DataBuffer[0] == HostID || HostID == 0)
                    {
                        if (HostID == 0) HostID = packetAns.DataBuffer[0];
                        return;
                    }
                    ErrorCode = (int)ModbusCodes.ERROR_PACKET_HOST_ID_ERROR;
                    throw new ModbusException(ModbusException.ExceptionReason.IDException);
                }
                ErrorCode = (int)ModbusCodes.ERROR_PACKET_CRC;
                throw new ModbusException(ModbusException.ExceptionReason.CRCException);
            }
            ErrorCode = (int)ModbusCodes.ERROR_PACKET_BUFFER_SIZE;
            throw new ModbusException(ModbusException.ExceptionReason.LengthException);
        }

        public void ReadSingleReg(MODBUS_PACKET packetReq, MODBUS_PACKET packetAns)
        {
            Thread.Sleep(50);

            byte[] inBuf = null;
            try
            {
                inBuf = InterfType == InterfaceType.TwoWire || InterfType == InterfaceType.NotDefined ? Read2W(packetAns) : Read1W(packetReq, packetAns);
            }
            catch
            {
                throw;
            }

            if(inBuf == null)
                throw new ModbusException(ModbusException.ExceptionReason.UnknownException);

            int bytesRead = inBuf.Length;
            Array.Copy(inBuf, 0, packetAns.DataBuffer, 0, bytesRead);

            if (bytesRead > 3 && (packetAns.DataBuffer[1] & 0x80) != 0)
            {
                ErrorCode = packetAns.DataBuffer[2];
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_FUNCTION)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalFuncException);
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_DATA_VALUE)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalValueException);
                if (packetAns.DataBuffer[2] == (byte)ModbusCodes.ERROR_ILLEGAL_DATA_ADRESS)
                    throw new ModbusException(ModbusException.ExceptionReason.IllegalAddrException);
                throw new ModbusException(ModbusException.ExceptionReason.UnknownException);
            }

            //контроль размера полученных данных
            if (!((packetAns.BufferSize < 4) || (packetAns.BufferSize > (int)ModbusCodes.MBCLIENT_MAX_LENGHT)))
            {
                //проверяем CRC
                if (CheckCRC(packetAns) == ModbusCodes.CRC_OK)
                {
                    //проверяем, нам ли предназначен пакет

                    if (packetAns.DataBuffer[0] == HostID || HostID == 0)
                    {
                        if (HostID == 0) HostID = packetAns.DataBuffer[0];
                        return;
                    }
                    ErrorCode = (int)ModbusCodes.ERROR_PACKET_HOST_ID_ERROR;
                    throw new ModbusException(ModbusException.ExceptionReason.IDException);
                }
                ErrorCode = (int)ModbusCodes.ERROR_PACKET_CRC;
                throw new ModbusException(ModbusException.ExceptionReason.CRCException);
            }
            ErrorCode = (int)ModbusCodes.ERROR_PACKET_BUFFER_SIZE;
            throw new ModbusException(ModbusException.ExceptionReason.LengthException);
        }

        public void CreateMBSend(MODBUS_PACKET packet)
        {
            ushort crc;
            if (packet.BufferSize > 252)
            {
                return;
            }
            packet.DataBuffer[0] = HostID;	//первый эл-т буфера - номер хоста
            //дописываем CRC
            crc = CRC16.GetCRC16(packet.DataBuffer, packet.BufferSize - 2, 0xFFFF);
            packet.DataBuffer[packet.BufferSize - 2] = (byte)(crc & 0xFF);
            packet.DataBuffer[packet.BufferSize - 1] = (byte)(crc >> 8);
        }

        public void WriteSingleRegister(ushort adress, ushort data)
        {
            WRITE_SINGLE_REGISTER p = new WRITE_SINGLE_REGISTER();
            p.function_code = (byte)ModbusCodes.MOD_WRITE_SINGLE_REGISTER;
            p.st_adr_hi = (byte)(adress >> 8);
            p.st_adr_lo = (byte)adress;
            p.value_hi = (byte)(data >> 8);
            p.value_lo = (byte)data;
            CreateMBSend(p);
            WRITECOMPORT(p.DataBuffer, 0, p.BufferSize);
            int TrustSize = p.BufferSize;
            ReadSingleReg(p, p);
            if (p.BufferSize != TrustSize)
                throw (new Exception("Ошибка WRITE_SINGLE_REGISTER"));

        }

        public void WriteSingleRegisterWithCheck(ushort adress, ushort data)
        {
            WRITE_SINGLE_REGISTER p = new WRITE_SINGLE_REGISTER();
            p.function_code = (byte)ModbusCodes.MOD_WRITE_SINGLE_REGISTER;
            p.st_adr_hi = (byte)(adress >> 8);
            p.st_adr_lo = (byte)adress;
            p.value_hi = (byte)(data >> 8);
            p.value_lo = (byte)data;
            CreateMBSend(p);
            //WRITECOMPORT(p.DataBuffer, 0, p.BufferSize);
            WRITECOMPORTSingleReg(p.DataBuffer, 0, p.BufferSize);
            int TrustSize = p.BufferSize;
            //Read(p, p);
            ReadSingleReg(p, p);
            if (p.BufferSize != TrustSize)
                throw (new Exception("Ошибка WRITE_SINGLE_REGISTER"));

        }

        public void WriteMemoryAligment(byte device, UInt32 adress, byte[] data, int count, int aligment)
        {
            int index = 0;
            byte[] temp = new byte[aligment];
            UInt32 page_adress = adress & 0xFF80;
            int count_write = 0;
            while (count > 0)
            {
                temp = ReadMemory(device, page_adress, (byte)aligment);
                if ((aligment - (adress - page_adress)) > count)
                {
                    count_write = count;
                }
                else
                {
                    count_write = (aligment - (int)(adress - page_adress));
                }
                Buffer.BlockCopy(data, index, temp, (int)(adress - page_adress), count_write);
                WriteMemory(device, page_adress, temp, (byte)aligment);
                count -= count_write;
                page_adress += (uint)aligment;
                adress = page_adress;
                index += count_write;
            }
        }


        public void WriteMemory(byte device, UInt32 adress, byte[] data, byte count)
        {
            MEMORY_OPERATE p = new MEMORY_OPERATE();
            p.count = count;
            p.function_code = (byte)ModbusCodes.MOD_WRITE_FLASH;
            p.device = device;
            p.adress = adress;
            p.data = data;
            CreateMBSend(p);
            WRITECOMPORT(p.DataBuffer, 0, p.BufferSize);
            int TrustSize = p.BufferSize;
            MEMORY_OPERATE p1 = new MEMORY_OPERATE();
            p1.count_read = count;

            Read(p, p1);
            if (p.BufferSize != TrustSize)
                throw (new Exception("Ошибка WRITE_MEMORY"));

        }

        public byte[] ReadMemoryAligment(byte device, UInt32 adress, int count, int aligment)
        {
            int index = 0;
            byte[] temp;
            byte[] data = new byte[count];
            while (count > 0)
            {
                if (aligment > count)
                    aligment = count;
                temp = ReadMemory(device, adress, (byte)aligment);
                Buffer.BlockCopy(temp, 0, data, index, aligment);
                count -= aligment;
                adress += (uint)aligment;
                index += aligment;
            }
            return data;
        }

        public byte[] ReadMemory(byte device, UInt32 adress, byte count)
        {
            MEMORY_OPERATE p = new MEMORY_OPERATE();
            p.count_read = count;
            p.function_code = (byte)ModbusCodes.MOD_READ_FLASH;
            p.device = device;
            p.adress = adress;
            CreateMBSend(p);

            WRITECOMPORT(p.DataBuffer, 0, p.BufferSize);

            MEMORY_OPERATE p1 = new MEMORY_OPERATE();
            p1.count = count;
            int TrustSize = p1.BufferSize;

            Read(p, p1);
            if (p1.BufferSize != TrustSize)
                throw (new Exception("Ошибка Read_MEMORY"));
            return p1.data;
        }

        public float ReadFloat(ushort adress)
        {
            READ_INPUT_REGS_RESP resp = ReadInputRegisters(adress, 2);
            return resp.GetInpRegsFLOAT(0);
        }

        public float[] ReadFloat(ushort adress, int count)
        {
            float[] rez = new float[count];
            READ_INPUT_REGS_RESP resp = ReadInputRegisters(adress, count * 2);
            for (int i = 0; i < count; i++)
            {
                rez[i] = resp.GetInpRegsFLOAT(i * 2);
            }
            return rez;
        }

        public void WriteFloat(ushort adress, float data)
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
            WriteSingleRegisterWithCheck(adress, temp_us[0]);
            WriteSingleRegisterWithCheck((ushort)(adress + 1), temp_us[1]);
        }

        public READ_INPUT_REGS_RESP ReadInputRegisters(ushort adress, int count)
        {
            READ_INPUT_REGS_REQ req = new READ_INPUT_REGS_REQ();
            READ_INPUT_REGS_RESP resp = new READ_INPUT_REGS_RESP(count);
            req.function_code = (byte)ModbusCodes.MOD_READ_INPUT_REGISTER;
            req.st_adr_hi = (byte)(adress >> 8);
            req.st_adr_lo = (byte)adress;
            req.quantity_hi = 0;
            req.quantity_lo = (byte)count;
            CreateMBSend(req);

            WRITECOMPORT(req.DataBuffer, 0, req.BufferSize);
            int TrustSize = resp.BufferSize;
            Read(req, resp);
            if (resp.BufferSize != TrustSize)
                throw (new Exception("Ошибка READ_INPUT_REGS"));
            return resp;
        }

        public READ_HOLDING_REGS_RESP ReadHoldingRegisters(ushort adress, int count)
        {
            READ_HOLDING_REGS_REQ req = new READ_HOLDING_REGS_REQ();
            READ_HOLDING_REGS_RESP resp = new READ_HOLDING_REGS_RESP(count);
            req.function_code = (byte)ModbusCodes.MOD_READ_HOLDING_REGISTERS;
            req.st_adr_hi = (byte)(adress >> 8);
            req.st_adr_lo = (byte)adress;
            req.quantity_hi = 0;
            req.quantity_lo = (byte)count;
            CreateMBSend(req);
            Thread.Sleep(5);
            WRITECOMPORT(req.DataBuffer, 0, req.BufferSize);
            int TrustSize = resp.BufferSize;
            Read(req, resp);
            if (resp.BufferSize != TrustSize)
                throw (new Exception("Ошибка READ_HOLDING_REGS"));
            return resp;
        }

        public void WriteMultipleRegisters(ushort adress, ushort[] data, byte kolvo)
        {
            WRITE_MULTIPLE_REGISTERS_REQ p = new WRITE_MULTIPLE_REGISTERS_REQ();
            p.function_code = (byte)ModbusCodes.MOD_WRITE_MULTIPLE_REGISTERS;
            p.st_adr_hi = (byte)(adress >> 8);
            p.st_adr_lo = (byte)adress;
            ushort[] data_temp = new ushort[kolvo];

            for (int i = 0; i < kolvo; i++)
                data_temp[i] = data[i];
            memrevercy_word(data_temp, kolvo);
            p.value = data_temp;
            p.kol_vo_b = (byte)(kolvo * 2);
            p.kol_vo_hi = (byte)0;
            p.kol_vo_lo = kolvo;
            CreateMBSend(p);
            Thread.Sleep(50);
            WRITECOMPORT(p.DataBuffer, 0, p.BufferSize);

            WRITE_MULTIPLE_REGISTERS_RESP q = new WRITE_MULTIPLE_REGISTERS_RESP();
            int TrustSize = q.BufferSize;
            Read(p, q);
            if (q.BufferSize != TrustSize)
                throw (new Exception("Ошибка WRITE_MULTIPLE_REGISTERS"));
        }

        private void WRITECOMPORT(byte[] buffer, int offset, int count)
        {
         /*   if (InterfType == InterfaceType.NotDefined)
                InterfType = TestInterfaceType();*/

            Thread.Sleep(50);
            comPort.DiscardInBuffer();
            comPort.Write(buffer, offset, count);
        }

        private void WRITECOMPORTSingleReg(byte[] buffer, int offset, int count)
        {
            if (InterfType == InterfaceType.NotDefined)
                InterfType = TestInterfaceType();

            Thread.Sleep(50);
            comPort.DiscardInBuffer();
            comPort.Write(buffer, offset, count);
        }
    }
}
