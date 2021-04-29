#ifndef AT45DB161D_H
#define AT45DB161D_H 

#include "VFS.h"

//Пример заполнения структуры для FAT на at45db161d
/*VFS_MOUNT fat_at45db = {.name = "at45db",
                          .pNext        = NULL,
                          .private_data = NULL,
                          .open         = at45_VFS_open,
                          .close        = at45_VFS_close,
                          .read         = at45_VFS_read,
                          .write        = at45_VFS_write,
                          .fsync        = at45_VFS_fsync,
                          .mkdir        = at45_VFS_mkdir,
                          .rmdir        = at45_VFS_rmdir,
                          .fseek        = at45_VFS_fseek,
                          .stat         = at45_VFS_stat,
                          .mount        = at45_VFS_mount,
                          .unmount      = at45_VFS_unmount,
                          .rename       = at45_VFS_rename,
                          .readdir      = at45_VFS_readdir,
                          .opendir      = at45_VFS_opendir,
                         };*/

extern int at45_VFS_open(FILE_HANDLE file, const char *name, uint8_t attr);
extern int at45_VFS_close(FILE_HANDLE file);
extern int at45_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count);
extern int at45_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count);
extern int at45_VFS_truncate(FILE_HANDLE file);
extern int at45_VFS_fsync(FILE_HANDLE file);
extern int at45_VFS_mkdir(const char *dir_name);
extern int at45_VFS_rmdir(const char *dir_name);
extern int at45_VFS_fseek(FILE_HANDLE file, uint32_t offset);
extern int at45_VFS_stat(char *name, STAT_HANDLE stat);
extern int at45_VFS_mount(char *name);
extern int at45_VFS_unmount(char *name);
extern int at45_VFS_rename(char *old_name, char *new_name);
extern int at45_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir);
extern int at45_VFS_opendir(DIR_HANDLE dir, char *path);
extern int at45_VFS_closedir(DIR_HANDLE dir);
extern int at45_VFS_get_free(uint32_t *count);

#endif
