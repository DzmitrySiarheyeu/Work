#include "Config.h"
#include "sio.h"
#include "DriversCore.h"
#include "MODEM_DRV.h"
#include "VFS.h"

extern xSemaphoreHandle ModemSem;

sio_fd_t sio_open(u8_t *name)
{
	return (sio_fd_t)OpenDevice((char *)name);
}

u32_t sio_read(sio_fd_t handle, u8_t *dst, u32_t count, sys_sem_t *sem)
{
	int c = 0;
	if(*sem != 0)
	{
		if( xSemaphoreTake( *sem, 40000) != pdTRUE )
			return 0;	
	}

	c = ReadDevice(handle, dst, count);

	if(*sem != 0) xSemaphoreGive(*sem);
		
	return c;
}

u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len, sys_sem_t *sem)
{
	uint16_t rx_count = 0;
	int count = 0;

	if(*sem != 0)
	{
		if( xSemaphoreTake( *sem, 40000) != pdTRUE )
			return 0;	
	}

	DeviceIOCtl(fd, RX_DATA_COUNT, (void *)&rx_count);
	
	if(rx_count > 0)
	{
		if(rx_count < len) len = rx_count;
		count = ReadDevice(fd, data, len);
	}

	if(*sem != 0) xSemaphoreGive(*sem);
	return count;
}

u32_t sio_write(sio_fd_t handle, u8_t *src, u32_t count, sys_sem_t *sem)
{
	int c = 0;
	// !!! Это надо перенести в модем write
//	if(modem_online() == 0) // пишем только если подключены к GPRS
//		return 0;
	if(*sem != 0)
	{
		if( xSemaphoreTake( *sem, 50000) != pdTRUE )
			return 0;	
	}

	c = WriteDevice(handle, src, count); 

	if(*sem != 0) xSemaphoreGive(*sem);
	return c;
}

void sio_read_abort(sio_fd_t handle, sys_sem_t *sem)
{
	if(modem_online() == 0) // пишем только если подключены к GPRS
		return;
	DeviceIOCtl(handle, FLUSH_DEVICE, NULL);
	//if(*sem != 0) xSemaphoreGive(*sem); //Не понял зачем здесь это Ваня написал
}
void sio_send(u8_t c, sio_fd_t handle, sys_sem_t *sem)
{
	sio_write(handle, &c, 1, sem);
}

u8_t sio_recv(sio_fd_t handle, sys_sem_t *sem)
{
	u8_t ch = 0;
	sio_read(handle, &ch, 1, sem);
  	return ch;
}



