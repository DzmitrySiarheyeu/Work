/*
    FreeRTOS V6.0.5 - Copyright (C) 2010 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS eBook                                  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
 * A sample implementation of pvPortMalloc() and vPortFree() that permits
 * allocated blocks to be freed, but does not combine adjacent free blocks
 * into a single larger block.
 *
 * See heap_1.c and heap_3.c for alternative implementations, and the memory
 * management pages of http://www.FreeRTOS.org for more information.
 */
#include <stdlib.h>
#include <stdint.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"
#include "Config.h"


#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Allocate the memory for the heap.  The struct is used to force byte
alignment without using any non-portable code. */
union xRTOS_HEAP
{
	#if portBYTE_ALIGNMENT == 8
		volatile portDOUBLE dDummy;
	#else
		volatile unsigned long ulDummy;
	#endif
	unsigned char ucHeap[ configTOTAL_HEAP_SIZE ];
} xHeap;

/* Define the linked list structure.  This is used to link free blocks in order
of their size. */
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
} xBlockLink;


static const unsigned short  heapSTRUCT_SIZE	= ( sizeof( xBlockLink ) + portBYTE_ALIGNMENT - ( sizeof( xBlockLink ) % portBYTE_ALIGNMENT ) );
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

/* Create a couple of list links to mark the start and end of the list. */
xBlockLink xStart, xEnd;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining = configTOTAL_HEAP_SIZE;

/* STATIC FUNCTIONS ARE DEFINED AS MACROS TO MINIMIZE THE FUNCTION CALL DEPTH. */

/*
 * Insert a block into the list of free blocks - which is ordered by size of
 * the block.  Small blocks at the start of the list and large blocks at the end
 * of the list.
 */
#define prvInsertBlockIntoFreeList( pxBlockToInsert )								\
{																					\
xBlockLink *pxIterator;																\
size_t xBlockSize;																	\
																					\
	xBlockSize = pxBlockToInsert->xBlockSize;										\
																					\
	/* Iterate through the list until a block is found that has a larger size */	\
	/* than the block we are inserting. */											\
	for( pxIterator = &xStart; pxIterator->pxNextFreeBlock->xBlockSize < xBlockSize; pxIterator = pxIterator->pxNextFreeBlock )	\
	{																				\
		/* There is nothing to do here - just iterate to the correct position. */	\
	}																				\
																					\
	/* Update the list to include the block being inserted in the correct */		\
	/* position. */															\
																	  \
																	\
	pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;					\
	pxIterator->pxNextFreeBlock = pxBlockToInsert;									\
}
/*-----------------------------------------------------------*/

/*void prvInsertBlockIntoFreeList( xBlockLink *pxBlockToInsert )								
{																					
xBlockLink *pxIterator;																
size_t xBlockSize;																	
																					
	xBlockSize = pxBlockToInsert->xBlockSize;										
																													
	for( pxIterator = &xStart; pxIterator->pxNextFreeBlock->xBlockSize < xBlockSize; pxIterator = pxIterator->pxNextFreeBlock )	
	{																				
	}																				
																																		
																	  
																	
	pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;					
	pxIterator->pxNextFreeBlock = pxBlockToInsert;									
}*/

#define prvHeapInit()																\
{																					\
xBlockLink *pxFirstFreeBlock;														\
																					\
	/* xStart is used to hold a pointer to the first item in the list of free */	\
	/* blocks.  The void cast is used to prevent compiler warnings. */				\
	xStart.pxNextFreeBlock = ( void * ) xHeap.ucHeap;								\
	xStart.xBlockSize = ( size_t ) 0;												\
																					\
	/* xEnd is used to mark the end of the list of free blocks. */					\
	xEnd.xBlockSize = configTOTAL_HEAP_SIZE;										\
	xEnd.pxNextFreeBlock = NULL;													\
																					\
	/* To start with there is a single free block that is sized to take up the		\
	entire heap space. */															\
	pxFirstFreeBlock = ( void * ) xHeap.ucHeap;										\
	pxFirstFreeBlock->xBlockSize = configTOTAL_HEAP_SIZE;							\
	pxFirstFreeBlock->pxNextFreeBlock = &xEnd;										\
}
/*-----------------------------------------------------------*/

