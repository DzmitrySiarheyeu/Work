#include "Config.h"
#if(_MBS_)

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "driverscore.h"
#include "MB_server_device.h"
#include "MBServer.h"
#include "SetParametrs.h"
#include "User_Error.h"


static int mbs_rw(MB_RW_STRUCT *mbRW);

static DEVICE_HANDLE OpenMBS(void);
static int CreateMBS(void);

static void CloseMBS(DEVICE_HANDLE handle);
static int ReadMBS(DEVICE_HANDLE handle, void * dst, int count);
static int WriteMBS(DEVICE_HANDLE handle, const void * dst, int count);
static int SeekMBS(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteMBSText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadMBSText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekMBSText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int IOCtlMBS(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

static void inc_rx_counter(uint8_t id);
static void inc_tx_counter(uint8_t id);
static void get_rx_counter(int *buf);
static void get_tx_counter(int *buf);

// тут будут храниться опрашиваемые id
static ID_STRUCT supportedIDs = {.idBuf = 0, .rxCount = 0, .txCount = 0, .idCount = 0};

static MB_SERVER_INFO MBS_info = { .read_sl_count = 0, .value_sl_count = 0, .read_sl_max = 0, .value_sl_max = 0, .Sem = 0, .sens_pres_flag = 0, .pSlave_val = 0, .pSlave_info = 0};

MBS_DATA  MBS_data = {
    .text_data_point = 0,
	.adress = 0,
	.reg = 0,
	.read_mode = 0
    };

const DEVICE_IO MBS_device_io = {
    .DeviceID = MBS,
	.CreateDevice =	CreateMBS,
	.CloseDevice = CloseMBS,
    .OpenDevice = OpenMBS,
    .ReadDevice = ReadMBS,
    .WriteDevice = WriteMBS,
    .SeekDevice = SeekMBS,
    .ReadDeviceText = ReadMBSText,
    .WriteDeviceText = WriteMBSText,
    .SeekDeviceText = SeekMBSText,
	.DeviceIOCtl = IOCtlMBS,
    .DeviceName = "MBS"
    };


DEVICE MBS_device = {
    .device_io = (DEVICE_IO *)&MBS_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &MBS_info,
    .pData = &MBS_data
    };


/************************************************************************/
/************************************************************************/


int CreateMBS(void)
{
	ReadSettingsFile(PARAMETRS_FILE_NAME);
	init_MBS();
	ParametrsMemFree();
	create_sem(MBS_info.Sem);
	if(MBS_info.Sem == NULL)
	{
		PUT_MES_TO_LOG("MBS: Semaphor = NULL", 0, 1);
		DEBUG_PUTS("MBS: Semaphor = NULL\n");
		return DEVICE_ERROR;
	}
	DEBUG_PUTS("MBS Created\n");
	return 	DEVICE_OK;

}

DEVICE_HANDLE OpenMBS(void)
{

	take_sem(MBS_info.Sem, portMAX_DELAY);

	//DEBUG_PUTS("SSP0 OPEN\n");

	return 	(DEVICE_HANDLE)&MBS_device;
}



int SeekMBS(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    MBS_DATA *pData = (MBS_DATA *)(((DEVICE *)handle)->pData);

	pData->adress = origin;
	pData->reg = (uint16_t)offset;

	return offset;

}


int WriteMBSText(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return -1;
}

int ReadMBSText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)
//    MB_SERVER_INFO *pMBSinfo = (MB_SERVER_INFO *)(((DEVICE *)handle)->pDriverData);
	MBS_DATA *pData = (MBS_DATA *)(((DEVICE *)handle)->pData);

//	SLAVE_VALUE *pSl_val = pMBSinfo->pSlave_val;
	int i = 0;
    char *BufferText;
	int sprintf_count = 0;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(MB_TEXT_BUF_LENTH);
		if(BufferText != NULL)
        {
			for(i = 0; i < supportedIDs.idCount; i++)
			{
				sprintf_count += sprintf(BufferText + sprintf_count, "ID - %02d TX - %05d; RX - %05d;\n", supportedIDs.idBuf[i], supportedIDs.txCount[i], supportedIDs.rxCount[i]);
			}
			                
			if((pData->text_data_point + count) > sprintf_count)
			{
				count = sprintf_count - pData->text_data_point;
			}
			memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->text_data_point), count);
			pData->text_data_point += count;
			vPortFree(BufferText);
            return count;
        }
		return 0;
	}
	return 0;
