/**
 *  @File: utils.c
 *
 *  *******************************************************************************************
 *
 *  @file      utils.c
 *
 *  @brief     Implements  Utility function API
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
#include "utils.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>

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

/**********************************************************************************************
 * Module static function prototypes
 **********************************************************************************************/

/**********************************************************************************************
 * Module externally exported functions
 **********************************************************************************************/

/**
 * @brief  Get BT address as colon seperated string
 * @param  addr - the bt address
 * @return String containing address
 */
const char *GetBTAddrAsString(const uint8_t addr[BLE_GAP_ADDR_LEN])
{
   static char str[20];
   snprintf(str, sizeof(str), "%02X:%02X:%02X:%02X:%02X:%02X",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
   return str;
}

/**
 * @brief  Utility function to get data as a hex string
 * @param  addr - the bt address
 * @return None
 */
void GetDataAsHex(const void *const data, size_t len, char *const buffer)
{
   buffer[0] = 0;
   char temp[4];

   uint8_t *val = (uint8_t *)data;

   for (size_t count = 0; count < len; count++)
   {
      (void)snprintf(temp, sizeof(temp), "%02X ", *val++);
      (void)strcat(buffer, temp);
   }
}

/**
 * @brief  Set the memory to zero
 * @param  memory - memory address
 * @param  size - number of bytes
 * @return None
 */
void ZeroMemory(void *const memory, size_t size)
{
   (void)memset(memory, 0, size);
}

/**
 * @brief  Determines if the given memory is all set to the value supplied
 * @param  memory - memory address
 * @param  value - the value
 * @param  size - number of bytes
 * @return true if it is, false otherwise
 */
bool IsMemory(const void *const memory, uint8_t value, size_t size)
{
   const uint8_t *ptr = memory;
   for (size_t index = 0; index < size; index++)
   {
      if (*ptr++ != value)
      {
         return false;
      }
   }
   return true;
}

/**
 * @brief  Helper function to get a node Id from the 3 bytes of node ID array
 * @param  nodeIdArray - node Id as array
 * @return the node Id as NodeId_t
 */
NodeId_t GetNodeIdFromArrayBytes(const uint8_t nodeIdArray[3])
{
   NodeId_t nodeId = nodeIdArray[2];
   nodeId <<= 8;
   nodeId |= nodeIdArray[1];
   nodeId <<= 8;
   nodeId |= nodeIdArray[0];
   return nodeId;
}

/**
 * @brief  Helper function to get a byte of node Id as array
 * @param  index - the array index byte
 * @param  nodeId - the node Id
 * @return the byte
 */
uint8_t GetArrayByteFromNodeId(uint8_t index, const NodeId_t nodeId)
{
   uint8_t bitShift = (index << 3); // * 8
   uint8_t ret = ((uint8_t)((nodeId >> bitShift) & 0xFF));
   return ret;
}

/**********************************************************************************************
 * Module static functions
 **********************************************************************************************/

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
