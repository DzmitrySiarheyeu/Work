//-----------------------------------------------------------------------------
// Software that is described herein is for illustrative purposes only  
// which provides customers with programming information regarding the  
// products. This software is supplied "AS IS" without any warranties.  
// NXP Semiconductors assumes no responsibility or liability for the 
// use of the software, conveys no license or title under any patent, 
// copyright, or mask work right to the product. NXP Semiconductors 
// reserves the right to make changes in the software without 
// notification. NXP Semiconductors also make no representation or 
// warranty that such application will be suitable for the specified 
// use without further testing or modification. 
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <string.h>

#include "sbl_iap.h"
#include "sbl_config.h"
#include "system_lpc11xx.h"

#ifdef CRP_ON
	const unsigned crp __attribute__((section(".ARM.__at_0x2FC"))) = CRP1;
#else
	const unsigned crp __attribute__((section(".ARM.__at_0x2FC"))) = CRP;
#endif

const unsigned sector_start_map[MAX_FLASH_SECTOR] = {SECTOR_0_START,             \
SECTOR_1_START,SECTOR_2_START,SECTOR_3_START,SECTOR_4_START,SECTOR_5_START,      \
SECTOR_6_START,SECTOR_7_START,SECTOR_8_START,SECTOR_9_START,SECTOR_10_START,     \
SECTOR_11_START,SECTOR_12_START,SECTOR_13_START,SECTOR_14_START,SECTOR_15_START, \
SECTOR_16_START,SECTOR_17_START,SECTOR_18_START,SECTOR_19_START,SECTOR_20_START, \
SECTOR_21_START,SECTOR_22_START,SECTOR_23_START,SECTOR_24_START,SECTOR_25_START, \
SECTOR_26_START,SECTOR_27_START,SECTOR_28_START,SECTOR_29_START					 };

const unsigned sector_end_map[MAX_FLASH_SECTOR] = {SECTOR_0_END,SECTOR_1_END,    \
SECTOR_2_END,SECTOR_3_END,SECTOR_4_END,SECTOR_5_END,SECTOR_6_END,SECTOR_7_END,   \
SECTOR_8_END,SECTOR_9_END,SECTOR_10_END,SECTOR_11_END,SECTOR_12_END,             \
SECTOR_13_END,SECTOR_14_END,SECTOR_15_END,SECTOR_16_END,SECTOR_17_END,           \
SECTOR_18_END,SECTOR_19_END,SECTOR_20_END,SECTOR_21_END,SECTOR_22_END,           \
SECTOR_23_END,SECTOR_24_END,SECTOR_25_END,SECTOR_26_END,                         \
SECTOR_27_END,SECTOR_28_END,SECTOR_29_END										 };

unsigned param_table[5];
unsigned result_table[5];

char flash_buf[FLASH_BUF_SIZE];

unsigned * flash_address;
unsigned byte_ctr;

void write_data(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count);
void find_erase_prepare_sector(unsigned cclk, unsigned flash_address);
void erase_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
void prepare_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
void iap_entry(unsigned param_tab[],unsigned result_tab[]);
void enable_interrupts(unsigned interrupts);
void disable_interrupts(unsigned interrupts);

unsigned write_flash(unsigned * dst, char * src, unsigned no_of_bytes)
{
	unsigned int IAPCclk = IAP_SYS_FREQ;	// system freq in terms of 1 kHz
	unsigned *sectorStart = (unsigned *)(((unsigned)dst / 4096) * 4096);
	memcpy(flash_buf, (char *)sectorStart, 4096);
	memcpy(flash_buf + (unsigned)dst - (unsigned)sectorStart, src, no_of_bytes);

	flash_address = sectorStart;
	byte_ctr = FLASH_BUF_SIZE;

	if( byte_ctr == FLASH_BUF_SIZE)
	{
	  /* We have accumulated enough bytes to trigger a flash write */
	  find_erase_prepare_sector(IAPCclk, (unsigned)flash_address);
      if(result_table[0] != CMD_SUCCESS)
      {
        while(1); /* No way to recover. Just let Windows report a write failure */
      }
      write_data(IAPCclk,(unsigned)flash_address,(unsigned *)flash_buf,FLASH_BUF_SIZE);
      if(result_table[0] != CMD_SUCCESS)
      {
        while(1); /* No way to recover. Just let Windows report a write failure */
      }
	  /* Reset byte counter and flash address */
	  byte_ctr = 0;
	  flash_address = 0;
	}
    return(CMD_SUCCESS);
}

void find_erase_prepare_sector(unsigned cclk, unsigned flash_address)
{
    unsigned i;
    unsigned interrupts;


	disable_interrupts(interrupts);

    for(i=USER_START_SECTOR;i<=MAX_USER_SECTOR;i++)
    {
        if(flash_address < sector_end_map[i])
        {
            if( flash_address == sector_start_map[i])
            {
                prepare_sector(i,i,cclk);
                erase_sector(i,i,cclk);
            }
            prepare_sector(i,i,cclk);
            break;
        }
    }
	enable_interrupts(interrupts);
}

void write_data(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count)
{
    unsigned interrupts;

	disable_interrupts(interrupts);
    param_table[0] = COPY_RAM_TO_FLASH;
    param_table[1] = flash_address;
    param_table[2] = (unsigned)flash_data_buf;
    param_table[3] = count;
    param_table[4] = cclk;
    iap_entry(param_table,result_table);
	enable_interrupts(interrupts);
}

void erase_sector(unsigned start_sector,unsigned end_sector,unsigned cclk)
{
    param_table[0] = ERASE_SECTOR;
    param_table[1] = start_sector;
    param_table[2] = end_sector;
    param_table[3] = cclk;
    iap_entry(param_table,result_table);
}

void prepare_sector(unsigned start_sector,unsigned end_sector,unsigned cclk)
{
    param_table[0] = PREPARE_SECTOR_FOR_WRITE;
    param_table[1] = start_sector;
    param_table[2] = end_sector;
    param_table[3] = cclk;
    iap_entry(param_table,result_table);
}

void iap_entry(unsigned param_tab[],unsigned result_tab[])
{
    void (*iap)(unsigned [],unsigned []);

    iap = (void (*)(unsigned [],unsigned []))IAP_ADDRESS;
    iap(param_tab,result_tab);
}

void enable_interrupts(unsigned interrupts)
{
    __enable_irq();
}

void disable_interrupts(unsigned interrupts)
{
    __disable_irq();
}
