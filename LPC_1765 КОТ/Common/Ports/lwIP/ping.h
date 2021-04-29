#ifndef __PING_H__
#define __PING_H__

#if(_PING_)
void PingInit(void);
void PingPool(void);
#endif
uint8_t IsPingOk(void);
void ResetPing(void);

#endif /* __PING_H__ */
