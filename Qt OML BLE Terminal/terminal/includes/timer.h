/**
 *  @File: uart.h
 *
 *  *******************************************************************************************
 *
 *  @file      uart.h
 *
 *  @brief     Defines Timer API
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

#pragma once

/**********************************************************************************************
 * Module includes
 **********************************************************************************************/
#include "..\..\OML BLE App\types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**********************************************************************************************
 * Module exported defines
 **********************************************************************************************/
#define TIME_ELAPSED_8U(oldTime, newTime)  ((uint8_t)((uint8_t)(newTime) - (uint8_t)(oldTime)))
#define TIME_ELAPSED_16U(oldTime, newTime) ((uint16_t)((uint16_t)(newTime) - (uint16_t)(oldTime)))
#define TIME_ELAPSED_32U(oldTime, newTime) ((uint32_t)((uint32_t)(newTime) - (uint32_t)(oldTime)))

/**********************************************************************************************
 * Module exported types
 **********************************************************************************************/
typedef uint32_t MSTimer_t;

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************
 * Module exported functions
 **********************************************************************************************/
void TIMER_Init(void);
uint64_t TIMER_NowMs(void);
void TIMER_DelayMs(uint32_t ms);

/**********************************************************************************************
 * Module exported variables
 **********************************************************************************************/

#ifdef __cplusplus
}
#endif

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
