#ifndef LOG_H
#define LOG_H

#define         CLOCK_DATA_LENGTH       16


//-**************************Вид сообщений******************************

#define   COMMON_MES      0
#define   MODEM_MES       2
#define   FTP_MES         1

//-*********************************************************************

typedef struct tagLogInfo
{
   // uint32_t current_file_size;   //  Текущий размер файла лога
    uint8_t  com_mes;
    uint8_t  modem_mes;
    uint8_t  ftp_mes;
    uint8_t  com_mes_level;
    uint8_t  modem_mes_level;
    uint8_t  ftp_mes_level;
    uint8_t  DateBuf[CLOCK_DATA_LENGTH + 5];  //  Буфер для записи времени
}s_LogInfo;

typedef struct
{
	uint16_t currentPos;
	uint16_t newestPos;
	uint16_t oldestPos;
	uint16_t numberOfRecords;
} __attribute__ ((__packed__)) LOG_COUNTERS;

int AddLogMessage(uint8_t *message, uint8_t type, uint8_t level);

void GetArchInfo(uint16_t *numberOfRecords, uint16_t *currentNumber);
void GoToTheNewestRecord(void);
void GetLogRecord(char *buf);

int InitLog(void);
int ClearLog(void);

#endif