#else
	return -1;
#endif
}

int SeekMBSText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
        MBS_DATA *pData = (MBS_DATA *)(((DEVICE *)handle)->pData);
//		MB_SERVER_INFO *pMBSinfo = (MB_SERVER_INFO *)(((DEVICE *)handle)->pDriverData);
        switch(origin)
        {
            case 0:
                if(offset < MB_TEXT_BUF_LENTH)
                {
                    pData->text_data_point = offset;
                }
                else
                {
                    pData->text_data_point = 0;
                }
                break;
            case 1:
                if((pData->text_data_point + offset) < MB_TEXT_BUF_LENTH)
                {
                    pData->text_data_point = pData->text_data_point + offset;
                }
                else
                {
                    pData->text_data_point = 0;
                } 
                break;
            case 2:
                if(offset < MB_TEXT_BUF_LENTH)
                {
                    pData->text_data_point = MB_TEXT_BUF_LENTH - offset;
                }
                else
                {
                    pData->text_data_point = 0;
                } 
                break;
            default:
                pData->text_data_point = 0;
                return 0;            
        }
        return pData->text_data_point;
#else
	return 0;
#endif
}

static int IOCtlMBS(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
	MBS_DATA *pMBSData = (MBS_DATA *)(((DEVICE *)handle)->pData);

	switch(code)
    {
		case SET_READ_MODE:
                pMBSData->read_mode = *(uint8_t *)pData;
                break;

		case GET_SUPPORTED_IDs_NUMBER:
				*pData = supportedIDs.idCount;
				break;

		case GET_SUPPORTED_IDs:
				memcpy(pData, supportedIDs.idBuf, supportedIDs.idCount);
				break;

		case GET_TX_PACKETS_COUNT:
				get_tx_counter((int *)pData);
				break;

		case GET_RX_PACKETS_COUNT:
				get_rx_counter((int *)pData);
				break;

		case MB_SERVER_RW:
			   return mbs_rw((MB_RW_STRUCT *)pData);
                
            default:
                return -1;
        }

        return 0;
}


void CloseMBS(DEVICE_HANDLE handle)
{
	MB_SERVER_INFO *pMBSinfo = (MB_SERVER_INFO *)(((DEVICE *)handle)->pDriverData);
	give_sem(pMBSinfo->Sem);
	//DEBUG_PUTS("SSP0 CLOSE\n");
}


int WriteMBS(DEVICE_HANDLE handle, const void * pSrc, int count)
{
    return -1;
}

int ReadMBS(DEVICE_HANDLE handle, void * pDst, int count)
{
	int i = 0;

	static uint8_t flag = 0xff;

	flag ^= 0xFF;

    MB_SERVER_INFO *pMBSinfo = (MB_SERVER_INFO *)(((DEVICE *)handle)->pDriverData);
	MBS_DATA *pData = (MBS_DATA *)(((DEVICE *)handle)->pData);

	SLAVE_VALUE *pSl_val = pMBSinfo->pSlave_val;

	count = -1;

    if(pData->read_mode == READ_ALL_DATA)
	{
		for(i = 0; i < pMBSinfo->value_sl_max; i++)
		{
			((float *)pDst)[i] = pSl_val[i].value;
		}
		count = i;

	}
	else
	{
		for(i = 0; i < pMBSinfo->value_sl_max; i++)
		{
			if(pSl_val[i].reg == pData->reg)
			{
				if(pSl_val[i].adress == pData->adress)
				{
					*((float *)pDst) = pSl_val[i].value;
					count = 1;
					if(pData->read_mode != READ_ONLY)
					{
						if(pSl_val[i].flag == 1) pSl_val[i].flag = 0;
						else count = -1;
					}
				}
			}
		}
	}

	if(flag)
		count = 0;
    
	return count;
}

