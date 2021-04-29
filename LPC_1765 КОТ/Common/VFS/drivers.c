#include <stdlib.h>
#include <string.h>

#include "drivers.h"
#include "VFS.h"
#include "DriversCore.h"
#include "portable.h"

extern DEVICE_LIST device_list;
VFS_MOUNT drivers =   {.name = "drivers",
                       .pNext        = NULL,
                       .private_data = NULL,
                       .open         = drv_VFS_open,
                       .close        = drv_VFS_close,
                       .read         = drv_VFS_read,
                       .write        = drv_VFS_write,
                       .fsync        = drv_VFS_fsync,
                       .mkdir        = drv_VFS_mkdir,
                       .rmdir        = drv_VFS_rmdir,
                       .fseek        = drv_VFS_fseek,
                       .stat         = drv_VFS_stat,
                       .mount        = drv_VFS_mount,
                       .unmount      = drv_VFS_unmount,
                       .rename       = drv_VFS_rename,
                       .readdir      = drv_VFS_readdir,
                       .opendir      = drv_VFS_opendir,
                       .closedir     = drv_VFS_closedir,
					   .getfree      = drv_VFS_get_free
					   ,
                      };

int drv_VFS_open(FILE_HANDLE file, const char *name, uint8_t attr)
{
    int Res;
    file->private_data = (void *)(OpenDevice(name + 1));
    if(file->private_data == NULL) return -1;
    Res = SeekDeviceText(file->private_data, 0, 2);
    if(Res == -1) return -1;
    file->size = Res;
    SeekDeviceText(file->private_data, 0, 0);
    if(file->private_data == NULL) return -1;
    else return 0;
}

int drv_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{   
    int Res;
 
    //if(file->attrib == VFS_ATTR_RW) 
  //      Res = ReadDevice((DEVICE_HANDLE *)file->private_data, (void *)buf, (int)count);
    //else if(file->attrib == VFS_ATTR_RW_TEXT)
        Res = ReadDeviceText((DEVICE_HANDLE *)file->private_data, (void *)buf, (int)count);
        if(Res == -1) return -1;
    //else return -1;

    file->l = Res;

    return 0;
}

int drv_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    int Res;

   // if(file->attrib == VFS_ATTR_RW) 
     //   Res = WriteDevice((DEVICE_HANDLE *)file->private_data, (void *)buf, (int)count);
   // else if(file->attrib == VFS_ATTR_RW_TEXT)
        Res = WriteDeviceText((DEVICE_HANDLE *)file->private_data, (void *)buf, (int)count);
    //else return -1;
    if(Res == -1) return -1;


    file->l = Res;

    return 0;
}

int drv_VFS_fseek(FILE_HANDLE file, uint32_t offset)
{
//    int Res;
    file->offset = offset;
   // if(file->attrib == VFS_ATTR_RW) 
     //   Res = SeekDevice((DEVICE_HANDLE *)file->private_data, (int)offset, (uint8_t)file->offset);
   // else if(file->attrib == VFS_ATTR_RW_TEXT)
        SeekDeviceText((DEVICE_HANDLE *)file->private_data, (int)offset, 0);
    //else return -1;

    return 0;
}

int drv_VFS_close(FILE_HANDLE file)
{
    CloseDevice((DEVICE_HANDLE *)file->private_data);
	return 0;
}

int drv_VFS_fsync(FILE_HANDLE file)
{
    return -1;
}

int drv_VFS_mkdir(const char *dir_name)
{
    return -1;
}

int drv_VFS_rmdir(const char *dir_name)
{
    return -1;
}
int drv_VFS_get_free(uint32_t *count)
{
	return -1;
}

int drv_VFS_stat(char *name, STAT_HANDLE stat)
{
    if( is_root(name) == 0 ) 
    {
        stat->attrib = AM_DIR;
        stat->size = 0;
        return 0;
    }

    //if( is_device_opened(name, pDevice) == 0 )
   // {
        
       // stat->attrib = AM_SYS;
        //stat->size = SeekDeviceText(pDevice, 0, 2);
       // SeekDeviceText(pDevice, 0, 0);
      //  return -1;
   // }

    return -1;
}

int drv_VFS_mount(char *name)
{
    return -1;
}

int drv_VFS_unmount(char *name)
{
    return -1;
}

int drv_VFS_rename(char *old_name, char *new_name)
{
    return -1;
}

int drv_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir)
{
	static DRIVER_LIST_ITEM  *pItem;
	DEVICE_HANDLE pDev;;
    static uint8_t j = 0;

    if( j == 0 ) pItem = device_list.pFirstDevice;
    
    DEVICE_IO *pDevice_io = pItem->pDevice_io;

    if( j <  device_list.NumberOfItems)
    {

    	if(pItem->pDevice_io != NULL)
		{
            strcpy(file->name, pDevice_io->DeviceName);
			pDev = OpenDevice(pDevice_io->DeviceName);
			if(pDev != NULL)
			{
	            file->attrib = AM_RDO;
	            file->size = SeekDeviceText(pDev, 0, 2); /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
				CloseDevice(pDev);
			}
            pItem = pItem->pNext;
            j++;
		}
        else
        {
            pItem = pItem->pNext;
            j++;
        }
    }
    else 
    {
        file->name[0] = NULL;
        j = 0;
    }
    
    return 0;
}

int drv_VFS_opendir(DIR_HANDLE dir, char *path)
{
    if( is_root(path) == 0 ) return 0;
    return -1;
}

int drv_VFS_closedir(DIR_HANDLE dir)
{
    return 0;
}

/*int is_device_opened(char *name, DEVICE *pDevice)
{
    pDevice = NULL; 

    pDevice = device_list.pFirstDevice;
    DEVICE_IO *pDevice_io = (DEVICE_IO *)pDevice->device_io;

    while( pDevice != NULL )
    {
        if( pDevice->flDeviceOpen != DEVICE_CLOSE )
            if( strcmp(pDevice_io->DeviceName, name + 1 ) == 0 ) return 0;
            
        pDevice = pDevice->pNext;
    }
    return -1;
}	*/

char *get_filename(char *path)
{
    return strrchr(path,'/');
}
