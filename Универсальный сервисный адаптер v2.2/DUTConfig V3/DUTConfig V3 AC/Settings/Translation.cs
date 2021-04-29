using System;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;

namespace DUTConfig_V3.Settings
{
    public class Translation
    {
        [XmlIgnore]
        public static Translation Current { get; private set; }
        public void SetCurrent(Translation current)
        {
            Current = current;
        }

        public class TranslationDesc
        {
            public string Language;
            public string PathToFile;
        }

        public string LanguageName { get; set; }

        public string btInterfaceSensorText { get; set; }
        public string btAnalogSensorText { get; set; }
        public string btFreqSensorText { get; set; }
        public string lbChooseSensorText { get; set; }
        public string ddbLanguageText { get; set; }
        public string ddbCalibrationText { get; set; }
        public string miCalibAnalogSensor { get; set; }
        public string miCalibFreqSensor { get; set; }
        public string mesNoLangInfo { get; set; }

        public string modbusNoAnswerError { get; set; }
        public string modbusHostIDError { get; set; }
        public string modbusCRCError { get; set; }
        public string modbusBufferLenError { get; set; }
        public string modbusIllegalError { get; set; }
        public string modbusUnknownError { get; set; }

        public string omniHostIDError { get; set; }
        public string omniCRCError { get; set; }
        public string omniBufferLenError { get; set; }
        public string omniMasterError { get; set; }

        public string btBackToMainText { get; set; }
        public string btConnectText { get; set; }
        public string btDisconnectText { get; set; }
        public string lbPortText { get; set; }
        public string lbModbusIDText { get; set; }
        public string lbBaudrateText { get; set; }
        public string lbValueTypeVText { get; set; }
        public string lbValueTypeFText { get; set; }
        public string StatusLabelConnectOnText { get; set; }
        public string StatusLabelConnectOffText { get; set; }
        public string miFileText { get; set; }
        public string miFileSaveText { get; set; }
        public string miFileLoadText { get; set; }
        public string miViewText { get; set; }
        public string miViewStandartText { get; set; }
        public string miViewExtendedText { get; set; }
        public string miChangeFirmwareText { get; set; }
        public string miThermoText { get; set; }
        public string lbEngineStateText { get; set; }
        public string lbEngineStateOnText { get; set; }
        public string lbEngineStateOffText { get; set; }
        public string lbPowerSupplyText { get; set; }
        public string lbSensorReadingsText { get; set; }
        public string lbImmersionLevelText { get; set; }
        public string lbInnerCoreAvgText { get; set; }
        public string lbInnerCoreText { get; set; }
        public string lbTempSensorOnText { get; set; }
        public string lbTempText { get; set; }
        public string lbApprTypeText { get; set; }
        public string lbApprSizeText { get; set; }
        public string lbAvgTypeText { get; set; }

        public string lbAvgIntervalText { get; set; }
        public string lbDeltaPowerSupplyText { get; set; }
        public string lbAvgAdaptEngineOnTimeIntervalText { get; set; }
        public string lbAvgAdaptEngineOffTimeIntervalText { get; set; }

        public string lbFullTankFreqText { get; set; }
        public string lbEmptyTankFreqText { get; set; }
        public string lbFreqRangeText { get; set; }
        public string lbErrorCodeText { get; set; }
        public string lbSWVersionText { get; set; }
        public string lbOmniAutoOnText { get; set; }
        public string lbOmniAutoPeriodText { get; set; }
        public string lbOmniNetModeText { get; set; }
        public string lbOmniMaxNText { get; set; }
        public string lbOmniCurrNText { get; set; }
        public string gbParamsText { get; set; }
        public string gbOmnicommText { get; set; }

        public string btTableClearText { get; set; }
        public string btTankEmptyText { get; set; }
        public string btTankFullText { get; set; }

        public string TempSensorOnText { get; set; }
        public string TempSensorOffText { get; set; }
        public string ApprTypeLinearText { get; set; }
        public string ApprTypePolyText { get; set; }
        public string AvgTypeRunText { get; set; }
        public string AvgTypeExpText { get; set; }
        public string AvgTypeAdaptText { get; set; }
        public string OmniAutoNoneText { get; set; }
        public string OmniAutoASCIIText { get; set; }
        public string OmniAutoBinText { get; set; }
        public string OmniNetOnlineText { get; set; }
        public string OmniNetOfflineText { get; set; }
        public string gbPolyText { get; set; }
        public string btEditText { get; set; }
        public string btEditCancelText { get; set; }
        public string btEditOkText { get; set; }
        public string ReadingActiveMessage { get; set; }
        public string WritingActiveMessage { get; set; }
        public string WriteDoneMessage { get; set; }

