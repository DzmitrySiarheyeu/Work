#include "config.h"
#include "ff.h"




xSemaphoreHandle Sem;

void ff_synk_init(void)
{
	create_sem(Sem);

}


int ff_cre_syncobj (BYTE drv, _SYNC_t *sem)
{
	// *sem = Sem;
	 return 1;
}

int ff_del_syncobj (_SYNC_t sem)
{
	return 1;
}

int ff_req_grant (_SYNC_t sem)
{
	if(take_sem(Sem, portMAX_DELAY) == pdFALSE) return 0;
	return 1;
}

void ff_rel_grant (_SYNC_t sem)
{
	give_sem(Sem);
}

