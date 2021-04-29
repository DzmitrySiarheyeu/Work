#include "uart_lpc17xx.h"
#if(_UART_)
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"	
#include "lpc17xx_gpio.h"




static void CloseUart(DEVICE_HANDLE handle);
static int ReadUart(DEVICE_HANDLE handle, void * dst, int count);
static int WriteUart(DEVICE_HANDLE handle,const void * dst, int count);
static int SeekUART(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteUartText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadUartText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekUARTText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int UartIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);





/**********************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////UART STRUCTURES//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************************/

#if(_UART0_)
	uint8_t uart0_RxBuf[_UART0_RX_BUFF_SIZE_];
	uint8_t uart0_TxBuf[_UART0_TX_BUFF_SIZE_];

    static DEVICE_HANDLE OpenUart0(void);
	static int CreateUart0(void);
    UART_INFO_LPC17XX uart_info_0 = {
								 .pUart = LPC_UART0,
								 .rx_buf.in = 0, 
								 .rx_buf.out = 0, 
								 .rx_buf.buf = uart0_RxBuf,
								 .rx_buf.buf_size = _UART0_RX_BUFF_SIZE_,
								 .tx_buf.in = 0, 
								 .tx_buf.out = 0, 
								 .tx_buf.buf = uart0_TxBuf,
								 .tx_buf.buf_size = _UART0_TX_BUFF_SIZE_,
								 .overflow = 0, 
								 .status = 0, 
								 .Sem_rx = NULL, 
								 .Sem_tx = NULL, 
								 .time_out_value = RX_TIME_OUT_VALUE_0,
								 .def_baudrate_value = _UART0_BAUDRATE_
								 };

UARTData  uart0Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO uart0_device_io = {
    .DeviceID = UART_0,
	.CreateDevice =	CreateUart0,
	.CloseDevice = CloseUart,
    .OpenDevice = OpenUart0,
    .ReadDevice = ReadUart,
    .WriteDevice = WriteUart,
    .SeekDevice = SeekUART,
    .ReadDeviceText = ReadUartText,
    .WriteDeviceText = WriteUartText,
    .SeekDeviceText = SeekUARTText,
    .DeviceIOCtl = UartIOCtl,
    .DeviceName = "UART0",
    };

DEVICE uart0_device = {
    .device_io = (DEVICE_IO *)&uart0_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &uart_info_0,
    .pData = &uart0Data	
    };

#endif



#if(_UART1_)

	uint8_t uart1_RxBuf[_UART1_RX_BUFF_SIZE_];
	uint8_t uart1_TxBuf[_UART1_TX_BUFF_SIZE_];

    static DEVICE_HANDLE OpenUart1(void);
	static int CreateUart1(void);
    UART_INFO_LPC17XX uart_info_1 = {
								 .pUart = LPC_UART1,
								 .rx_buf.in = 0, 
								 .rx_buf.out = 0, 
								 .rx_buf.buf = uart1_RxBuf,
								 .rx_buf.buf_size = _UART1_RX_BUFF_SIZE_,
								 .tx_buf.in = 0, 
								 .tx_buf.out = 0, 
								 .tx_buf.buf = uart1_TxBuf,
								 .tx_buf.buf_size = _UART1_TX_BUFF_SIZE_,
								 .overflow = 0, 
								 .status = 0, 
								 .Sem_rx = NULL, 
								 .Sem_tx = NULL, 
								 .time_out_value = RX_TIME_OUT_VALUE_1,
								 .def_baudrate_value = _UART1_BAUDRATE_
								 };

UARTData  uart1Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO uart1_device_io = {
    .DeviceID = UART_1,
	.CreateDevice =	CreateUart1,
	.CloseDevice = CloseUart,
    .OpenDevice = OpenUart1,
    .ReadDevice = ReadUart,
    .WriteDevice = WriteUart,
    .SeekDevice = SeekUART,
    .ReadDeviceText = ReadUartText,
    .WriteDeviceText = WriteUartText,
    .SeekDeviceText = SeekUARTText,
    .DeviceIOCtl = UartIOCtl,
    .DeviceName = "UART1",
    };

DEVICE uart1_device = {
    .device_io = (DEVICE_IO *)&uart1_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &uart_info_1,
    .pData = &uart1Data	
    };

#endif

#if(_UART2_)

	uint8_t uart2_RxBuf[_UART2_RX_BUFF_SIZE_];
	uint8_t uart2_TxBuf[_UART2_TX_BUFF_SIZE_];
				
    static DEVICE_HANDLE OpenUart2(void);
	static int CreateUart2(void);
    UART_INFO_LPC17XX uart_info_2 = {
								 .pUart = LPC_UART2,
								 .rx_buf.in = 0, 
								 .rx_buf.out = 0, 
								 .rx_buf.buf = uart2_RxBuf,
								 .rx_buf.buf_size = _UART2_RX_BUFF_SIZE_,
								 .tx_buf.in = 0, 
								 .tx_buf.out = 0, 
								 .tx_buf.buf = uart2_TxBuf,
								 .tx_buf.buf_size = _UART2_TX_BUFF_SIZE_, 
								 .overflow = 0, 
								 .status = 0, 
								 .Sem_rx = NULL, 
								 .Sem_tx = NULL, 
								 .time_out_value = RX_TIME_OUT_VALUE_2,
								 .def_baudrate_value = _UART2_BAUDRATE_
								 };

