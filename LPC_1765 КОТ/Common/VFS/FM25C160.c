#include "config.h"
#include <stdlib.h>
#include <string.h>
#include "fm25c160.h"
#include "DriversCore.h"
#include "VFS.h"
#include "main.h"
#include "User_Error.h"
#include "ff.h"
#include "Indy_Log.h"

extern uint16_t CRC16(uint8_t * puchMsg, uint16_t usDataLen, uint16_t prevCRC);

VFS_MOUNT fm25c160 =   {.name = "FRAM",
                        .pNext = NULL,
                        .private_data = NULL,
                        .open = FRAM_VFS_open,
                        .close = FRAM_VFS_close,
                        .read = FRAM_VFS_read,
                        .write = FRAM_VFS_write,
                        .fsync = FRAM_VFS_fsync,
                        .mkdir = FRAM_VFS_mkdir,
                        .rmdir = FRAM_VFS_rmdir,
                        .fseek = FRAM_VFS_fseek,
                        .stat = FRAM_VFS_stat,
                        .mount = FRAM_VFS_mount,
                        .unmount = FRAM_VFS_unmount,
                        .rename = FRAM_VFS_rename,
                        .readdir = FRAM_VFS_readdir,
                        .opendir = FRAM_VFS_opendir,
                        .closedir = FRAM_VFS_closedir
                       };

uint16_t recount_crc(FILE_HANDLE file)
{
    uint32_t /*len_addr,*/ file_addr = 0;
    uint16_t CRC;
    FILES *c_file = 0;


    DEVICE_HANDLE handle = (DEVICE_HANDLE *)file->private_data;

    if( strcmp(file->name, "/FRAM/LOG") == 0 )
    {
         file_addr = (uint32_t)&c_file->log_file.buf;
        // len_addr = (uint32_t)&c_file->log_file.file_len;
    }

   // SeekDevice(handle, addr, 0);
   // ReadDevice(handle, (void *)&file->size, 2);

    uint8_t *temp_buf = (uint8_t *)pvPortMalloc((size_t)file->size);

    SeekDevice(handle, file_addr, 0);
    ReadDevice(handle, (void *)temp_buf, file->size);

    CRC = CRC16(temp_buf, file->size, 0xFFFF);

    vPortFree(temp_buf);

    return CRC;  
}

              

