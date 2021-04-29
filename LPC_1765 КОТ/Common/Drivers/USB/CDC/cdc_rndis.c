#include <stdint.h>
//#include <stddef.h>
#include "Config.h"
#include <string.h>
#include "usb.h"
#include "usbhw.h"
#include "cdc_rndis.h"
#include "type.h"
#include "cdcuser.h"
#include "usb_rndis_descriptor.h"

void RNDIS_Halt_Command(void);
void RNDIS_Initialize_Command(uint8_t **data, uint32_t *size);
void RNDIS_Query_Command(uint8_t **data, uint32_t *size);
void RNDIS_Reset_Command(uint8_t **data, uint32_t *size);
void RNDIS_Set_Command(uint8_t **data, uint32_t *size);
void RNDIS_Indicate_Status_Command (uint8_t **data, uint32_t *size);
void RNDIS_Keepalive_Command(uint8_t **data, uint32_t *size);








#define MAX_EXPECTED_COMMAND_SIZE           (76)
/* this is the max observed command size we get on control endpoint --
   we are allocating memory for command at initialization, instead of 
   dynamically allocating it when command is received, to avoid memory 
   fragmentation */
#define MAX_EXPECTED_RESPONSE_SIZE          (RESPONSE_RNDIS_QUERY_MSG_SIZE + \
                                             (NUM_OIDS_SUPPORTED << 2))
    /* left shifted by 2 to have effect of mutiplication by 4 i.e. size of uint_32 */ 

uint8_t g_response_available_data[NOTIF_PACKET_SIZE] =
{
    NOTIF_REQUEST_TYPE, RESPONSE_AVAIL_NOTIF,
    0x00, 0x00,/*wValue*/
    0x00,0x00,/*interface - modifies*/
    0x00,0x00,/* wlength*/
};

CDC_PSTN_STRUCT cdc_pstn;
uint8_t response_data[MAX_EXPECTED_RESPONSE_SIZE + 2]; 
uint8_t rndis_command[MAX_EXPECTED_COMMAND_SIZE + 2];

static uint32_t g_list_supp_oid[NUM_OIDS_SUPPORTED] = 
{
    OID_GEN_SUPPORTED_LIST,             
    OID_GEN_HARDWARE_STATUS,                 
    OID_GEN_MEDIA_SUPPORTED,                 
    OID_GEN_MEDIA_IN_USE,                
    OID_GEN_MAXIMUM_FRAME_SIZE,          
    OID_GEN_LINK_SPEED,                  
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,          
    OID_GEN_VENDOR_ID,                   
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_CURRENT_PACKET_FILTER,       
    OID_GEN_MAXIMUM_TOTAL_SIZE,          
    OID_GEN_MEDIA_CONNECT_STATUS,       
    OID_GEN_XMIT_OK,                      
    OID_GEN_RCV_OK,                    
    OID_GEN_XMIT_ERROR,               
    OID_GEN_RCV_ERROR,                 
    OID_GEN_RCV_NO_BUFFER,              
    OID_802_3_PERMANENT_ADDRESS,          
    OID_802_3_CURRENT_ADDRESS,         
    OID_802_3_MULTICAST_LIST,        
    OID_802_3_MAXIMUM_LIST_SIZE,          
    OID_802_3_RCV_ERROR_ALIGNMENT,     
    OID_802_3_XMIT_ONE_COLLISION,    
    OID_802_3_XMIT_MORE_COLLISIONS    
};
#define VENDOR_INFO_SIZE        (16)
static uint8_t g_vendor_info[VENDOR_INFO_SIZE] = " FSL RNDIS PORT ";


CDC_PSTN_STRUCT_PTR cdc_pstn_ptr = NULL;



void PSTN_Response_Available(void) 
{

    if (NULL == cdc_pstn_ptr)
    {
      //  DEBUG_PUTS("PSTN_Response_Available: cdc_pstn_ptr is NULL\n");
        return;
    }
    /* update array for current interface */
    g_response_available_data[4] = cdc_pstn_ptr->current_interface;
    
//    (void)USB_Class_CDC_Send_Data(cdc_obj_ptr->cdc_handle, 
//        cdc_obj_ptr->cic_send_endpoint,
//        g_response_available_data,NOTIF_PACKET_SIZE); 

	USB_WriteEP (CDC_CEP_IN, g_response_available_data, NOTIF_PACKET_SIZE);   // send notification
}



