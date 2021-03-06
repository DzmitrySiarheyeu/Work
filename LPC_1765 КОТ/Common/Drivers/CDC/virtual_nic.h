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
* $FileName: virtual_nic.h$
* $Version : 3.6.2.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
* @brief The file contains Macro's and functions needed by the Virtual Network
*        Interface Application
*
*****************************************************************************/


#ifndef _VIRTUAL_NIC_H
#define _VIRTUAL_NIC_H  1

#include "DriversCore.h"


/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define DATA_BUFF_SIZE                  (DIC_BULK_OUT_ENDP_PACKET_SIZE)
/*****************************************************************************
 * Global variables
 *****************************************************************************/

/******************************************************************************
 * Types
 *****************************************************************************/
 typedef struct  __attribute__ ((__packed__)) _rndis_packet_msg_format
 {
    /*Specifies the Remote NDIS message type. 
      This is set to REMOTE_NDIS_PACKET_MSG = 0x1.*/
    uint32_t message_type;
    /*Message length in bytes, including appended packet data, out-of-band 
      data, per-packet-info data, and both internal and external padding*/
    uint32_t message_len;
    /*Specifies the offset in bytes from the start of the DataOffset field of 
      this message to the start of the data. This is an integer multiple of 4*/
    uint32_t data_offset;
    /*Specifies the number of bytes in the data content of this message.*/
    uint32_t data_len;
    /*Specifies the offset in bytes of the first out of band data record from 
      the start of the DataOffset field of this message.  Set to 0 if there is
      no out-of-band data. Otherwise this is an integer multiple of 4*/  
    uint32_t oob_data_offset;
    /*Specifies in bytes the total length of the out of band data.*/
    uint32_t oob_data_len;
    /*Specifies the number of out of band records in this message*/
    uint32_t num_oob_data_elems;
    /*Specifies in bytes the offset from the beginning of the DataOffset field
      in the REMOTE_NDIS_PACKET_MSG data message to the start of the first per 
      packet info data record.  Set to 0 if there is no per-packet data. 
      Otherwise this is an integer multiple of 4*/
    uint32_t per_pkt_info_offset;
    /*Specifies in bytes the total length of the per packet information 
      contained in this message*/
    uint32_t per_pkt_info_len;
    /*Reserved for connection-oriented devices.  Set to 0.*/
    uint32_t vc_handle;
    /*Reserved. Set to 0.*/
    uint32_t reserved;  
 }RNDIS_PKT_MSG_STRUCT,  *PTR_RNDIS_PKT_MSG_STRUCT;



#define REMOTE_NDIS_PACKET_MSG          (0x00000001)
#define RNDIS_USB_OVERHEAD_SIZE         (44)
#define RNDIS_DATA_OFFSET               (36)

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
int RndisWrite(DEVICE_HANDLE handle, uint8_t *nic_data_ptr, uint32_t length);
int RndisRxReady(DEVICE_HANDLE handle);
int RndisRead(DEVICE_HANDLE handle, uint8_t *nic_data_ptr, uint32_t length);

#endif 

/* EOF */