UARTData  uart2Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO uart2_device_io = {
    .DeviceID = UART_2,
	.CreateDevice =	CreateUart2,
	.CloseDevice = CloseUart,
    .OpenDevice = OpenUart2,
    .ReadDevice = ReadUart,
    .WriteDevice = WriteUart,
    .SeekDevice = SeekUART,
    .ReadDeviceText = ReadUartText,
    .WriteDeviceText = WriteUartText,
    .SeekDeviceText = SeekUARTText,
    .DeviceIOCtl = UartIOCtl,
    .DeviceName = "UART2",
    };

DEVICE uart2_device = {
    .device_io = (DEVICE_IO *)&uart2_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &uart_info_2,
    .pData = &uart2Data
    };

#endif


#if(_UART3_)

	uint8_t uart3_RxBuf[_UART3_RX_BUFF_SIZE_];
	uint8_t uart3_TxBuf[_UART3_TX_BUFF_SIZE_];

    static DEVICE_HANDLE OpenUart3(void);
	static int CreateUart3(void);
    UART_INFO_LPC17XX uart_info_3 = {
								 .pUart = LPC_UART3,
								 .rx_buf.in = 0, 
								 .rx_buf.out = 0, 
								 .rx_buf.buf = uart3_RxBuf,
								 .rx_buf.buf_size = _UART3_RX_BUFF_SIZE_,
								 .tx_buf.in = 0, 
								 .tx_buf.out = 0, 
								 .tx_buf.buf = uart3_TxBuf,
								 .tx_buf.buf_size = _UART3_TX_BUFF_SIZE_,
								 .overflow = 0, 
								 .status = 0, 
								 .Sem_rx = NULL, 
								 .Sem_tx = NULL, 
								 .time_out_value = RX_TIME_OUT_VALUE_3,
								 .def_baudrate_value = _UART3_BAUDRATE_
								 };

UARTData  uart3Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO uart3_device_io = {
    .DeviceID = UART_3,
	.CreateDevice =	CreateUart3,
	.CloseDevice = CloseUart,
    .OpenDevice = OpenUart3,
    .ReadDevice = ReadUart,
    .WriteDevice = WriteUart,
    .SeekDevice = SeekUART,
    .ReadDeviceText = ReadUartText,
    .WriteDeviceText = WriteUartText,
    .SeekDeviceText = SeekUARTText,
    .DeviceIOCtl = UartIOCtl,
    .DeviceName = "UART3",
    };

DEVICE uart3_device = {
    .device_io = (DEVICE_IO *)&uart3_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &uart_info_3,
    .pData = &uart3Data
    };

#endif



#if(_UART0_)

