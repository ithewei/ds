#ifndef __BASICDEF_H__
#define __BASICDEF_H__

//#define BASIC_MTU_PAYLOADSIZE	1056
/*
	BASIC_MTU_PAYLOADSIZE = RUDP_MTU(1344) - 
							FLOODING_MAXHD(16) -
							PLD_MAXHD(8) - 
							Margin(24) 
						  = 1296
 */
#define BASIC_MTU_PAYLOADSIZE	1296

#endif
