/*
* Copyright(C) NXP Semiconductors, 2011
* All rights reserved.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* LPC products.  This software is supplied "AS IS" without any warranties of
* any kind, and NXP Semiconductors and its licensor disclaim any and 
* all warranties, express or implied, including all implied warranties of 
* merchantability, fitness for a particular purpose and non-infringement of 
* intellectual property rights.  NXP Semiconductors assumes no responsibility
* or liability for the use of the software, conveys no license or rights under any
* patent, copyright, mask work right, or any other intellectual property rights in 
* or to any products. NXP Semiconductors reserves the right to make changes
* in the software without notification. NXP Semiconductors also makes no 
* representation or warranty that such application will be suitable for the
* specified use without further testing or modification.
* 
* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' and its 
* licensor's relevant copyrights in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.
*/


/** \file
 *  \brief USB Endpoint definitions for the LPC17xx microcontrollers.
 *  \copydetails Group_EndpointManagement_LPC17xx
 *
 *  \note This file should not be included directly. It is automatically included as needed by the USB driver
 *        dispatch header located in lpcroot/libraries/nxpUSBLib/Drivers/USB/USB.h.
 */

/** \ingroup Group_EndpointRW
 *  \defgroup Group_EndpointRW_LPC17xx Endpoint Data Reading and Writing (LPC17xx)
 *  \brief Endpoint data read/write definitions for the LPC architecture.
 *
 *  Functions, macros, variables, enums and types related to data reading and writing from and to endpoints.
 */

/** \ingroup Group_EndpointPrimitiveRW
 *  \defgroup Group_EndpointPrimitiveRW_LPC17xx Read/Write of Primitive Data Types (LPC17xx)
 *  \brief Endpoint primitive read/write definitions for the LPC17xx architecture.
 *
 *  Functions, macros, variables, enums and types related to data reading and writing of primitive data types
 *  from and to endpoints.
 */

/** \ingroup Group_EndpointPacketManagement
 *  \defgroup Group_EndpointPacketManagement_LPC17xx Endpoint Packet Management (LPC17xx)
 *  \brief Endpoint packet management definitions for the Atmel LPC17xx architecture.
 *
 *  Functions, macros, variables, enums and types related to packet management of endpoints.
 */

/** \ingroup Group_EndpointManagement
 *  \defgroup Group_EndpointManagement_LPC17xx Endpoint Management (LPC17xx)
 *  \brief Endpoint management definitions for the LPC17xx architecture.
 *
 *  Functions, macros and enums related to endpoint management when in USB Device mode. This
 *  module contains the endpoint management macros, as well as endpoint interrupt and data
 *  send/receive functions for various data types.
 *
 *  @{
 */

