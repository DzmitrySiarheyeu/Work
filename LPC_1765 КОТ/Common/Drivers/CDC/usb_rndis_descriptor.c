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
* $FileName: usb_descriptor.c$
* $Version : 3.6.3.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
* @brief The file contains USB descriptors 
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include "usb.h"
#include "usb_rndis_descriptor.h"
#include "type.h"
#include "usbdesc.h"


const uint8_t USB_EPDescriptor[] =
{
    CIC_NOTIF_ENDPOINT, 
    USB_INTERRUPT_PIPE, 
    USB_SEND,
    CIC_NOTIF_ENDP_PACKET_SIZE,
    DIC_BULK_IN_ENDPOINT,
    USB_BULK_PIPE, 
    USB_SEND,
    /*DIC_BULK_IN_ENDP_PACKET_SIZE*/64,
    DIC_BULK_OUT_ENDPOINT,
    USB_BULK_PIPE, 
    USB_RECV,
    /*DIC_BULK_OUT_ENDP_PACKET_SIZE*/64                 
};


const uint8_t USB_QualifierDescriptor[] =
{
	10,6,0,2,224,1,3,64,1,0
};


const uint8_t USB_DeviceDescriptor[] =
{
    /* "Device Dexcriptor Size */
    DEVICE_DESCRIPTOR_SIZE,               
    /* "Device" Type of descriptor */   
    USB_DEVICE_DESCRIPTOR,                
    /*  BCD USB version  */  
    USB_uint_16_low(BCD_USB_VERSION), USB_uint_16_high(BCD_USB_VERSION),
    /* Device Class is indicated in the interface descriptors */   
    0xE0,//DEVICE_DESC_DEVICE_CLASS,
    /*  Device Subclass is indicated in the interface descriptors  */      
    0x01,//DEVICE_DESC_DEVICE_SUBCLASS,
    /*  Device Protocol  */     
    0x03,//DEVICE_DESC_DEVICE_PROTOCOL,
    /* Max Packet size */
    CONTROL_MAX_PACKET_SIZE,
    /* Vendor ID */
    0xb4,0x0b,// 0x04,0x25,
    /* Product ID */
    0x03,0x03,//0x01,0x03,  
    /* BCD Device version */
    0x00,0x00,
    /* Manufacturer string index */
    0x01,     
    /* Product string index */  
    0x02,                        
    /*  Serial number string index */
    0x03,                  
    /*  Number of configurations */
    DEVICE_DESC_NUM_CONFIG_SUPPOTED                           
};

