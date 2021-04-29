#include "Config.h"
#if (_MB_TCP_CLIENT_)
#include <stdint.h>
#include "MB_client.h"
#include "MB_tcp_client.h"
#include "MB_tcp_port.h"


//static UCHAR    ucMBAddress = ;

 static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_DISABLED;


eMBErrorCode eMBTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = (eMBErrorCode)xMBTCPPortInit( ucTCPPort ) ) != MB_ENOERR )
    {
        DEBUG_PUTS("MB_TCP NOT OPEN: TCP init\n");
    }
    else if( !xMBPortEventInit(  ) )
    {
        DEBUG_PUTS("MB_TCP NOT OPEN: RTOS init\n");
    }

    return eStatus;
}


eMBErrorCode eMBEnable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        eMBTCPStart(  );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode eMBDisable( void )
{
    eMBErrorCode    eStatus;

    if( eMBState == STATE_ENABLED )
    {
        eMBTCPStop(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}


eMBErrorCode eMBClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        vMBTCPPortClose();
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}


eMBErrorCode eMBPoll( void )
{
    static UCHAR   *ucMBFrame;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;

    //int             i;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBEventType    eEvent;

    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBPortEventGet( &eEvent ) == TRUE )
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:
            eStatus = eMBTCPReceive( &ucRcvAddress, &ucMBFrame, &usLength );
            if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucRcvAddress == _MD_ID_ ) || ( ucRcvAddress == MB_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost( EV_EXECUTE );
                }
            }
            break;

        case EV_EXECUTE:
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];

			switch(ucFunctionCode)
			{
#if _MB_RD_IN_REG_FUNC_
				case MB_FUNC_READ_INPUT_REGISTER:
					eException = (eMBException)MBCReadInputRegisters((READ_INPUT_REGS *)ucMBFrame, (uint8_t *)&usLength);	
					break;
#endif
#if _MB_WR_S_REG_FUNC_
				case MB_FUNC_WRITE_REGISTER:
					eException = (eMBException)MBCWriteSingleRegister((WRITE_SINGLE_REGISTER*)ucMBFrame, (uint8_t *)&usLength);
					break;
#endif
#if	_MB_WR_FILE_FUNC_
				case MB_FUNC_WRITE_FILE :
					 eException = (eMBException)MBCWriteFile((WRITE_FILE *)ucMBFrame, (uint8_t *)&usLength);
					 break;
#endif
#if	_MB_RD_FILE_FUNC_
			   case MB_FUNC_READ_FILE :
					 eException = (eMBException)MBCReadFile((READ_FILE *)ucMBFrame, (uint8_t *)&usLength);
					 break;
#endif
				default:
					eException = MB_EX_ILLEGAL_FUNCTION;
                        break;
			}

            if( ucRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;
                    ucMBFrame[usLength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
                    ucMBFrame[usLength++] = eException;
                }               
                eStatus = eMBTCPSend( _MD_ID_, ucMBFrame, usLength );
            }
            break;

        case EV_FRAME_SENT:
            break;
        }
    }
    return MB_ENOERR;
}


void MBTask( void *pvParameters )
{
	eMBErrorCode    xStatus;

	 do
	 {
                if( eMBTCPInit( MB_TCP_DEFAULT_PORT ) != MB_ENOERR )
                {
                    DEBUG_PUTS("can't initalize modbus stack!\r\n");
                }
                else if( eMBEnable(  ) != MB_ENOERR )
                {
                    DEBUG_PUTS("can't enable modbus stack!\r\n" );
                }
                else
                {
                    do
                    {

                        /* Application code here. */
                        xStatus = eMBPoll(  );


                    }
                    while( xStatus == MB_ENOERR );

                    ( void )eMBDisable(  );
                    ( void )eMBClose(  );
                }

    }
    while( pdTRUE );
}


#endif