int CreateUart0(void)
{
    UART_CFG_Type UARTConfigStruct;   // UART Configuration structure variable
    UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // UART FIFO configuration Struct variable
    PINSEL_CFG_Type PinCfg;   // Pin configuration for UART1

    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _UART0_PORT_;
    PinCfg.Pinnum = _UART0_RXD_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _UART0_TXD_PIN_;
    PINSEL_ConfigPin(&PinCfg);

    UARTConfigStruct.Baud_rate = _UART0_BAUDRATE_;
    UARTConfigStruct.Databits = UART_DATABIT_8;
    UARTConfigStruct.Parity = UART_PARITY_NONE;
    UARTConfigStruct.Stopbits = UART_STOPBIT_1;
    UART_Init(LPC_UART0, &UARTConfigStruct);

    UARTFIFOConfigStruct.FIFO_DMAMode = DISABLE;
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV3;
	UARTFIFOConfigStruct.FIFO_ResetRxBuf = ENABLE;
	UARTFIFOConfigStruct.FIFO_ResetTxBuf = ENABLE;

    UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);

    UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);
    UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);

    NVIC_SetPriority(UART0_IRQn, 31);//, ((0x01<<3)|0x01));
    NVIC_EnableIRQ(UART0_IRQn);


    UART_TxCmd (LPC_UART0, ENABLE);


	vSemaphoreCreateBinary(uart_info_0.Sem_tx);
	if(uart_info_0.Sem_tx == NULL)
	{
		PUT_MES_TO_LOG("UART0: Semaphor_tx = NULL", 0, 1);
		DEBUG_PUTS("UART0: Semaphor_tx = NULL\n");
		return DEVICE_ERROR;
	}

	vSemaphoreCreateBinary(uart_info_0.Sem_rx);
	if(uart_info_0.Sem_rx == NULL)
	{
		PUT_MES_TO_LOG("UART0: Semaphor_rx = NULL", 0, 1);
		DEBUG_PUTS("UART0: Semaphor_rx = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("UART0 Created\n");

	return 	DEVICE_OK;
}

DEVICE_HANDLE OpenUart0(void)
{

	if(uart0_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 uart0_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		PUT_MES_TO_LOG("UART0 RE_OPEN", 0, 2);
		DEBUG_PUTS("UART0 RE_OPEN\n");
	}
	return 	&uart0_device;
}

#endif // UART0


#if(_UART1_)

int CreateUart1(void)
{

    UART_CFG_Type UARTConfigStruct;   // UART Configuration structure variable
    UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // UART FIFO configuration Struct variable
    PINSEL_CFG_Type PinCfg;   // Pin configuration for UART1

	if(_UART1_PORT_ == 0)
    	PinCfg.Funcnum = 1;
	else if(_UART1_PORT_ == 2)
		PinCfg.Funcnum = 2;

    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _UART1_PORT_;
    PinCfg.Pinnum = _UART1_RXD_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _UART1_TXD_PIN_; 
    PINSEL_ConfigPin(&PinCfg);

#if(USE_RS485_CONTROL)
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _UART1_PORT_;
    PinCfg.Pinnum = _UART1_RS485_DE_PIN_;
    PINSEL_ConfigPin(&PinCfg);	   
#endif

    UARTConfigStruct.Baud_rate = _UART1_BAUDRATE_;
    UARTConfigStruct.Databits = UART_DATABIT_8;
    UARTConfigStruct.Parity = UART_PARITY_NONE;
    UARTConfigStruct.Stopbits = UART_STOPBIT_1;
    UART_Init((LPC_UART_TypeDef *)LPC_UART1, &UARTConfigStruct);

	UARTFIFOConfigStruct.FIFO_DMAMode = DISABLE;
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV3;
	UARTFIFOConfigStruct.FIFO_ResetRxBuf = ENABLE;
	UARTFIFOConfigStruct.FIFO_ResetTxBuf = ENABLE;

#if(USE_RS485_CONTROL)
	UART1_RS485_CTRLCFG_Type RS485_Cfg;
	RS485_Cfg.NormalMultiDropMode_State = DISABLE;
	RS485_Cfg.Rx_State = ENABLE;
	RS485_Cfg.AutoAddrDetect_State = DISABLE;
	RS485_Cfg.AutoDirCtrl_State = ENABLE;
	RS485_Cfg.DirCtrlPin = UART1_RS485_DIRCTRL_DTR;
	RS485_Cfg.DirCtrlPol_Level = SET;
	RS485_Cfg.DelayValue = 0;
	UART_RS485Config(LPC_UART1, &RS485_Cfg);
	UART_RS485ReceiverCmd(LPC_UART1, ENABLE);  
#endif

    UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART1, &UARTFIFOConfigStruct);

    UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_RBR, ENABLE);
    UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_THRE, ENABLE);

    NVIC_SetPriority(UART1_IRQn, 31);//((0x01<<3)|0x01));
    NVIC_EnableIRQ(UART1_IRQn);

    UART_TxCmd ((LPC_UART_TypeDef *)LPC_UART1, ENABLE);

    vSemaphoreCreateBinary(uart_info_1.Sem_tx);
	if(uart_info_1.Sem_tx == NULL)
	{
		PUT_MES_TO_LOG("UART1: Semaphor_tx = NULL", 0, 1);
		DEBUG_PUTS("UART1: Semaphor_tx = NULL\n");
		return DEVICE_ERROR;
	}

	vSemaphoreCreateBinary(uart_info_1.Sem_rx);
	if(uart_info_1.Sem_rx == NULL)
	{
		PUT_MES_TO_LOG("UART1: Semaphor_rx = NULL", 0, 1);
		DEBUG_PUTS("UART1: Semaphor_rx = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("UART1 Created\n");

	return 	DEVICE_OK;
}

DEVICE_HANDLE OpenUart1(void)
{

	if(uart1_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 uart1_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		PUT_MES_TO_LOG("UART1 RE_OPEN", 0, 2);
		DEBUG_PUTS("UART1 RE_OPEN\n");
	}
	return 	&uart1_device;
}

#endif  // UART1

#if(_UART2_)
int CreateUart2(void)
{

    UART_CFG_Type UARTConfigStruct;   // UART Configuration structure variable
    UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // UART FIFO configuration Struct variable
    PINSEL_CFG_Type PinCfg;   // Pin configuration for UART2

	if(_UART2_PORT_ == 0)
    	PinCfg.Funcnum = 1;
	else if(_UART2_PORT_ == 2)
		PinCfg.Funcnum = 2;

    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _UART2_PORT_;
    PinCfg.Pinnum = _UART2_RXD_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _UART2_TXD_PIN_;
    PINSEL_ConfigPin(&PinCfg);

    UARTConfigStruct.Baud_rate = _UART2_BAUDRATE_;
    UARTConfigStruct.Databits = UART_DATABIT_8;
    UARTConfigStruct.Parity = UART_PARITY_NONE;
    UARTConfigStruct.Stopbits = UART_STOPBIT_1;
    UART_Init(LPC_UART2, &UARTConfigStruct);

    UARTFIFOConfigStruct.FIFO_DMAMode = DISABLE;
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV3;
	UARTFIFOConfigStruct.FIFO_ResetRxBuf = ENABLE;
	UARTFIFOConfigStruct.FIFO_ResetTxBuf = ENABLE;

    UART_FIFOConfig(LPC_UART2, &UARTFIFOConfigStruct);

    UART_IntConfig(LPC_UART2, UART_INTCFG_RBR, ENABLE);
    UART_IntConfig(LPC_UART2, UART_INTCFG_THRE, ENABLE);

    NVIC_SetPriority(UART2_IRQn, 31);// ((0x01<<3)|0x01));
    NVIC_EnableIRQ(UART2_IRQn);


    UART_TxCmd (LPC_UART2, ENABLE);

    vSemaphoreCreateBinary(uart_info_2.Sem_tx);
	if(uart_info_2.Sem_tx == NULL)
	{
		PUT_MES_TO_LOG("UART2: Semaphor_tx = NULL", 0, 1);
		DEBUG_PUTS("UART2: Semaphor_tx = NULL\n");
		return DEVICE_ERROR;
	}

	vSemaphoreCreateBinary(uart_info_2.Sem_rx);
	if(uart_info_2.Sem_rx == NULL)
	{
		PUT_MES_TO_LOG("UART2: Semaphor_rx = NULL", 0, 1);
		DEBUG_PUTS("UART2: Semaphor_rx = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("UART2 Created\n");

	return 	DEVICE_OK;

}

DEVICE_HANDLE OpenUart2(void)
{

	if(uart2_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 uart2_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		PUT_MES_TO_LOG("UART2 RE_OPEN", 0, 2);
		DEBUG_PUTS("UART2 RE_OPEN\n");
	}
	return 	&uart2_device;
}

#endif  // UART2


#if(_UART3_)
int CreateUart3(void)
{

    UART_CFG_Type UARTConfigStruct;   // UART Configuration structure variable
    UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // UART FIFO configuration Struct variable
    PINSEL_CFG_Type PinCfg;   // Pin configuration for UART3

	if(_UART3_PORT_ == 0)
	{
		if(_UART3_TXD_PIN_ == 0)
    		PinCfg.Funcnum = 2;
		else
			PinCfg.Funcnum = 3;
	}
	else if(_UART3_PORT_ == 4)
    	PinCfg.Funcnum = 3;
		 
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _UART3_PORT_;
    PinCfg.Pinnum = _UART3_RXD_PIN_; 
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _UART3_TXD_PIN_; 
    PINSEL_ConfigPin(&PinCfg);

    UARTConfigStruct.Baud_rate = _UART3_BAUDRATE_;
    UARTConfigStruct.Databits = UART_DATABIT_8;
    UARTConfigStruct.Parity = UART_PARITY_NONE;
    UARTConfigStruct.Stopbits = UART_STOPBIT_1;
    UART_Init(LPC_UART3, &UARTConfigStruct);

    UARTFIFOConfigStruct.FIFO_DMAMode = DISABLE;
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV3;
	UARTFIFOConfigStruct.FIFO_ResetRxBuf = ENABLE;
	UARTFIFOConfigStruct.FIFO_ResetTxBuf = ENABLE;

    UART_FIFOConfig(LPC_UART3, &UARTFIFOConfigStruct);

    UART_IntConfig(LPC_UART3, UART_INTCFG_RBR, ENABLE);
    UART_IntConfig(LPC_UART3, UART_INTCFG_THRE, ENABLE);

    NVIC_SetPriority(UART3_IRQn, 31);// ((0x01<<3)|0x01));
    NVIC_EnableIRQ(UART3_IRQn);


    UART_TxCmd (LPC_UART3, ENABLE);	

    vSemaphoreCreateBinary(uart_info_3.Sem_tx);
	if(uart_info_3.Sem_tx == NULL)
	{
		PUT_MES_TO_LOG("UART3: Semaphor_tx = NULL", 0, 1);
		DEBUG_PUTS("UART3: Semaphor_tx = NULL\n");
		return DEVICE_ERROR;
	}

	vSemaphoreCreateBinary(uart_info_3.Sem_rx);
	if(uart_info_3.Sem_rx == NULL)
	{
		PUT_MES_TO_LOG("UART3: Semaphor_rx = NULL", 0, 1);
		DEBUG_PUTS("UART3: Semaphor_rx = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("UART3 Created\n");

	return 	DEVICE_OK;
}

DEVICE_HANDLE OpenUart3(void)
{

	if(uart3_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 uart3_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		PUT_MES_TO_LOG("UART3 RE_OPEN", 0, 2);
		DEBUG_PUTS("UART3 RE_OPEN\n");
	}
	return 	&uart3_device;
}

#endif // UART3




/**********************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////COMMON FUNCTIONS////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************************/





int WriteUartText(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return -1;
}

int ReadUartText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)
	UARTData *pData = (UARTData *)(((DEVICE *)handle)->pData);
    char *BufferText;
	int sprintf_count;
	UART_INFO_LPC17XX *pUartInfo = (UART_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    UART_BUF *p = &(pUartInfo->rx_buf);
	uint16_t rx_in;
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
        {
			sprintf_count = sprintf(BufferText, "Recive - %10u\n", pData->rx_count);
            sprintf_count += sprintf(BufferText + sprintf_count, "Transmit %10u\nRecive buf:\n", pData->tx_count);
			rx_in = p->in;
			while(sprintf_count < 290)
			{
				BufferText[sprintf_count] = p->buf[rx_in];
				sprintf_count++;
				if(rx_in == 0)
					rx_in = p->buf_size;
				rx_in--;
			}
                    
			if((pData->DataPoint + count) > sprintf_count)
			{
				count = sprintf_count - pData->DataPoint;
			}
			memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->DataPoint), count);
			pData->DataPoint += count;
			vPortFree(BufferText);
            return count;
        }
	}

    return 0;
#else
	return -1;
#endif
}

int SeekUART(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    return -1;
}

int SeekUARTText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
        UARTData *pData = (UARTData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_UART_DATA_OFFSET)
                {
                    pData->DataPoint = offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                }
                break;
            case 1:
                if((pData->DataPoint + offset) < MAX_UART_DATA_OFFSET)
                {
                    pData->DataPoint = pData->DataPoint + offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                } 
                break;
            case 2:
                if(offset < MAX_UART_DATA_OFFSET)
                {
                    pData->DataPoint = MAX_UART_DATA_OFFSET - offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                } 
                break;
            default:
                pData->DataPoint = 0;
                return 0;            
        }
        return pData->DataPoint;