void *pvPortMalloc( size_t xWantedSize )
{
xBlockLink *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
static portBASE_TYPE xHeapHasBeenInitialised = pdFALSE;
void *pvReturn = NULL;

	vTaskSuspendAll();
	{
		/* If this is the first call to malloc then the heap will require
		initialisation to setup the list of free blocks. */
		if( xHeapHasBeenInitialised == pdFALSE )
		{
			prvHeapInit();
			xHeapHasBeenInitialised = pdTRUE;
		}

		/* The wanted size is increased so it can contain a xBlockLink
		structure in addition to the requested amount of bytes. */
		if( xWantedSize > 0 )
		{
			xWantedSize += heapSTRUCT_SIZE;

			/* Ensure that blocks are always aligned to the required number of bytes. */
			if( xWantedSize & portBYTE_ALIGNMENT_MASK )
			{
				/* Byte alignment required. */
				xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
			}
		}

		if( ( xWantedSize > 0 ) && ( xWantedSize < configTOTAL_HEAP_SIZE ) )
		{
			/* Blocks are stored in byte order - traverse the list from the start
			(smallest) block until one of adequate size is found. */
			pxPreviousBlock = &xStart;
			pxBlock = xStart.pxNextFreeBlock;
			while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock ) )
			{
				pxPreviousBlock = pxBlock;
				pxBlock = pxBlock->pxNextFreeBlock;
			}

			/* If we found the end marker then a block of adequate size was not found. */
			if( pxBlock != &xEnd )
			{
				/* Return the memory space - jumping over the xBlockLink structure
				at its start. */
				pvReturn = ( void * ) ( ( ( unsigned char * ) pxPreviousBlock->pxNextFreeBlock ) + heapSTRUCT_SIZE );

				/* This block is being returned for use so must be taken our of the
				list of free blocks. */
				pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

				/* If the block is larger than required it can be split into two. */
				if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
				{
					/* This block is to be split into two.  Create a new block
					following the number of bytes requested. The void cast is
					used to prevent byte alignment warnings from the compiler. */
					pxNewBlockLink = ( void * ) ( ( ( unsigned char * ) pxBlock ) + xWantedSize );

					/* Calculate the sizes of two blocks split from the single
					block. */
					pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
					pxBlock->xBlockSize = xWantedSize;

					/* Insert the new block into the list of free blocks. */
					prvInsertBlockIntoFreeList( ( pxNewBlockLink ) );
				}
				
				xFreeBytesRemaining -= xWantedSize;
			}
		}
	}
	xTaskResumeAll();

	#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
	}
	#endif

	return pvReturn;
}
/*-----------------------------------------------------------*/
void defragmentation_my(void);


