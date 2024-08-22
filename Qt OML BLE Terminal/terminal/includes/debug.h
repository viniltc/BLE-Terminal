/**
 *  @File: debug.h
 *
 *  *******************************************************************************************
 *
 *  @file      debug.h
 *
 *  @brief     Defines a debug API to aid development
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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
 * Module exported defines
 **********************************************************************************************/
#define DEBUG_LEVEL_INFO  1u
#define DEBUG_LEVEL_WARN  2u
#define DEBUG_LEVEL_ERROR 3u
#define DEBUG_LEVEL_NONE  4u

// Set global debug level here
#define DEBUG_LEVEL_ENABLED DEBUG_LEVEL_INFO

// Enable/Disable the individual module debug here
#define DEBUG_APP        1u
#define DEBUG_ADV        1u
#define DEBUG_APPCEN     1u
#define DEBUG_APPPER     1u
#define DEBUG_CONFIG     1u
#define DEBUG_MAIN       1u
#define DEBUG_MAINCEN    1u
#define DEBUG_MAINPER    1u
#define DEBUG_MCU        1u
#define DEBUG_MCUCMD     1u
#define DEBUG_NET        1u
#define DEBUG_NETCON     1u
#define DEBUG_NETDISC    1u
#define DEBUG_NETMGT     1u
#define DEBUG_NVM        1u
#define DEBUG_NVMFDS     1u
#define DEBUG_OMLSERVICE 1u
#define DEBUG_RESET      1u
#define DEBUG_SCAN       1u
#define DEBUG_UART       1u

#define DEBUG_LEVEL_ERROR 3u

/**********************************************************************************************
 * Module exported types
 **********************************************************************************************/

/**********************************************************************************************
 * Module exported functions
 **********************************************************************************************/
void DBG(const uint8_t level, const char *format, ...);
void DBG_Rsp(int status, const char *format, ...);
void DBG_Evt(const char *format, ...);
void DBG_Hex(const char *desc, const void *addr, const uint16_t len, int perLine);

/**********************************************************************************************
 * Module exported variables
 **********************************************************************************************/

#ifdef __cplusplus
}
#endif

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
