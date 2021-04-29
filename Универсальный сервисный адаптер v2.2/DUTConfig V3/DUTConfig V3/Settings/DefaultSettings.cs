using System;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;

namespace DUTConfig_V3.Settings
{
    public class DefaultSettings
    {
        public DefaultSettings()
        {
            CurrentLanguage = "";
            LangsDesc = new List<Translation.TranslationDesc>();

            Port = "";
            Baudrate = 19200;
            ModbusID = 1;
        }

        [XmlIgnore]
        public List<Translation.TranslationDesc> LangsDesc { get; set; }
        [XmlIgnore]
        public string CurrentLanguage { get; set; }

        public string DefaultLanguage { get; set; }
        public string Port { get; set; }
        public ulong Baudrate { get; set; }
        public byte ModbusID { get; set; }
    }
}
