#ifndef USER_EVENTS_H
#define USER_EVENTS_H

#define evKvit_1_i_on	(0 + 100)
#define evKvit_1_s_on	(1 + 100)
#define evKvit_2_i_on	(2 + 100)
#define evKvit_2_s_on	(3 + 100)
#define evKvit_3_i_on	(4 + 100)
#define evKvit_3_s_on	(5 + 100)
#define evKvit_4_i_on	(6 + 100)
#define evKvit_4_s_on	(7 + 100)
#define evKvit_1_i_off	(8 + 100)
#define evKvit_1_s_off	(9 + 100)
#define evKvit_2_i_off	(10 + 100)
#define evKvit_2_s_off	(11 + 100)
#define evKvit_3_i_off	(12 + 100)
#define evKvit_3_s_off	(13 + 100)
#define evKvit_4_i_off	(14 + 100)
#define evKvit_4_s_off	(15 + 100)

#define devFail_1_on	(16 + 100)
#define devFail_2_on	(17 + 100)
#define devFail_3_on	(18 + 100)
#define devFail_4_on	(19 + 100)
#define devFail_1_off	(20 + 100)
#define devFail_2_off	(21 + 100)
#define devFail_3_off	(22 + 100)
#define devFail_4_off	(23 + 100)

#define doorEv_on		(24 + 100)
#define doorEv_off		(25 + 100)

#define     E_STOP				(26 + 100)
#define     E_MOVE				(27 + 100)
#define     E_ROAMING			(28 + 100)
#define     E_HOME				(29 + 100)

void QueueProcEvent(void);
void InitEvents(void);
extern void events_task(void *pvParameters);

void kvit_done(uint8_t page);

#endif

