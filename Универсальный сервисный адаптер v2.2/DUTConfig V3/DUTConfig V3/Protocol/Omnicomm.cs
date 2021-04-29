using System;
using System.IO.Ports;
using System.Collections.Generic;
using System.Text;

namespace DUTConfig_V3.Protocol
{
    class Omnicomm
    {
        class OmnicommRequest
        {
            byte _id;
            byte _funcId;
            public int Length
            {
                get { return 4; }
            }

            public OmnicommRequest(byte id, byte funcId)
            {
                _id = id;
                _funcId = funcId;
            }

            public byte[] ReqBuf
            {
                get
                {
                    byte[] buf = new byte[4];
                    buf[0] = 0x31;
                    buf[1] = _id;
                    buf[2] = _funcId;
                    buf[3] = DoCRC8(buf, 3);

                    return buf;
                }
            }
        }

        public class OmnicommResponce
        {
            public byte ID { get; private set; }
            public sbyte T { get; private set; }
            public ushort N { get; private set; }
            public ushort F { get; private set; }

            public OmnicommResponce(byte[] ansBuf, byte reqId)
            {
                if (ansBuf.Length != 9)
                    throw new OmnicommException(OmnicommException.ExceptionReason.LengthError);

                if(ansBuf[0] != 0x3E)
                    throw new OmnicommException(OmnicommException.ExceptionReason.MasterError);

                if(reqId != ansBuf[1] && reqId != 0xFF)
                    throw new OmnicommException(OmnicommException.ExceptionReason.IDError);

                byte crc = DoCRC8(ansBuf, ansBuf.Length - 1);
                if (crc != ansBuf[ansBuf.Length - 1])
                    throw new OmnicommException(OmnicommException.ExceptionReason.CRCError);

                T = (sbyte)ansBuf[3];
                N = (ushort)BitConverter.ToUInt16(ansBuf, 4);
                F = (ushort)BitConverter.ToUInt16(ansBuf, 6);
            }
        }

        public class OmnicommException : Exception
        {
            public enum ExceptionReason { IDError, CRCError, LengthError, MasterError }

            public ExceptionReason Reason;

            public OmnicommException(ExceptionReason reason) :
                base()
            {
                Reason = reason;
            }
        }

        static public OmnicommResponce ReadOmnicommData(SerialPort port, byte id, bool isOneWire)
        {
            OmnicommRequest req = new OmnicommRequest(id, 0x06);

            try
            {
                byte[] reqBuf = req.ReqBuf;
                port.Write(reqBuf, 0, req.Length);

                if (isOneWire)
                {
                    int toSkip = 4;
                    while (toSkip-- > 0)
                        port.ReadByte();
                }

                int toRead = 9, cnt = 0;
                byte[] ansBuf = new byte[toRead];

                while (toRead > 0)
                {
                    int n = port.Read(ansBuf, cnt, toRead);
                    cnt += n;
                    toRead -= n;
                }

                return new OmnicommResponce(ansBuf, id);
            }
            catch (Exception)
            {
                throw;
            }
        }

        private static byte CRC8(byte b, byte crc)
        {
            byte i = 8;
	        do
	        {
		        if (((b ^ crc) & 0x01 ) > 0)
			        crc = (byte)(((crc ^ 0x18) >> 1) | 0x80);
			        else
				        crc >>= 1;
		        b >>= 1;
	        } while ((--i) > 0);

	        return(crc);
        }

        public static byte DoCRC8(byte[] pcBlock, int len)
        {
            byte crc = 0;
            for(int i = 0; i < len; i++)
            {
                crc = CRC8(pcBlock[i], crc);
            }

            return crc;
        }
    }
}
