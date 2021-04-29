#include <stdlib.h>
#include <string.h>

#include "saver_fs.h"
#include "saver.h"
#include "VFS.h"
#include "DriversCore.h"
#include "portable.h"

extern DEVICE_LIST device_list;
VFS_MOUNT saver_fs =   {.name = "saver",
                       .pNext        = NULL,
                       .private_data = NULL,
                       .open         = svr_VFS_open,
                       .close        = svr_VFS_close,
                       .read         = svr_VFS_read,
                       .write        = svr_VFS_write,
                       .fsync        = svr_VFS_fsync,
                       .mkdir        = svr_VFS_mkdir,
                       .rmdir        = svr_VFS_rmdir,
                       .fseek        = svr_VFS_fseek,
                       .stat         = svr_VFS_stat,
                       .mount        = svr_VFS_mount,
                       .unmount      = svr_VFS_unmount,
                       .rename       = svr_VFS_rename,
                       .readdir      = svr_VFS_readdir,
                       .opendir      = svr_VFS_opendir,
                       .closedir     = svr_VFS_closedir,
					   .getfree		 = svr_VFS_get_free
                      };

static uint8_t seek_flag = 0;

int svr_VFS_open(FILE_HANDLE file, const char *name, uint8_t attr)
{
    SAVER_FS_INFO *pFile = GetSaverFile(name + 1);
    file->private_data = (void *)pFile;
	file->size = pFile->size;
	file->attrib = pFile->seg;
    if(file->private_data == NULL) return -1;
    else return 0;
}

int svr_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{   
    int Res;
 
    DEVICE_HANDLE hSaver;

	hSaver = OpenDevice("SAVER");

	if(seek_flag == 1)
	{
		 SeekDeviceText(hSaver, file->offset, file->attrib);
		 seek_flag = 0;
	}  

    Res = ReadDeviceText(hSaver, (void *)buf, (int)count);
    if(Res == -1)
	{
		CloseDevice(hSaver);
		return -1;
	} 

    file->l = Res;

	CloseDevice(hSaver);
    return 0;
}

int svr_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    return 0;
}


int svr_VFS_get_free(uint32_t *count)
{
	return -1;
}

int svr_VFS_fseek(FILE_HANDLE file, uint32_t offset)
{
    file->offset = offset;

	seek_flag = 1;

    return offset;
}

int svr_VFS_close(FILE_HANDLE file)
{
	return 0;
}

int svr_VFS_fsync(FILE_HANDLE file)
{
    return -1;
}

int svr_VFS_mkdir(const char *dir_name)
{
    return -1;
}

int svr_VFS_rmdir(const char *dir_name)
{
    return -1;
}

int svr_VFS_stat(char *name, STAT_HANDLE stat)
{
    if( is_root(name) == 0 ) 
    {
        stat->attrib = AM_DIR;
        stat->size = 0;
        return 0;
    }
    return -1;
}

int svr_VFS_mount(char *name)
{
    return -1;
}

int svr_VFS_unmount(char *name)
{
    return -1;
}

int svr_VFS_rename(char *old_name, char *new_name)
{
    return -1;
}

int svr_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir)
{
	static SAVER_FS_INFO  *pFile;
    static uint8_t j = 0;

    if( j == 0 ) pFile = first_file;
    

	if( pFile !=  NULL)
    {

            strcpy(file->name, pFile->name);
	        file->attrib = AM_RDO;
	        file->size = pFile->size; /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
            pFile = pFile->pNext;
            j++;
    }
    else 
    {
        file->name[0] = NULL;
        j = 0;
    }
    
    return 0;
}

int svr_VFS_opendir(DIR_HANDLE dir, char *path)
{
    if( is_root(path) == 0 ) return 0;
    return -1;
}

int svr_VFS_closedir(DIR_HANDLE dir)
{
    return 0;
}