const uint8_t USB_ConfigDescriptor[] =
{   
    CONFIG_ONLY_DESC_SIZE, /*  Configuration Descriptor Size - always 9 bytes*/   
    USB_CONFIG_DESCRIPTOR, /* "Configuration" type of descriptor */   
    USB_uint_16_low(CONFIG_DESC_SIZE),
    USB_uint_16_high(CONFIG_DESC_SIZE), /*  Total length of the Configuration descriptor */   
    /*  NumInterfaces */   
    CONFIG_DESC_NUM_INTERFACES_SUPPOTED,
    0x01,                      /*  Configuration Value */      
    0x00,                      /*  Configuration Description String Index*/   
    /*  Attributes.support RemoteWakeup and self power */
    BUS_POWERED/*|SELF_POWERED|(REMOTE_WAKEUP_SUPPORT<<REMOTE_WAKEUP_SHIFT)*/,
    /*  Current draw from bus */
    CONFIG_DESC_CURRENT_DRAWN, 
    
    /* CIC INTERFACE DESCRIPTOR */  
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    CIC_ENDP_COUNT, /* management and notification(optional)element present */
    0xE0,//CDC_CLASS, /* Communication Interface Class */
    0x01,//CIC_SUBCLASS_CODE, 
    0x03, //CIC_PROTOCOL_CODE,
    0x00, /* Interface Description String Index*/
        
    /* CDC Class-Specific descriptor */
    CDC_HEADER_FUNC_DESC_SIZE,/* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    0x01, 0x00, 0x01,//HEADER_FUNC_DESC,
    //0x10, 0x01,/* USB Class Definitions for CDC spec release number in BCD */
    
    //CDC_CALL_MANAG_DESC_SIZE,/* Size of this descriptor */
    //USB_CS_INTERFACE, /* descriptor type*/
    //CALL_MANAGEMENT_FUNC_DESC,   
    //0x01,/*D0(if set): device handales call management itself
     //      D1(if set): process commands multiplexed over the data interface*/
    //0x01,/* Indicates multiplexed commands are handled via data interface */
   
    CDC_ABSTRACT_DESC_SIZE,             /* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    0x02, 0x00,//ABSTRACT_CONTROL_FUNC_DESC, 
   ///* 0x06*/0x02,/* Device supports request send break, device supports reuest 
//            combination o set_line_coding, set_control_line_state, 
//            get_line_coding and the notification serial state */
   
    CDC_UNION_FUNC_DESC_SIZE,             /* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    0x02, 0x00, 0x01,//UNION_FUNC_DESC, 
    //0x00,   /* Interface Number of Control */
    //0x01,    /* Interface Number of Subordinate (Data Class) Interface */   
       

    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    CIC_NOTIF_ENDPOINT|(USB_SEND << 7),
    USB_INTERRUPT_PIPE, 
    USB_uint_16_low(CIC_NOTIF_ENDP_PACKET_SIZE),
    USB_uint_16_high(CIC_NOTIF_ENDP_PACKET_SIZE),
    0x04,//0x02,
 
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    (uint8_t)(0x00+DATA_CLASS_SUPPORT), /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    DIC_ENDP_COUNT, /* notification element included */
    0x0A, /* DATA Interface Class */
    0x00, /* Data Interface SubClass Code */
    DIC_PROTOCOL_CODE,
    0x00, /* Interface Description String Index*/
 
    /*Endpoint descriptor */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    DIC_BULK_IN_ENDPOINT|(USB_SEND << 7),
    USB_BULK_PIPE, 
    USB_uint_16_low(DIC_BULK_IN_ENDP_PACKET_SIZE),
    USB_uint_16_high(DIC_BULK_IN_ENDP_PACKET_SIZE),
    0x00,/* This value is ignored for Bulk ENDPOINT */
    
    /*Endpoint descriptor */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    DIC_BULK_OUT_ENDPOINT|(USB_RECV << 7),
    USB_BULK_PIPE, 
    USB_uint_16_low(DIC_BULK_OUT_ENDP_PACKET_SIZE),
    USB_uint_16_high(DIC_BULK_OUT_ENDP_PACKET_SIZE),
    0x00, 0x00 /* This value is ignored for Bulk ENDPOINT */
};

/* USB String Descriptor (optional) */
const uint8_t USB_StringDescriptor[] = {
/* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
/* Index 0x01: Manufacturer */
  (6*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'E',0,
  'T',0,
  'S',0,
  '-',0,
  'B',0,
  'Y',0,
/* Index 0x02: Product */
  (18*2 + 2),                        /* bLength ( 17 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'A',0,
  'R',0,
  'M',0,
  ' ',0,
  'r',0,
  'n',0,
  'd',0,
  'i',0,
  's',0,
  ' ',0,
  'v',0,
  'e',0,
  'r',0,
  ' ',0,
  '1',0,
  '.',0,
  '0',0,
  '0',0,
/* Index 0x03: Serial Number */
  (12*2 + 2),                        /* bLength (12 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  '0',0,
  '0',0,
  '3',0,
  '2',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
/* Index 0x04: Interface 0, Alternate Setting 0 */
  ( 5*2 + 2),                        /* bLength (4 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'R',0,
  'N',0,
  'D',0,
  'I',0,
  'S',0,
};

/* EOF */