int FRAM_VFS_open(FILE_HANDLE file, char *name, uint8_t attr)
{
    DEVICE_HANDLE handle;
    FILES *c_file = 0;
    uint32_t len_addr;
    uint32_t CRC_addr;
    uint32_t file_addr;

    //открывает устройство FRAM
    handle = OpenDevice("FRAM");

    //если не открыли - возвращаем ошибку
    if(handle == NULL) return -1;

    file->private_data = (void *)handle;
    
    //узнаем адреса длины файла, CRC файла и начала файла
    if( strcmp(file->name, "/FRAM/SENSORS") == 0 ) 
    {
        len_addr = (uint32_t)&c_file->sensors_file.file_len;
        CRC_addr = (uint32_t)&c_file->sensors_file.file_CRC;
        file_addr = (uint32_t)&c_file->sensors_file.buf;
    }
    else if( strcmp(file->name, "/FRAM/IVENTS_S") == 0 )
    {
        len_addr = (uint32_t)&c_file->ivents_s_file.file_len;
        CRC_addr = (uint32_t)&c_file->ivents_s_file.file_CRC;
        file_addr = (uint32_t)&c_file->ivents_s_file.buf;
    }
    else if( strcmp(file->name, "/FRAM/DISPLAY") == 0 )
    {
        len_addr = (uint32_t)&c_file->menu_file.file_len;
        CRC_addr = (uint32_t)&c_file->menu_file.file_CRC;
        file_addr = (uint32_t)&c_file->menu_file.buf;
    }
    else if( strcmp(file->name, "/FRAM/LOG") == 0 )
    {
        len_addr = (uint32_t)&c_file->log_file.file_len;
        CRC_addr = (uint32_t)&c_file->log_file.file_CRC;
        file_addr = (uint32_t)&c_file->log_file.buf;
    }
    else return -1;

    uint16_t len;
    static uint16_t CRC;

    //читаем длину
    SeekDevice(handle, len_addr, 0);
    ReadDevice(handle, (void *)&len, 2);

    file->size = len & 0x0000FFFF;

    //читаем CRC
    SeekDevice(handle, CRC_addr, 0);
    ReadDevice(handle, (void *)&CRC, 2);
    
    //выделяем пямять для чтения файла
    //будем читать кусками по READ_FILE_SIZE байт
    uint8_t * buf;
    buf = (uint8_t *)pvPortMalloc((size_t)READ_FILE_SIZE);

    //не выделили память - возвращаем ошибку
    if(buf == NULL)
    {
        return -1;
    }

    uint16_t bytes_to_read;  //количество байт, которое надо прочитать

    uint16_t temp_CRC = 0xFFFF; //при всех вычисления начальное значение равно oxFFFF

    uint16_t read_shift = 0; //смещение адреса для чтения файла 

    //начинаем читать
    do
    {
        //если длина файла больше READ_FILE_SIZE - считаваем кусок в READ_FILE_SIZE байт
        if(len > READ_FILE_SIZE)
        {
            bytes_to_read = READ_FILE_SIZE;
        }
        //иначе считываем весь файл
        else
        {
            bytes_to_read = len;
        }

        SeekDevice(handle, file_addr + read_shift, 0);
        ReadDevice(handle, (void *)buf, bytes_to_read);

        //вычисляем CRC прочитанного куска
        temp_CRC = CRC16(buf, bytes_to_read, temp_CRC);

        //уменьшаем длину на количество прочитанного
        len -= bytes_to_read;

        //увеличиваем смещение
        read_shift += bytes_to_read;

    }while(len);

    //освобождаем память
    vPortFree(buf);

    //если CRC совпадает
    if(temp_CRC == CRC)
    {
        //возвращаем ОК
        return 0;
    }
    //если не совпадает
    else
    {
        //если открываем только для чтения
        if(attr & FA_CREATE_NEW == 0)
        {
            //возвращаем ошибку
            return -1;
        }
    }

    //поврежденный файл лога нужно проинициализировать заново
    //(остальные не нужно, т.к. они полностью перезаписываются при записи файлов конфигурации)
    //перезаписываем длину файла и CRC файла
    if( strcmp(file->name, "/FRAM/LOG") == 0 )
    {
        len = INIT_LOG_LENGTH;
        SeekDevice(handle, len_addr, 0);
        WriteDevice(handle, (void *)&len, 2);
        
        CRC = temp_CRC;
        SeekDevice(handle, CRC_addr, 0);
        WriteDevice(handle, (void *)&CRC, 2);
    }

    return -1;
}

int FRAM_VFS_close(FILE_HANDLE file)
{
    return 0;  
}

