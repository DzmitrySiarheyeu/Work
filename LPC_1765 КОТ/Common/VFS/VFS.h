#ifndef _VFS_H_
#define _VFS_H_

#include <stdint.h>
#include "ff.h"  

#define VFS_OK             0
#define VFS_ERROR          -1

#define VFS_ATTR_RW        0
#define VFS_ATTR_RW_TEXT   1

#define MAX_VFS_NAME_LENGHT 10

typedef struct __attribute__ ((__packed__)) tagFILE
{
    uint32_t offset; 
    uint32_t size;
    void *private_data;
    int l;
    uint8_t attrib;
    char name[3*MAX_VFS_NAME_LENGHT];
}FILE_OBJ, *FILE_HANDLE;

typedef struct __attribute__ ((__packed__))  tagDIR
{ 
    char name[3*MAX_VFS_NAME_LENGHT];
    void *private_data;
}DIR_OBJ, *DIR_HANDLE;

typedef struct __attribute__ ((__packed__)) tagVFS_STAT	 
{
    uint32_t size;            
    uint16_t date;             
    uint16_t time;             
    uint8_t attrib;           
    char name[8+1+3+1];    
}STAT_OBJ, *STAT_HANDLE;

typedef int (*OPEN_FUNC)(FILE_HANDLE file, const char *name, uint8_t attr);
typedef int (*CLOSE_FUNC)(FILE_HANDLE file);
typedef int (*READ_FUNC)(FILE_HANDLE file, uint8_t *buf, uint32_t count);
typedef int (*WRITE_FUNC)(FILE_HANDLE file, uint8_t *buf, uint32_t count);
typedef int (*TRUNCATE_FUNC)(FILE_HANDLE file);
typedef int (*FSYNK_FUNC)(FILE_HANDLE file);
typedef int (*MKDIR_FUNC)(const char *dir_name);
typedef int (*RMDIR_FUNC)(const char *dir_name);
typedef int (*FSEEK_FUNC)(FILE_HANDLE, uint32_t offset);
typedef int (*STAT_VFS_FUNC)(char *name, STAT_HANDLE stat);
typedef int (*MOUNT_FUNK)(char *name);
typedef int (*UNMOUNT_FUNK)(char *name);
typedef int (*RENAME_FUNK)(char *old_name, char *new_name);
typedef int (*READDIR_FUNK)(FILE_HANDLE file, DIR_HANDLE dir);
typedef int (*OPENDIR_FUNK)(DIR_HANDLE dir, char *path);
typedef int (*CLOSEDIR_FUNC)(DIR_HANDLE dir);
typedef int (*GETFREE_FUNC)(uint32_t *count);

typedef struct tagVFSmount
{
    char name[MAX_VFS_NAME_LENGHT];
    struct tagVFSmount *pNext;
    void *private_data;
    OPEN_FUNC open;
    CLOSE_FUNC close;
    READ_FUNC read;
    WRITE_FUNC write;
	TRUNCATE_FUNC truncate;
    FSYNK_FUNC fsync;
    MKDIR_FUNC mkdir;
    RMDIR_FUNC rmdir;
    FSEEK_FUNC fseek;
    STAT_VFS_FUNC stat;
    MOUNT_FUNK mount;
    UNMOUNT_FUNK unmount;
    RENAME_FUNK rename;	
    READDIR_FUNK readdir;
    OPENDIR_FUNK opendir;
    CLOSEDIR_FUNC closedir;
	GETFREE_FUNC  getfree;
}VFS_MOUNT;

extern int VFS_open(FILE_HANDLE file, const char *name, uint8_t attr);
extern int VFS_close(FILE_HANDLE);
extern int VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count);
extern int VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count);
extern int VFS_truncate(FILE_HANDLE file);
extern int VFS_fsync(FILE_HANDLE);
extern int VFS_mkdir(const char *dir_name);
extern int VFS_rmdir(const char *dir_name);
extern int VFS_fseek(FILE_HANDLE file, uint32_t offset);
extern int VFS_stat(char *name, STAT_HANDLE stat);
extern int VFS_mount(char *name);
extern int VFS_unmount(char *name);
extern int VFS_rename(const char *old_name, const char *new_name);
extern int VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir);
extern int VFS_opendir(DIR_HANDLE dir, char *path);
extern int VFS_closedir(DIR_HANDLE dir);
extern void VFS_reg(VFS_MOUNT *vfs_obj);
extern VFS_MOUNT *VFS_get(const char *dir_name);
extern const char *VFS_get_path(const char *path);
int VFS_get_free(char *dir_name, uint32_t *count);

extern int is_root(char *path);

typedef struct tagVFS_LIST
{
    VFS_MOUNT *pFirstVFS;
    uint8_t    VFS_number;
}VFS_LIST;

// Структура описывающая список объектов VFS

extern VFS_MOUNT fat_at45db;
extern VFS_MOUNT drivers;
extern VFS_MOUNT sensors_VFS;
extern VFS_MOUNT saver_fs;

#endif
