/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: virtual_nic.c$
* $Version : 3.6.3.0$
* $Date    : Nov-8-2010$
*
* Comments:
*
* @brief  The file emulates a USB PORT as Network Card.
*****************************************************************************/ 

/******************************************************************************
 * Includes
 *****************************************************************************/

#include <stdint.h>
//#include <stddef.h>
#include "Config.h"
#include <string.h>
#include "usb.h"
#include "usbhw.h"
#include "cdc_rndis.h"
#include "type.h"
#include "cdcuser.h"
#include "virtual_nic.h"
#include "usb_rndis_descriptor.h"



/*****************************************************************************
 * Constant and Macro's 
 *****************************************************************************/
/* Base unit for ENIT layer is 1Mbps while for RNDIS its 100bps*/
#define ENET_CONVERT_FACTOR         (10000)
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/              

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
//void USB_App_Callback(uint_8 event_type, void* val,pointer arg);
//void USB_Notif_Callback(uint_8 event_type,void* val,pointer arg); 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
static uint8_t g_zero_send = 0x00;

static uint8_t first_send_buffer[DIC_BULK_IN_ENDP_PACKET_SIZE];
static uint8_t first_read_buff[DIC_BULK_OUT_ENDP_PACKET_SIZE];
static uint8_t first_read_buff_2[DIC_BULK_OUT_ENDP_PACKET_SIZE];



int RndisWrite(DEVICE_HANDLE handle, uint8_t *nic_data_ptr, uint32_t length)
{
	/* if total data to be sent to host is greater than 
       DIC_BULK_IN_ENDP_PACKET_SIZE than break it to max endpoint
       size for bulk In -- approach used to increase performance by avoiding 
       large memory allocations and memcopies in application layer */       
    uint32_t usb_tx_len = length + RNDIS_USB_OVERHEAD_SIZE;  
    uint32_t first_packet_size = 0;
    uint8_t *first_send_buff = first_send_buffer;
    uint8_t zero_send_flag;

	int count = 0, count_2 = 0;
    
	// usb_tx_len +=  DIC_BULK_IN_ENDP_PACKET_SIZE - usb_tx_len%DIC_BULK_IN_ENDP_PACKET_SIZE;
	// length += DIC_BULK_IN_ENDP_PACKET_SIZE - usb_tx_len%DIC_BULK_IN_ENDP_PACKET_SIZE;
	    
    /* RNDIS Protocol defines 1 byte call of 0x00, if 
    transfer size is multiple of endpoint packet size */
    zero_send_flag = (uint8_t)((usb_tx_len%DIC_BULK_IN_ENDP_PACKET_SIZE) ?
         0 : 1);
    
    /* whichver is smaller but not less than RNDIS_USB_OVERHEAD_SIZE */
    first_packet_size = usb_tx_len > DIC_BULK_IN_ENDP_PACKET_SIZE ? DIC_BULK_IN_ENDP_PACKET_SIZE : usb_tx_len;
    
    if(first_packet_size < RNDIS_USB_OVERHEAD_SIZE)
    {/* for DIC_BULK_IN_ENDP_PACKET_SIZE as 8, 16 or 32
        minimun  first_packet_size has to be either usb_tx_len
        (which is definitely greater than RNDIS_USB_OVERHEAD_SIZE)
        or atleast 64 which is the next allowed multiple of 
        DIC_BULK_IN_ENDP_PACKET_SIZE*/ 
        first_packet_size = usb_tx_len > 64 ? 64 : usb_tx_len;
    }
        
    /* we are reserving one DWORD before packet for distingusihing between 
       packet with USB HEADER and the one only with payload */
	memset(first_send_buff, 0, (first_packet_size));


    /* Zero marks Packet with USB HEADER 
       non zero value is used to preserve enet_pcb value before packet*/
    //*((uint32_t *)first_send_buff) = (uint32_t)0;       
    
    //first_send_buff += 4;/* address for actual payload */
    
    /* Prepare USB Header */
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->message_type = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_PACKET_MSG);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->message_len = LONG_LE_TO_HOST(usb_tx_len);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->data_offset = LONG_LE_TO_HOST_CONST(RNDIS_DATA_OFFSET);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->data_len = LONG_LE_TO_HOST(length);
    
    /* fill rest of first_send_buff buffers with payload as much as possible */
    USB_mem_copy(nic_data_ptr, first_send_buff + RNDIS_USB_OVERHEAD_SIZE, first_packet_size - RNDIS_USB_OVERHEAD_SIZE);   

   // return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
    //    first_send_buff, first_packet_size);
	
	count = WriteDevice(handle, first_send_buff, first_packet_size); 
	if(count == -1) return -1;
	if(count != first_packet_size) return -1;
	count = first_packet_size - RNDIS_USB_OVERHEAD_SIZE;
             

    if(usb_tx_len > first_packet_size)
    {       
        /* enet_pcb address has been appended before buffer pointer used for 
           Send data call on USB */
//        *((uint32_t *)(nic_data_ptr + (first_packet_size - RNDIS_USB_OVERHEAD_SIZE)) - 1) = 
//            (uint32_t)enet_pcb;
        
       // return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
         //   nic_data_ptr + (first_packet_size - RNDIS_USB_OVERHEAD_SIZE) , 
           // usb_tx_len - first_packet_size);
	   count_2 = WriteDevice(handle, nic_data_ptr + (first_packet_size - RNDIS_USB_OVERHEAD_SIZE), usb_tx_len - first_packet_size);
	   if(count_2 == -1) return -1;
	   count_2 += count;
               
    }

    if(zero_send_flag == 1)
    {
      // return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
       //     &g_zero_send, sizeof(uint_8));
	//	if(WriteDevice(handle, &g_zero_send,  sizeof(uint8_t)) == -1) return -1; 
    }
    
    return count_2;





