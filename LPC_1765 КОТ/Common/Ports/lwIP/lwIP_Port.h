#ifndef LWIP_PORT_H
#define LWIP_PORT_H


typedef enum tagePPPThreadControl
{
    CONNECTING, CONNECTED, DISCONNECT
} ePPPThreadControl;

void CreatePPPTask(void);

int ppp_new_con( void );
void vlwIPInit( void );
void     vPPPStatusCB( void *ctx, int errCode, void *arg );

extern ePPPThreadControl ePPPThrCtl;



#endif


