using System;
using System.Xml.Serialization;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace DUTConfig_V3.Settings
{
    class AppSettings
    {
        public class SettingsException : Exception
        {
            public SettingsException(string message) :
                base(message)
            {
            }

            public SettingsException() :
                base()
            {
            }
        }

        public static string AppFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                Properties.Resources.CompanyName + "\\" +
                Properties.Resources.AppName + "V3\\");

        public static T GetSettingsFromStringXML<T>(string file)
           where T : new()
        {
            MemoryStream stream = new MemoryStream(Encoding.UTF8.GetBytes(file));

            try
            {
                XmlSerializer xml = new XmlSerializer(typeof(T));

                T settings = (T)xml.Deserialize(stream);
                return settings;
            }
            catch (Exception)
            {
                throw new SettingsException("Не удалось загрузить втроенный перевод");
            }
            finally
            {
                if (stream != null)
                    stream.Close();
            }
        }

        public static T GetSettingsFromXML<T>(string path)
            where T : new()
        {
            Stream stream = null;

            try
            {
                stream = File.OpenRead(path);
                XmlSerializer xml = new XmlSerializer(typeof(T));

                T settings = (T)xml.Deserialize(stream);
                return settings;
            }
            catch (ArgumentException)
            {
                if (path == null || path == "")
                    throw new SettingsException("Путь к файлу настройки не задан");
                else
                    throw new SettingsException("Не удалось найти файл " + path);
            }
            catch (PathTooLongException)
            {
                throw new SettingsException(string.Format("Имя файла настройки ({0}) слишком длинное", path));
            }
            catch (IOException)
            {
                throw new SettingsException("Не удалось найти файл " + path);
            }
            catch (UnauthorizedAccessException)
            {
                throw new SettingsException("Нет доступа к файлу " + path);
            }
            catch (Exception)
            {
                throw new SettingsException("Не удалось десереализовать файл " + path);
            }
            finally
            {
                if (stream != null)
                    stream.Close();
            }
        }

        public static void SaveSettingsToXML<T>(string path, T settings)
            where T : new()
        {
            Stream stream = null;

            try
            {
                stream = File.Create(path);
                XmlSerializer xml = new XmlSerializer(typeof(T));
                xml.Serialize(stream, settings);
            }
            catch (ArgumentException)
            {
                if (path == null || path == "")
                    throw new SettingsException("Путь к файлу настройки не задан");
                else
                    throw new SettingsException("Не удалось найти файл " + path);
            }
            catch (PathTooLongException)
            {
                throw new SettingsException(string.Format("Имя файла настройки ({0}) слишком длинное", path));
            }
            catch (IOException)
            {
                throw new SettingsException("Не удалось найти файл " + path);
            }
            catch (UnauthorizedAccessException)
            {
                throw new SettingsException("Нет доступа на запись к файлу " + path);
            }
            catch (Exception)
            {
                throw new SettingsException("Не удалось сереализовать настройки в файл " + path);
            }
            finally
            {
                if (stream != null)
                    stream.Close();
            }
        }
    }
}
