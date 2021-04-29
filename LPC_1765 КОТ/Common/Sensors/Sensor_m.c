#include "Config.h"
#if(_SENSORS_)

#include <stdint.h>
#include "SensorCore.h"



SENSOR_INFO Fuel1Info;
SENSOR Fuel_1 = {.SensorName = "FUEL_SENSOR_1", .pSensorData = &Fuel1Info};


SENSOR_INFO Fuel2Info;
SENSOR Fuel_2 = {.SensorName = "FUEL_SENSOR_2", .pSensorData = &Fuel2Info};


void InitSensorSystem(void)
{
    AddSensor(&Fuel_1);
    AddSensor(&Fuel_2);
}


#endif
