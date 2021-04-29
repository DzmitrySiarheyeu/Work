#include <__cross_studio_io.h>
#include <stdint.h>
#include "diskio.h"
#include "DriversCore.h"



/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

extern DEVICE_HANDLE hFlash;

static volatile
DSTATUS Stat = STA_NOINIT;    /* Disk status */



static
BYTE PowerFlag = 0;     /* indicates if "power" is on */




static
void power_off (void)
{
    PowerFlag = 0;
}

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
    BYTE n, ty, ocr[4];


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
    if (drv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

  

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
                buff += 512;
             } while (--count);
        }


    

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
              buff += 512;
            } while (--count);
        }

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
    BYTE n, *ptr = buff;
    WORD csize;


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
            
                *(DWORD*)buff = (DWORD)4096;
                res = RES_OK;
            break;

        case GET_SECTOR_SIZE :    /* Get sectors on the disk (WORD) */
            *(WORD*)buff = 512;
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

    return    ((2007UL-1980) << 25)    // Year = 2007
            | (6UL << 21)            // Month = June
            | (5UL << 16)            // Day = 5
            | (11U << 11)            // Hour = 11
            | (38U << 5)            // Min = 38
            | (0U >> 1)                // Sec = 0
            ;

}
