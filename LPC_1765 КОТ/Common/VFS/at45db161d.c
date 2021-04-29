#include "config.h"

#include <stdlib.h>
#include <string.h>
#include "at45db161d.h"
#include "VFS.h"
#include "ff.h"
#include "portable.h"


FATFS fat12_fs;

VFS_MOUNT fat_at45db = {.name = "at45db",
                        .pNext = NULL,
                        .private_data = NULL,
                        .open = at45_VFS_open,
                        .close = at45_VFS_close,
                        .read = at45_VFS_read,
                        .write = at45_VFS_write,
						.truncate = at45_VFS_truncate,
                        .fsync = at45_VFS_fsync,
                        .mkdir = at45_VFS_mkdir,
                        .rmdir = at45_VFS_rmdir,
                        .fseek = at45_VFS_fseek,
                        .stat = at45_VFS_stat,
                        .mount = at45_VFS_mount,
                        .unmount = at45_VFS_unmount,
                        .rename = at45_VFS_rename,
                        .readdir = at45_VFS_readdir,
                        .opendir = at45_VFS_opendir,
                        .closedir = at45_VFS_closedir,
						.getfree =  at45_VFS_get_free
                       };

int at45_VFS_open(FILE_HANDLE file, const char *name, uint8_t attr)
{
    FRESULT Res;
    FIL *fil;

    file->private_data = pvPortMalloc((size_t)(sizeof(FIL))); //Выделяем память

    if( file->private_data == NULL ) return -1; //Ничего не выделили
	memset(file->private_data, 0, sizeof(FIL));

    fil = (FIL *)file->private_data;

    file->attrib = attr;

    Res = f_open(fil, name, attr);

    if(Res == FR_OK) 
    	file->size = fil->fsize;
    else 
	{
		vPortFree(file->private_data);
		file->private_data = NULL;
	}

    return Res;
}

int at45_VFS_close(FILE_HANDLE file)
{
    int Res = 0;
    FIL *fil;

	if(file->private_data == NULL)
		return Res;

    fil = (FIL *)file->private_data;
    Res = f_close(fil);

    vPortFree(file->private_data);
	file->private_data = NULL;

    return Res;
}

int at45_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    FIL *fil;
    FRESULT Res;

    fil = (FIL *)file->private_data;
    Res = f_read(fil, buf, count, (UINT *)&file->l);

    return Res;
}

int at45_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    FIL *fil;
    FRESULT Res;

    fil = (FIL *)file->private_data;
    Res = f_write(fil, buf, count, (UINT *)&file->l);

    return Res;
}

int at45_VFS_get_free(uint32_t *count)
{
	DWORD clusters = 0;
	FRESULT Res;
	FATFS* fs = &fat12_fs;
	Res = f_getfree("0:", &clusters, &fs);
	*count = clusters * _FLASH_FAT_SECTOR_SIZE_ * fs->csize;
	return Res;	
}

int at45_VFS_truncate(FILE_HANDLE file)
{
    FIL *fil;
    FRESULT Res;

    fil = (FIL *)file->private_data;
    Res = f_truncate(fil);

    return Res;
}

int at45_VFS_fsync(FILE_HANDLE file)
{
    FIL *fil;
    FRESULT Res;

    fil = (FIL *)file->private_data;
    Res = f_sync(fil);

    return Res;
}

int at45_VFS_mkdir(const char *dir_name)
{
    FRESULT Res;

    Res = f_mkdir(dir_name);

    return Res;
}

int at45_VFS_rmdir(const char *dir_name)
{
    FRESULT Res;

    Res = f_unlink(dir_name);

    return Res;
}

int at45_VFS_fseek(FILE_HANDLE file, uint32_t offset)
{
    FIL *fil;
    FRESULT Res;

    file->offset = offset;
    fil = (FIL *)file->private_data;
    Res = f_lseek(fil, offset);

    return Res;
}

int at45_VFS_stat(char *name, STAT_HANDLE stat)
{
    FRESULT Res;
    FILINFO *finfo;

    if( is_root(name) == 0 ) 
    {
        stat->attrib = AM_DIR;
        stat->size = 0;
        return 0;
    }

    finfo = (FILINFO *)pvPortMalloc((size_t)sizeof(FILINFO));
    if( finfo == NULL) return -1;
	memset(finfo, 0, sizeof(FILINFO));
	memset(stat, 0, sizeof(STAT_OBJ));
    
    Res = f_stat(name, finfo);

    stat->size = finfo->fsize;
    stat->date = finfo->fdate;
    stat->time = finfo->ftime;
    stat->attrib = finfo->fattrib;
    strcpy(stat->name, finfo->fname);
    
    vPortFree((void *)finfo);

    return Res;
}

int at45_VFS_mount(char *name)
{
    FRESULT Res;
    
    Res = f_mount(0, &fat12_fs);

    return Res;
}

int at45_VFS_unmount(char *name)
{
    return -1;
}

int at45_VFS_rename(char *old_name, char *new_name)
{
    FRESULT Res;
    
    Res = f_rename(old_name, new_name);

    return Res;
}

int at45_VFS_opendir(DIR_HANDLE dir, char *path)
{
    DIR *dirr;
    FRESULT Res;
    
    dir->private_data = pvPortMalloc((size_t)sizeof(DIR));
    if( dir->private_data == NULL ) return -1;
	memset(dir->private_data, 0, sizeof(DIR));
    dirr = (DIR *)dir->private_data;
    
    if( is_root(path) == 0 ) Res = f_opendir(dirr, "/");

    else Res = f_opendir(dirr, path);

    return Res;
}

int at45_VFS_closedir(DIR_HANDLE dir)
{
    vPortFree(dir->private_data);
    return 0;
}

int at45_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir)
{
    DIR *dirr;
    FILINFO *finfo;
    FRESULT Res;
    
    dirr = (DIR *)dir->private_data;

    finfo = (FILINFO *)pvPortMalloc((size_t)sizeof(FILINFO));
    if( finfo == NULL ) return -1;
	memset(finfo, 0, sizeof(FILINFO));

    Res = f_readdir(dirr, finfo);

    strcpy(file->name, finfo->fname);
    file->size = finfo->fsize;
    file->attrib = finfo->fattrib;

    vPortFree((void *)finfo);

    return Res;
}