int USB_Pstn_Init(void) 
{
	int error=0;
	cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)&cdc_pstn;
	if(cdc_pstn_ptr == NULL)
	{
	//	DEBUG_PUTS("CDC_RNDIS INIT: no mem for CDC_PSTN_STRUCT\n");
		return -1;
	}
    /* Initially RNDIS is in Uninitialized state */
    cdc_pstn_ptr->rndis_device_state = RNDIS_UNINITIALIZED;
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusNotReady;
    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateUnknown;
    cdc_pstn_ptr->rndis_dev_max_tx_size = RNDIS_MAX_FRAME_SIZE + RNDIS_USB_HEADER_SIZE;
    cdc_pstn_ptr->response_data_ptr = NULL; /* Initializing */
    cdc_pstn_ptr->rndis_command_ptr = NULL;
        
    //cdc_pstn_ptr->response_data_ptr = pvPortMalloc((size_t)MAX_EXPECTED_RESPONSE_SIZE);
	response_data[MAX_EXPECTED_RESPONSE_SIZE] = 0x5A;
	response_data[MAX_EXPECTED_RESPONSE_SIZE + 1] = 0x5A;
	cdc_pstn_ptr->response_data_ptr = response_data;
    if(cdc_pstn_ptr->response_data_ptr == NULL)
    {
      //  DEBUG_PUTS("CDC_RNDIS INIT: no mem for response_data\n");
		return -1;
    }
    
    //cdc_pstn_ptr->rndis_command_ptr = pvPortMalloc((size_t)MAX_EXPECTED_COMMAND_SIZE);
	rndis_command[MAX_EXPECTED_COMMAND_SIZE] = 0x5A;
	rndis_command[MAX_EXPECTED_COMMAND_SIZE + 1] = 0x5A;
	cdc_pstn_ptr->rndis_command_ptr = rndis_command;
    if(cdc_pstn_ptr->rndis_command_ptr == NULL)
    {
    //    DEBUG_PUTS("CDC_RNDIS INIT: no mem for response_command\n");
		return -1;
    }
    
    return error;     
}



