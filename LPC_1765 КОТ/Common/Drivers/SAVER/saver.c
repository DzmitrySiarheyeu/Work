#include "saver.h"
#if(_SAVER_)

#include <stdlib.h>
#include <string.h>

#include "FLASH_DRV.h"
#include "WDT.h"

// Проверка на то что размер данных кратен 256
//extern char __CHECK__[1/(!(256%ARCH_REC_LEN))];

uint16_t CRC16 (uint8_t * puchMsg, uint8_t usDataLen, uint16_t prevCRC);
void Saver_user_init(void);
uint8_t AddSaverFile(SAVER_FS_INFO *pFile);

static int CreateSaver(void);
static DEVICE_HANDLE OpenSaver(void);
static void CloseSaver(DEVICE_HANDLE handle);
static int ReadSaver(DEVICE_HANDLE handle, void * dst, int count);
static int WriteSaver(DEVICE_HANDLE handle,const void * dst, int count);
static int SeekSaver(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteSaverText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadSaverText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekSaverText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int SaverIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

SAVER_FS_INFO *first_file = NULL;

SAVER_FS_INFO LogFile = {
						.name = "LOG",
						.size = (LOG_SEG_SIZE * _SAVER_FLASH_PAGE_SIZE_),
						.seg = LOG_SEG,
					/*#ifdef _25Q128_
						.addr = ((SEG_RAM_START_SECTOR + 288)*_SAVER_FLASH_PAGE_SIZE_),
					#else
						.addr = ((SEG_RAM_START_SECTOR + 288)*_SAVER_FLASH_PAGE_SIZE_),
					#endif	*/
						.addr = 0,
						.pNext = NULL
						};

#if(_ARCH_SAVER_)

SAVER_FS_INFO ArchFile = {
						.name = "ARCH",
						.size = (ARCH_SEG_SIZE * _SAVER_FLASH_PAGE_SIZE_),
						.seg = ARCH_SEG,
					/*#ifdef _25Q128_
						.addr = ((SEG_RAM_START_SECTOR + 352)*_SAVER_FLASH_PAGE_SIZE_),
					#else
						.addr = ((SEG_RAM_START_SECTOR + 352)*_SAVER_FLASH_PAGE_SIZE_),
					#endif */
						.addr = 0,
						.pNext = NULL
						};
#endif

SAVER_INFO saver_info = { 
    .pFlash = NULL,
	.curr_seg = 0
     };


const DEVICE_IO saver_device_io = {
    .DeviceID = SAVER,
	.CreateDevice =	CreateSaver,
	.CloseDevice = CloseSaver,
    .OpenDevice = OpenSaver,
    .ReadDevice = ReadSaver,
    .WriteDevice = WriteSaver,
    .SeekDevice = SeekSaver,
    .ReadDeviceText = ReadSaverText,
    .WriteDeviceText = WriteSaverText,
    .SeekDeviceText = SeekSaverText,
	.DeviceIOCtl = SaverIOCtl,
    .DeviceName = "SAVER",
    };

DEVICE saver_device = {
    .flDeviceOpen = 0,
    .device_io = (DEVICE_IO *)&saver_device_io,
    .pDriverData = &saver_info,
    .pData = NULL
    };


int CreateSaver(void)
{
	DEVICE_HANDLE h_flash;

    h_flash = OpenDevice(_SAVER_L_L_DRIVER_NAME_);
    if(h_flash == 0)
    {
		PUT_MES_TO_LOG("Saver: unable to open FLASH", 0, 1);
        DEBUG_PUTS("Saver: unable to open FLASH\n");
        return NULL;
    }

    saver_info.pFlash = h_flash;

	Saver_user_init();

	AddSaverFile(&LogFile);

#if(_ARCH_SAVER_)

	AddSaverFile(&ArchFile);

#endif

	CloseDevice(saver_info.pFlash);

	DEBUG_PUTS("Saver Created\n");
    return DEVICE_OK;
}

DEVICE_HANDLE OpenSaver(void)
{
    DEVICE_HANDLE h_flash;

	if(saver_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 saver_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		  DEBUG_PUTS("Saver RE_OPEN\n");
	}

    h_flash = OpenDevice(_SAVER_L_L_DRIVER_NAME_);
    if(h_flash == 0)
    {
		PUT_MES_TO_LOG("Saver: unable to open FLASH", 0, 1);
        DEBUG_PUTS("Saver: unable to open FLASH\n");
        return NULL;
    }

    saver_info.pFlash = h_flash;

	return 	(DEVICE_HANDLE)&saver_device;    
}

void CloseSaver(DEVICE_HANDLE handle)
{
	SAVER_INFO *p_saver_info = (SAVER_INFO *)(((DEVICE *)handle)->pDriverData);
	CloseDevice(p_saver_info->pFlash);

	if(saver_device.flDeviceOpen == DEVICE_OPEN)
	{
		 saver_device.flDeviceOpen = DEVICE_CLOSE;
	}
	else
	{
		  DEBUG_PUTS("Saver RE_CLOSE\n");
	}
}


static int ReadSaver(DEVICE_HANDLE handle, void * pDst, int count) 
{
	SAVER_INFO *p_saver_info = (SAVER_INFO *)(((DEVICE *)handle)->pDriverData);
	P_SEG_INFO pSeg = &p_saver_info->seg[p_saver_info->curr_seg];
    uint32_t addr = pSeg->curr_addr + pSeg->base_addr;
    uint16_t crc = 0;
	int i = 0, len = 0;

	if(pSeg->crc_flag == 0) // Если нет CRC значит данные идут последовательно
	{						 //  и считывать их можно как угодно без удаления CRC
		

		if((count * pSeg->rec_len + pSeg->curr_addr) > (pSeg->size * _SAVER_FLASH_PAGE_SIZE_))  // Если нужное количество данных выходит за границу сегмента 
		{																		        //  Читаем сначала до конца сегмента
			len = (pSeg->size * _SAVER_FLASH_PAGE_SIZE_) - pSeg->curr_addr;
			// Переводим драйвер Flash в режим RAW
		    DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
		    // Устанавливаем адрес чтения
		    DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
		    // Читаем буфер
		    ReadDevice(p_saver_info->pFlash, (void *)pDst,  len);

			pSeg->curr_addr = 0;
			addr = pSeg->base_addr;										              // потом дочитаем оставшееся с начала сегмента
		}																			   // так как сегмент - это циклический буфер

		// Переводим драйвер Flash в режим RAW
		DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
		// Устанавливаем адрес чтения
		DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
		// Читаем буфер
		ReadDevice(p_saver_info->pFlash, (void *)((char *)pDst + len),  count * pSeg->rec_len - len);
		i = count;
		pSeg->curr_addr += (count * pSeg->rec_len) - len;
	}
	else
	{

		if((pSeg->curr_addr - (pSeg->curr_addr/ _SAVER_FLASH_PAGE_SIZE_)*_SAVER_FLASH_PAGE_SIZE_) == 0)
		{
			int delta = _SAVER_FLASH_PAGE_SIZE_ - (_SAVER_FLASH_PAGE_SIZE_/pSeg->rec_len)*pSeg->rec_len;
			if(pSeg->curr_addr == 0)
			{
				addr =  (_SAVER_FLASH_PAGE_SIZE_ * pSeg->size) - pSeg->rec_len - delta + pSeg->base_addr;
			}
			else
			{
				 addr = pSeg->curr_addr - pSeg->rec_len - delta + pSeg->base_addr;
			} 
		}
		else
		{
			addr = pSeg->curr_addr - pSeg->rec_len + pSeg->base_addr;
		}


		// Переводим драйвер Flash в режим RAW
		DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
		// Устанавливаем адрес чтения
		DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
		// Читаем буфер
		ReadDevice(p_saver_info->pFlash, (void *)saver_info.buf,  pSeg->rec_len);
	
	    crc = CRC16((uint8_t *)saver_info.buf, (pSeg->rec_len - 2), 0xFFFF);
	    

	    if(crc == *((uint16_t *)&saver_info.buf[pSeg->rec_len - 2]))
		{
			memcpy((char *)pDst, (char *)saver_info.buf, (pSeg->rec_len - 2));
			//memcpy((char *)((char *)pDst + i*pSeg->rec_len), (char *)saver_info.buf, (pSeg->rec_len - 2));
			i = 1;
		}
	
	}	

	return i;
}

static int WriteSaver(DEVICE_HANDLE handle,const void * pSrc, int count)
{ 
	SAVER_INFO *p_saver_info = (SAVER_INFO *)(((DEVICE *)handle)->pDriverData);
	P_SEG_INFO pSeg = &p_saver_info->seg[p_saver_info->curr_seg];
    uint32_t addr = 0;
    uint16_t crc = 0;
	uint32_t addr_ofset = 0, adrr_f = 0;
	int len = 0, tot_len = 0;
	uint32_t sector_num = 0;

	if(pSeg->crc_flag == 0) // Если нет CRC значит данные идут последовательно
	{
		sector_num = pSeg->curr_addr / _SAVER_FLASH_PAGE_SIZE_;
		len = _SAVER_FLASH_PAGE_SIZE_ - (pSeg->curr_addr - sector_num * _SAVER_FLASH_PAGE_SIZE_);  // Длина до конца сектора

		tot_len =  count * pSeg->rec_len;		    //  Длина данных для записи

		if(tot_len < len) len = tot_len; // Если нужно будет перелизать в другой сектор
										 // Сначала допишем до конца старый 
		while(tot_len)
		{
			addr = pSeg->curr_addr +  pSeg->base_addr;

			adrr_f = pSeg->curr_addr + len;   // Прибавляем то количество которое хотим записать
			if((adrr_f - (adrr_f/ _SAVER_FLASH_PAGE_SIZE_)*_SAVER_FLASH_PAGE_SIZE_) == 0)  // и смотрим не перейдем ли после записи в новый сектор
			{															//  если мы все же перешли в новый сектор
				if(adrr_f >= pSeg->size * _SAVER_FLASH_PAGE_SIZE_) adrr_f = 0;	  // смотрим не вышли ли мы за приделы своего сегмента, если вышли корректируем
				SeekDevice(p_saver_info->pFlash, (adrr_f + pSeg->base_addr)/_SAVER_FLASH_PAGE_SIZE_, 0);	// стираем следующий сектор
                DeviceIOCtl(p_saver_info->pFlash, ERASE_BLOCK, 0);
			}

			// Переводим драйвер Flash в режим RAW
	        DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
	        // Устанавливаем адрес чтения
	        DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
			
			WriteDevice(p_saver_info->pFlash, (void *)((uint8_t *)pSrc + addr_ofset),  len);

			pSeg->curr_addr += len;		// прибавляем количество записанного
			if(pSeg->curr_addr >= pSeg->size * _SAVER_FLASH_PAGE_SIZE_) pSeg->curr_addr = 0;	  // проверка пределов сегмента

			tot_len -= len;         // Уменьшаем количество требуемых для записи данных
			addr_ofset += len;		// Наращиваем смещение для буфера с данными
			if(tot_len <= _SAVER_FLASH_PAGE_SIZE_) len = tot_len;   // Пишем либо целиком сектор, либо меньше
			else len = _SAVER_FLASH_PAGE_SIZE_;
		}

		pSeg->fin_addr = pSeg->curr_addr;
	}
	else
	{
		sector_num = pSeg->fin_addr / _SAVER_FLASH_PAGE_SIZE_;
		len = _SAVER_FLASH_PAGE_SIZE_ - (pSeg->fin_addr - sector_num * _SAVER_FLASH_PAGE_SIZE_);  // Длина до конца сектора

		memcpy((char *)p_saver_info->buf, (char *)pSrc, pSeg->rec_len - 2);
		crc = CRC16((uint8_t *)pSrc, pSeg->rec_len - 2, 0xFFFF);
		memcpy((char *)(p_saver_info->buf + pSeg->rec_len - 2), (char *)&crc, 2);

		addr = pSeg->fin_addr +  pSeg->base_addr;


		if(len < pSeg->rec_len)		// Если до конца сектора осталось меньше чем нам надо
		{
			adrr_f = pSeg->fin_addr;    // Запоминаем текущую пазицию
			pSeg->fin_addr +=len;		 // Основную позицию сдвигаем на новый сектор 
			if(pSeg->fin_addr >= pSeg->size * _SAVER_FLASH_PAGE_SIZE_) pSeg->fin_addr = 0;	 // Проверка границ сегмента
			addr = pSeg->fin_addr +  pSeg->base_addr;

			// Переводим драйвер Flash в режим RAW
		    DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);	// Теперь нужно записать сами данные в новом очищенном секторе
		    // Устанавливаем адрес чтения
		    DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);	
			WriteDevice(p_saver_info->pFlash, (void *)p_saver_info->buf,  pSeg->rec_len);

			pSeg->fin_addr += pSeg->rec_len;
			
		/*	// 	Проверка только что записанных данных
			// Переводим драйвер Flash в режим RAW
			DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
			// Устанавливаем адрес чтения
			DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
			// Читаем буфер
			ReadDevice(p_saver_info->pFlash, (void *)saver_info.buf,  pSeg->rec_len);
			crc_2 = CRC16((uint8_t *)saver_info.buf, (pSeg->rec_len - 2), 0xFFFF);	    
	    	if(crc == crc_2)		
				pSeg->fin_addr += pSeg->rec_len;		 // Прибавляем записанное
			else
				pSeg->fin_addr = 0; */

											  //  и наконец необходимо забить мусором оставленную область в конце сектора (предыдущего)
		/*	addr = adrr_f + pSeg->base_addr;
			DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
		    // Устанавливаем адрес чтения
		    DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);	
			WriteDevice(p_saver_info->pFlash, (void *)p_saver_info->buf,  len);	 */
		}
		else if(len < pSeg->rec_len * 2)	 // Если эта последняя запись которая влезает в сектор, то нужно очистить следующий
		{
			adrr_f = pSeg->fin_addr + len;		 // Вычисляем адрес следующего сектора
			if(adrr_f >= pSeg->size * _SAVER_FLASH_PAGE_SIZE_) adrr_f = 0;	 // Проверка границ сегмента
			addr = adrr_f +  pSeg->base_addr;
			SeekDevice(p_saver_info->pFlash, addr/_SAVER_FLASH_PAGE_SIZE_, 0);	 // Очищаем след сектор
            DeviceIOCtl(p_saver_info->pFlash, ERASE_BLOCK, 0);
			
			 
			addr = pSeg->fin_addr +  pSeg->base_addr;		// Теперь нужно записать сами данные еще в старом секторе
			// Переводим драйвер Flash в режим RAW
		    DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);	
		    // Устанавливаем адрес чтения
		    DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);	
			WriteDevice(p_saver_info->pFlash, (void *)p_saver_info->buf,  pSeg->rec_len);

			pSeg->fin_addr += pSeg->rec_len;     //  Прибавляем записанное

		}
		else
		{
			adrr_f = pSeg->fin_addr + pSeg->rec_len;    // Прибавляем то количество которое хотим записать
			if((adrr_f - (adrr_f/ _SAVER_FLASH_PAGE_SIZE_)*_SAVER_FLASH_PAGE_SIZE_) == 0)	   // и смотрим не перейдем ли после записи в новый сектор
			{																 //  если мы все же перешли в новый сектор
				if(adrr_f >= pSeg->size * _SAVER_FLASH_PAGE_SIZE_) adrr_f = 0;			 // смотрим не вышли ли мы за приделы своего сегмента, если вышли корректируем
				//addr = adrr_f +  pSeg->base_addr;
				SeekDevice(p_saver_info->pFlash, (adrr_f + pSeg->base_addr)/_SAVER_FLASH_PAGE_SIZE_, 0);  // стираем следующий сектор
	            DeviceIOCtl(p_saver_info->pFlash, ERASE_BLOCK, 0);
			}
			
			// Переводим драйвер Flash в режим RAW
		    DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
		    // Устанавливаем адрес чтения
		    DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
				
			WriteDevice(p_saver_info->pFlash, (void *)p_saver_info->buf,  pSeg->rec_len);

			pSeg->fin_addr += pSeg->rec_len;		 // Прибавляем записанное

			/*// 	Проверка только что записанных данных
			// Переводим драйвер Flash в режим RAW
			DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
			// Устанавливаем адрес чтения
			DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
			// Читаем буфер
			ReadDevice(p_saver_info->pFlash, (void *)saver_info.buf,  pSeg->rec_len);
			crc_2 = CRC16((uint8_t *)saver_info.buf, (pSeg->rec_len - 2), 0xFFFF);	    
	    	if(crc == crc_2)		
				pSeg->fin_addr += pSeg->rec_len;		 // Прибавляем записанное
			else
				pSeg->fin_addr = 0;  */                              
			if(pSeg->fin_addr >= pSeg->size * _SAVER_FLASH_PAGE_SIZE_) pSeg->fin_addr = 0;		   // Проверка граници сегмента
		}
		pSeg->curr_addr = pSeg->fin_addr;	
	}	 
	return count;
}