        public string ApprTableDataErrorMessage { get; set; }
        public string ApprTableTooShortMessage { get; set; }
        public string WritePolyRequest { get; set; }

        public string TarImmersHeaderText { get; set; }
        public string TarVolumeHeaderText { get; set; }
        public string PolyCoeffHeaderText { get; set; }
        public string PolyValueHeaderText { get; set; }

        public string GraphUserChartLabel { get; set; }
        public string GraphApproxChartLabel { get; set; }
        public string GraphMenuSaveAs { get; set; }
        public string GraphMenuPageSetup { get; set; }
        public string GraphMenuPrint { get; set; }
        public string GraphMenuPointValues { get; set; }
        public string GraphMenuUnZoom { get; set; }
        public string GraphMenuDefaultScale { get; set; }

        public string DUTParamsDamage { get; set; }
        public string DUTParamsError { get; set; }
        public string Error0 { get; set; }
        public string ErrorX { get; set; }
        public string Error300 { get; set; }
        public string Error320 { get; set; }
        public string Error340 { get; set; }
        public string Error360 { get; set; }
        public string Error380 { get; set; }
        public string Error400 { get; set; }
        public string Error420 { get; set; }
        public string Error440 { get; set; }

        public string btOpenFirmFileText { get; set; }
        public string btDownloadFirmText { get; set; }
        public string FirmCRCErrorMessage { get; set; }
        public string InvalidAnsErrorMessage { get; set; }
        public string FirmFileNotSpecifiedErrorMessage { get; set; }

        public string ThermoFileOpenFailMessage { get; set; }
        public string ThermoFileSaveFailMessage { get; set; }

        public string ThermoFormText { get; set; }
        public string ThermoCalibModeText { get; set; }
        public string ThermoCalibRangeText { get; set; }
        public string ThermoCalibTableText { get; set; }
        public string ThermoCalibTableColumnNumText { get; set; }
        public string ThermoCalibTableColumnFreqText { get; set; }
        public string ThermoPolyPowText { get; set; }
        public string ThermoCurrentCoeffsText { get; set; }
        public string ThermoCoeffsTableColumnPowText { get; set; }
        public string ThermoCoeffsTableColumnCoeffText { get; set; }

        public string ThermoClearText { get; set; }
        public string ThermoPolyCoeffsText { get; set; }
        public string ThermoLoadFileText { get; set; }
        public string ThermoSaveFileText { get; set; }
        public string ThermoCalcCoeffsText { get; set; }
        public string ThermoWriteText { get; set; }
        public string ThermoReadText { get; set; }
        public string ThermoWriteDefaultText { get; set; }
        public string ThermoChartXLabelText { get; set; }
        public string ThermoChartYLabelText { get; set; }
        public string ThermoChartYReadingLabelText { get; set; }
        public string ThermoChartTitleText { get; set; }
        public string ThermoChartReadedText { get; set; }
        public string ThermoChartRestoredText { get; set; }
        public string ThermoChartResultText { get; set; }
        public string ThermoCalcFailMessage { get; set; }
        public string ThermoPolyFailMessage { get; set; }

        public string SMSParamsText { get; set; }
        public string SMSIDText { get; set; }
        public string SMSBaudrateText { get; set; }
        public string SMSTimeoutText { get; set; }
        public string SMSRequestText { get; set; }
        public string SMSResponceText { get; set; }
        public string SMSCheckText { get; set; }
        public string SMSGenerateText { get; set; }

        public string CalibFormText { get; set; }
        public string btDoCalibText { get; set; }
        public string btCancelCalibText { get; set; }
        public string calibStageTimeoutMessage { get; set; }
        public string calibShortcutReqMessage { get; set; }
        public string calibShortcutRemoveMessage { get; set; }
        public string calibStage1Message { get; set; }
        public string calibStage2Message { get; set; }
        public string calibStage3Message { get; set; }
        public string calibStage4Message { get; set; }
        public string calibStage5Message { get; set; }
        public string calibStage6Message { get; set; }

    }
}
