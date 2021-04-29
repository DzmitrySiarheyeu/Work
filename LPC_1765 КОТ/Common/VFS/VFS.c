#include <stdlib.h>
#include <string.h>
#include "Config.h"
#include "VFS.h"
#include "ff.h"


static VFS_LIST vfs_list = {.pFirstVFS = NULL, .VFS_number = 0};
                  /* = {.name = "FIRST",
                        .pNext = NULL,
                        .private_data = NULL,
                        .open = NULL,
                        .close = NULL,
                        .read = NULL,
                        .write = NULL,
                        .fsync = NULL,
                        .mkdir = NULL,
                        .rmdir = NULL,
                        .fseek = NULL,
                        .stat = NULL,
                        .mount = NULL,
                        .unmount = NULL,
                        .rename = NULL,
                        .readdir = NULL,
                        .opendir = NULL,
                       };*/

/***********************************************************************************/
/***********************************************************************************/

void VFS_reg(VFS_MOUNT *vfs_obj)
{
    if(vfs_list.pFirstVFS == NULL) //Если добавляем первое устройство
    {
        vfs_list.pFirstVFS = vfs_obj;
        vfs_obj->pNext = NULL;
    }
    else 
    {
        vfs_obj->pNext = vfs_list.pFirstVFS;
        vfs_list.pFirstVFS = vfs_obj;
    }
    vfs_list.VFS_number++;
}

VFS_MOUNT *VFS_get(const char *dir_name)
{
    char temp[10];
    uint8_t i = 1;
    VFS_MOUNT *pVFS_MOUNT;
    
    for(i = 1; i < MAX_VFS_NAME_LENGHT; i++)
    {
        if( dir_name[i] != '/' && dir_name[i] != '\0' ) temp[i - 1] = dir_name[i];
        else break;
    }
    
    temp[i - 1] = 0;

    if(i < MAX_VFS_NAME_LENGHT) temp[i] = '\0';
    else 
    {
        #if(_DEBUG_)
        DEBUG_PUTS("VFS not found");
        #endif
        return NULL;
    }

    pVFS_MOUNT = vfs_list.pFirstVFS;
    while(pVFS_MOUNT != NULL)
    {
        if(strcmp(pVFS_MOUNT->name, temp) == 0) return pVFS_MOUNT;
        pVFS_MOUNT = pVFS_MOUNT->pNext;
    }
//    DEBUG_PUTS("VFS not found");
    return NULL;
}

const char *VFS_get_path(const char *path)
{
    return strchr(path + 1, '/');
}

/***********************************************************************************/
/***********************************************************************************/


int VFS_open(FILE_HANDLE file, const char *name, uint8_t attr)
{
    int err = 0;
    VFS_MOUNT *VFS;

    VFS = VFS_get(name);

    if(VFS == NULL) return VFS_ERROR;
    
    strcpy(file->name, name);

    err = VFS->open(file, VFS_get_path(name), attr);

    return err;
}

/***********************************************************************************/

int VFS_close(FILE_HANDLE file)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(file->name);

    return VFS->close(file);
}

/***********************************************************************************/

int VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(file->name);

    if(VFS == NULL) return VFS_ERROR;
    
    return VFS->read(file, buf, count);
}

/***********************************************************************************/

int VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(file->name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->write(file, buf, count);
}

/***********************************************************************************/

int VFS_truncate(FILE_HANDLE file)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(file->name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->truncate(file);
}

/***********************************************************************************/

int VFS_fsync(FILE_HANDLE file)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(file->name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->fsync(file);
}

/***********************************************************************************/

int VFS_mkdir(const char *dir_name)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(dir_name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->mkdir(VFS_get_path(dir_name));
}

/***********************************************************************************/

int VFS_rmdir(const char *dir_name)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(dir_name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->rmdir(VFS_get_path(dir_name));
}

/***********************************************************************************/

int VFS_get_free(char *dir_name, uint32_t *count)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(dir_name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->getfree(count);
}

/***********************************************************************************/

int VFS_fseek(FILE_HANDLE file, uint32_t offset)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(file->name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->fseek(file, offset);
}

/***********************************************************************************/

int VFS_stat(char *name, STAT_HANDLE stat)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->stat(VFS_get_path(name), stat);
}

/***********************************************************************************/

int VFS_mount(char *name)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->mount(name);
}

/***********************************************************************************/

int VFS_unmount(char *name)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->unmount(name);
}

/***********************************************************************************/

int VFS_rename(const char *old_name, const char *new_name)
{
    VFS_MOUNT *VFS;

    VFS = VFS_get(old_name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->rename(VFS_get_path(old_name) + 1, VFS_get_path(new_name) + 1);
}

/***********************************************************************************/

int VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir)
{
    VFS_MOUNT *VFS;

    /*-----------------------------------------------------*/
    /*-----------------------------------------------------*/

    static uint8_t i = 0;
    static VFS_MOUNT *pVFS = NULL;
 
    if ( strcmp(dir->name, "/") == 0 ) 
    {
        if( i == 0 ) pVFS = vfs_list.pFirstVFS;

        if( i < vfs_list.VFS_number )
        {
            strcpy(file->name, pVFS->name);
            file->attrib = AM_DIR | AM_RDO;
            pVFS = pVFS->pNext;
            i++;
        }
        else 
        {
            i = 0;
            file->name[0] = NULL;
        }
        
        return 0;
    } 

    /*-----------------------------------------------------*/
    /*-----------------------------------------------------*/


    VFS = VFS_get(dir->name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->readdir(file, dir);
}

/***********************************************************************************/

int VFS_opendir(DIR_HANDLE dir, char *path)
{
    VFS_MOUNT *VFS;

    /*-----------------------------------------------------*/
    /*-----------------------------------------------------*/

    if( strcmp(path, "/") == 0 ) 
    {
        strcpy(dir->name, path);
        dir->private_data = NULL;
        return 0;
    }

    /*-----------------------------------------------------*/
    /*-----------------------------------------------------*/

    VFS = VFS_get(path);

    if(VFS == NULL) return VFS_ERROR;

    strcpy(dir->name, path);

    return VFS->opendir(dir, VFS_get_path(path));
}

/***********************************************************************************/

int VFS_closedir(DIR_HANDLE dir)
{
    VFS_MOUNT *VFS;

    if ( strcmp(dir->name, "/") == 0 ) return 0;

    VFS = VFS_get(dir->name);

    if(VFS == NULL) return VFS_ERROR;

    return VFS->closedir(dir);
}

/***********************************************************************************/

int is_root(char *path)
{
    if( strchr(path, '/' ) == NULL ) return 0;

    return -1;
}