static int SeekSaver(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin)
{
    SAVER_INFO *p_saver_info = (SAVER_INFO *)(((DEVICE *)handle)->pDriverData);

    if(origin >= SEGMENT_NUMBER) p_saver_info->curr_seg = 0;
	else p_saver_info->curr_seg = origin;  
	
	P_SEG_INFO pSeg = &p_saver_info->seg[p_saver_info->curr_seg]; 

    if(offset >= pSeg->size * _SAVER_FLASH_PAGE_SIZE_)	pSeg->curr_addr = pSeg->fin_addr;
	else
	{
		if(pSeg->crc_flag == 0)
			pSeg->curr_addr = offset;  
	}
   
	return 	 0;
}


static int ReadSaverText(DEVICE_HANDLE handle, void * pDst, int count)
{
	SAVER_INFO *p_saver_info = (SAVER_INFO *)(((DEVICE *)handle)->pDriverData);
	P_SEG_INFO pSeg = &p_saver_info->seg[p_saver_info->curr_text_seg];

	uint32_t addr = p_saver_info->curr_text_addr + pSeg->base_addr;

	if((p_saver_info->curr_text_addr + count) > pSeg->size*_SAVER_FLASH_PAGE_SIZE_)
	{
		 count = pSeg->size*_SAVER_FLASH_PAGE_SIZE_ - p_saver_info->curr_text_addr;
	}

	// Переводим драйвер Flash в режим RAW
	DeviceIOCtl(p_saver_info->pFlash, SWITCH_TO_RAW, 0);
		// Устанавливаем адрес чтения
	DeviceIOCtl(p_saver_info->pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
		// Читаем буфер
	ReadDevice(p_saver_info->pFlash, (void *)pDst,  count);

	p_saver_info->curr_text_addr += count;

    return count;
}
static int WriteSaverText(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return 0;
}
static int SeekSaverText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
	SAVER_INFO *p_saver_info = (SAVER_INFO *)(((DEVICE *)handle)->pDriverData);
	p_saver_info->curr_text_addr = offset;
	p_saver_info->curr_text_seg = origin;
    return 0;
}