#ifndef __ENDPOINT_LPC17XX_H__
#define __ENDPOINT_LPC17XX_H__

	/* Includes: */
		#include "../EndpointCommon.h"
	
	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_USB_DRIVER)
			#error Do not include this file directly. Include lpcroot/libraries/nxpUSBLib/Drivers/USB/USB.h instead.
		#endif

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define ENDPOINT_DETAILS_MAXEP		6							/* Maximum of supported endpoint */
			#define USED_PHYSICAL_ENDPOINTS		(ENDPOINT_DETAILS_MAXEP*2) 	/* This macro effect memory size of the DCD */

			extern volatile bool SETUPReceived;
			extern DMADescriptor dmaDescriptor[USED_PHYSICAL_ENDPOINTS];
			
			extern void SIE_WriteCommandData (uint32_t cmd, uint32_t val);
			extern void SIE_WriteCommamd (uint32_t cmd);

			extern bool isOutReceived;
			extern bool isInReady;

			void WriteControlEndpoint(uint8_t *pData, uint32_t cnt);
			void ReadControlEndpoint(uint8_t *pData);
			void DcdDataTransfer(uint8_t PhyEP, uint8_t *pData, uint32_t cnt);
		/* Inline Functions: */

		/* Function Prototypes: */
			void Endpoint_ClearEndpoints(void);
			bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number,
			                                    const uint8_t UECFG0XData,
			                                    const uint8_t UECFG1XData);

	#endif

		/* Inline Functions: */
			/** Configures the specified endpoint number with the given endpoint type, direction, bank size
			 *  and banking mode. Once configured, the endpoint may be read from or written to, depending
			 *  on its direction.
			 *
			 *  \param[in] Number     Endpoint number to configure. This must be more than 0 and less than
			 *                        \ref ENDPOINT_TOTAL_ENDPOINTS.
			 *
			 *  \param[in] Type       Type of endpoint to configure, a \c EP_TYPE_* mask. Not all endpoint types
			 *                        are available on Low Speed USB devices - refer to the USB 2.0 specification.
			 *
			 *  \param[in] Direction  Endpoint data direction, either \ref ENDPOINT_DIR_OUT or \ref ENDPOINT_DIR_IN.
			 *                        All endpoints (except Control type) are unidirectional - data may only be read
			 *                        from or written to the endpoint bank based on its direction, not both.
			 *
			 *  \param[in] Size       Size of the endpoint's bank, where packets are stored before they are transmitted
			 *                        to the USB host, or after they have been received from the USB host (depending on
			 *                        the endpoint's data direction). The bank size must indicate the maximum packet size
			 *                        that the endpoint can handle.
			 *
			 *  \param[in] Banks      Number of banks to use for the endpoint being configured, an \c ENDPOINT_BANK_* mask.
			 *                        More banks uses more USB DPRAM, but offers better performance. Isochronous type
			 *                        endpoints <b>must</b> have at least two banks.
			 *
			 *  \note When the \c ORDERED_EP_CONFIG compile time option is used, Endpoints <b>must</b> be configured in
			 *        ascending order, or bank corruption will occur.
			 *        \n\n
			 *
			 *  \note Different endpoints may have different maximum packet sizes based on the endpoint's index - refer to
			 *        the chosen microcontroller model's datasheet to determine the maximum bank size for each endpoint.
			 *        \n\n
			 *
			 *  \note The default control endpoint should not be manually configured by the user application, as
			 *        it is automatically configured by the library internally.
			 *        \n\n
			 *
			 *  \note This routine will automatically select the specified endpoint upon success. Upon failure, the endpoint
			 *        which failed to reconfigure correctly will be selected.
			 *
			 *  \return Boolean \c true if the configuration succeeded, \c false otherwise.
			 */
			/*static inline */bool Endpoint_ConfigureEndpoint(const uint8_t Number,
			                                              const uint8_t Type,
			                                              const uint8_t Direction,
			                                              const uint16_t Size,
			                                              const uint8_t Banks) /*ATTR_ALWAYS_INLINE*/;

			/** Resets the endpoint bank FIFO. This clears all the endpoint banks and resets the USB controller's
			 *  data In and Out pointers to the bank's contents.
			 *
			 *  \param[in] EndpointNumber Endpoint number whose FIFO buffers are to be reset.
			 */
			static inline void Endpoint_ResetEndpoint(const uint8_t EndpointNumber) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ResetEndpoint(const uint8_t EndpointNumber)
			{

			}

			/** Enables the currently selected endpoint so that data can be sent and received through it to
			 *  and from a host.
			 *
			 *  \note Endpoints must first be configured properly via \ref Endpoint_ConfigureEndpoint().
			 */
			static inline void Endpoint_EnableEndpoint(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_EnableEndpoint(void)
			{

			}

			/** Disables the currently selected endpoint so that data cannot be sent and received through it
			 *  to and from a host.
			 */
			static inline void Endpoint_DisableEndpoint(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_DisableEndpoint(void)
			{

			}

			/** Determines if the currently selected endpoint is enabled, but not necessarily configured.
			 *
			 * \return Boolean \c true if the currently selected endpoint is enabled, \c false otherwise.
			 */
			static inline bool Endpoint_IsEnabled(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsEnabled(void)
			{
				return true;
			}

			/** Retrieves the number of busy banks in the currently selected endpoint, which have been queued for
			 *  transmission via the \ref Endpoint_ClearIN() command, or are awaiting acknowledgement via the
			 *  \ref Endpoint_ClearOUT() command.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 *
			 *  \return Total number of busy banks in the selected endpoint.
			 */
			static inline uint8_t Endpoint_GetBusyBanks(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Endpoint_GetBusyBanks(void)
			{
				return 0;
			}

			/** Aborts all pending IN transactions on the currently selected endpoint, once the bank
			 *  has been queued for transmission to the host via \ref Endpoint_ClearIN(). This function
			 *  will terminate all queued transactions, resetting the endpoint banks ready for a new
			 *  packet.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 */
			static inline void Endpoint_AbortPendingIN(void)
			{

			}

			/** Determines if the currently selected endpoint is configured.
			 *
			 *  \return Boolean \c true if the currently selected endpoint has been configured, \c false otherwise.
			 */
			static inline bool Endpoint_IsConfigured(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsConfigured(void)
			{
				return true;
			}

			/** Returns a mask indicating which INTERRUPT type endpoints have interrupted - i.e. their
			 *  interrupt duration has elapsed. Which endpoints have interrupted can be determined by
			 *  masking the return value against <tt>(1 << <i>{Endpoint Number}</i>)</tt>.
			 *
			 *  \return Mask whose bits indicate which endpoints have interrupted.
			 */
			static inline uint8_t Endpoint_GetEndpointInterrupts(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint8_t Endpoint_GetEndpointInterrupts(void)
			{
				return 0;
			}

			/** Determines if the specified endpoint number has interrupted (valid only for INTERRUPT type
			 *  endpoints).
			 *
			 *  \param[in] EndpointNumber  Index of the endpoint whose interrupt flag should be tested.
			 *
			 *  \return Boolean \c true if the specified endpoint has interrupted, \c false otherwise.
			 */
			static inline bool Endpoint_HasEndpointInterrupted(const uint8_t EndpointNumber) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_HasEndpointInterrupted(const uint8_t EndpointNumber)
			{
				return ((Endpoint_GetEndpointInterrupts() & (1 << EndpointNumber)) ? true : false);
			}

			/** Indicates the number of bytes currently stored in the current endpoint's selected bank.
			 *
			 *  \note The return width of this function may differ, depending on the maximum endpoint bank size
			 *        of the selected LPC model.
			 *
			 *  \ingroup Group_EndpointRW_LPC17xx
			 *
			 *  \return Total number of bytes in the currently selected Endpoint's FIFO buffer.
			 */
			static inline uint16_t Endpoint_BytesInEndpoint(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint16_t Endpoint_BytesInEndpoint(void)
			{
				if (endpointselected==ENDPOINT_CONTROLEP)
				{
					return usb_data_buffer_size;
				}
				else
				{
					//return (dmaDescriptor[ endpointhandle[endpointselected] ].PresentCount);
					return usb_data_buffer_OUT_size;
				}
			}

			/** Determines if the selected IN endpoint is ready for a new packet to be sent to the host.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 *
			 *  \return Boolean \c true if the current endpoint is ready for an IN packet, \c false otherwise.
			 */
			static inline bool Endpoint_IsINReady(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsINReady(void)
			{
				if (endpointselected==ENDPOINT_CONTROLEP)
				{

					return isInReady;
				}else
				{
					uint8_t SelEP_Data;
					if (dmaDescriptor[ endpointhandle[endpointselected] ].Retired == true){
						SIE_WriteCommamd( CMD_SEL_EP(endpointhandle[endpointselected]) );
						SelEP_Data = SIE_ReadCommandData( DAT_SEL_EP(endpointhandle[endpointselected]) ) ;
						if((SelEP_Data & 1) == 0)
							return true;
					}
					return false;
				}
				
			}

			/** Determines if the selected OUT endpoint has received new packet from the host.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 *
			 *  \return Boolean \c true if current endpoint is has received an OUT packet, \c false otherwise.
			 */
			static inline bool Endpoint_IsOUTReceived(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsOUTReceived(void)
			{
				if (endpointselected==ENDPOINT_CONTROLEP)
				{

					return isOutReceived;
				}else
				{
//					return (dmaDescriptor[ endpointhandle[endpointselected] ].Retired && 
//							(dmaDescriptor[ endpointhandle[endpointselected] ].Status == 2 || dmaDescriptor[ endpointhandle[endpointselected] ].Status == 3)
//							) ? true : false;
					return (usb_data_buffer_OUT_size) ? true : false;
				}
			}

			/** Determines if the current CONTROL type endpoint has received a SETUP packet.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 *
			 *  \return Boolean \c true if the selected endpoint has received a SETUP packet, \c false otherwise.
			 */
			static inline bool Endpoint_IsSETUPReceived(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsSETUPReceived(void)
			{
				return SETUPReceived;
			}

			/** Clears a received SETUP packet on the currently selected CONTROL type endpoint, freeing up the
			 *  endpoint for the next packet.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 *
			 *  \note This is not applicable for non CONTROL type endpoints.
			 */
			static inline void Endpoint_ClearSETUP(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ClearSETUP(void)
			{
				SETUPReceived = FALSE;
				usb_data_buffer_index = 0;
				usb_data_buffer_size = 0;
				SIE_WriteCommamd(CMD_SEL_EP(ENDPOINT_CONTROLEP));
				SIE_WriteCommamd(CMD_CLR_BUF);
			}

			/** Sends an IN packet to the host on the currently selected endpoint, freeing up the endpoint for the
			 *  next packet and switching to the alternative endpoint bank if double banked.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 */
			static inline void Endpoint_ClearIN(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ClearIN(void)
			{
				uint8_t PhyEP = (endpointselected==ENDPOINT_CONTROLEP ? 1: endpointhandle[endpointselected]);
				
				if (endpointselected==ENDPOINT_CONTROLEP)
				{
					WriteControlEndpoint(usb_data_buffer, usb_data_buffer_index);
					usb_data_buffer_index = 0;
					usb_data_buffer_size = 0;
				}else
				{
					DcdDataTransfer(PhyEP, usb_data_buffer_IN, usb_data_buffer_IN_index);
					LPC_USB->USBDMARSet = _BIT(PhyEP);
					usb_data_buffer_IN_index = 0;
				}
			}

			/** Acknowledges an OUT packet to the host on the currently selected endpoint, freeing up the endpoint
			 *  for the next packet and switching to the alternative endpoint bank if double banked.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 */
			extern int bbb;
			void Endpoint_ClearOUT(void);
			//static inline void Endpoint_ClearOUT(void) ATTR_ALWAYS_INLINE;
//			static inline void Endpoint_ClearOUT(void)
//			{
//				usb_data_buffer_index = 0;
//				if(endpointselected == ENDPOINT_CONTROLEP)	   /* Control only */
//				{
//					SIE_WriteCommamd(CMD_SEL_EP(ENDPOINT_CONTROLEP));
//					SIE_WriteCommamd(CMD_CLR_BUF);
//					isOutReceived = false;
//				}else
//				{
//				//	if(usb_data_buffer_OUT_size == 0){
//				//		usb_data_buffer_OUT_index = 0;
//				//		dmaDescriptor[ endpointhandle[endpointselected] ].Status = 0;
//						//LPC_USB->USBEpDMAEn |= (1 << endpointselected);
//				//		LPC_USB->USBDMAIntEn |= (1<<1);
//				//	}
//					if(!(LPC_USB->USBDMAIntEn & (1<<1)))
//					{
//						if(usb_data_buffer_OUT_size == 0)
//						{
//							bbb++;
//							usb_data_buffer_OUT_index = 0;
//							dmaDescriptor[ endpointhandle[endpointselected] ].Status = 0;
//							LPC_USB->USBDMAIntEn |= (1<<1);
//						}	
//					}
//				}
//			}

			/** Stalls the current endpoint, indicating to the host that a logical problem occurred with the
			 *  indicated endpoint and that the current transfer sequence should be aborted. This provides a
			 *  way for devices to indicate invalid commands to the host so that the current transfer can be
			 *  aborted and the host can begin its own recovery sequence.
			 *
			 *  The currently selected endpoint remains stalled until either the \ref Endpoint_ClearStall() macro
			 *  is called, or the host issues a CLEAR FEATURE request to the device for the currently selected
			 *  endpoint.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 */
			//static inline void Endpoint_StallTransaction(void) ATTR_ALWAYS_INLINE;
			void Endpoint_StallTransaction(void);

			/** Clears the STALL condition on the currently selected endpoint.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 */
			static inline void Endpoint_ClearStall(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ClearStall(void)
			{
				uint8_t PhysicalEp = endpointhandle[endpointselected] + (endpointselected==ENDPOINT_CONTROLEP ? 1 : 0);
				
				HAL_DisableUSBInterrupt();
				SIE_WriteCommandData(CMD_SET_EP_STAT(PhysicalEp), DAT_WR_BYTE(0));
				HAL_EnableUSBInterrupt();
			}

			/** Determines if the currently selected endpoint is stalled, false otherwise.
			 *
			 *  \ingroup Group_EndpointPacketManagement_LPC17xx
			 *
			 *  \return Boolean \c true if the currently selected endpoint is stalled, \c false otherwise.
			 */
			static inline bool Endpoint_IsStalled(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsStalled(void)
			{
				bool isStalled;

				HAL_DisableUSBInterrupt();
				SIE_WriteCommamd( CMD_SEL_EP(endpointhandle[endpointselected]) );
				isStalled = SIE_ReadCommandData( DAT_SEL_EP(endpointhandle[endpointselected]) ) & EP_SEL_ST ? true : false;
				HAL_EnableUSBInterrupt();
				
				return isStalled;       /* Device Status */
			}

			/** Resets the data toggle of the currently selected endpoint. */
			static inline void Endpoint_ResetDataToggle(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ResetDataToggle(void)
			{
			}

		/* External Variables: */
			/** Global indicating the maximum packet size of the default control endpoint located at address
			 *  0 in the device. This value is set to the value indicated in the device descriptor in the user
			 *  project once the USB interface is initialized into device mode.
			 *
			 *  If space is an issue, it is possible to fix this to a static value by defining the control
			 *  endpoint size in the \c FIXED_CONTROL_ENDPOINT_SIZE token passed to the compiler in the makefile
			 *  via the -D switch. When a fixed control endpoint size is used, the size is no longer dynamically
			 *  read from the descriptors at runtime and instead fixed to the given value. When used, it is
			 *  important that the descriptor control endpoint size value matches the size given as the
			 *  \c FIXED_CONTROL_ENDPOINT_SIZE token - it is recommended that the \c FIXED_CONTROL_ENDPOINT_SIZE token
			 *  be used in the device descriptors to ensure this.
			 *
			 *  \note This variable should be treated as read-only in the user application, and never manually
			 *        changed in value.
			 */
			#if (!defined(FIXED_CONTROL_ENDPOINT_SIZE) || defined(__DOXYGEN__))
				extern uint8_t USB_Device_ControlEndpointSize;
			#else
				#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE
			#endif

		/* Function Prototypes: */
			/** Completes the status stage of a control transfer on a CONTROL type endpoint automatically,
			 *  with respect to the data direction. This is a convenience function which can be used to
			 *  simplify user control request handling.
			 */
			void Endpoint_ClearStatusStage(void);

			/** Spin-loops until the currently selected non-control endpoint is ready for the next packet of data
			 *  to be read or written to it.
			 *
			 *  \note This routine should not be called on CONTROL type endpoints.
			 *
			 *  \ingroup Group_EndpointRW_LPC17xx
			 *
			 *  \return A value from the \ref Endpoint_WaitUntilReady_ErrorCodes_t enum.
			 */
			uint8_t Endpoint_WaitUntilReady(void);

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