void PoolMBS(void)
{
	uint8_t err = 0;

	if(MBS_info.sens_pres_flag == 0) return;

	do{

		SLAVE_VALUE *pSl_value = &(((SLAVE_VALUE *)(MBS_info.pSlave_val))[MBS_info.value_sl_count]);
		SLAVE_INFO  *pSl_info = &(((SLAVE_INFO *)(MBS_info.pSlave_info))[MBS_info.read_sl_count++]);
		int i = 0 ;
	
		err = ServerReadInputRegisters(pSl_info->reg, pSl_info->quantity, pSl_info->adress);
		inc_tx_counter(pSl_info->adress);
	
		if(err == 1)
		{
			inc_rx_counter(pSl_info->adress);
			for(i = 0; i < pSl_info->quantity; i++)
			{
				pSl_value[i].value = GetInpRegsFLOAT(i);
				pSl_value[i].flag = 1;
				pSl_value[i].bad_atmp_cnt = 0;
			}
		}
		else 
		{
			for(i = 0; i < pSl_info->quantity; i++)
			{
				if(++pSl_value[i].bad_atmp_cnt > pSl_value[i].bad_atmp_lim)
				{
					*((uint32_t *)&pSl_value[i].value) = QNaN;
					pSl_value[i].flag = 1;
					pSl_value[i].bad_atmp_cnt = 0;
				}
			}	
		}
	
		MBS_info.value_sl_count += pSl_info->quantity;
		if(MBS_info.value_sl_count >= MBS_info.value_sl_max) MBS_info.value_sl_count = 0;

	}while(MBS_info.read_sl_count < MBS_info.read_sl_max);
	//if(MBS_info.value_sl_count >= MBS_info.value_sl_max) MBS_info.value_sl_count = 0;
	if(MBS_info.read_sl_count >= MBS_info.read_sl_max) MBS_info.read_sl_count = 0;
}

/****************************** ModBus RW **********************************/
/*************** для записи / чтения произвольного регистра ****************/

static int mbs_rw(MB_RW_STRUCT *mbRW)
{
	// запись
	if(mbRW->opType == 'w')	
	{
		// 2 байта
		if(mbRW->valType == 'u')
		{
			//memrevercy(mbRW->valBuf, 2);
			if(!ServerWriteSingleRegister1(mbRW->addr, *(uint16_t *)mbRW->valBuf, mbRW->id))
				return -1;
		}
		// 4 байта
		else
		{
			//memrevercy(mbRW->valBuf, 4);

			if(!ServerWriteSingleRegister1(mbRW->addr, *((uint16_t *)mbRW->valBuf + 1), mbRW->id))
				return -1;

			if(!ServerWriteSingleRegister1(mbRW->addr + 1, *(uint16_t *)mbRW->valBuf, mbRW->id))
				return -1;
		}
			
				
	}
	// чтение
	else
	{
		if(!ServerReadInputRegisters1(mbRW->addr, mbRW->valType == 'u' ? 1 : 2, mbRW->id))
			return -1;

		switch(mbRW->valType)
		{
			case 'u':
				*(uint16_t *)mbRW->valBuf = GetInpRegsUSHORT(0);
				break;
			case 'l':
				*(uint32_t *)mbRW->valBuf = GetInpRegsLONG(0);
				break;
			case 'f':
				*(float *)mbRW->valBuf = GetInpRegsFLOAT(0);
				break;
		}
	}

	return 0;
}

// наращиваем счетчик удачно принятых пакетов
void inc_rx_counter(uint8_t id)
{
	uint8_t cnt = 0;
	// ищем id устройства
	for(; cnt < supportedIDs.idCount; cnt++)
	{
		// нашли
		if(id == supportedIDs.idBuf[cnt])
		{
			// наращиваем счетчик
			supportedIDs.rxCount[cnt]++;
			return;
		}
	}
}
// наращиваем счетчик отправленных пакетов
void inc_tx_counter(uint8_t id)
{
	uint8_t cnt = 0;
	// ищем id устройства
	for(; cnt < supportedIDs.idCount; cnt++)
	{
		// нашли
		if(id == supportedIDs.idBuf[cnt])
		{
			// наращиваем счетчик
			supportedIDs.txCount[cnt]++;
			return;
		}
	}
}

