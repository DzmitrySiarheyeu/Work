//#include <__cross_studio_io.h>
#include <stdint.h>
#include "diskio.h"
#include "Clock_drv.h"
#include "DriversCore.h"
#include "Config.h"



/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

extern DEVICE_HANDLE hFlash;

static volatile
DSTATUS Stat = STA_NOINIT;    /* Disk status */



//static
//BYTE PowerFlag = 0;     /* indicates if "power" is on */




//static
//void power_off (void)
//{
 //   PowerFlag = 0;
//}

static
int chk_power(void)        /* Socket power state: 0=off, 1=on */
{
    return 1;
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE drv        /* Physical drive nmuber (0) */
)
{


    if (drv) return STA_NOINIT;            /* Supports only single drive */
    if (Stat & STA_NODISK) return Stat;    /* No card in the socket */

    Stat &= ~STA_NOINIT;        /* Clear STA_NOINIT */

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv        /* Physical drive nmuber (0) */
)
{
    if (drv) return STA_NOINIT;        /* Supports only single drive */
    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,            /* Physical drive nmuber (0) */
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..255) */
)
{

	DEVICE_HANDLE hFlash;
    if (drv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;


	hFlash = OpenDevice("FLASH");
  

    if (count == 1) {    /* Single block read */
            if(SeekDevice(hFlash, (uint16_t)sector, 0) == -1) count = 1;
            else if(ReadDevice(hFlash, (uint8_t *)buff, 0) == -1) count = 1;
            else count = 0;
    }
    else {                /* Multiple block read */
          do {
                if(SeekDevice(hFlash, (uint16_t)sector, 0) == -1) break;
                if(ReadDevice(hFlash, (uint8_t *)buff, 0) == -1) break;
                sector++;
                buff += _FLASH_FAT_SECTOR_SIZE_;
             } while (--count);
        }

		CloseDevice(hFlash);


    

    return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
    BYTE drv,            /* Physical drive nmuber (0) */
    const BYTE *buff,    /* Pointer to the data to be written */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..255) */
)
{
    if (drv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (Stat & STA_PROTECT) return RES_WRPRT;

	DEVICE_HANDLE hFlash;
    hFlash = OpenDevice("FLASH");

    if (count == 1) {    /* Single block write */
        if(SeekDevice(hFlash, (uint16_t)sector, 0) == -1) count = 1;
        else if(WriteDevice(hFlash, (uint8_t *)buff, 0) == -1) count = 1;
        else count = 0;
    }
    else {
            do
            {/* Multiple block write */
              if(SeekDevice(hFlash, (uint16_t)sector, 0) == -1) break;
              if(WriteDevice(hFlash, (uint8_t *)buff, 0) == -1) break;
              sector++;
              buff += _FLASH_FAT_SECTOR_SIZE_;
            } while (--count);
        }

		CloseDevice(hFlash);

    return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive nmuber (0) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    DRESULT res;
    BYTE *ptr = buff;


    if (drv) return RES_PARERR;

    res = RES_ERROR;

    if (ctrl == CTRL_POWER) {
        switch (*ptr) {
        case 0:        /* Sub control code == 0 (POWER_OFF) */
            if (chk_power())
            res = RES_OK;
            break;
        case 1:        /* Sub control code == 1 (POWER_ON) */
            res = RES_OK;
            break;
        case 2:        /* Sub control code == 2 (POWER_GET) */
            *(ptr+1) = (BYTE)chk_power();
            res = RES_OK;
            break;
        default :
            res = RES_PARERR;
        }
    }
    else {
        if (Stat & STA_NOINIT) return RES_NOTRDY;



        switch (ctrl) {
        case GET_SECTOR_COUNT :    /* Get number of sectors on the disk (DWORD) */
            
                *(DWORD*)buff = (DWORD)SECTORS_NUM;
                res = RES_OK;
            break;

        case GET_SECTOR_SIZE :    /* Get sectors on the disk (WORD) */
            *(WORD*)buff = _FLASH_FAT_SECTOR_SIZE_;
            res = RES_OK;
            break;

        case CTRL_SYNC :    /* Make sure that data has been written */
            //if (wait_ready() == 0xFF)
                res = RES_OK;
            break;

        default:
            res = RES_PARERR;
        }

    }

    return res;
}





/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */

DWORD get_fattime (void)
{
	int err = 0;
    uint32_t total_time = 0, year;
    uint8_t temp = 0;

	DEVICE_HANDLE hClock = OpenDevice("Clock");

    err = SeekDevice(hClock, SEC_D, 0); if(err == -1) { CloseDevice(hClock); return err;}
    err = ReadDevice(hClock, (uint8_t *)&temp, 1); if(err == -1) { CloseDevice(hClock); return err;}
    total_time |= (temp >> 1);

    err = SeekDevice(hClock, MIN_D, 0); if(err == -1) { CloseDevice(hClock); return err;}
    err = ReadDevice(hClock, (uint8_t *)&temp, 1); if(err == -1) { CloseDevice(hClock); return err;}
    total_time |= ((uint32_t)temp << 5);

    err = SeekDevice(hClock, HOUR_D, 0); if(err == -1) { CloseDevice(hClock); return err;}
    err = ReadDevice(hClock, (uint8_t *)&temp, 1); if(err == -1) { CloseDevice(hClock); return err;}
    total_time |= ((uint32_t)temp << 11);

    err = SeekDevice(hClock, DATE_D, 0); if(err == -1) { CloseDevice(hClock); return err;}
    err = ReadDevice(hClock, (uint8_t *)&temp, 1); if(err == -1) { CloseDevice(hClock); return err;}
    total_time |= ((uint32_t)temp << 16);

    err = SeekDevice(hClock, MONTH_D, 0); if(err == -1) { CloseDevice(hClock); return err;}
    err = ReadDevice(hClock, (uint8_t *)&temp, 1); if(err == -1) { CloseDevice(hClock); return err;}
    total_time |= ((uint32_t)temp << 21);

    //err = SeekDevice(hClock, CENTURY_D, 0); if(err == -1) return err;
    //err = ReadDevice(hClock, (uint8_t *)&temp, 1); if(err == -1) return err;
    //temp = 21;
    //year = (uint32_t)((uint32_t)(temp - 1) * 100);
    year = 2000;

    err = SeekDevice(hClock, YEAR_D, 0); if(err == -1) { CloseDevice(hClock); return err;}
    err = ReadDevice(hClock, (uint8_t *)&temp, 1); if(err == -1) { CloseDevice(hClock); return err;}    
    year += (uint32_t)temp;
    total_time |= (((uint32_t)year - 1980) << 25);

	CloseDevice(hClock);


    return total_time;

//    return    ((2007UL-1980) << 25)    // Year = 2007
//            | (6UL << 21)            // Month = June
//            | (5UL << 16)            // Day = 5
//            | (11U << 11)            // Hour = 11
//            | (38U << 5)            // Min = 38
//            | (0U >> 1)                // Sec = 0
//            ;

}
