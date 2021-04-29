#include "Config.h"
#if(_SENSORS_)

#include <stdlib.h>
#include <string.h>

#include "sensors.h"
#include "VFS.h"
#include "SensorCore.h"
#include "portable.h"

extern SENSOR_LIST sensor_list;

VFS_MOUNT sensors_VFS =   {.name = "sensors",
                       .pNext        = NULL,
                       .private_data = NULL,
                       .open         = sens_VFS_open,
                       .close        = sens_VFS_close,
                       .read         = sens_VFS_read,
                       .write        = sens_VFS_write,
                       .fsync        = sens_VFS_fsync,
                       .mkdir        = sens_VFS_mkdir,
                       .rmdir        = sens_VFS_rmdir,
                       .fseek        = sens_VFS_fseek,
                       .stat         = sens_VFS_stat,
                       .mount        = sens_VFS_mount,
                       .unmount      = sens_VFS_unmount,
                       .rename       = sens_VFS_rename,
                       .readdir      = sens_VFS_readdir,
                       .opendir      = sens_VFS_opendir,
                       .closedir     = sens_VFS_closedir,
					   .getfree		 = sens_VFS_get_free
                      };

int sens_VFS_open(FILE_HANDLE file, const char *name, uint8_t attr)
{
    int Res;
    file->private_data = (SENSOR_HANDLE *)(OpenSensorProc(name + 1));
    if(file->private_data == NULL) return -1;
    Res = SeekSensorText(file->private_data, 0, 2);
    if(Res == -1) return -1;
    file->size = Res;
    Res = SeekSensorText(file->private_data, 0, 0);
    if(Res == -1) return -1;
    if(file->private_data == NULL) return -1;
    else return 0;
}

int sens_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{   
    int Res;
 
    //if(file->attrib == VFS_ATTR_RW) 
  //      Res = ReadDevice((DEVICE_HANDLE *)file->private_data, (void *)buf, (int)count);
    //else if(file->attrib == VFS_ATTR_RW_TEXT)
        Res = ReadSensorText((SENSOR_HANDLE *)(file->private_data), buf, (int)count);
        if(Res == -1) return -1;
    //else return -1;

    file->l = Res;

    return 0;
}

int sens_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    return -1;
}

int sens_VFS_get_free(uint32_t *count)
{
	return -1;
}

int sens_VFS_fseek(FILE_HANDLE file, uint32_t offset)
{
//    int Res;
    file->offset = offset;
   // if(file->attrib == VFS_ATTR_RW) 
     //   Res = SeekDevice((DEVICE_HANDLE *)file->private_data, (int)offset, (uint8_t)file->offset);
   // else if(file->attrib == VFS_ATTR_RW_TEXT)
       SeekSensorText((SENSOR_HANDLE *)(file->private_data), (int)offset, 0);
    //else return -1;

    return 0;
}

int sens_VFS_close(FILE_HANDLE file)
{
    return 0;
}

int sens_VFS_fsync(FILE_HANDLE file)
{
    return -1;
}

int sens_VFS_mkdir(const char *dir_name)
{
    return -1;
}

int sens_VFS_rmdir(const char *dir_name)
{
    return -1;
}

int sens_VFS_stat(char *name, STAT_HANDLE stat)
{
    SENSOR *pSensor = NULL;
    if( is_root(name) == 0 ) 
    {
        stat->attrib = AM_DIR;
        stat->size = 0;
        return 0;
    }

    if( is_sensor_opened(name, pSensor) == 0 )
    {
        
       // stat->attrib = AM_SYS;
        //stat->size = SeekDeviceText(pDevice, 0, 2);
       // SeekDeviceText(pDevice, 0, 0);
        return -1;
    }

    return -1;
}

int sens_VFS_mount(char *name)
{
    return -1;
}

int sens_VFS_unmount(char *name)
{
    return -1;
}

int sens_VFS_rename(char *old_name, char *new_name)
{
    return -1;
}

int sens_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir)
{
    static SENSOR *pSensor = NULL; 
    static uint8_t j = 0;

    if( j == 0 ) pSensor = sensor_list.pFirstSensor;

    if( j <  sensor_list.NumberOfItems)
    {
        if(pSensor->flSensorOpen != SENSOR_CLOSE) 
        {
            strcpy(file->name, pSensor->SensorName);
            file->attrib = AM_RDO;
            file->size = 10;//SeekSensorText(pSensor, 0, 2); /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
            pSensor = pSensor->pNext;
        }
        j++;
    }
    else 
    {
        file->name[0] = NULL;
        j = 0;
    }
    
    return 0;
}

int sens_VFS_opendir(DIR_HANDLE dir, char *path)
{
    if( is_root(path) == 0 ) return 0;
    return -1;
}

int sens_VFS_closedir(DIR_HANDLE dir)
{
    return 0;
}

int is_sensor_opened(char *name, SENSOR *pSensor)
{
    pSensor = NULL; 

    pSensor = sensor_list.pFirstSensor;

    while( pSensor != NULL )
    {
        if( pSensor->flSensorOpen != SENSOR_CLOSE )
            if( strcmp(pSensor->SensorName, name + 1 ) == 0 ) return 0;
            
        pSensor = pSensor->pNext;
    }
    return -1;
}

#endif