//	uint32_t usb_tx_len = length + RNDIS_USB_OVERHEAD_SIZE;
//
//	memset(first_send_buffer, 0, RNDIS_USB_OVERHEAD_SIZE);
//    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buffer)->message_type = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_PACKET_MSG);
//    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buffer)->message_len = LONG_LE_TO_HOST(usb_tx_len);
//    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buffer)->data_offset = LONG_LE_TO_HOST_CONST(RNDIS_DATA_OFFSET);
//    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buffer)->data_len = LONG_LE_TO_HOST(length);
//
//	int zero_send_flag = (uint8_t)((usb_tx_len%DIC_BULK_IN_ENDP_PACKET_SIZE) ? 0 : 1);
//	int count = 0;
//	uint8_t temp = 0;
//
//	WriteDevice(handle, first_send_buffer, RNDIS_USB_OVERHEAD_SIZE);
//	count = WriteDevice(handle, nic_data_ptr, length);
////	if(zero_send_flag == 1)
////		WriteDevice(handle, &temp, 1);
	return count;	
	
}


uint8_t zero_recv_flag = 0;
uint8_t first_recv_flag = 0;
uint32_t temp_count = 0;
int RndisRxReady(DEVICE_HANDLE handle)
{
	int count;
    int message_len = 0;

	count = ReadDevice(handle, first_read_buff, DIC_BULK_OUT_ENDP_PACKET_SIZE);
	if(count == -1 || count == 0) return 0;

	message_len = LONG_LE_TO_HOST(*((uint32_t *)first_read_buff + 1));
	zero_recv_flag = 0;
	if(!(message_len%DIC_BULK_OUT_ENDP_PACKET_SIZE))
    {
        /* RNDIS Protocol defines 1 byte call of 0x00, if 
        transfer size is multiple of endpoint packet size */
    	message_len++;
	zero_recv_flag = 1;
    }

/*	if(message_len > 700 || *((uint32_t *)first_read_buff) != REMOTE_NDIS_PACKET_MSG)
	{
		return 0;
	}
	if(message_len == 0x0206)
		message_len = message_len;*/
	temp_count = 0;	 
	memcpy(first_read_buff_2, first_read_buff, 64);

	first_recv_flag = 1;

	return (message_len - RNDIS_USB_OVERHEAD_SIZE);





//	int message_len = 0, offset = 0, data_len = 0, count = 0;
//	RNDIS_PKT_MSG_STRUCT header;
//	count = ReadDevice(handle, &header, RNDIS_USB_OVERHEAD_SIZE);
//	if(count == -1 || count == 0) return 0;
//	if(count !=  RNDIS_USB_OVERHEAD_SIZE)
//		 count=count;
//
//	data_len = LONG_LE_TO_HOST(header.data_len);
//	message_len = LONG_LE_TO_HOST(header.message_len);
//	offset = LONG_LE_TO_HOST(header.data_offset);
//	if(offset != 36)
//		offset = offset;
//
//	if(!(message_len%DIC_BULK_OUT_ENDP_PACKET_SIZE))
//    {
// //       /* RNDIS Protocol defines 1 byte call of 0x00, if 
////        transfer size is multiple of endpoint packet size */
//		zero_recv_flag = 1;
//    }
//	else zero_recv_flag = 0;
//	return data_len;

} 


int RndisRead(DEVICE_HANDLE handle, uint8_t *nic_data_ptr, uint32_t length)
{
	int frame_remaining_len = 0, count = 0, frame_first_len;
	/* Если при определении длинны длинна вместе заголовком usb оказалась
	   больше чем размер EndPoin, вычисляем оставшеюся для прочтения данных длинну
	   и длинну данных которые прочитали при определении размера*/

	if(first_recv_flag != 1)
	{
		 count = ReadDevice(handle, nic_data_ptr, length);
		 if(count != length)
		  length = length;
		  temp_count+=count;
		 return count;
	}

	first_recv_flag = 0;

	if(length > (DIC_BULK_OUT_ENDP_PACKET_SIZE - RNDIS_USB_OVERHEAD_SIZE))	
	{
		frame_remaining_len = length - (DIC_BULK_OUT_ENDP_PACKET_SIZE - RNDIS_USB_OVERHEAD_SIZE);
		frame_first_len = DIC_BULK_OUT_ENDP_PACKET_SIZE - RNDIS_USB_OVERHEAD_SIZE;
	}
	else
	{
		/*Если при определении размера прочитали и все данные то динна оставшихся данных для чтения
		  равна длинне прочитанных при определении размера данных*/
		frame_remaining_len	= length;
		frame_first_len = length;
	}


	memcpy(nic_data_ptr, (first_read_buff + RNDIS_USB_OVERHEAD_SIZE), frame_first_len);

	if(frame_first_len == length) 
	{
//		if(zero_recv_flag == 1)
//			ReadDevice(handle, (uint8_t *)&frame_first_len, 1);
		return length;
	}

	/*Если при определении размера были прочитаны не все данные - дочитываем их*/

	count = ReadDevice(handle, nic_data_ptr + frame_first_len, frame_remaining_len);
	temp_count+=count;
	if(count != frame_remaining_len)
		  frame_remaining_len = frame_remaining_len;


	if(count == -1) return -1;

	count += frame_first_len;

//	if(zero_recv_flag == 1)
//			ReadDevice(handle, (uint8_t *)&frame_first_len, 1);

	return count;










//	uint8_t temp;
//	int count = ReadDevice(handle, nic_data_ptr, length);
//	if(count !=  length)
//		 count=count;
////	if(zero_recv_flag == 1)
////		ReadDevice(handle, &temp, 1);
////	if(temp != 0)
////		temp = temp;
//	return count;
}


/* EOF */