void get_rx_counter(int *buf)
{
	uint8_t cnt = 0;
	uint8_t id = (uint8_t)*buf; // в параметре находиться id устройства

	// ищем id устройства
	for(; cnt < supportedIDs.idCount; cnt++)
	{
		// нашли
		if(id == supportedIDs.idBuf[cnt])
		{
			// записывает значение счетчика в тот-же буфер
			*buf = supportedIDs.rxCount[cnt];
			return;
		}
	}
	
	*buf = -1;	
}

void get_tx_counter(int *buf)
{
	uint8_t cnt = 0;
	uint8_t id = (uint8_t)*buf; // в параметре находиться id устройства

	// ищем id устройства
	for(; cnt < supportedIDs.idCount; cnt++)
	{
		// нашли
		if(id == supportedIDs.idBuf[cnt])
		{
			// записывает значение счетчика в тот-же буфер
			*buf = supportedIDs.txCount[cnt];
			return;
		}
	}

	*buf = -1;
}

/*************************************************************************/

int mbs_get_reg(char * id_dev)
{
	char *p_data_adr = NULL;
	int v_reg_f = 0;
	int v_reg_l = 0;
	
	p_data_adr = (char *)GetPointToElement(id_dev, "Reg");

	if(p_data_adr == NULL) 
	{
		MBS_info.sens_pres_flag = 0;
		return -1;
	}
	while(*p_data_adr != 0)
	{
		v_reg_f = atoi(p_data_adr);
		p_data_adr = strpbrk(p_data_adr, ".;");
		
		if(p_data_adr != NULL && *p_data_adr == '.')   //  Если '.' значит указан интервал регистров
		{
			p_data_adr+=2;
			v_reg_l = atoi(p_data_adr);
			if(v_reg_l == 0)
				v_reg_l = 	v_reg_f;
			MBS_info.value_sl_max += v_reg_l - v_reg_f + 1;
			p_data_adr = strpbrk(p_data_adr, ".;");
		}
		else
			MBS_info.value_sl_max++; 
		MBS_info.read_sl_max++;
		if(p_data_adr == NULL)
			break;
		p_data_adr++;
	}
	return 0;
}