volatile int i = 0;
void vPortFree( void *pv )
{
	vTaskSuspendAll();
	{
		unsigned char *puc = ( unsigned char * ) pv;
		xBlockLink *pxLink;
		
			if( pv )
			{
				/* The memory being freed will have an xBlockLink structure immediately
				before it. */
				puc -= heapSTRUCT_SIZE;
		
				/* This casting is to keep the compiler from issuing warnings. */
				pxLink = ( void * ) puc;

		
					/* Add this block to the list of free blocks. */
					prvInsertBlockIntoFreeList( ( ( xBlockLink * ) pxLink ) );
					xFreeBytesRemaining += pxLink->xBlockSize;

				   defragmentation_my();
			}
	}
	xTaskResumeAll();
}
/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize( void )
{
	return xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

void vPortInitialiseBlocks( void )
{
	/* This just exists to keep the linker quiet. */
}


int defragmentation(void)
{
	xBlockLink *cur_Block, *check_Block, *preCheck_Block, *preCur_Block;
    uint32_t f_addr = 0;

    cur_Block = xStart.pxNextFreeBlock;
   // cur_Block = (xBlockLink *)&xStart;       // Текущий блок
    check_Block = cur_Block->pxNextFreeBlock; // Блок сравниваемый с текущим
    preCheck_Block = cur_Block;      //  Блок который идет перед сравниваемым
    preCur_Block = (xBlockLink *)&xStart;  // блок который идет перед текущим

    while(cur_Block->pxNextFreeBlock) // цикл пока текущий блок не окажится xEnd
    {
        f_addr = (uint32_t)cur_Block + cur_Block->xBlockSize; // ищится блок с этим адресом
        while(check_Block->pxNextFreeBlock) // цикл пока сравниваемый блок не окажется xEnd
        {
            if(f_addr == (uint32_t)check_Block) // Если нашлись два рядом стоящих блока
            {
                if(cur_Block->xBlockSize < check_Block->xBlockSize) // Если текущий блок находится в списке перед сравниваемым
                {
                    cur_Block->xBlockSize += check_Block->xBlockSize;
                    if(cur_Block->pxNextFreeBlock == check_Block)  // Если сравниваемый блок расположен сразу за текущим
                    {
                        preCur_Block->pxNextFreeBlock = check_Block->pxNextFreeBlock; // соединяем блок стоящий перед текущим с блоком стоящим после сравниваемого
                    }
                    else
                    {
                        preCur_Block->pxNextFreeBlock = cur_Block->pxNextFreeBlock; // соединяем блок стоящий перед текущим с блоком стоящим после текущего
                        preCheck_Block->pxNextFreeBlock = check_Block->pxNextFreeBlock;  //  соединяем блок стоящий перед сравниваемым с блоком стоящим после сравниваемого
                    }
                }
                else if(cur_Block->xBlockSize > check_Block->xBlockSize) // Если текущий блок находится в списке после сравниваемого
                {
                    cur_Block->xBlockSize += check_Block->xBlockSize;
                    if(check_Block->pxNextFreeBlock == cur_Block) // Если текущий блок расположен сразу за сравниваемым 
                    {
                        preCheck_Block->pxNextFreeBlock = cur_Block->pxNextFreeBlock;  // соединяем блок стоящий перед сравниваемым с блоком стоящим после текущего
                    }
                    else
                    {
                        preCur_Block->pxNextFreeBlock = cur_Block->pxNextFreeBlock; // соединяем блок стоящий перед текущим с блоком стоящим после текущего
                        preCheck_Block->pxNextFreeBlock = check_Block->pxNextFreeBlock;  //  соединяем блок стоящий перед сравниваемым с блоком стоящим после сравниваемого
                    }
                }
				else return -1;

                prvInsertBlockIntoFreeList(cur_Block);
               // vUserDefrag();
                return 1;
            }

            preCheck_Block = check_Block;
			if(check_Block == preCheck_Block->pxNextFreeBlock) while(1);
          	check_Block = preCheck_Block->pxNextFreeBlock;
        }
        preCur_Block = cur_Block;
		if(cur_Block == preCur_Block->pxNextFreeBlock) while(1);
        cur_Block = preCur_Block->pxNextFreeBlock;
        check_Block = xStart.pxNextFreeBlock;
        preCheck_Block = (xBlockLink *)&xStart;
    }
	return 0;
}


void defragmentation_my(void)
{
	volatile int flag = 0;
	vPortEnterCritical();
	{
		do
		{
			flag = defragmentation();
	
		}while(flag != 0 && flag != -1); 
			
	}vPortExitCritical(); 
}

/*void vUserDefrag(void)
{
	int flag = 0;
	vPortEnterCritical();
	{
		do
		{
			flag = defragmentation();
	
		}while(flag);
			
	}vPortExitCritical(); 
}  */

void vGetFreeMem(uint16_t * xMaxBlock, uint16_t * xTotalMem)
{
    xBlockLink * xBlock = &xStart; 
    *xMaxBlock = 0;
    *xTotalMem = 0;

    do
    {
        if( xBlock->xBlockSize > *xMaxBlock )
        {
            *xMaxBlock = xBlock->xBlockSize;
        }

        *xTotalMem += xBlock->xBlockSize;

        xBlock = xBlock->pxNextFreeBlock;
    }
    while( xBlock->pxNextFreeBlock );

   // *xMaxBlock = xFreeBytesRemaining;
}
