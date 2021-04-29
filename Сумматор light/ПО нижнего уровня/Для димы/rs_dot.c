#include "rs_dot.h"

volatile unsigned short MasterTimer = 0;

inline void MBMasterIncTimer0(void)
{
	MasterTimer++;
}

inline void StartModbusMasterTimer0(void)
{
	MasterTimer = 0;
}

inline unsigned char NextDot(unsigned char dot)
{
    dot ++;
	if (dot >= Reverse2(RegFile.mb_struct.dot_info.N)) dot = 0;
    return dot;
}

unsigned char GetRsDotLevelPercent(unsigned char dotNum, double * value )
{
    static unsigned char CurrentRsDot = 0;
    static unsigned char CurrentDotState = RS_DOT_STATE_TIMEOUT;

	if (CurrentRsDot != dotNum)
	{
		return RS_DOT_STATE_WAITING;
	}

	if (CurrentDotState != RS_DOT_STATE_WAITING)
	{
		if (MasterTimer > T35_Ticks)
		{
           	//sending request
			READ_INPUT_REGS_REQ * p = (READ_INPUT_REGS_REQ *)(DataBuffer0 + 1);
		
			p->function_code = MOD_READ_INPUT_REGISTER;
		
			p->st_adr_hi = 0;
			p->st_adr_lo = 2;
		
			p->quantity_hi = 0;
			p->quantity_lo = 2;
		
			StartModbusMasterTimer0();
			MBSend(dotNum + 1, sizeof(READ_INPUT_REGS_REQ));
			CurrentDotState = RS_DOT_STATE_WAITING;
		}
		else
		{
			return RS_DOT_STATE_WAITING;
		}
	}
	else 
	{
		if (MasterTimer > TimeOut_Ticks)
		{
			// TimeOut
            CurrentDotState = RS_DOT_STATE_TIMEOUT;
			CurrentRsDot = NextDot(CurrentRsDot);

			StartModbusMasterTimer0();
		}
		else
		{
			byte size = MBReceive0(CurrentRsDot + 1);
			if (size > 0)
			{
				READ_INPUT_REGS_RESP * p = (READ_INPUT_REGS_RESP *)(DataBuffer0 + 1);

				*value = *((double*)&(p->InpRegs));
				*value = Reverse4(*value) / 100;

				CurrentDotState = RS_DOT_STATE_READY;
				CurrentRsDot = NextDot(CurrentRsDot);

				StartModbusMasterTimer0();
			}
		}
	}
	
   	return CurrentDotState;
}
