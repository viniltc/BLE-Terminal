/**
 *  @File: utils.h
 *
 *  *******************************************************************************************
 *
 *  @file      utils.h
 *
 *  @brief     Defines Utility function API
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

/**********************************************************************************************
 * Module exported types
 **********************************************************************************************/

/**********************************************************************************************
 * Module exported functions
 **********************************************************************************************/
const char *GetBTAddrAsString(const uint8_t addr[BLE_GAP_ADDR_LEN]);
void GetDataAsHex(const void *const data, size_t len, char *const buffer);
void ZeroMemory(void *const memory, size_t size);
bool IsMemory(const void *const memory, uint8_t value, size_t size);
NodeId_t GetNodeIdFromArrayBytes(const uint8_t nodeIdArray[3]);
uint8_t GetArrayByteFromNodeId(uint8_t index, const NodeId_t nodeId);

/**********************************************************************************************
 * Module exported variables
 **********************************************************************************************/

#ifdef __cplusplus
}
#endif

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
