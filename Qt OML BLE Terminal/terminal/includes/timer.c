/**
 *  @File: timer.c
 *
 *  *******************************************************************************************
 *
 *  @file      timer.c
 *
 *  @brief     Implements Timer API
 *  *******************************************************************************************
 *
 *  Copyright: Odstock Medical Limited (C) 2024
 *
 *  All rights are reserved. Reproduction or transmission in whole or in part,
 *  in any form or by any means, electronic, mechanical or otherwise, is
 *  prohibited without the prior written consent of the copyright owner.
 *
 *  To obtain written consent please contact the software release authority :
 *
 *  Odstock Medical Ltd. The National Clinical FES Centre, Salisbury District Hospital
 *  Salisbury, Wiltshire SP2 8BJ, Tel +44 (0)1722 439 540
 *
 **/

/**********************************************************************************************
 * Module includes
 **********************************************************************************************/
#include "timer.h"
#include "Windows.h"
#include <sys/timeb.h>

/**********************************************************************************************
 * Module constant defines
 **********************************************************************************************/

/**********************************************************************************************
 * External functions
 **********************************************************************************************/

/**********************************************************************************************
 * Module type definitions
 **********************************************************************************************/

/**********************************************************************************************
 * Module static variables
 **********************************************************************************************/
static uint64_t s_startMs = 0;

/**********************************************************************************************
 * Module static function prototypes
 **********************************************************************************************/

/**********************************************************************************************
 * Module externally exported functions
 **********************************************************************************************/

void TIMER_Init(void)
{
   s_startMs = TIMER_NowMs();
}
/**
 * @brief  Get the time in ms now
 * @param  None
 * @return the time in ms now
 */
uint64_t TIMER_NowMs(void)
{
   struct _timeb timebuffer;
   _ftime(&timebuffer);

   uint64_t nowMs = (uint64_t)(((timebuffer.time * 1000u) + timebuffer.millitm));
   return (nowMs - s_startMs);
}

/**
 * @brief  Simple blocking ms timer
 * @param  ms - the time to delay in ms
 * @return None
 */
void TIMER_DelayMs(uint32_t ms)
{
   Sleep(ms);
}

/**********************************************************************************************
 * Module static functions
 **********************************************************************************************/

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
