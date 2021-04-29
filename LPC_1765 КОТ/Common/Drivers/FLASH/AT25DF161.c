#include "FLASH_DRV.h"

#if(_FLASH_AT25DF161_)

static int CreateFlash(void);
static DEVICE_HANDLE OpenFlash(void);
static void CloseFlash(DEVICE_HANDLE handle);
static int ReadFlash(DEVICE_HANDLE handle, void * dst, int count);
static int WriteFlash(DEVICE_HANDLE handle, void * dst, int count);
static int SeekFlash(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteFlashText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadFlashText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekFlashText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);

static int FlashIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

void FlashFunc(uint16_t op, DEVICE_HANDLE handle);
int FlashWriteFATFun(DEVICE_HANDLE handle, uint8_t *buf, uint16_t count);
int FlashReadFATFun(DEVICE_HANDLE handle,  uint8_t *buf, uint16_t count);

int FlashWriteRAWFun(DEVICE_HANDLE handle, uint8_t *buf, uint16_t count);
int FlashReadRAWFun(DEVICE_HANDLE handle,  uint8_t *buf, uint16_t count);


int FlashPageErase(DEVICE_HANDLE handle);


const DEVICE_IO flash_device_io = {
    .DeviceID = FLASH_1,
	.CreateDevice =	CreateFlash,
	.CloseDevice = CloseFlash,
    .OpenDevice = OpenFlash,
    .ReadDevice = ReadFlash,
    .WriteDevice = WriteFlash,
    .SeekDevice = SeekFlash,
    .ReadDeviceText = ReadFlashText,
    .WriteDeviceText = WriteFlashText,
    .SeekDeviceText = SeekFlashText,
    .DeviceIOCtl = FlashIOCtl,
    .DeviceName = "FLASH",
    };




int WriteFlashText(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return 0;
}
int ReadFlashText(DEVICE_HANDLE handle, void * pDst, int count)
{
    return 0;
}
int SeekFlashText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    return 0;
}



int CreateFlash(void)
{
    uint8_t res = 0;
    DEVICE_HANDLE hSPI;

    config_cs();

    hSPI = OpenDevice(_FLASH_L_L_DRIVER_NAME_);

    if(hSPI == NULL)
    {
      DEBUG_PUTS("FLASH: unable to open SPI\n");
	  PUT_MES_TO_LOG("FLASH: unable to open SPI", 0, 1);
      return DEVICE_ERROR;
    }

    FLASH_INFO *pFLASHInfo = (FLASH_INFO *)pvPortMalloc((size_t)sizeof(FLASH_INFO));
	if(pFLASHInfo == NULL)
	{
		PUT_MES_TO_LOG("FLASH: no memory", 0, 1);
		DEBUG_PUTS("FLASH: no memory\n");
		return DEVICE_ERROR;
	}
	DEVICE     *pDevice = (DEVICE *)pvPortMalloc((size_t)sizeof(DEVICE));
	if(pDevice == NULL)
	{
		PUT_MES_TO_LOG("FLASH: no memory", 0, 1);
		DEBUG_PUTS("FLASH: no memory\n");
		return DEVICE_ERROR;
	}

    pFLASHInfo->pSPI = hSPI;
	pFLASHInfo->block_num = 0; 
	pFLASHInfo->page_num = 0; 
	pFLASHInfo->work_for_FAT = 1;
	
	pDevice->device_io = (DEVICE_IO *)&flash_device_io;
	pDevice->pData = NULL;
	pDevice->pDriverData = pFLASHInfo;


    select_flash();
    FlashFunc(WRITE_ENABLE, pDevice);	// Разрешам запись
    unselect_flash();

    select_flash();
    FlashFunc(WRITE_STATUS_REGISTR_B1, pDevice);
    WriteDevice(hSPI, &res, 1);
    unselect_flash();

    select_flash();
    FlashFunc(WRITE_DISABLE, pDevice);	// Разрешам запись
    unselect_flash();

	CloseDevice(pFLASHInfo->pSPI);

	vPortFree(pFLASHInfo);
	vPortFree(pDevice);
//	vUserDefrag();

	DEBUG_PUTS("FLASH Created\n");
    return DEVICE_OK;
}