void init_MBS(void)
{
	char p_data_id[30], *p_temp, *p_data_adr;
	
	SLAVE_VALUE *pSl_value = NULL;
	SLAVE_INFO  *pSl_info = NULL;
	uint16_t  tout = 0;
	int i, j, k;
	int v_reg_f = 0;
	int v_reg_l = 0;
    
    if(GetSettingsFileStatus())
    {
        p_temp = (char *)GetPointToElement("MB_Server", "ID");    //  Вытаскиваем строку с id устройств
		if(p_temp == NULL || strlen(p_temp) >= sizeof(p_data_id)) 
		{
			MBS_info.sens_pres_flag = 0;
			return;
		}
		strcpy(p_data_id, p_temp);
		p_temp = p_data_id;
		
		/******************** ModBus id и счетчики пакетов *************************/
		supportedIDs.idCount = 0;
		while(p_temp != NULL && *(p_temp + 1) != 0)
		{
			p_temp = strchr(p_temp + 1, ';');
			if(p_temp)
				*p_temp = 0;
			supportedIDs.idCount++;
		}

		supportedIDs.idBuf = (uint8_t *)pvPortMalloc((size_t)supportedIDs.idCount);
		if(supportedIDs.idBuf == NULL)
		{
			MBS_info.sens_pres_flag = 0;
			return;	
		}
		memset(supportedIDs.idBuf, 0, supportedIDs.idCount);
		
		p_temp = p_data_id;
		for(i = 0; i < supportedIDs.idCount; i++)
		{
			supportedIDs.idBuf[i] = atoi(p_temp);
			if(mbs_get_reg(p_temp) == -1)
				return;
			p_temp += strlen(p_temp) + 1;
		}

		supportedIDs.rxCount = (uint32_t *)pvPortMalloc((size_t)supportedIDs.idCount * sizeof(uint32_t));
		if(supportedIDs.rxCount == NULL)
		{
			MBS_info.sens_pres_flag = 0;
			return;	
		}
		memset(supportedIDs.rxCount, 0, supportedIDs.idCount * sizeof(uint32_t));

		supportedIDs.txCount = (uint32_t *)pvPortMalloc((size_t)supportedIDs.idCount * sizeof(uint32_t));
		if(supportedIDs.txCount == NULL)
		{
			MBS_info.sens_pres_flag = 0;
			return;	
		}
		memset(supportedIDs.txCount, 0, supportedIDs.idCount * sizeof(uint32_t));
		/*************************************************************************/

		pSl_value = (SLAVE_VALUE *)pvPortMalloc((size_t)(sizeof(SLAVE_VALUE) * MBS_info.value_sl_max));
		if(pSl_value == NULL)
		{
			MBS_info.sens_pres_flag = 0;
			return;
		}

		pSl_info = (SLAVE_INFO *)pvPortMalloc((size_t)(sizeof(SLAVE_INFO) * MBS_info.read_sl_max));
		if(pSl_info == NULL)
		{
			MBS_info.sens_pres_flag = 0;
			return;
		}

		p_temp = p_data_id;
		for(j = 0; j < supportedIDs.idCount; j++)
		{
			p_data_adr = (char *)GetPointToElement(p_temp, "TOut");
			if(p_data_adr == NULL)
				tout = 60;
			else tout = atoi(p_data_adr);

			p_data_adr = (char *)GetPointToElement(p_temp, "Reg");

			if(p_data_adr == NULL) 
			{
				MBS_info.sens_pres_flag = 0;
				return;
			}
					
			p_temp += strlen(p_temp) + 1;

			while(*p_data_adr != 0)
			{
				v_reg_f = atoi(p_data_adr);
				p_data_adr = strpbrk(p_data_adr, ".;");
		
				if(p_data_adr != NULL && *p_data_adr == '.')   //  Если '.' значит указан интервал регистров
				{
					p_data_adr+=2;
					v_reg_l = atoi(p_data_adr);
					if(v_reg_l == 0)
						v_reg_l = 	v_reg_f;
					MBS_info.value_sl_count += v_reg_l - v_reg_f + 1;
					p_data_adr = strpbrk(p_data_adr, ".;");
				}
				else
				{
					MBS_info.value_sl_count++;
					v_reg_l = v_reg_f;
				}

				for(i = (MBS_info.value_sl_count - (v_reg_l - v_reg_f + 1)), k = 0; i < MBS_info.value_sl_count; i++, k++)
				{
					pSl_value[i].adress = supportedIDs.idBuf[j];
					pSl_value[i].reg = v_reg_f++;
					pSl_value[i].flag = 0;
					*((uint32_t *)&pSl_value[i].value) = QNaN;
					pSl_value[i].bad_atmp_cnt = 0;
					pSl_value[i].bad_atmp_lim = (tout);
				}

				v_reg_f -= k;

				pSl_info[MBS_info.read_sl_count].adress = supportedIDs.idBuf[j];
				pSl_info[MBS_info.read_sl_count].reg = v_reg_f;
				pSl_info[MBS_info.read_sl_count].quantity = v_reg_l - v_reg_f + 1;

				MBS_info.read_sl_count++;
				if(p_data_adr == NULL)
					break;
				p_data_adr++;
			}
		}

		MBS_info.pSlave_val = pSl_value;
		MBS_info.pSlave_info = pSl_info;
		MBS_info.value_sl_count = 0;
		MBS_info.read_sl_count = 0;

		MBS_info.sens_pres_flag = 1;
    }
}


void DelMBDev(void)
{
	if(supportedIDs.idBuf != 0)
		{vPortFree(supportedIDs.idBuf); supportedIDs.idBuf = 0;}
	if(supportedIDs.rxCount != 0)
		{vPortFree(supportedIDs.rxCount); supportedIDs.rxCount = 0;}
	if(supportedIDs.txCount != 0)
		{vPortFree(supportedIDs.txCount); supportedIDs.txCount = 0;}
		
	if(MBS_info.pSlave_val != 0)
		{vPortFree(MBS_info.pSlave_val); MBS_info.pSlave_val = 0;}
	if(MBS_info.pSlave_info != 0)
		{vPortFree(MBS_info.pSlave_info); MBS_info.pSlave_info = 0;}

	MBS_info.sens_pres_flag = 0;
	
	MBS_info.read_sl_max = 0;
	MBS_info.value_sl_max = 0;	
	MBS_info.read_sl_count = 0;
	MBS_info.value_sl_count = 0;	
}

#endif




