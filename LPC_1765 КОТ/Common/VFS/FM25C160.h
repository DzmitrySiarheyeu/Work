#ifndef FRAMDB161D_H
#define FRAMDB161D_H 

#include "VFS.h"

#define READ_FILE_SIZE 50 //размер порции данных для чтения из файла

//Пример заполнения структуры для FAT на FRAMdb161d
/*VFS_MOUNT fat_FRAMdb = {.name = "FRAMdb",
                          .pNext        = NULL,
                          .private_data = NULL,
                          .open         = FRAM_VFS_open,
                          .close        = FRAM_VFS_close,
                          .read         = FRAM_VFS_read,
                          .write        = FRAM_VFS_write,
                          .fsync        = FRAM_VFS_fsync,
                          .mkdir        = FRAM_VFS_mkdir,
                          .rmdir        = FRAM_VFS_rmdir,
                          .fseek        = FRAM_VFS_fseek,
                          .stat         = FRAM_VFS_stat,
                          .mount        = FRAM_VFS_mount,
                          .unmount      = FRAM_VFS_unmount,
                          .rename       = FRAM_VFS_rename,
                          .readdir      = FRAM_VFS_readdir,
                          .opendir      = FRAM_VFS_opendir,
                         };*/

extern int FRAM_VFS_open(FILE_HANDLE file, char *name, uint8_t attr);
extern int FRAM_VFS_close(FILE_HANDLE file);
extern int FRAM_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count);
extern int FRAM_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count);
extern int FRAM_VFS_fsync(FILE_HANDLE file);
extern int FRAM_VFS_mkdir(char *dir_name);
extern int FRAM_VFS_rmdir(char *dir_name);
extern int FRAM_VFS_fseek(FILE_HANDLE file, uint32_t offset);
extern int FRAM_VFS_stat(char *name, STAT_HANDLE stat);
extern int FRAM_VFS_mount(char *name);
extern int FRAM_VFS_unmount(char *name);
extern int FRAM_VFS_rename(char *old_name, char *new_name);
extern int FRAM_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir);
extern int FRAM_VFS_opendir(DIR_HANDLE dir, char *path);
extern int FRAM_VFS_closedir(DIR_HANDLE dir);

extern VFS_MOUNT fm25c160;

#endif