static int SaverIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
	return 0;
}

#if(SAVER_INIT)	   
	static char f_buf[1024];
#endif

void Saver_addr_init(uint8_t seg)
{
	P_SEG_INFO pSeg = &saver_info.seg[seg];
	uint32_t addr = 0, offset_addr = 0, len = 0;
	uint16_t crc = 0;
	int i = 0;

#if(SAVER_INIT)   
	SeekDevice(saver_info.pFlash, (pSeg->base_addr/_SAVER_FLASH_PAGE_SIZE_), 0);
	DeviceIOCtl(saver_info.pFlash, ERASE_BLOCK, 0);
	memset(f_buf, 0, 1024);
	for(addr = pSeg->base_addr + _SAVER_FLASH_PAGE_SIZE_; addr < (pSeg->base_addr + pSeg->size * _SAVER_FLASH_PAGE_SIZE_); addr += 1024)
    {
		DeviceIOCtl(saver_info.pFlash, SWITCH_TO_RAW, 0);
		DeviceIOCtl(saver_info.pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);		
		WriteDevice(saver_info.pFlash, (void *)f_buf,  1024);
		ResetWDT();
	} 	 
#endif		  
		
	for(addr = pSeg->base_addr; addr < (pSeg->base_addr + pSeg->size * _SAVER_FLASH_PAGE_SIZE_); addr += pSeg->rec_len, offset_addr += pSeg->rec_len)
    {

		len = _SAVER_FLASH_PAGE_SIZE_ - (offset_addr - (offset_addr/_SAVER_FLASH_PAGE_SIZE_) * _SAVER_FLASH_PAGE_SIZE_);  // Длина до конца сектора

		if(len < pSeg->rec_len)		// Если до конца сектора осталось меньше чем нам надо
		{
			addr += len;
			offset_addr += len;

			if(addr >= (pSeg->base_addr + pSeg->size * _SAVER_FLASH_PAGE_SIZE_)) break;
		}

        // Переводим драйвер Flash в режим RAW
    	DeviceIOCtl(saver_info.pFlash, SWITCH_TO_RAW, 0);
        // Устанавливаем адрес чтения
        DeviceIOCtl(saver_info.pFlash, SET_FLASH_ADDR, (uint8_t *)&addr);
        // Читаем буфер
        ReadDevice(saver_info.pFlash, (void *)saver_info.buf,  pSeg->rec_len);
    
        /*
        Если прочитали незаписанную память (0xFF...)
        */
        for(i = 0; i < pSeg->rec_len; i++)
		{
			if(saver_info.buf[i] != 0xFF) break;
		}
		if(i == pSeg->rec_len) break;
    
        /*
        Прочитали записанную память.
        Если требуется вычисляем CRC.
        */
		pSeg->fin_addr = offset_addr + pSeg->rec_len;
		if(pSeg->crc_flag == 1)
		{
            crc = CRC16((uint8_t *)saver_info.buf, (pSeg->rec_len - 2), 0xFFFF);
    
            /*
            Если CRC сходится - запоминаем адрес
            */
            if(crc == *((uint16_t *)&saver_info.buf[pSeg->rec_len - 2]))
            {
                pSeg->curr_addr = offset_addr + pSeg->rec_len;
            }
		}
		#ifdef NDEBUG
         ResetWDT();
    #endif
   }
    
}


uint8_t AddSaverFile(SAVER_FS_INFO *pFile)
{
    if(first_file == NULL)
    {
        first_file = pFile;
        pFile->pNext = NULL;
    }
    else
    {
        pFile->pNext = first_file;
        first_file = pFile;
    }
    return 0;
}

SAVER_FS_INFO *GetSaverFile(char *name)
{
    SAVER_FS_INFO *pFile = first_file;
    while(pFile != NULL)
    {
        if(strcmp(pFile->name, name) == 0)
        {
            return (SAVER_FS_INFO *) pFile;
        }
        pFile = pFile->pNext;
    }
    return NULL;
}

#endif