/**************************************************************************//*!
 *
 * @name  PSTN_Rndis_Message_Set
 *
 * @brief  This function is called in response to PSTN_Rndis_Message_Set 
 *
 * @param handle            handle to Identify the controller
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 

int sup_flag = 0;

uint8_t PSTN_Rndis_Message_Set
(
    USB_SETUP_STRUCT *setup_packet,
    uint8_t **data, 
    uint32_t *size
)
{   
    uint32_t message_type;
    //UNUSED_ARGUMENT(data)
    UNUSED_ARGUMENT(size)


    if (NULL == cdc_pstn_ptr)
    {
       // DEBUG_PUTS("CDC_RNDIS MESSAGE SET: cdc_pstn_ptr == NULL\n");
		return USBERR_ERROR;
    }
    
    cdc_pstn_ptr->current_interface = (uint8_t)(setup_packet->wIndex.WB.L) ; 

    if(!(cdc_pstn_ptr->current_interface < 
         USB_MAX_SUPPORTED_INTERFACES))
    {       
     //   DEBUG_PUTS("CDC_RNDIS MESSAGE SET: USBERR_INVALID_REQ_TYPE\n"); 
        return USBERR_INVALID_REQ_TYPE;
    }   

    if(setup_packet->wLength > MAX_EXPECTED_COMMAND_SIZE) 
    {
      //  DEBUG_PUTS("CDC_RNDIS MESSAGE SET: setup_packet->length > MAX_EXPECTED_COMMAND_SIZE\n");  
        return USBERR_ERROR;
    }

    /* Command data has been appended at the end of setup token in framework.c
       Copy data from that buffer to buffer in pstn.c memory for that buffer 
       will be freed on completion of setup transfer*/
	else
    	USB_mem_copy(*data - setup_packet->wLength, cdc_pstn_ptr->rndis_command_ptr,
        	setup_packet->wLength);

    message_type = LONG_LE_TO_HOST(*((uint32_t *)(cdc_pstn_ptr->rndis_command_ptr)));
    
    if(message_type == REMOTE_NDIS_HALT_MSG)
    {   
        /* No response is send to host on receving Halt Command */
        RNDIS_Halt_Command();
    }
    else
    {
    //	g_response_available_data[4] = cdc_pstn_ptr->current_interface;
	//	USB_WriteEP (CDC_CEP_IN, g_response_available_data, NOTIF_PACKET_SIZE);   // send notification 
		PSTN_Response_Available();     
    }
    
    if(message_type == REMOTE_NDIS_INITIALIZE_MSG)
    {
        /* Update the NDIS HW status */
        cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusInitializing;
    }

    
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  PSTN_Rndis_Message_Get
 *
 * @brief  This function is called in response to PSTN_Rndis_Message_Get 
 *
 * @param handle            handle to Identify the controller
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
uint8_t PSTN_Rndis_Message_Get
(
    USB_SETUP_STRUCT * setup_packet,
    uint8_t **data, 
    uint32_t *size
)
{       
    uint32_t *message_type_ptr;
    uint16_t setup_pkt_len =  setup_packet->wLength;

    if (NULL == cdc_pstn_ptr)
    {
       // DEBUG_PUTS("CDC_RNDIS MESSAGE GET: cdc_pstn_ptr == NULL\n"); 
        return USBERR_ERROR;
    }


    *size = 0;
    
    message_type_ptr = (uint32_t *)(cdc_pstn_ptr->rndis_command_ptr);
    /* we can avoid one swap operation by using message_type_ptr in 
       PSTN_Rndis_Message_Set instead of message_type, but this gives 
       cleaner implementation as all manipulations and parsing on command 
       are done in this function */
    *message_type_ptr = LONG_LE_TO_HOST(*message_type_ptr);

    switch(*message_type_ptr)
    {
        case REMOTE_NDIS_INITIALIZE_MSG: 
            /* Preparing for response to REMOTE_NDIS_INITIALIZE_MSG command 
               i.e. REMOTE_NDIS_INITIALIZE_CMPLT data */
            RNDIS_Initialize_Command(data,size); 
		//	DEBUG_PUTS("CDC_RNDIS MESSAGE GET: REMOTE_NDIS_INITIALIZE_MSG RNDIS Command\n");           
            break;
        case REMOTE_NDIS_QUERY_MSG:
            /* Preparing for response to REMOTE_NDIS_QUERY_MSG command 
               i.e. REMOTE_NDIS_QUERY_CMPLT data */
            /* correct the endianess of OID */
            *((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 3) = LONG_LE_TO_HOST(*((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 3));
            RNDIS_Query_Command(data,size); 
		//	DEBUG_PUTS("CDC_RNDIS MESSAGE GET: REMOTE_NDIS_QUERY_MSG Command\n");        
            break;
        case REMOTE_NDIS_SET_MSG : 
            /* Preparing for response to REMOTE_NDIS_SET_MSG command 
               i.e. REMOTE_NDIS_SET_CMPLT data */
            /* Correct the endianess of OID and InformationBufferLength
               and InformationBufferOffset*/
            *((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 3) = LONG_LE_TO_HOST(*((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 3));
            *((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 4) = LONG_LE_TO_HOST(*((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 4));
            *((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 5) = LONG_LE_TO_HOST(*((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 5));
            RNDIS_Set_Command(data,size);
		//	DEBUG_PUTS("CDC_RNDIS MESSAGE GET: REMOTE_NDIS_SET_MSG Command\n");           
            break;
        case REMOTE_NDIS_RESET_MSG :
            /* Preparing for response to REMOTE_NDIS_RESET_MSG command 
               i.e. REMOTE_NDIS_RESET_CMPLT data */
            RNDIS_Reset_Command(data,size);  
		//	DEBUG_PUTS("CDC_RNDIS MESSAGE GET: REMOTE_NDIS_RESET_MSG RNDIS Command\n");       
            break;
        case REMOTE_NDIS_INDICATE_STATUS_MSG : 
            /* Preparing for response to REMOTE_NDIS_INDICATE_STATUS_MSG 
               command */       
            RNDIS_Indicate_Status_Command(data,size);   
		//	DEBUG_PUTS("CDC_RNDIS MESSAGE GET: REMOTE_NDIS_INDICATE_STATUS_MSG RNDIS Command\n");        
            break;
        case REMOTE_NDIS_KEEPALIVE_MSG : 
            /* Preparing for response to REMOTE_NDIS_KEEPALIVE_MSG command 
               i.e. REMOTE_NDIS_KEEPALIVE_CMPLT data */
            RNDIS_Keepalive_Command(data,size);   
		//	DEBUG_PUTS("CDC_RNDIS MESSAGE GET: REMOTE_NDIS_KEEPALIVE_MSG Command\n");      
            break;      
        default : 
          //  DEBUG_PUTS("CDC_RNDIS MESSAGE GET: UNSUPPORTED RNDIS Command\n"); 
            return USBERR_INVALID_REQ_TYPE;         
    }
    
    if(*size > MAX_EXPECTED_RESPONSE_SIZE) 
    {
      //  DEBUG_PUTS("CDC_RNDIS MESSAGE GET: MAX_EXPECTED_RESPONSE_SIZE insufficient\n"); 
    }
    
    if(setup_pkt_len < *size)
    {
	//	DEBUG_PUTS("CDC_RNDIS MESSAGE GET: Device has more data to send than Host needs.\n");
        return USBERR_INVALID_REQ_TYPE;
    }

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Initialize_Command
 *
 * @brief  This function is called to initialize the RNDIS device and prepare
 *         data to be sent to host as Remote_NDIS_INITIALIZE_CMPLT 
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Initialize_Command
(
    uint8_t **data, 
    uint32_t *size
)
{
    uint32_t max_tx_size ;
    
    if (NULL == cdc_pstn_ptr)
    {

	//	DEBUG_PUTS("RNDIS_Initialize_Command: cdc_pstn_ptr is NULL\n");
        return;
    }

    /* preparing for Byte 0-3 : MessageType*/   
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_INITIALIZE_CMPLT);

    /* preparing for Byte 4-7 : MessageLength*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST_CONST(RESPONSE_RNDIS_INITIALIZE_MSG_SIZE);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint32_t *)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint32_t));
    
    /* preparing for Byte 12-15 : Status*/
    if(cdc_pstn_ptr->rndis_device_state == RNDIS_UNINITIALIZED)
    {
        *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_SUCCESS); 
    }
    else
    {
        *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_FAILURE);
    }
    
    /* preparing for Byte 16-19 ; MajorVersion*/
    /* We are currently returning the same driver version to host in 
       response to initialization command as reported by host driver */
    USB_mem_copy(((uint32_t *)(cdc_pstn_ptr->rndis_command_ptr) + 3), 
        ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 4),sizeof(uint32_t));

    /* preparing for Byte 20-23 : MinorVersion*/
    USB_mem_copy(((uint32_t *)(cdc_pstn_ptr->rndis_command_ptr) + 4), 
        ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 5),sizeof(uint32_t));

    /* preparing for Byte 24-27 : DeviceFlags*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(RNDIS_DF_CONNECTIONLESS);
    
    /* preparing for Byte 28-31 : Medium*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 7) = LONG_LE_TO_HOST(NdisMedium802_3);
    
    /* preparing for Byte 32-35 : MaxPacketsPerTransfer*/
    /* We are not implementing multiple packet transfer support in our RNDIS */
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 8) = LONG_LE_TO_HOST_CONST(RNDIS_SINGLE_PACKET_TRANSFER);
                
    /* preparing for Byte 36-39 : MaxTransferSize*/
    /* We are currently returning the same max transfer size to host
       as it send to device in its corresponding filed in 
       initialization command */
    cdc_pstn_ptr->rndis_host_max_tx_size = LONG_LE_TO_HOST(*((uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 5));
    max_tx_size = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_dev_max_tx_size);
    //max_tx_size = cdc_pstn_ptr->rndis_host_max_tx_size;
	//max_tx_size = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_host_max_tx_size);
    USB_mem_copy(&max_tx_size, 
        ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 9),sizeof(uint32_t));

    /* preparing for Byte 40-43 : PacketAlignmentFactor*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 10) = LONG_LE_TO_HOST_CONST(PACKET_ALIGNMENT_FACTOR);
    
    /* preparing for Byte 44-47 : PacketAlignmentFactor*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 11) = AF_LIST_OFFSET;

    /* preparing for Byte 48-51 : PacketAlignmentFactor*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 12) = AF_LIST_SIZE;
            
    *data = cdc_pstn_ptr->response_data_ptr;
    *size = RESPONSE_RNDIS_INITIALIZE_MSG_SIZE; 

    /* initializing RNDIS variables */
    cdc_pstn_ptr->rndis_device_state = RNDIS_INITIALIZED;
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusReady;
    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateDisconnected;  
    cdc_pstn_ptr->num_frames_tx_ok = 0;
    cdc_pstn_ptr->num_frames_rx_ok = 0;
    cdc_pstn_ptr->num_frames_tx_error = 0;
    cdc_pstn_ptr->num_frames_rx_error = 0;
    cdc_pstn_ptr->num_recv_frames_missed = 0;
    cdc_pstn_ptr->num_recv_frames_alignment_error = 0;
    cdc_pstn_ptr->num_frames_tx_one_collision = 0;
    cdc_pstn_ptr->num_frames_tx_many_collision = 0;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Keepalive_Command
 *
 * @brief  This function is called to check the health of RNDIS device and prepare
 *         data to be sent to host as REMOTE_NDIS_KEEPALIVE_CMPLT 
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Keepalive_Command
(
    uint8_t **data, 
    uint32_t *size
)
{

    if (NULL == cdc_pstn_ptr)
    {
	//	DEBUG_PUTS("RNDIS_Keepalive_Command: cdc_pstn_ptr is NULL\n");
        return;
    }
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_KEEPALIVE_CMPLT);

    /* preparing for Byte 4-7 : MessageLength*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST_CONST(RESPONSE_RNDIS_KEEPALIVE_MSG_SIZE);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint32_t *)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint32_t));
    
    /* preparing for Byte 12-15 : Status*/
    if(cdc_pstn_ptr->rndis_device_state == RNDIS_DATA_INITIALIZED)
    {
        *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_SUCCESS); 
    }
    else
    {
        *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_FAILURE);
    }
    
    *data = (uint8_t *)cdc_pstn_ptr->response_data_ptr;
    *size = RESPONSE_RNDIS_KEEPALIVE_MSG_SIZE;      
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Query_Command
 *
 * @brief  This function is called to query the RNDIS device for its 
 *         characteristics or statistics information or status and prepare
 *         data to be sent to host as Remote_NDIS_Query_CMPLT 
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Query_Command
(
    uint8_t **data, 
    uint32_t *size
)
{
    uint32_t *oid_ptr;
    uint8_t i;
    uint32_t info_buf_len = 0;
    uint32_t return_status = RNDIS_STATUS_SUCCESS;

    if (NULL == cdc_pstn_ptr)
    { 
	//	DEBUG_PUTS("RNDIS_Query_Command: cdc_pstn_ptr is NULL\n");
        return;
    }
    oid_ptr = (uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 3;
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_QUERY_CMPLT);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint32_t *)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint32_t));
    

    /* preparing for Byte 20-23 : InformationBufferOffset*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 5) = LONG_LE_TO_HOST_CONST(0x00000010);
        
    switch(*oid_ptr)
    {
        case OID_GEN_SUPPORTED_LIST : 
		//	DEBUG_PUTS("OID_GEN_SUPPORTED_LIST\n");  
            /* List of supported OIDs - Query Manadatory - General Operational Characteristic */
            for(i=0;i<NUM_OIDS_SUPPORTED;i++)
            {   /* change the endianess of data before sending on USB Bus */
                g_list_supp_oid[i] = LONG_LE_TO_HOST(g_list_supp_oid[i]);             
            }                       
            info_buf_len = sizeof(g_list_supp_oid);         
            USB_mem_copy(g_list_supp_oid, 
                (uint32_t *)cdc_pstn_ptr->response_data_ptr + 6, info_buf_len);            
            break;
        case OID_GEN_HARDWARE_STATUS :
		//	DEBUG_PUTS("OID_GEN_HARDWARE_STATUS\n");
            /* Hardware status  - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_hw_state);
            break;
        case OID_GEN_MEDIA_SUPPORTED :
		//	DEBUG_PUTS("OID_GEN_MEDIA_SUPPORTED\n");
            /* Media types supported (encoded) - Query Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(NdisMedium802_3);
            break;
        case OID_GEN_MEDIA_IN_USE :
		//	DEBUG_PUTS("OID_GEN_MEDIA_IN_USE\n");
            /* Media types in use (encoded) - Query Manadatory - General Operational Characteristic*/ 
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(NdisMedium802_3);
            break;
        case OID_GEN_MAXIMUM_FRAME_SIZE : 
		//	DEBUG_PUTS("OID_GEN_MAXIMUM_FRAME_SIZE\n");
            /* Maximum in bytes, frame size - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST((uint32_t)(RNDIS_MAX_FRAME_SIZE-14)); 
            break;
        case OID_GEN_LINK_SPEED :
		//	DEBUG_PUTS("OID_GEN_LINK_SPEED\n");
            /* Link speed in units of 100 bps - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
           /* if(cdc_pstn_ptr->pstn_callback.callback != NULL)
            {   
                cdc_pstn_ptr->pstn_callback.callback(USB_APP_GET_LINK_SPEED,
                (void*)((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6),
                cdc_pstn_ptr->pstn_callback.arg);
            } */

            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST((uint32_t)1000000);
            break;
        case OID_GEN_TRANSMIT_BLOCK_SIZE : 
		//	DEBUG_PUTS("OID_GEN_TRANSMIT_BLOCK_SIZE\n");
            /* Minimum amount of storage, in bytes, that a single packet 
               occupies in the transmit buffer space of the NIC - 
               Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(/*DIC_BULK_IN_ENDP_PACKET_SIZE*/RNDIS_MAX_FRAME_SIZE);
            break;
        case OID_GEN_RECEIVE_BLOCK_SIZE : 
		//	DEBUG_PUTS("OID_GEN_RECEIVE_BLOCK_SIZE\n");
            /* Amount of storage, in bytes, that a single packet occupies in 
               the receive buffer space of the NIC - Query Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(/*DIC_BULK_OUT_ENDP_PACKET_SIZE*/RNDIS_MAX_FRAME_SIZE);
            break;
        case OID_GEN_VENDOR_ID :
		//	DEBUG_PUTS("OID_GEN_VENDOR_ID\n");
            /* Vendor NIC code - Query Mandatory - General Operational Characteristic*/
            /* This object specifies a three-byte IEEE-registered vendor code, 
               followed by a single byte that the vendor assigns to identify 
               a particular NIC. The IEEE code uniquely identifies the vendor 
               and is the same as the three bytes appearing at the beginning
               of the NIC hardware address.Vendors without an IEEE-registered 
               code should use the value 0xFFFFFF. */
            info_buf_len = sizeof(uint32_t);
            
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(
                (uint32_t) (((uint32_t) RNDIS_VENDOR_ID << 8) | (uint32_t) NIC_IDENTIFIER_VENDOR));
            break;
        case OID_GEN_VENDOR_DESCRIPTION :
		//	DEBUG_PUTS("OID_GEN_VENDOR_DESCRIPTION\n");
            /* Vendor network card description - Query Mandatory - General Operational Characteristic*/
            info_buf_len = VENDOR_INFO_SIZE;
            USB_mem_copy(g_vendor_info, (uint32_t *)cdc_pstn_ptr->response_data_ptr + 6, info_buf_len);            
            break;
        case OID_GEN_CURRENT_PACKET_FILTER :
		//	DEBUG_PUTS("OID_GEN_CURRENT_PACKET_FILTER\n"); 
            /* Current packet filter (encoded) - Query and Set Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_packet_filter);
            break;
        case OID_GEN_MAXIMUM_TOTAL_SIZE : 
		//	DEBUG_PUTS("OID_GEN_MAXIMUM_TOTAL_SIZE\n");
            /* Maximum total packet length in bytes - Query Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_dev_max_tx_size);
		//	*((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = (uint32_t)600;
            break;
        case OID_GEN_MEDIA_CONNECT_STATUS : 
		//	DEBUG_PUTS("OID_GEN_MEDIA_CONNECT_STATUS\n");
            /* Whether the NIC is connected to the network - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_media_connect_status);
            break;
        case OID_GEN_XMIT_OK : 
		//	DEBUG_PUTS("OID_GEN_XMIT_OK\n");
            /* Frames transmitted without errors - Query Mandatory - General Statistics*/ 
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_ok);
            break;
        case OID_GEN_RCV_OK : 
		//	DEBUG_PUTS("OID_GEN_RCV_OK\n");
            /* Frames received without errors - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_rx_ok);
            break;
        case OID_GEN_XMIT_ERROR : 
		//	DEBUG_PUTS("OID_GEN_XMIT_ERROR\n");
            /* Frames not transmitted or transmitted with errors - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_error);
            break;
        case OID_GEN_RCV_ERROR : 
		//	DEBUG_PUTS("OID_GEN_RCV_ERROR\n");
            /* Frames received with errors - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_rx_error);
            break;
        case OID_GEN_RCV_NO_BUFFER : 
		//	DEBUG_PUTS("OID_GEN_RCV_NO_BUFFER\n");
            /* Frame missed, no buffers - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint32_t);
            //*((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_recv_frames_missed);
			*((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_recv_frames_missed);
            break;
        case OID_802_3_PERMANENT_ADDRESS : 
		//	DEBUG_PUTS("OID_802_3_PERMANENT_ADDRESS\n");
            /* Permanent station address - Query Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = RNDIS_ETHER_ADDR_SIZE; 
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[0] = 0x12;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[1] = 0x1A;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[2] = 0x01;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[3] = 0xC0;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[4] = 0x1E;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[5] = 0x00;
			//*((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = RNDIS_ETHER_ADDR_SIZE;
//            USB_mem_copy(&cdc_obj_ptr->mac_address, 
//                ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6), 
//                info_buf_len);  
            
            break;
        case OID_802_3_CURRENT_ADDRESS : 
		//	DEBUG_PUTS("OID_802_3_CURRENT_ADDRESS\n");
            /* Current station address - Query Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = RNDIS_ETHER_ADDR_SIZE; 
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[0] = 0x12;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[1] = 0x1A;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[2] = 0x01;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[3] = 0xC0;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[4] = 0x1E;
			((uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6))[5] = 0x00;
//            USB_mem_copy(cdc_obj_ptr->mac_address, 
//                (uint8_t *)((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6), 
//                info_buf_len);  
            
            break;
        case OID_802_3_MULTICAST_LIST : 
		//	DEBUG_PUTS("OID_802_3_MULTICAST_LIST\n");
            /* Current multicast address list - Query and Set Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = RNDIS_ETHER_ADDR_SIZE;
            /* Currently Our RNDIS driver does not support multicast addressing */
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(0x00000000);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 7) = LONG_LE_TO_HOST_CONST(0x00000000);
            break;
        case OID_802_3_MAXIMUM_LIST_SIZE :
		//	DEBUG_PUTS("OID_802_3_MAXIMUM_LIST_SIZE\n");
            /* Maximum size of multicast address list - Query Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = sizeof(uint32_t);
            /* Currently Our RNDIS driver does not support multicast addressing */
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(RNDIS_MULTICAST_LIST_SIZE);
            break;
        case OID_802_3_RCV_ERROR_ALIGNMENT :
		//	DEBUG_PUTS("OID_802_3_RCV_ERROR_ALIGNMENT\n"); 
            /* Frames received with alignment error - Query Mandatory - Ethernet Statistics*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_recv_frames_alignment_error); 
            break;
        case OID_802_3_XMIT_ONE_COLLISION : 
		//	DEBUG_PUTS("OID_802_3_XMIT_ONE_COLLISION\n");
            /* Frames transmitted with one collision - Query Mandatory - Ethernet Statistics*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_one_collision);
            break;
        case OID_802_3_XMIT_MORE_COLLISIONS : 
		//	DEBUG_PUTS("OID_802_3_XMIT_MORE_COLLISIONS\n");
            /* Frames transmitted with more than one collision - Query Mandatory - Ethernet Statistics*/
            info_buf_len = sizeof(uint32_t);
            *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_many_collision);
            break;
        default : 
          //  DEBUG_PRINTF_ONE("OID 0x%x NOT SUPPORTED(QUERY)\n",*oid_ptr);
            return_status = RNDIS_STATUS_NOT_SUPPORTED;
            break;
    }

    /* preparing for Byte 12-15 : Status*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST(return_status);
    
    *size = RESPONSE_RNDIS_QUERY_MSG_SIZE + info_buf_len;       
    /* preparing for Byte 4-7 : MessageLength*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST(*size);
    /* preparing for Byte 16-19 : InformationBufferLength*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 4) = LONG_LE_TO_HOST(info_buf_len);
    *data = cdc_pstn_ptr->response_data_ptr;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Set_Command
 *
 * @brief  This function is called to Configure the operational parameters of
 *         RNDIS device.
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/
 
 BOOL media_connected = FALSE;
  
void RNDIS_Set_Command
(
    uint8_t **data, 
    uint32_t *size
)
{
    uint32_t *oid_ptr;
    uint32_t *info_buf_len_ptr;
    uint32_t *info_buf_offset_ptr;
    uint32_t return_status = RNDIS_STATUS_SUCCESS;

    if (NULL == cdc_pstn_ptr)
    {
     //   DEBUG_PUTS("RNDIS_Set_Command: cdc_pstn_ptr is NULL\n");
        return;
    }
    oid_ptr = (uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 3;
    info_buf_len_ptr = (uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 4;
    info_buf_offset_ptr = (uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 5;
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_SET_CMPLT);

    *size = RESPONSE_RNDIS_SET_MSG_SIZE;        
    /* preparing for Byte 4-7 : MessageLength*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST(*size);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint32_t *)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint32_t *)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint32_t));
    
    switch(*oid_ptr)
    {
        case OID_GEN_CURRENT_PACKET_FILTER : 
            /* Current packet filter (encoded) - Query and Set Manadatory - General Operational Characteristic*/
            if(*info_buf_len_ptr != 4 ) 
            {
                return_status = RNDIS_STATUS_INVALID_DATA;
            }
            else
            {   
                

             //   cdc_pstn_ptr->rndis_packet_filter = LONG_LE_TO_HOST( *((uint32_t *)((uint8_t *)((uint32_t *)
             //       cdc_pstn_ptr->rndis_command_ptr + 2) + *info_buf_offset_ptr)));
                /*if(cdc_pstn_ptr->pstn_callback.callback != NULL)
                {   
                    cdc_pstn_ptr->pstn_callback.callback(USB_APP_GET_LINK_STATUS,
                    (void*)(&media_connected),
                    cdc_pstn_ptr->pstn_callback.arg);
                } */

             //   if((cdc_pstn_ptr->rndis_packet_filter)&&(media_connected == TRUE))
             //   {
                    cdc_pstn_ptr->rndis_device_state = RNDIS_DATA_INITIALIZED;  
                    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateConnected;
					//media_connected = TRUE;
              //  }
            //    else
            //    {
           //         cdc_pstn_ptr->rndis_device_state = RNDIS_INITIALIZED;   
            //        cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateDisconnected;
				//	media_connected = TRUE;
            //    }
            }
            break;
        case OID_802_3_MULTICAST_LIST : 
            /* Current multicast address list - Query and Set Mandatory - Ethernet Operational Characteristic*/
            if(*info_buf_len_ptr != RNDIS_ETHER_ADDR_SIZE) 
            {
                return_status = RNDIS_STATUS_INVALID_DATA;
            }
            else
            {   uint64_t multi_cast_list;
                multi_cast_list = *((uint64_t *)((uint8_t *)((uint32_t *)
                    cdc_pstn_ptr->rndis_command_ptr + 2) + *info_buf_offset_ptr));
                if(multi_cast_list)
                {/* Currently Our RNDIS driver does not support multicast addressing */
                    return_status = RNDIS_STATUS_NOT_SUPPORTED;
                }
            }

            break;
        default : 
           // DEBUG_PRINTF_ONE("OID 0x%x NOT SUPPORTED(SET)\n",*oid_ptr);
            return_status = RNDIS_STATUS_NOT_SUPPORTED;
            break;
    }
    /* preparing for Byte 12-15 : Status*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST(return_status);
    *data = cdc_pstn_ptr->response_data_ptr;        
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Reset_Command
 *
 * @brief  This function is called to soft reset the RNDIS device.
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Reset_Command
(
    uint8_t **data, 
    uint32_t *size
)
{
   
    if (NULL == cdc_pstn_ptr)
    {
      //  DEBUG_PUTS("RNDIS_Reset_Command: cdc_pstn_ptr is NULL\n");
        return;
    }

//    uint32_t  *oid_ptr = (uint32_t *)cdc_pstn_ptr->rndis_command_ptr + 3;
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_RESET_CMPLT);

    *size = RESPONSE_RNDIS_RESET_MSG_SIZE;      
    /* preparing for Byte 4-7 : MessageLength*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST(*size);
    
    /* preparing for Byte 8-11 : Status*/
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 2) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_SUCCESS);

    /* preparing for Byte 12-15 : AddressingReset*/
    /* No need for host to resend addressing information */
    *((uint32_t *)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(0x00000000);

    *data = cdc_pstn_ptr->response_data_ptr;
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusReset;
    cdc_pstn_ptr->rndis_device_state = RNDIS_UNINITIALIZED;
    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateUnknown;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Indicate_Status_Command
 *
 * @brief  Used to indicate change in status of device
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Indicate_Status_Command 
(
    uint8_t **data, 
    uint32_t *size
)
{
    UNUSED_ARGUMENT(data)
    UNUSED_ARGUMENT(size)   
    return;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Halt_Command
 *
 * @brief  This function is called to halt the RNDIS device.
 *         i.e. to terminate the network connection
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Halt_Command(void)
{

    if (NULL == cdc_pstn_ptr)
    {
    //    DEBUG_PUTS("RNDIS_Halt_Command: cdc_pstn_ptr is NULL\n");
        return;
    }
   // DEBUG_PUTS("RNDIS_Halt_Command\n");
    cdc_pstn_ptr->rndis_device_state = RNDIS_UNINITIALIZED;
    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateDisconnected;  
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusClosing;
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusNotReady;
}

uint32_t _psp_swap4byte(uint32_t n)
{ /* Body */
   /* uint8_t temp[4];

    temp[3] = *((uint8_t *)&n);
    temp[2] = ((uint8_t *)&n)[1];
    temp[1] = ((uint8_t *)&n)[2];
    temp[0] = ((uint8_t *)&n)[3];
    return (*(uint32_t *)temp);*/
	return n;
} /* Endbody */