DEVICE_HANDLE OpenFlash(void)
{
	DEVICE_HANDLE hSPI;

	hSPI = OpenDevice(_FLASH_L_L_DRIVER_NAME_);

    if(hSPI == NULL)
    {
      DEBUG_PUTS("FLASH: unable to open SPI\n");
	  PUT_MES_TO_LOG("FLASH: unable to open SPI", 0, 1);
      return NULL;
    }

    FLASH_INFO *pFLASHInfo = (FLASH_INFO *)pvPortMalloc((size_t)sizeof(FLASH_INFO));
	if(pFLASHInfo == NULL)
	{
		PUT_MES_TO_LOG("FLASH: no memory", 0, 1);
		DEBUG_PUTS("FLASH: no memory\n");
		return NULL;
	}
	DEVICE     *pDevice = (DEVICE *)pvPortMalloc((size_t)sizeof(DEVICE));
	if(pDevice == NULL)
	{
		PUT_MES_TO_LOG("FLASH: no memory", 0, 1);
		DEBUG_PUTS("FLASH: no memory\n");
		return NULL;
	}

    pFLASHInfo->pSPI = hSPI;
	pFLASHInfo->block_num = 0; 
	pFLASHInfo->page_num = 0; 
	pFLASHInfo->work_for_FAT = 1;
	
	pDevice->device_io = (DEVICE_IO *)&flash_device_io;
	pDevice->pData = NULL;
	pDevice->pDriverData = pFLASHInfo;

	return 	(DEVICE_HANDLE)pDevice;
}


void CloseFlash(DEVICE_HANDLE handle)
{
	FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

	DEVICE_HANDLE hSpi = pFlashInfo->pSPI;

	vPortFree(pFlashInfo);
	vPortFree(handle);

	CloseDevice(hSpi);

//	vUserDefrag();
}


int SeekFlash(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin)
{
    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

       // if(offset < 512)
       // {
            pFlashInfo->block_num = (uint32_t)offset ;
            return (int)offset;
       // }
       // else
       // {
       //     return -1;
       // }
}

static int FlashIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

        switch(code)
        {
            case SWITCH_TO_RAW:
                pFlashInfo->work_for_FAT = 0;
                break;

            case SET_FLASH_ADDR:   // Функцией SeekFlash мы не сможем установить 32-битный адрес
                pFlashInfo->block_num = *(uint32_t *)pData;  
                break;

            case ERASE_BLOCK:
                FlashPageErase(handle);
                break;

            default:
                return -1;
        }

        return 0;
}

int WriteFlash(DEVICE_HANDLE handle, void * pSrc, int count)
{   

    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

        if(pFlashInfo->work_for_FAT == 1)
        {
            if(FlashWriteFATFun(handle, (uint8_t *)pSrc, /*512*/_FLASH_FAT_SECTOR_SIZE_) != -1)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            if(FlashWriteRAWFun(handle, (uint8_t *)pSrc, count) != -1)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
}

int ReadFlash(DEVICE_HANDLE handle, void * pDst, int count) 
{

    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

        if(pFlashInfo->work_for_FAT == 1)
        {
            if(FlashReadFATFun(handle, (uint8_t *)pDst, /*512*/_FLASH_FAT_SECTOR_SIZE_) != -1)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            if(FlashReadRAWFun(handle, (uint8_t *)pDst, count) != -1)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
}

// Записать код операции во флэш
void FlashFunc(uint16_t op, DEVICE_HANDLE handle)
{
    uint32_t addr;

    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

    WriteDevice(pFlashInfo->pSPI, (uint8_t *)&op, 1);   // Записываем код операции
    
    switch(op)
    {
        case ERASE_64K_BLOCK:
            addr = pFlashInfo->block_num * 65536;   //Вычисление адреса начала 64-кб блока
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 2, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 1, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 0, 1);
            break;

        case ERASE_32K_BLOCK:
            addr = pFlashInfo->block_num * 32768;   //Вычисление адреса начала 32-кб блока
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 2, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 1, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 0, 1);
            break;

        case ERASE_4K_BLOCK:
            addr = pFlashInfo->block_num * 4096;   //Вычисление адреса начала 4-кб блока
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 2, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 1, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 0, 1);
            break;

        case BYTE_PAGE_PROGRAM:
        case READ:
            if(pFlashInfo->work_for_FAT == 1)
                addr = pFlashInfo->block_num * _FLASH_FAT_SECTOR_SIZE_ + pFlashInfo->page_num * 256;   //Вычисление адреса начала страницы внутри 4-кб блока (для FAT)
            else
                addr = pFlashInfo->block_num;  //просто адрес (для RAW)
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 2, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 1, 1);
            WriteDevice(pFlashInfo->pSPI, (uint8_t *)&addr + 0, 1);
            break;

        default:
            break;
    }
}

//Ожидание готовности Flash
int WaitRDY(DEVICE_HANDLE handle)
{
    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);
    uint16_t flash_satus = 0xFF;
	select_flash();
	FlashFunc(READ_STATUS_REGISTR, handle);
    do
    {
        ReadDevice(pFlashInfo->pSPI, (uint8_t *)&flash_satus, 2);
    }
    while(flash_satus & FLASH_STATUS_BUSY);  
	unselect_flash();
    return 0;
}


