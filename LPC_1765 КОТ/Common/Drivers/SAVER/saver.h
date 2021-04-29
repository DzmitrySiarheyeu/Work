#ifndef __SAVER_H__
#define __SAVER_H__
#include "Config.h"
#if(_SAVER_)

#include <stdint.h>
#include "DriversCore.h"






typedef struct tagSEGMENT_INFO
{
    uint32_t            base_addr;	   // Начальный адрес сегмента
	uint32_t			size;		   //  Размер сегмента
	uint8_t				rec_len;	   //  Длина одной записи для сегмента с CRC
	uint8_t 			crc_flag;
    uint32_t            curr_addr;	   //  Произвольный указатель на любую запись
	uint32_t            fin_addr;	   // Указатель на первое свободное место для записи
}SEGMENT_INFO, *P_SEG_INFO;


typedef struct tagSAVER_INFO
{
    DEVICE_HANDLE        	*pFlash;
	uint8_t 			 	curr_seg;
    SEGMENT_INFO 			seg[SEGMENT_NUMBER];
	uint8_t					buf[MAX_BUF_LEN];
	uint8_t                 curr_text_seg;
	uint32_t                curr_text_addr;
}SAVER_INFO;

typedef struct tsgSaver_fs_info
{
	char name[10];
	uint8_t  seg;
	uint32_t size;
	uint32_t addr;
	struct tsgSaver_fs_info *pNext;
}SAVER_FS_INFO;



extern SAVER_INFO saver_info;
extern SAVER_FS_INFO *first_file;
void Saver_addr_init(uint8_t seg);
SAVER_FS_INFO *GetSaverFile(char *name);

#endif

#endif



