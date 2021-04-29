#ifndef SAVER_FS_H
#define SAVER_FS_H

#include "VFS.h"
#include "DriversCore.h"

//Пример заполнения структуры для системы драйверов
/*VFS_MOUNT drivers = {.name = "drivers",
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
                      };*/

extern VFS_MOUNT saver_fs;

int svr_VFS_open(FILE_HANDLE file, const char *name, uint8_t attr);
int svr_VFS_close(FILE_HANDLE file);
int svr_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count);
int svr_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count);
int svr_VFS_fsync(FILE_HANDLE file);
int svr_VFS_mkdir(const char *dir_name);
int svr_VFS_rmdir(const char *dir_name);
int svr_VFS_fseek(FILE_HANDLE file, uint32_t offset);
int svr_VFS_stat(char *name, STAT_HANDLE stat);
int svr_VFS_mount(char *name);
int svr_VFS_unmount(char *name);
int svr_VFS_rename(char *old_name, char *new_name);
int svr_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir);
int svr_VFS_opendir(DIR_HANDLE dir, char *path);
int svr_VFS_closedir(DIR_HANDLE dir);
int svr_VFS_get_free(uint32_t *count);


#endif

