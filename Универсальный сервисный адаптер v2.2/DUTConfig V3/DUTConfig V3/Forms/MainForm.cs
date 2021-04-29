using System;
using System.Xml.Serialization;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DUTConfig_V3.Settings;

namespace DUTConfig_V3.Forms
{
    public partial class MainForm : Form
    {
        DefaultSettings defSettings;
        Translation currentTranslation;
        string appDir;

        public MainForm()
        {
            InitializeComponent();
            this.Text = Properties.Resources.AppName + " " + Properties.Resources.AppVersion;

            appDir = AppSettings.AppFolder;

            miCalibAnalogSensor.Click += new EventHandler((s, e) => DoCalibration(CalibrationForm.AnalogSensorType.TypeAnalog));
            miCalibFreqSensor.Click += new EventHandler((s, e) => DoCalibration(CalibrationForm.AnalogSensorType.TypeFreq));
        }

        #region ON LOAD/CLOSE

        private void MainForm_Load(object sender, EventArgs e)
        {
            CheckAppFoldersAndFiles();
            LoadDefault();
        }

        private void CheckAppFoldersAndFiles()
        {
            try
            {
                if (!Directory.Exists(appDir))
                    Directory.CreateDirectory(appDir);

                

                if (!File.Exists(appDir + Properties.Resources.DefSettingsFile))
                {
                    DefaultSettings settings = new DefaultSettings()
                    {
                        Baudrate = 19200,
                        ModbusID = 1,
                        Port = "",
                        DefaultLanguage = Properties.Resources.DefaultLanguage
                    };

                    AppSettings.SaveSettingsToXML<DefaultSettings>(appDir + Properties.Resources.DefSettingsFile, settings);
                }

                if (!File.Exists(appDir + Properties.Resources.ThermoStockFile))
                {
                    Termo_correction thermo = new Termo_correction();

                    AppSettings.SaveSettingsToXML<Termo_correction>(appDir + Properties.Resources.ThermoStockFile, thermo);
                }

                if (!File.Exists(appDir + Properties.Resources.PointsFile))
                {
                    etc.Points points = new etc.Points()
                    {

                    };

                    AppSettings.SaveSettingsToXML<etc.Points>(appDir + Properties.Resources.PointsFile, points);
                }
            }
            catch { }
        }