#else
	return 0;
#endif
}


void CloseUart (DEVICE_HANDLE handle)
{
	DEVICE *pDevice = (DEVICE *)handle;

	if(pDevice->flDeviceOpen == DEVICE_OPEN)
	{
		 pDevice->flDeviceOpen = DEVICE_CLOSE;
	}
	else
	{
		PUT_MES_TO_LOG("UART RE_CLOSE", 0, 2);
		DEBUG_PUTS("UART RE_CLOSE\n");
	}
}

void StartTransmitUart(UART_INFO_LPC17XX *pUartInfo)
{
	UART_BUF *p = &(pUartInfo->tx_buf);
	int i = 0;
	vPortEnterCritical();
		//if(UART_CheckBusy((LPC_UART_TypeDef *)pUartInfo->pUart) != SET)
		if((((LPC_UART_TypeDef *)pUartInfo->pUart)->LSR & UART_LSR_THRE) != 0)
        {
			while(i < 4) // пока передающий буфер пуст или не закончатся данные
    		{
        		if(p->out == p->in)// && pUartInfo->tx_empty == 1)
        		{
            		break; // выходим, данные закончились
        		}
         		i++;
        		((LPC_UART_TypeDef *)pUartInfo->pUart)->THR = p->buf[p->out];
        		p->out++;
        		if(p->out == p->buf_size) p->out = 0;
   			}
        }
		vPortExitCritical();
}

