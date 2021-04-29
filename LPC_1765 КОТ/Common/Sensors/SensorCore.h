#include "Config.h"
#if(_SENSORS_)

#ifndef SENSORS_CORE_H
#define SENSORS_CORE_H
#include "DriversCore.h"

#include <stdint.h>

// ћаксимальна€ длина имени устройства
#define SENSOR_NAME_MAX_LENGHT   15
#define SENSOR_MAX_IN_BUF        20
#define SENSOR_MAX_OUT_BUF       5

#define MAX_SENSOR_DATA_TEXT_OFFSET    36 

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_OK     0
#define SENSOR_ERROR  1
#define SENSOR_OPEN   1
#define SENSOR_CLOSE  0

typedef void* SENSOR_HANDLE;



// —труктура описывающа€ список устройств
typedef struct tagSensorList
{
    uint8_t NumberOfItems;
    struct tagSensor *pFirstSensor;
}SENSOR_LIST;

typedef float (*SENSOR_PROC_FUNC)(SENSOR_HANDLE handle);
typedef int (*INIT_PROC )(SENSOR_HANDLE handle, char *buf, uint16_t count);

//typedef int (*SENSOR_IOCTL)(SENSOR_HANDLE handle, uint8_t code, uint8_t *pData);

typedef struct __attribute__ ((__packed__)) tagProcInfo
{
    uint16_t *Buf;
    float *Data;
    uint16_t Depth;
    uint16_t Flag;
}PROC_INFO;


typedef struct __attribute__ ((__packed__)) tagSensorInfo
{
    float Data;
    float PhysValue;
    PROC_INFO PrivatDataProc;
    PROC_INFO PrivatPhysProc;
    uint16_t TextDataPoint;
//    #if _MULTITASKING_
//    xSemaphoreHandle Sem;
//    #endif // _MULTITASKING_
	uint8_t f_valid_data;
}SENSOR_INFO;



typedef struct __attribute__ ((__packed__)) tagSensor
{
    uint8_t SensorID;            //   
    SENSOR_LIST *pSensorList;    //   
    struct tagSensor *pNext;     //
    DEVICE_HANDLE  SensorDevise; //
    SENSOR_PROC_FUNC  DataProcSensor;  //
    SENSOR_PROC_FUNC  PhysProcSensor;  //
    INIT_PROC         DataInit;
    INIT_PROC         PhysInit;
    char SensorName[ SENSOR_NAME_MAX_LENGHT ];     //   
    uint8_t flSensorOpen;                          // 
    void *pSensorData;                   //         

}SENSOR;





SENSOR_HANDLE OpenSensorProc(char *name);
SENSOR_HANDLE OpenSensor(char *name);
int ReadSensor(SENSOR_HANDLE handle, void *pDst, int count);
int ReadSensorText(SENSOR_HANDLE handle, void *pSrc, int count);
int SeekSensorText(SENSOR_HANDLE handle, uint16_t offset, uint8_t origin);
float DataProcSensor(SENSOR_HANDLE handle);
float PhysProcSensor(SENSOR_HANDLE handle);
int DataInit(SENSOR_HANDLE handle, char *buf, uint16_t count);
int PhysInit(SENSOR_HANDLE handle, char *buf, uint16_t count);
int SensorIOCtl(SENSOR_HANDLE handle, uint8_t code, uint8_t *pData);

void PoolSensor(void);
uint8_t AddSensor(SENSOR *pSensor);
void critical_error(char * text);
void DeleteAllSensors(void);




#define   SENSOR_DEVICE       0
#define   DATA_PROC           1
#define   PHYS_PROC           2
#define   INIT_DATA_PROC      3
#define   PHYS_PROC_OUTBUF    4
#define   INIT_PHYS_PROC      5
#define   ALL_SETTINGS        6


void InitSensCheck(void);
//void CheckSensors(void);
extern SENSOR_HANDLE AttachSensor(char *name);
extern void InitSensorSystem(void);

uint8_t GetNumberOfSensors(void);
SENSOR_HANDLE GetFirstSensor(void);

#ifdef __cplusplus
}
#endif

#endif
#endif