int FRAM_VFS_read(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    uint32_t /*CRC_addr,*/ file_addr;
    DEVICE_HANDLE handle = (DEVICE_HANDLE *)file->private_data;
    FILES *c_file = 0;
//    uint16_t CRC;
    
    if( strcmp(file->name, "/FRAM/SENSORS") == 0 ) 
    {
//        CRC_addr = (uint32_t)&c_file->sensors_file.file_CRC;
        file_addr = (uint32_t)&c_file->sensors_file.buf;
    }
    else if( strcmp(file->name, "/FRAM/IVENTS_S") == 0 )
    {
//         CRC_addr = (uint32_t)&c_file->ivents_s_file.file_CRC;
         file_addr = (uint32_t)&c_file->ivents_s_file.buf;
    }
    else if( strcmp(file->name, "/FRAM/DISPLAY") == 0 )
    {
//         CRC_addr = (uint32_t)&c_file->menu_file.file_CRC;
         file_addr = (uint32_t)&c_file->menu_file.buf;
    }
    else if( strcmp(file->name, "/FRAM/LOG") == 0 )
    {
//         CRC_addr = (uint32_t)&c_file->log_file.file_CRC;
         file_addr = (uint32_t)&c_file->log_file.buf;
    }
    else
    {
        return -1;
    }

    if( strcmp(file->name, "/FRAM/LOG") == 0 )
    {
         SeekDevice(handle, file_addr + file->offset, 0);
         ReadDevice(handle, (void *)buf, count);
    }
    else
    {
//
//        //читаем CRC файла
//        SeekDevice(handle, CRC_addr, 0);
//        ReadDevice(handle, (void *)&CRC, 2);

        //читаем сам файл
        SeekDevice(handle, file_addr, 0);
        ReadDevice(handle, (void *)buf, count);

        //сравниваем CRC
//        if(CRC16(buf, count, 0xFFFF) != CRC) 
//        {
//            if( strcmp(file->name, "/FRAM/SENSORS") == 0 ) 
//            {
//                add_critical_error_to_queue(ERR_CRC_SENS_FILE_STR);
//                set_critical_error_flag();
//            }
//            else if( strcmp(file->name, "/FRAM/IVENTS_S") == 0 )
//            {
//                add_critical_error_to_queue(ERR_CRC_IVENT_FILE_STR);
//                set_critical_error_flag();
//            }
//            else if( strcmp(file->name, "/FRAM/DISPLAY") == 0 )
//            {
//                add_critical_error_to_queue(ERR_CRC_MENU_FILE_STR);
//                set_critical_error_flag();
//            }
//            return -1;
//        }

        return 0;
    }
	return 0;
}

int FRAM_VFS_write(FILE_HANDLE file, uint8_t *buf, uint32_t count)
{
    uint16_t  file_addr;//, len_addr;
    static uint16_t CRC_addr;
    DEVICE_HANDLE handle = (DEVICE_HANDLE *)file->private_data;
    FILES *c_file = 0;
    uint16_t CRC;
    
    if( strcmp(file->name, "/FRAM/LOG") == 0 )
    {
         CRC_addr = (uint32_t)&c_file->log_file.file_CRC;
         file_addr = (uint32_t)&c_file->log_file.buf;
//         len_addr = (uint32_t)&c_file->log_file.file_len;
    }
    else
    {
        return -1;
    }

    SeekDevice(handle, file_addr + file->offset, 0);
    WriteDevice(handle, (void *)buf, count);

    //!!!Размер файла не увеличивается, если мы дописываем в конец позицию сохранения и счетчик событий!!!
//    if(file->offset != MAX_MESSAGE_LENGTH * MAX_MESSAGE_NUMBER && file->offset != MAX_MESSAGE_LENGTH * MAX_MESSAGE_NUMBER + 4)
//    {
//        file->size += count;
//        if(file->size > MAX_MESSAGE_LENGTH * MAX_MESSAGE_NUMBER)
//            file->size = MAX_MESSAGE_LENGTH * MAX_MESSAGE_NUMBER;
//    }

//    SeekDevice(handle, len_addr, 0);
//    WriteDevice(handle, (void *)&file->size, 2);

    CRC = recount_crc(file);

    SeekDevice(handle, CRC_addr, 0);
    WriteDevice(handle, (void *)&CRC, 2);

	return 	count;
}

int FRAM_VFS_fsync(FILE_HANDLE file)
{
    return 0;
}

int FRAM_VFS_mkdir(char *dir_name)
{
    return 0;
}

int FRAM_VFS_rmdir(char *dir_name)
{
    return 0;
}

int FRAM_VFS_fseek(FILE_HANDLE file, uint32_t offset)
{
    file->offset = offset;
    return offset;
}

int FRAM_VFS_stat(char *name, STAT_HANDLE stat)
{
    return 0;
}

int FRAM_VFS_mount(char *name)
{
    return 0;
}

int FRAM_VFS_unmount(char *name)
{
    return -1;
}

int FRAM_VFS_rename(char *old_name, char *new_name)
{
    return 0;
}

int FRAM_VFS_opendir(DIR_HANDLE dir, char *path)
{
    return 0;
}

int FRAM_VFS_closedir(DIR_HANDLE dir)
{
    return 0;
}

int FRAM_VFS_readdir(FILE_HANDLE file, DIR_HANDLE dir)
{
    return 0;
}