int WriteUart (DEVICE_HANDLE handle,const void * pSrc, int count)
{
    UART_INFO_LPC17XX *pUartInfo = (UART_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    UART_BUF *p = &(pUartInfo->tx_buf);
	UARTData *pData = (UARTData *)(((DEVICE *)handle)->pData);
	uint16_t free_space = 0;
    int i;   
	      
        for(i = 0; i < count; i++)
        {
            //while(p->in == p->out && pUartInfo->tx_empty == 0)
			if(p->in >= p->out) free_space = p->buf_size - (p->in - p->out);   //  выясняем сколько свободного места в буфере
    		else free_space = p->out - p->in;
		    while(free_space == 1)
            {
				StartTransmitUart(pUartInfo);
                xSemaphoreTake(pUartInfo->Sem_tx, portMAX_DELAY );
				if(p->in >= p->out) free_space = p->buf_size - (p->in - p->out);   //  выясняем сколько свободного места в буфере
    			else free_space = p->out - p->in;
            }
            p->buf[p->in++] = ((uint8_t *)pSrc)[i];
            if(p->in == p->buf_size) p->in = 0;
           // pUartInfo->tx_empty = 0;
        }
		StartTransmitUart(pUartInfo);
		pData->tx_count+=count;   //  Статистика принятых байт=
        return count;

}

int ReadUart(DEVICE_HANDLE handle, void * pDst, int count) {
 
    UART_INFO_LPC17XX *pUartInfo = (UART_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    UART_BUF *p = &(pUartInfo->rx_buf);
    UARTData *pData = (UARTData *)(((DEVICE *)handle)->pData);
	pUartInfo->time_out = 0;
	pUartInfo->status = 0;
    int i = 0;

        for(i = 0; i < count; i++)
        {
            while(p->out == p->in) // Если счетчики равны - буфер пуст
            {
                if(pUartInfo->time_out == 1)
                {
										pUartInfo->time_out = 0;
										pUartInfo->status = UART_STATUS_CH_TIMEOUT;
                    return i;
                }  
                if(xSemaphoreTake(pUartInfo->Sem_rx, pUartInfo->time_out_value ) == pdFALSE)
                {
										pUartInfo->status = UART_STATUS_TIMEOUT;
                    return i;
                }  
            }
            ((uint8_t *)pDst)[i] = p->buf[p->out];  // прочли данные из буфера
            p->out++;                               
            if(p->out == p->buf_size) p->out = 0;         
            pData->rx_count++;   //  Статистика принятых байт
        } 
        return i;

}

int UartIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    UART_INFO_LPC17XX *pUartInfo = (UART_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
	LPC_UART_TypeDef * pUart = (LPC_UART_TypeDef *)pUartInfo->pUart;
    UART_BUF *p_rx = &(pUartInfo->rx_buf);
    UART_BUF *p_tx = &(pUartInfo->tx_buf);
//	UART_CONFIG_INFO *pConfig = NULL;
//	UART_CFG_Type UARTConfigStruct;   
    //UART_FIFO_CFG_Type UARTFIFOConfigStruct; 
    uint16_t count = 0; 
		uint8_t parity = 0;

    switch(code)
    {
        case RX_DATA_COUNT:
            if(p_rx->in >= p_rx->out) count = p_rx->in - p_rx->out;
            else count = (p_rx->buf_size - p_rx->out + p_rx->in);
            memcpy(pData, (uint8_t *)&count, 2);
            break;
        case FREE_TX_SPACE:
            if(p_tx->out > p_tx->in) count = p_tx->out - p_tx->in;
            else count = (p_tx->buf_size - p_tx->in + p_tx->out);
            memcpy(pData, (uint8_t *)&count, 2);
            break;
		case FLUSH_DEVICE:
            p_tx->out = p_tx->in;
			p_rx->out = p_rx->in;
			//p_rx->in = p_rx->out;
			// Если очистка производится из другого потока (lwIP sio_abort) мы должны завершить ожидание по приему
			pUartInfo->time_out = 1;
			xSemaphoreGive(pUartInfo->Sem_rx);
            break;
		//////////////////////////////////////////////////////////////////
////////////////////////RFC2217 Functions/////////////////////////
/////////////////////////////START////////////////////////////////
//////////////////////////////////////////////////////////////////

        case SET_BAUDRATE:
            uart_set_divisors(pUart, *(uint32_t *)pData);
            break;
        
        case GET_BAUDRATE:
            break;
				case RE_INIT:
						UART_DeInit(pUart);
						(DEVICE_IO *)(((DEVICE *)handle)->device_io)->CreateDevice();
            break;

        case SET_DATASIZE:
            pUart->LCR &= 0xFC;
            switch(*(uint8_t *)pData)
            {
                case 5:
                    pUart->LCR |= UART_LCR_WLEN5;
                    break;
                case 6:
                    pUart->LCR |= UART_LCR_WLEN6;
                    break;
                case 7:
                    pUart->LCR |= UART_LCR_WLEN7;
                    break;
                case 8:
                    pUart->LCR |= UART_LCR_WLEN8;
                    break;
                default:
                    return -1;
            }
            break;

        case GET_DATASIZE:
            break;

        case SET_PARITY:
            parity = *(uint8_t *)pData;
            if(parity > 1)
            {
                pUart->LCR |= (1 << 3);

                switch(parity)
                {
					case 1:        //NONE
                        pUart->LCR &= ~((1 << 4) | (1 << 5) | (1 << 3));
                        break;
                    case 2:        //ODD
						pUart->LCR &= ~((1 << 4) | (1 << 5) | (1 << 3));
                        pUart->LCR |= (1 << 3);
                        break;
                    case 3:        //EVEN
                        pUart->LCR &= ~((1 << 4) | (1 << 5) | (1 << 3));
                        pUart->LCR |= ((1 << 4) | (1 << 3));
                        break;
                    case 4:        //MARK
                        pUart->LCR &= ~((1 << 4) | (1 << 5) | (1 << 3));
                        pUart->LCR |= ((1 << 5) | (1 << 3));
                        break;
                    case 5:        //SPACE
						pUart->LCR &= ~((1 << 4) | (1 << 5) | (1 << 3));
                        pUart->LCR |= ((1 << 4) | (1 << 5) | (1 << 3));
                        break;
                }
            }
            else if(parity == 1)   //NO PARITY
                pUart->LCR &= ~(1 << 3);
            else                   //GET PARITY (пока не используем)
                pUart->LCR &= ~(1 << 3);
            
            break;

        case GET_PARITY:
            break;

        case SET_STOPSIZE:
           // pUart->LCR &= 0xFB;
            switch(*(uint8_t *)pData)
            {
                case 1:
                    pUart->LCR &= ~(1 << 2);
                    break;
                case 2:
                    pUart->LCR |= (1 << 2);
                    break;
                default:
                    return -1;
            }
            break;

        case GET_STOPSIZE:
            break;

        case SET_CONTROL:
            switch(*pData)
            {
                case SET_BREAK_ON: 
                    pUart->LCR |= (1 << 6); 
                    break;
                case SET_BREAK_OFF:
                    pUart->LCR &= ~(1 << 6); 
                    break;
                default:
                    break;
            }
            break;

        case PURGE_DATA:
            vPortEnterCritical();
			{
                if(*pData & 0x01)
                {
                    //p_rx = &(pUartInfo->rx_buf);

                    p_rx->in = 0;
                    p_rx->out = 0;
                }
    
                if(*pData & 0x02)
                {
                   // p_rx = &(pUartInfo->tx_buf);

                    p_rx->in = 0;
                    p_rx->out = 0;
                }
             }vPortExitCritical();
            break;

		case SET_RX_TIMEOUT:
			pUartInfo->time_out_value = *(uint32_t *)pData;
			break;
			
		case GET_RX_TIMEOUT:
			memcpy(pData, (uint8_t *)&pUartInfo->time_out_value, 4);
			break;
		case GET_STATUS_DEVICE:
			memset(pData, 0, 4);
			memcpy(pData, (uint8_t *)&pUartInfo->status, 1);
			break;

		case SET_DEF_BAUDRATE:
			uart_set_divisors(pUart, pUartInfo->def_baudrate_value);
			break;
        

//////////////////////////////////////////////////////////////////
////////////////////////RFC2217 Functions/////////////////////////
//////////////////////////////END/////////////////////////////////
//////////////////////////////////////////////////////////////////

        case DISABLE_RX:

            break;
            

        case ENABLE_RX:

            break;

        case DISABLE_TX:

            break;

        case ENABLE_TX:

            break; 
	/*	case DEVICE_CONFIG:
			 pConfig = (UART_CONFIG_INFO *)pData;
			 UARTConfigStruct.Baud_rate = pConfig->baud_rate;
			 UARTConfigStruct.Parity = (UART_PARITY_Type)(pConfig->parity);
			 UARTConfigStruct.Databits = (UART_DATABIT_Type)(pConfig->databits);
			 UARTConfigStruct.Stopbits = (UART_STOPBIT_Type)(pConfig->stopbits);
			 UARTFIFOConfigStruct.FIFO_ResetRxBuf = ENABLE;
			 UARTFIFOConfigStruct.FIFO_ResetTxBuf = ENABLE;
			 UARTFIFOConfigStruct.FIFO_Level = (UART_FITO_LEVEL_Type)(pConfig->fifo_Level);
			 pUartInfo->time_out_value = pConfig->rx_timeout;
			 UART_Init((LPC_UART_TypeDef *)pUartInfo->pUart, &UARTConfigStruct);
			 UART_FIFOConfig((LPC_UART_TypeDef *)pUartInfo->pUart, &UARTFIFOConfigStruct);
        */default:
            break;
    }
  /*  uint8_t parity;
    UART_INFO_LPC2300 *pUartInfo = (UART_INFO_LPC2300 *)(((DEVICE *)handle)->pDriverData);
    switch(code)
    {
        case BAUD_RATE:
            pUartInfo->pUART->UxDLL = DLL_VALUE(*(uint32_t *)pData);
            pUartInfo->pUART->UxDLM = DLM_VALUE(*(uint32_t *)pData);
            break;
        case WORLD_LENGTH:
            pUartInfo->pUART->UxLCR &= 0xFC;
            switch(*(uint8_t *)pData)
            {
                case 5:
                    pUartInfo->pUART->UxLCR |= 0x00;
                    break;
                case 6:
                    pUartInfo->pUART->UxLCR |= 0x01;
                    break;
                case 7:
                    pUartInfo->pUART->UxLCR |= 0x02;
                    break;
                case 8:
                    pUartInfo->pUART->UxLCR |= 0x03;
                    break;
                default:
                    return -1;
            }
            break;
        case STOP_BIT:
            pUartInfo->pUART->UxLCR &= 0xFB;
            switch(*(uint8_t *)pData)
            {
                case 1:
                    pUartInfo->pUART->UxLCR |= 0x00;
                    break;
                case 2:
                    pUartInfo->pUART->UxLCR |= 0x04;
                    break;
                default:
                    return -1;
            }
            break;
        case PARITY:
            pUartInfo->pUART->UxLCR &= 0xC7;
            parity = *(uint8_t *)pData;
            pUartInfo->pUART->UxLCR |= ((parity & 0xF0) >> 1);
            pUartInfo->pUART->UxLCR |= ((parity & 0x0F) << 4);
            break;
        case ALL_SETTINGS:
            if(UartIOCtl(handle, BAUD_RATE, pData))                     return -1;
            if(UartIOCtl(handle, WORLD_LENGTH, (uint8_t *)&pData[4]))   return -1;
            if(UartIOCtl(handle, STOP_BIT, (uint8_t *)&pData[5]))       return -1;
            if(UartIOCtl(handle, PARITY, (uint8_t *)&pData[6]))         return -1;
            break;
        default:
            return -1;
    }
   */ return 0;
}





/**********************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////INDIVUDUAL FUNCTIONS////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************************/








/**********************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////UART INTERRUPT HANDLERS///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************************/





void rx_interupt(UART_INFO_LPC17XX *pUartInfo, uint8_t time_out)
{

    static signed portBASE_TYPE xHigherPriorityTaskWoken;

    struct buf_st *p = &(pUartInfo->rx_buf);      //  буфер приема
    int i = (time_out ? 0:2);

	if(i == 0)
		i = 0;

    do
    {
        p->buf[p->in++] = ((LPC_UART_TypeDef *)pUartInfo->pUart)->RBR;
        if(p->in >= p->buf_size) p->in = 0;
        if(p->in == p->out)  // Если дошли до конца буфера, будет перезатирание данных
        {
            p->out++;                          // увеличиваем указатель прочитанных байт (презапись байта приравнивается к его прочтению)
            pUartInfo->overflow = 1;           //  отмечаем что было переполнение
            if(p->out >= p->buf_size) p->out = 0;
        }

    }while((((LPC_UART_TypeDef *)(pUartInfo->pUart))->FIFOLVL & 0xF) != i);  // пока в буфере RX есть данные

    pUartInfo->time_out = time_out;  //  Если это прерывание по таймауту, зафиксируем это (в данном случаи time_out будет отлична от нуля)
    xSemaphoreGiveFromISR( pUartInfo->Sem_rx, &xHigherPriorityTaskWoken );   // Если кто-то в ожидании данных, прекращаем это ожидание

}

void tx_interupt(UART_INFO_LPC17XX *pUartInfo)
{
    int i = 0;

    static signed portBASE_TYPE xHigherPriorityTaskWoken;

    uint16_t free_space = 0;
    struct buf_st *p = &(pUartInfo->tx_buf);      //  буфер передачи

    while(i < 16) // пока передающий буфер пуст или не закончатся данные
    {
        if(p->out == p->in)// && pUartInfo->tx_empty == 1)
        {
           // pUartInfo->tx_empty = 1;
            break; // выходим, данные закончились
        }
         i++;
        ((LPC_UART_TypeDef *)pUartInfo->pUart)->THR = p->buf[p->out];
        p->out++;
        if(p->out == p->buf_size) p->out = 0;
    }

    if(p->in >= p->out) free_space = p->buf_size - (p->in - p->out);   //  выясняем сколько свободного места в буфере
    else free_space = p->out - p->in;

    if(free_space > (p->buf_size/2)) xSemaphoreGiveFromISR( pUartInfo->Sem_tx, &xHigherPriorityTaskWoken );   // Если кто-то в ожидании свободного места в буфере, прекращаем это ожидание

    //UART_IntConfig((LPC_UART_TypeDef *)pUartInfo->pUart, UART_INTCFG_THRE, ENABLE);
     
}



#if(_UART3_)

void Uart_3_ISR(void)
{
    uint32_t intsrc;

    intsrc = (LPC_UART3->IIR & 0xE);
    
    if(intsrc == (2 << 1)) rx_interupt(&uart_info_3, 0);  // Прерывание по приему
    if(intsrc == (6 << 1)) rx_interupt(&uart_info_3, 1);  // Прерывание по таймауту
    if(intsrc == (1 << 1)) tx_interupt(&uart_info_3);  // Прерывание по передачи
 
}

#endif //_UART3_

#if(_UART2_)

void Uart_2_ISR(void)
{
    uint32_t intsrc;

    intsrc = (LPC_UART2->IIR & 0xE);
    if(intsrc == (2 << 1)) 
		rx_interupt(&uart_info_2, 0);  // Прерывание по приему
    if(intsrc == (6 << 1)) 
		rx_interupt(&uart_info_2, 1);  // Прерывание по таймауту
    if(intsrc == (1 << 1)) 
		tx_interupt(&uart_info_2);  // Прерывание по передачи
 
}

#endif //_UART2_

#if(_UART1_)

void Uart_1_ISR(void)
{
//static int flag = 0;
    uint32_t intsrc;
/*	if(flag == 0)
	{
		GPIO_SetValue(1, 1 << 19);
		flag = 1;
	}
	else
	{
		GPIO_ClearValue(1, 1 << 19);
		flag = 0;
	}  */

    intsrc = (LPC_UART1->IIR & 0xE);
    if(intsrc == (2 << 1)) 
		rx_interupt(&uart_info_1, 0);  // Прерывание по приему
    if(intsrc == (6 << 1)) 
		rx_interupt(&uart_info_1, 1);  // Прерывание по таймауту
    if(intsrc == (1 << 1)) 
		tx_interupt(&uart_info_1);  // Прерывание по передачи
 
}

#endif //_UART1_

#if(_UART0_)

void Uart_0_ISR(void)
{
    uint32_t intsrc;

    intsrc = (LPC_UART0->IIR & 0xE);
    if(intsrc == (2 << 1)) 
		rx_interupt(&uart_info_0, 0);  // Прерывание по приему
    if(intsrc == (6 << 1)) 
		rx_interupt(&uart_info_0, 1);  // Прерывание по таймауту
    if(intsrc == (1 << 1)) 
		tx_interupt(&uart_info_0);  // Прерывание по передачи
 
}

#endif //_UART0_

#endif /*_UART_*/