        private void LoadDefault()
        {
            try
            {
                defSettings = AppSettings.GetSettingsFromXML<DefaultSettings>(appDir + Properties.Resources.DefSettingsFile);
            }
            catch (Exception ex)
            {
                defSettings = new DefaultSettings();
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            try
            {
                // он есть всегда
                ToolStripMenuItem item = ddbLanguage.DropDownItems.Add(Properties.Resources.DefaultLanguage) as ToolStripMenuItem;
                if (defSettings.DefaultLanguage == Properties.Resources.DefaultLanguage)
                    item.Checked = true;

                // ищем переводы в папке
                DirectoryInfo dInfo = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory + "\\lang");

                foreach (FileInfo langInfo in dInfo.GetFiles("*.lang"))
                {
                    try
                    {
                        Translation trans = AppSettings.GetSettingsFromXML<Translation>(langInfo.FullName);

                        if (trans.LanguageName == Properties.Resources.DefaultLanguage)
                            continue;

                        defSettings.LangsDesc.Add(new Translation.TranslationDesc()
                        {
                            Language = trans.LanguageName,
                            PathToFile = langInfo.FullName
                        });
                    }
                    catch
                    {
                    }
                }

                string pathToTrans = "";
                foreach (Translation.TranslationDesc d in defSettings.LangsDesc)
                {
                    item = ddbLanguage.DropDownItems.Add(d.Language) as ToolStripMenuItem;
                    if (d.Language == defSettings.DefaultLanguage)
                    {
                        pathToTrans = d.PathToFile;
                        item.Checked = true;
                    }
                }

                if (pathToTrans != "")
                {
                    LoadTranslation(pathToTrans);
                    defSettings.CurrentLanguage = defSettings.DefaultLanguage;
                }
                else
                {
                    defSettings.CurrentLanguage = Properties.Resources.DefaultLanguage;
                    LoadDefaultTranslation();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void LoadDefaultTranslation()
        {
            currentTranslation = AppSettings.GetSettingsFromStringXML<Translation>(Properties.Resources.russian);

            TranslateManual(currentTranslation);
            currentTranslation.SetCurrent(currentTranslation);
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (defSettings.CurrentLanguage != "")
                    defSettings.DefaultLanguage = defSettings.CurrentLanguage;

                AppSettings.SaveSettingsToXML<DefaultSettings>(appDir + Properties.Resources.DefSettingsFile, defSettings);
            }
            catch (AppSettings.SettingsException ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        #endregion

        #region TRANSLATION

        private void LoadTranslation(string path)
        {
            try
            {
                currentTranslation = AppSettings.GetSettingsFromXML<Translation>(path);

                TranslateManual(currentTranslation);
                currentTranslation.SetCurrent(currentTranslation);
            }
            catch
            {
                throw;
            }
        }

        private void TranslateManual(Translation trans)
        {
            btInterfaceSensor.Text = trans.btInterfaceSensorText;
            btAnalogSensor.Text = trans.btAnalogSensorText;
            btFreqSensor.Text = trans.btFreqSensorText;
            lbChooseSensor.Text = trans.lbChooseSensorText;

            ddbLanguage.Text = trans.ddbLanguageText;
            ddbCalibration.Text = trans.ddbCalibrationText;
            miCalibAnalogSensor.Text = trans.miCalibAnalogSensor;
            miCalibFreqSensor.Text = trans.miCalibFreqSensor;
        }

        private void ddbLanguage_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            if (defSettings.CurrentLanguage == e.ClickedItem.Text)
                return;

            foreach (ToolStripMenuItem i in ddbLanguage.DropDownItems)
                i.Checked = false;

            (e.ClickedItem as ToolStripMenuItem).Checked = true;

            if (e.ClickedItem.Text == Properties.Resources.DefaultLanguage)
            {
                // загружаем русский из ресурсов
                LoadDefaultTranslation();
                defSettings.CurrentLanguage = e.ClickedItem.Text;
                return;
            }

            Translation.TranslationDesc desc = defSettings.LangsDesc.Find(delegate(Translation.TranslationDesc d)
            {
                return d.Language == e.ClickedItem.Text;
            });

            if (desc == null)
            {
                MessageBox.Show(string.Format(currentTranslation.mesNoLangInfo, e.ClickedItem.Text),
                    Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            try
            {
                LoadTranslation(desc.PathToFile);
                defSettings.CurrentLanguage = e.ClickedItem.Text;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        #endregion

        #region SENSOR TYPES

        private void btInterfaceSensor_Click(object sender, EventArgs e)
        {
            Hide();

            try
            {
                InterfaceForm form = new InterfaceForm(defSettings, currentTranslation);
                form.ShowDialog();
                form.Dispose();
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

            Show();
        }

        private void btAnalogSensor_Click(object sender, EventArgs e)
        {
            Hide();

            try
            {
                AnalogForm form = new AnalogForm(AnalogForm.AnalogSensorType.TypeAnalog, defSettings, currentTranslation);
                form.ShowDialog();
                form.Dispose();
            }
            catch
            {
            }

            Show();
        }

        private void btFreqSensor_Click(object sender, EventArgs e)
        {
            Hide();

            try
            {
                AnalogForm form = new AnalogForm(AnalogForm.AnalogSensorType.TypeFreq, defSettings, currentTranslation);
                form.ShowDialog();
                form.Dispose();
            }
            catch
            {
            }

            Show();
        }

        #endregion

        #region CALIBRATION

        private void DoCalibration(CalibrationForm.AnalogSensorType type)
        {
            Hide();

            CalibrationForm form = new CalibrationForm(type, defSettings);
            form.ShowDialog();
            form.Dispose();

            Show();
        }

        #endregion
    }
}