int FlashPageErase(DEVICE_HANDLE handle)
{
    select_flash();
    FlashFunc(WRITE_ENABLE, handle);	// Разрешам запись
    unselect_flash();
  
    WaitRDY(handle);                          // Ждем 
    
    select_flash();
    FlashFunc(ERASE_4K_BLOCK, handle);	// Стираем блок 4кб
    unselect_flash();

    WaitRDY(handle);                          // Ждем

    select_flash();
    FlashFunc(WRITE_DISABLE, handle);	// Запрещаем запись
    unselect_flash();

    WaitRDY(handle);                          // Ждем 

    return 0;
}

// Записать во флэш для FAT
int FlashWriteFATFun(DEVICE_HANDLE handle, uint8_t *buf, uint16_t count)
{
    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);
    
    select_flash();
    FlashFunc(WRITE_ENABLE, handle);	// Разрешам запись
    unselect_flash();
  
    WaitRDY(handle);                          // Ждем 
    
    select_flash();
    FlashFunc(ERASE_4K_BLOCK, handle);	// Стираем блок 4кб
    unselect_flash();

    WaitRDY(handle);                          // Ждем 

    uint8_t page_counter;

    for(page_counter = 0; page_counter < 4096 / 256; page_counter++) // нам нужно записать 16 страниц по 256 байт
    {
        pFlashInfo->page_num = (uint32_t)page_counter;  //пишем страницу page_counter

        select_flash();
        FlashFunc(WRITE_ENABLE, handle);	// Разрешам запись
        unselect_flash();
  
        WaitRDY(handle);                          // Ждем 

        select_flash();
        FlashFunc(BYTE_PAGE_PROGRAM, handle);	// Команда на запись
        WriteDevice(pFlashInfo->pSPI, buf + page_counter * 256, 256); // Пишем страницу
        unselect_flash();

        WaitRDY(handle);                          // Ждем
    }     

    select_flash();
    FlashFunc(WRITE_DISABLE, handle);	// Запрещаем запись
    unselect_flash();

    WaitRDY(handle);                          // Ждем 

    return 0;
}


// Чтение из флэш для FAT
int FlashReadFATFun(DEVICE_HANDLE handle,  uint8_t *buf, uint16_t count)
{
    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

    uint8_t page_counter;

    for(page_counter = 0; page_counter < 4096 / 256; page_counter++) // нам нужно прочитать 16 страниц по 256 байт
    {
        pFlashInfo->page_num = (uint32_t)page_counter;  //читаем страницу page_counter

        select_flash();
        FlashFunc(READ, handle);	// Команда на чтение
        ReadDevice(pFlashInfo->pSPI, buf + page_counter * 256, 256); // читаем страницу
        unselect_flash();

        WaitRDY(handle);                          // Ждем 
    }     

    return 0;
}

// "Чистая" записать во флэш
int FlashWriteRAWFun(DEVICE_HANDLE handle, uint8_t *buf, uint16_t count)
{
	uint32_t tot_len = 0, len = 0, addr_ofset = 0;
	FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

	len = 256 - (pFlashInfo->block_num - ((pFlashInfo->block_num/256) * 256));  // Длина до конца страници
	tot_len = count;
	
	if(tot_len < len) len = tot_len; // Если нужно будет перелизать в другуую страницу
										 // Сначала допишем до конца старую                          

	while(tot_len)
	{
		select_flash();
	    FlashFunc(WRITE_ENABLE, handle);	// Разрешам запись
	    unselect_flash();
	
	    WaitRDY(handle);		 // Ждем 

		select_flash();
	    FlashFunc(BYTE_PAGE_PROGRAM, handle);	// Команда на запись
	    WriteDevice(pFlashInfo->pSPI, (buf + addr_ofset), len); // Пишем
	    unselect_flash();

    	WaitRDY(handle);

		select_flash();
	    FlashFunc(WRITE_DISABLE, handle);	// Запрещаем запись
	    unselect_flash();
	
	    WaitRDY(handle); 
		
		pFlashInfo->block_num += len;
		addr_ofset += len;
		tot_len -= len;
		
		if(tot_len <= 256) len = tot_len;
		else len = 256; 
	}
                       // Ждем 

    pFlashInfo->work_for_FAT = 1;   // Возвращаемся обратно в режим FAT

    return 0;
}


// "Чистое" чтение из флэш
int FlashReadRAWFun(DEVICE_HANDLE handle,  uint8_t *buf, uint16_t count)
{
    FLASH_INFO *pFlashInfo = (FLASH_INFO *)(((DEVICE *)handle)->pDriverData);

    select_flash();
    FlashFunc(READ, handle);	// Команда на чтение
    ReadDevice(pFlashInfo->pSPI, buf, count); // читаем
    unselect_flash();

    WaitRDY(handle);                          // Ждем 

    pFlashInfo->work_for_FAT = 1;   // Возвращаемся обратно в режим FAT

    return 0;
}

#endif



