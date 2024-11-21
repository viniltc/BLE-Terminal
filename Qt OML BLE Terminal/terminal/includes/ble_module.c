/**
 *  @File: ble_module.c
 *
 *  *******************************************************************************************
 *
 *  @file      ble_module.c
 *
 *  @brief     Implements the BLE Module API
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
#include "ble_module.h"
#include "..\..\OML BLE App\mcu_cmds.h"
#include "..\..\OML BLE App\utils.h"
#include "crc8.h"
#include "debug.h"
#include "serial.h"
#include "timer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "debug_signals_wrapper.h"
//#include <QDebug>

/**********************************************************************************************
 * Module constant defines
 **********************************************************************************************/

/**********************************************************************************************
 * External functions
 **********************************************************************************************/
//#define DBG_Evt(...) \
//    do { \
//        char buffer[256]; \
//        snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
//        emitDebugEvent(buffer); \
//    } while(0)

//#define DBG_Rsp(status, ...) \
//    do { \
//        char buffer[256]; \
//        snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
//        emitDebugResponse(buffer); \
//    } while(0)


#define DBG_Evt(...) \
    do { \
        char buffer[256]; \
        int ret = snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
        if (ret >= 0 && ret < sizeof(buffer)) { \
            emitDebugEvent(buffer); \
        } else { \
            emitDebugEvent("Buffer overflow in DBG_Evt"); \
        } \
    } while(0)
#define DBG(...) \
    do { \
        char buffer[256]; \
        int ret = snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
        if (ret >= 0 && ret < sizeof(buffer)) { \
            emitDebugMain(buffer); \
        } else { \
            emitDebugMain("Buffer overflow in DBG_Evt"); \
        } \
    } while(0)


#define DBG_Rsp(status, ...) \
    do { \
        char buffer[256]; \
        int ret = snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
        if (ret >= 0 && ret < sizeof(buffer)) { \
            emitDebugResponse(buffer); \
        } else { \
            emitDebugResponse("Buffer overflow in DBG_Rsp"); \
        } \
    } while(0)
#define DBG_Hex(status, ...) \
    do { \
        char buffer[256]; \
        int ret = snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
        if (ret >= 0 && ret < sizeof(buffer)) { \
            emitDebugHex(buffer); \
        } else { \
            emitDebugHex("Buffer overflow in DBG_Rsp"); \
        } \
    } while(0)
/**********************************************************************************************
 * Module type definitions
 **********************************************************************************************/

#pragma pack(push, 1)

typedef struct
{
   uint8_t frameHdr1;
   uint8_t frameHdr2;
   uint8_t payloadLen;
   // payload data[]
   // crc

} MCUProtocolHeader_t;

typedef enum
{
   eWAITING_FOR_HEADER1 = 0,
   eWAITING_FOR_HEADER2,
   eWAITING_FOR_LENGTH,
   eWAITING_FOR_DATA,
} MCURXState_e;

#pragma pack(pop)

/**********************************************************************************************
 * Module static variables
 **********************************************************************************************/
static MCURXState_e s_rxState = eWAITING_FOR_HEADER1;
static uint8_t s_rxFrame[MCU_PROTOCOL_FRAME_SIZE_MAX] = {0};

/**********************************************************************************************
 * Module static function prototypes
 **********************************************************************************************/
static void GetDataAsHex(const void *const data, size_t len, char *const buffer);

/**********************************************************************************************
 * Module externally exported functions
 **********************************************************************************************/

/**
 * @brief  Reset the state machine
 * @param  None
 * @return None
 */
void BLEModule_Init(void)
{
   s_rxState = eWAITING_FOR_HEADER1;
}

/**
 * @brief  Parse and validate a recieved MCU Frame
 * @param  ch - byte to process
 * @param  s_rxFrame - address of storage for rx frame
 * @return None
 */
void BLEModule_OnRx(const uint8_t ch)
{
   static uint8_t lengthField = 0;
   static uint8_t rxCount = 0;
   static uint8_t rxRem = 0;

   switch (s_rxState)
   {
      case eWAITING_FOR_HEADER1: {
         if (MCU_PROTOCOL_FRAME_HEADER1 == ch)
         {
            rxCount = 0;
            s_rxFrame[rxCount++] = ch;
            s_rxState = eWAITING_FOR_HEADER2;
         }
         break;
      }
      case eWAITING_FOR_HEADER2: {
         if (MCU_PROTOCOL_FRAME_HEADER2 == ch)
         {
            s_rxFrame[rxCount++] = ch;
            s_rxState = eWAITING_FOR_LENGTH;
         }
         else
         {
            s_rxState = eWAITING_FOR_HEADER1;
            DBG(DEBUG_LEVEL_ERROR, "%s() bad header\n", __func__);
         }
         break;
      }

      case eWAITING_FOR_LENGTH: {
         if ((ch >= MCU_PROTOCOL_LENGTH_FIELD_MIN) &&
             (ch <= MCU_PROTOCOL_LENGTH_FIELD_MAX))
         {
            s_rxFrame[rxCount++] = ch;
            lengthField = ch;
            rxRem = ch;
            s_rxState = eWAITING_FOR_DATA;
         }
         else
         {
            // bad length
            s_rxState = eWAITING_FOR_HEADER1;
            DBG(DEBUG_LEVEL_ERROR, "%s() bad length\n", __func__);
         }
         break;
      }

      case eWAITING_FOR_DATA: {
         s_rxFrame[rxCount++] = ch;
         rxRem--;

         // check if we have received all the data
         if (0 == rxRem)
         {
            uint8_t suppliedCS = ch;
            uint8_t calcCS = crc8ccitt_block(0, s_rxFrame, lengthField + 2u); // 2 bytes being 2 header bytes and length byte less CRC byte
            if (suppliedCS == calcCS)
            {
               // We have received a valid frame from MCU, extract command and call handler
               BLEModule_Handler(&s_rxFrame[3], s_rxFrame[2] - 1u);
            }
            else
            {
               // bad checksum
               DBG(DEBUG_LEVEL_ERROR, "%s() bad crc\n", __func__);
            }
            s_rxState = eWAITING_FOR_HEADER1;
         }
         else
         { // carry on receiving
         }
         break;
      }

      default: {
         s_rxState = eWAITING_FOR_HEADER1;
         break;
      }
   }
}

/**
 * @brief  Transmit a payload to OMLBLE module in protocol frame format
 * @param  payload - payload data
 * @param  payloadLen - number of payload bytes
 * @return STATUS_SUCCESS if payload transmitted, Status_t otherwise, false otherwise
 */
void BLEModule_Tx(const void *payload, size_t payloadLen)
{
   bool ret = false;

   if (payloadLen <= MCU_PROTOCOL_PAYLOAD_MAX)
   {
      MCUProtocolHeader_t header;
      header.frameHdr1 = MCU_PROTOCOL_FRAME_HEADER1;
      header.frameHdr2 = MCU_PROTOCOL_FRAME_HEADER2;
      header.payloadLen = (uint8_t)(payloadLen + 1u); // add 1 for CRC

      // transmit header
      SerialWriteBytes(&header, sizeof(header));

      // transmit payload
      SerialWriteBytes(payload, payloadLen);

      // calc and transmit CRC
      uint8_t crc = crc8ccitt_block(0, &header, sizeof(header));
      crc = crc8ccitt_block(crc, payload, payloadLen);
      SerialWriteBytes(&crc, 1u);
   }
   else
   {
      DBG(DEBUG_LEVEL_ERROR, "%s() error. bad payload size:%d\n", __func__, (int)payloadLen);
   }
}

/**
 * @brief  Called on receipt of a payload from OML BLE in a valid packet
 * @param  buf - payload data
 * @param  bufLen - number of payload bytes
 * @return None
 */
void BLEModule_Handler(const uint8_t *buf, size_t bufLen)
{
   if (buf[0] & MCU_RSP_MASK)
   {
      BLEModule_RspHandler(buf, bufLen);
   }
   else
   {
      BLEModule_EvtHandler(buf, bufLen);
   }
}

/**
 * @brief  Called on receipt of a command response from the OM BLE module
 * @param  rspBuf - response payload data
 * @param  rspBufLen - size of rspBuf in bytes
 * @return None
 */
void BLEModule_RspHandler(const uint8_t *rspBuf, size_t rspBufLen)
{
   assert(0 != (rspBuf[0] & MCU_RSP_MASK));

   switch (rspBuf[0])
   {
      case MCU_RSP_NOP: {
         const MCU_RSP_NOP_t *rsp = (MCU_RSP_NOP_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_NOP. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_ON_MCU_RESET: {
         const MCU_RSP_ON_MCU_RESET_t *rsp = (MCU_RSP_ON_MCU_RESET_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_ON_MCU_RESET. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_ON_MCU_BOOTLOADER: {
         const MCU_RSP_ON_MCU_BOOTLOADER_t *rsp = (MCU_RSP_ON_MCU_BOOTLOADER_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_ON_MCU_BOOTLOADER. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_ON_MCU_SLEEP: {
         const MCU_RSP_ON_MCU_SLEEP_t *rsp = (MCU_RSP_ON_MCU_SLEEP_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_ON_MCU_SLEEP. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_BLE_REBOOT: {
         const MCU_RSP_BLE_REBOOT_t *rsp = (MCU_RSP_BLE_REBOOT_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_BLE_REBOOT. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_BLE_POWEROFF: {
         const MCU_RSP_BLE_POWEROFF_t *rsp = (MCU_RSP_BLE_POWEROFF_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_BLE_POWEROFF. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_BLE_UARTOFF: {
         const MCU_RSP_BLE_UARTOFF_t *rsp = (MCU_RSP_BLE_UARTOFF_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_BLE_UARTOFF. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_BLE_FACTORY_RESET: {
         const MCU_RSP_BLE_FACTORY_RESET_t *rsp = (MCU_RSP_BLE_FACTORY_RESET_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_BLE_FACTORY_RESET. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_BLE_DFU_MODE: {
         const MCU_RSP_BLE_DFU_MODE_t *rsp = (MCU_RSP_BLE_DFU_MODE_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_BLE_DFU_MODE. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_FW_VERSION: {
         const MCU_RSP_GET_FW_VERSION_t *rsp = (MCU_RSP_GET_FW_VERSION_t *)rspBuf;

         char hashStr[80];
         GetDataAsHex(rsp->hash, sizeof(rsp->hash), hashStr);

         char shaStr[20];
         (void)memcpy(shaStr, rsp->sha, sizeof(rsp->sha));
         shaStr[sizeof(rsp->sha)] = '\0';

         if (rsp->shaDirty)
         {
            (void)strcat(shaStr, "+");
         }

         DBG_Rsp(rsp->status, "MCU_RSP_GET_FW_VERSION.\nVersion:%d.%d\nHash:%s\nSHA:%s\nStatus:x%X (%s)\n", rsp->fwMajor, rsp->fwMinor, hashStr, shaStr, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_AUTH_KEY: {
         const MCU_RSP_SET_AUTH_KEY_t *rsp = (MCU_RSP_SET_AUTH_KEY_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_AUTH_KEY. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_TX_POWER: {
         const MCU_RSP_SET_TX_POWER_t *rsp = (MCU_RSP_SET_TX_POWER_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_TX_POWER. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_NODE_ROLE: {
         const MCU_RSP_SET_NODE_ROLE_t *rsp = (MCU_RSP_SET_NODE_ROLE_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_NODE_ROLE. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_NODE_ROLE: {
         const MCU_RSP_GET_NODE_ROLE_t *rsp = (MCU_RSP_GET_NODE_ROLE_t *)rspBuf;
         NodeRole_t nodeRole = (NodeRole_t)rsp->nodeRole;
         DBG_Rsp(rsp->status, "MCU_RSP_GET_NODE_ROLE. NodeRole:%d (%s), Status:x%X (%s)\n", nodeRole, BLEModule_GetNodeRole(nodeRole), rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_NODE_ID: {
         const MCU_RSP_SET_NODE_ID_t *rsp = (MCU_RSP_SET_NODE_ID_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_NODE_ID. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_NODE_ID: {
         const MCU_RSP_GET_NODE_ID_t *rsp = (MCU_RSP_GET_NODE_ID_t *)rspBuf;
         NodeId_t nodeId = (NodeId_t)GetNodeIdFromArrayBytes(rsp->nodeId);
         DBG_Rsp(rsp->status, "MCU_RSP_GET_NODE_ID. NodeId:%d, Status:x%X (%s)\n", nodeId, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_NODE_TYPE: {
         const MCU_RSP_SET_NODE_TYPE_t *rsp = (MCU_RSP_SET_NODE_TYPE_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_NODE_TYPE. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_NODE_TYPE: {
         const MCU_RSP_GET_NODE_TYPE_t *rsp = (MCU_RSP_GET_NODE_TYPE_t *)rspBuf;
         NodeType_t nodeType = (NodeType_t)rsp->nodeType;
         DBG_Rsp(rsp->status, "MCU_RSP_GET_NODE_TYPE. NodeType:%d (%s), Status:x%X (%s)\n", nodeType, BLEModule_GetNodeType(nodeType), rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_CONNECTION_PARAMS: {
         const MCU_RSP_SET_CONNECTION_PARAMS_t *rsp = (MCU_RSP_SET_CONNECTION_PARAMS_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_CONNECTION_PARAMS. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      };
      case MCU_RSP_SET_GAP_EVENT_LENGTH: {
         const MCU_RSP_SET_GAP_EVENT_LENGTH_t *rsp = (MCU_RSP_SET_GAP_EVENT_LENGTH_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_GAP_EVENT_LENGTH. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_GAP_EVENT_LENGTH: {
         const MCU_RSP_GET_GAP_EVENT_LENGTH_t *rsp = (MCU_RSP_GET_GAP_EVENT_LENGTH_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_GET_GAP_EVENT_LENGTH. Units:%d. Status:x%X (%s)\n", rsp->units, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_SCAN_PARAMS: {
         const MCU_RSP_SET_SCAN_PARAMS_t *rsp = (MCU_RSP_SET_SCAN_PARAMS_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_SCAN_PARAMS. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_SCAN_PARAMS: {
         const MCU_RSP_GET_SCAN_PARAMS_t *rsp = (MCU_RSP_GET_SCAN_PARAMS_t *)rspBuf;

         uint16_t timeout = rsp->timeout[1];
         timeout <<= 8;
         timeout |= rsp->timeout[0];

         uint16_t window = rsp->window[1];
         window <<= 8;
         window |= rsp->window[0];

         uint16_t interval = rsp->interval[1];
         interval <<= 8;
         interval |= rsp->interval[0];

         DBG_Rsp(rsp->status, "MCU_RSP_GET_SCAN_PARAMS. Timeout:%d, Window:%d, Interval:%d, Status:x%X (%s)\n",
                 timeout, window, interval, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SCAN: {
         const MCU_RSP_SCAN_t *rsp = (MCU_RSP_SCAN_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SCAN. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_ADV_PARAMS: {
         const MCU_RSP_SET_ADV_PARAMS_t *rsp = (MCU_RSP_SET_ADV_PARAMS_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_ADV_PARAMS. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_ADV_PARAMS: {
         const MCU_RSP_GET_ADV_PARAMS_t *rsp = (MCU_RSP_GET_ADV_PARAMS_t *)rspBuf;

         uint32_t interval = rsp->interval[3];
         interval <<= 8;
         interval |= rsp->interval[2];
         interval <<= 8;
         interval |= rsp->interval[1];
         interval <<= 8;
         interval |= rsp->interval[0];

         uint16_t duration = rsp->duration[1];
         duration <<= 8;
         duration |= rsp->duration[0];

         DBG_Rsp(rsp->status, "MCU_RSP_GET_ADV_PARAMS. Interval:%d, Duration:%d, Status:x%X (%s)\n",
                 interval, duration, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_ADVERTISE: {
         const MCU_RSP_ADVERTISE_t *rsp = (MCU_RSP_ADVERTISE_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_ADVERTISE. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_ADVERT_DATA: {
         const MCU_RSP_SET_ADVERT_DATA_t *rsp = (MCU_RSP_SET_ADVERT_DATA_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_ADVERT_DATA. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_ADVERT_DATA: {
         const MCU_RSP_GET_ADVERT_DATA_t *rsp = (MCU_RSP_GET_ADVERT_DATA_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_GET_ADVERT_DATA. Data:[0]%d [1]%d [2]%d, Status:x%X (%s)\n", rsp->advData[0], rsp->advData[1], rsp->advData[2], rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SAVE_CONFIG: {
         const MCU_RSP_SAVE_CONFIG_t *rsp = (MCU_RSP_SAVE_CONFIG_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SAVE_CONFIG. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_CONNECT: {
         const MCU_RSP_CONNECT_t *rsp = (MCU_RSP_CONNECT_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_CONNECT. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_DISCONNECT: {
         const MCU_RSP_DISCONNECT_t *rsp = (MCU_RSP_DISCONNECT_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_DISCONNECT. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_PAIR: {
         const MCU_RSP_PAIR_t *rsp = (MCU_RSP_PAIR_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_PAIR. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_UNPAIR: {
         const MCU_RSP_UNPAIR_t *rsp = (MCU_RSP_UNPAIR_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_UNPAIR. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_UNPAIR_ALL: {
         const MCU_RSP_UNPAIR_ALL_t *rsp = (MCU_RSP_UNPAIR_ALL_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_UNPAIR_ALL. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_PAIR_ENTRY_COUNT: {
         const MCU_RSP_GET_PAIR_ENTRY_COUNT_t *rsp = (MCU_RSP_GET_PAIR_ENTRY_COUNT_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_GET_PAIR_ENTRY_COUNT. Count:%d, Status:x%X (%s)\n", rsp->count, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_PAIR_ENTRY: {
         const MCU_RSP_GET_PAIR_ENTRY_t *rsp = (MCU_RSP_GET_PAIR_ENTRY_t *)rspBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(rsp->nodeId);
         DBG_Rsp(rsp->status, "MCU_RSP_GET_PAIR_ENTRY. Index:%d, NodeType:%d (%s), NodeId:%d, Status:x%X (%s)\n", rsp->index, rsp->nodeType, BLEModule_GetNodeType(rsp->nodeType), nodeId, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_CONNECTION_COUNT: {
         const MCU_RSP_GET_CONNECTION_COUNT_t *rsp = (MCU_RSP_GET_CONNECTION_COUNT_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_GET_CONNECTION_COUNT. Count:%d, Status:x%X (%s)\n", rsp->count, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_CONNECTION: {
         const MCU_RSP_GET_CONNECTION_t *rsp = (MCU_RSP_GET_CONNECTION_t *)rspBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(rsp->nodeId);
         DBG_Rsp(rsp->status, "MCU_RSP_GET_CONNECTION. Index:%d, NodeId:%d, Status:x%X (%s)\n", rsp->index, nodeId, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_SET_ADVERT_RSSI_THRESHOLD: {
         const MCU_RSP_SET_ADVERT_RSSI_THRESHOLD_t *rsp = (MCU_RSP_SET_ADVERT_RSSI_THRESHOLD_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_SET_ADVERT_RSSI_THRESHOLD. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_GET_ADVERT_RSSI_THRESHOLD: {
         const MCU_RSP_GET_ADVERT_RSSI_THRESHOLD_t *rsp = (MCU_RSP_GET_ADVERT_RSSI_THRESHOLD_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_GET_ADVERT_RSSI_THRESHOLD. RSSI Threshold:%d, Status:x%X (%s)\n", rsp->rssiThreshold, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_RADIO_TEST_DTM: {
         const MCU_RSP_RADIO_TEST_DTM_t *rsp = (MCU_RSP_RADIO_TEST_DTM_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_RADIO_TEST_DTM. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_RADIO_TEST_MOD_CARRIER: {
         const MCU_RSP_RADIO_TEST_MOD_CARRIER_t *rsp = (MCU_RSP_RADIO_TEST_MOD_CARRIER_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_RADIO_TEST_MOD_CARRIER. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_TX_PAYLOAD: {
         const MCU_RSP_TX_PAYLOAD_t *rsp = (MCU_RSP_TX_PAYLOAD_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_TX_PAYLOAD. TxSeqNum:%d, Status:x%X (%s)\n", rsp->txSeqNum, rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_REMOTE_MCU_PING_REQUEST: {
         const MCU_RSP_REMOTE_MCU_PING_REQUEST_t *rsp = (MCU_RSP_REMOTE_MCU_PING_REQUEST_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_REMOTE_MCU_PING_REQUEST. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_REMOTE_MCU_PING_REPLY: {
         const MCU_RSP_REMOTE_MCU_PING_REPLY_t *rsp = (MCU_RSP_REMOTE_MCU_PING_REPLY_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_REMOTE_MCU_PING_REPLY. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_REMOTE_MCU_RESET_REQUEST: {
         const MCU_RSP_REMOTE_MCU_RESET_REQUEST_t *rsp = (MCU_RSP_REMOTE_MCU_RESET_REQUEST_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_REMOTE_MCU_RESET_REQUEST. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_REMOTE_MCU_BOOTLOADER_REQUEST: {
         const MCU_RSP_REMOTE_MCU_BOOTLOADER_REQUEST_t *rsp = (MCU_RSP_REMOTE_MCU_BOOTLOADER_REQUEST_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_REMOTE_MCU_BOOTLOADER_REQUEST. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_REMOTE_MCU_RESET_NOW: {
         const MCU_RSP_REMOTE_MCU_RESET_NOW_t *rsp = (MCU_RSP_REMOTE_MCU_RESET_NOW_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_REMOTE_MCU_RESET_NOW. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_REMOTE_BLE_DFU_MODE: {
         const MCU_RSP_REMOTE_BLE_DFU_MODE_t *rsp = (MCU_RSP_REMOTE_BLE_DFU_MODE_t *)rspBuf;
         DBG_Rsp(rsp->status, "MCU_RSP_REMOTE_BLE_DFU_MODE. Status:x%X (%s)\n", rsp->status, BLEModule_GetStatusString(rsp->status));
         break;
      }
      case MCU_RSP_UNKNOWN_COMMAND: {
         DBG_Rsp(STATUS_MCU_UNKNOWN_COMMAND, "MCU_RSP_UNKNOWN_COMMAND. Status:x%X (%s)\n", STATUS_MCU_UNKNOWN_COMMAND, BLEModule_GetStatusString(STATUS_MCU_UNKNOWN_COMMAND));
         break;
      }
      default: {
         DBG(DEBUG_LEVEL_ERROR, "%s() Error. Unknown response: x%02X\n", __func__, rspBuf[0]);
         break;
      }
   }
}

/**
 * @brief  Called on receipt of an event from the OM BLE module
 * @param  evtBuf - event payload data
 * @param  evtBufLen - size of evtBuf in bytes
 * @return None
 */
void BLEModule_EvtHandler(const uint8_t *evtBuf, size_t evtBufLen)
{
   static NodeId_t lastNodeId = 0;
   static uint8_t lastEvt = 0;

   assert(0 == (evtBuf[0] & MCU_RSP_MASK));

   switch (evtBuf[0])
   {
      case MCU_EVT_BLE_REBOOT: {
         const MCU_EVT_BLE_REBOOT_t *evt = (MCU_EVT_BLE_REBOOT_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);

         DBG_Evt("MCU_EVT_BLE_REBOOT. NodeRole:%d (%s), NodeType:%d (%s), NodeId:%d, PairedCount:%d, Version:%d.%d\n",
                 evt->nodeRole,
                 BLEModule_GetNodeRole(evt->nodeRole),
                 evt->nodeType,
                 BLEModule_GetNodeType(evt->nodeType),
                 nodeId,
                 evt->pairedCount,
                 evt->fwMajor,
                 evt->fwMinor);
         break;
      }
      case MCU_EVT_BLE_POWEROFF: {
         const MCU_EVT_BLE_POWEROFF_t *evt = (MCU_EVT_BLE_POWEROFF_t *)evtBuf;
         DBG_Evt("MCU_EVT_BLE_POWEROFF -----\n");
         break;
      }
      case MCU_EVT_MCU_RESET_REQUESTED: {
         const MCU_EVT_MCU_RESET_REQUESTED_t *evt = (MCU_EVT_MCU_RESET_REQUESTED_t *)evtBuf;
         (void)evt;
         DBG_Evt("MCU_EVT_MCU_RESET_REQUESTED\n");
         break;
      }
      case MCU_EVT_MCU_BOOTLOADER_REQUESTED: {
         const MCU_EVT_MCU_BOOTLOADER_REQUESTED_t *evt = (MCU_EVT_MCU_BOOTLOADER_REQUESTED_t *)evtBuf;
         (void)evt;
         DBG_Evt("MCU_EVT_MCU_BOOTLOADER_REQUESTED\n");
         break;
      }
      case MCU_EVT_NODE_FOUND: {
         const MCU_EVT_NODE_FOUND_t *evt = (MCU_EVT_NODE_FOUND_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
#if 0 
         //if ((lastEvt != MCU_EVT_NODE_FOUND) || (nodeId != lastNodeId))
#endif
         {
            DBG_Evt("MCU_EVT_NODE_FOUND. NodeType:%d (%s), NodeId:%d, PairedNodeId:%d, AdvData:%02x%02x%02x, RSSI:%ddBm, V%d.%d\n",
                    evt->nodeType,
                    BLEModule_GetNodeType(evt->nodeType),
                    nodeId,
                    GetNodeIdFromArrayBytes(evt->pairedNodeId),
                    evt->advData[0], evt->advData[1], evt->advData[2],
                    (int8_t)evt->rssi,
                    evt->fwVersionMajor, evt->fwVersionMinor);
         }
         lastNodeId = nodeId;
         break;
      }
      case MCU_EVT_NODE_PAIRED: {
         const MCU_EVT_NODE_PAIRED_t *evt = (MCU_EVT_NODE_PAIRED_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_NODE_PAIRED. NodeType:%d (%s), NodeId:%d\n", evt->nodeType, BLEModule_GetNodeType(evt->nodeType), nodeId);
         break;
      }
      case MCU_EVT_NODE_PAIR_FAILED: {
         const MCU_EVT_NODE_PAIR_FAILED_t *evt = (MCU_EVT_NODE_PAIR_FAILED_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_NODE_PAIR_FAILED. NodeType:%d (%s), NodeId:%d, Status:x%X (%s)\n", evt->nodeType, BLEModule_GetNodeType(evt->nodeType), nodeId, evt->status, BLEModule_GetStatusString(evt->status));
         break;
      }
      case MCU_EVT_NODE_UNPAIRED: {
         const MCU_EVT_NODE_UNPAIRED_t *evt = (MCU_EVT_NODE_UNPAIRED_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_NODE_UNPAIRED. NodeType:%d (%s), NodeId:%d\n", evt->nodeType, BLEModule_GetNodeType(evt->nodeType), nodeId);
         break;
      }
      case MCU_EVT_NODE_CONNECTED: {
         const MCU_EVT_NODE_CONNECTED_t *evt = (MCU_EVT_NODE_CONNECTED_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         uint16_t minConnIntvl = evt->minConnIntvl[1];
         minConnIntvl <<= 8;
         minConnIntvl |= evt->minConnIntvl[0];

         uint16_t maxConnIntvl = evt->maxConnIntvl[1];
         maxConnIntvl <<= 8;
         maxConnIntvl |= evt->maxConnIntvl[0];

         uint16_t slaveLatency = evt->slaveLatency[1];
         slaveLatency <<= 8;
         slaveLatency |= evt->slaveLatency[0];

         uint16_t supTimeout = evt->supTimeout[1];
         supTimeout <<= 8;
         supTimeout |= evt->supTimeout[0];

         DBG_Evt("MCU_EVT_NODE_CONNECTED. NodeId:%d, Min:%d, Max:%d, Lat:%d, supTimeout:%d\n",
                 nodeId, minConnIntvl, maxConnIntvl, slaveLatency, supTimeout);
         break;
      }
      case MCU_EVT_NODE_DISCONNECTED: {
         const MCU_EVT_NODE_DISCONNECTED_t *evt = (MCU_EVT_NODE_DISCONNECTED_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_NODE_DISCONNECTED. NodeId:%d, Reason:x%02x (%s)\n", nodeId, evt->reason, BLEModule_GetDisconnectReason(evt->reason));
         break;
      }
      case MCU_EVT_NODE_CONNECT_TIMEOUT: {
         const MCU_EVT_NODE_CONNECT_TIMEOUT_t *evt = (MCU_EVT_NODE_CONNECT_TIMEOUT_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_NODE_CONNECT_TIMEOUT. NodeId:%d\n", nodeId);
         break;
      }
      case MCU_EVT_NODE_CONNECT_AUTH_ERROR: {
         const MCU_EVT_NODE_CONNECT_AUTH_ERROR_t *evt = (MCU_EVT_NODE_CONNECT_AUTH_ERROR_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_NODE_CONNECT_AUTH_ERROR. NodeId:%d\n", nodeId);
         break;
      }
      case MCU_EVT_RX_PAYLOAD: {
         MCU_EVT_RX_PAYLOAD_t *evt = (MCU_EVT_RX_PAYLOAD_t *)evtBuf;
         NodeId_t srcNodeId = GetNodeIdFromArrayBytes(evt->srcNodeId);
         DBG_Evt("MCU_EVT_RX_PAYLOAD. SrcNodeId:%d, Len:%d, RSSI:%d\n", srcNodeId, evt->payloadLen, (int8_t)evt->rssi);
         DBG_Hex("payload", &evt->payloadLen + 1u, evt->payloadLen, 16u);
         break;
      }
      case MCU_EVT_RX_ACK: {
         MCU_EVT_RX_ACK_t *evt = (MCU_EVT_RX_ACK_t *)evtBuf;
         NodeId_t srcNodeId = GetNodeIdFromArrayBytes(evt->srcNodeId);
         DBG_Evt("MCU_EVT_RX_ACK. From SrcNodeId:%d, TxSeqNum:%d\n", srcNodeId, evt->txSeqNum);
         break;
      }
      case MCU_EVT_PING_REQUEST: {
         MCU_EVT_PING_REQUEST_t *evt = (MCU_EVT_PING_REQUEST_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_PING_REQUEST. NodeId:%d\n", nodeId);
         break;
      }
      case MCU_EVT_PING_REPLY: {
         MCU_EVT_PING_REPLY_t *evt = (MCU_EVT_PING_REPLY_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_PING_REPLY. NodeId:%d\n", nodeId);
         break;
      }
      case MCU_EVT_REMOTE_MCU_RESET_REQUEST: {
         MCU_EVT_REMOTE_MCU_RESET_REQUEST_t *evt = (MCU_EVT_REMOTE_MCU_RESET_REQUEST_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_REMOTE_MCU_RESET_REQUEST. NodeId:%d, Status:x%X (%s)\n", nodeId, evt->status, BLEModule_GetStatusString(evt->status));
         break;
      }
      case MCU_EVT_REMOTE_MCU_BOOTLOADER_REQUEST: {
         MCU_EVT_REMOTE_MCU_BOOTLOADER_REQUEST_t *evt = (MCU_EVT_REMOTE_MCU_BOOTLOADER_REQUEST_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_REMOTE_MCU_BOOTLOADER_REQUEST.  NodeId:%d, Status:x%X (%s)\n", nodeId, evt->status, BLEModule_GetStatusString(evt->status));
         break;
      }
      case MCU_EVT_REMOTE_MCU_RESET_NOW: {
         MCU_EVT_REMOTE_MCU_RESET_NOW_t *evt = (MCU_EVT_REMOTE_MCU_RESET_NOW_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_REMOTE_MCU_RESET_NOW. NodeId:%d, Status:x%X (%s)\n", nodeId, evt->status, BLEModule_GetStatusString(evt->status));
         break;
      }
      case MCU_EVT_REMOTE_BLE_DFU_MODE: {
         MCU_EVT_REMOTE_BLE_DFU_MODE_t *evt = (MCU_EVT_REMOTE_BLE_DFU_MODE_t *)evtBuf;
         NodeId_t nodeId = GetNodeIdFromArrayBytes(evt->nodeId);
         DBG_Evt("MCU_EVT_REMOTE_BLE_DFU_MODE. NodeId:%d, Status:x%X (%s)\n", nodeId, evt->status, BLEModule_GetStatusString(evt->status));
         break;
      }
      case MCU_EVT_BUTTON: {
         MCU_EVT_BUTTON_t *evt = (MCU_EVT_BUTTON_t *)evtBuf;
         DBG_Evt("MCU_EVT_BUTTON.  Button:%d Action:%s\n", evt->buttonNum, evt->pressed ? "pressed" : "released");
         break;
      }
      case MCU_EVT_CONN_PARAMS_UPDATE: {
         MCU_EVT_CONN_PARAMS_UPDATE_t *evt = (MCU_EVT_CONN_PARAMS_UPDATE_t *)evtBuf;
         uint16_t minConnIntvl = evt->minConnIntvl[1];
         minConnIntvl <<= 8;
         minConnIntvl |= evt->minConnIntvl[0];

         uint16_t maxConnIntvl = evt->maxConnIntvl[1];
         maxConnIntvl <<= 8;
         maxConnIntvl |= evt->maxConnIntvl[0];

         uint16_t slaveLatency = evt->slaveLatency[1];
         slaveLatency <<= 8;
         slaveLatency |= evt->slaveLatency[0];

         uint16_t supTimeout = evt->supTimeout[1];
         supTimeout <<= 8;
         supTimeout |= evt->supTimeout[0];

         DBG_Evt("MCU_EVT_CONN_PARAMS_UPDATE. Min:%d, Max:%d, Lat:%d, supTimeout:%d\n",
                 minConnIntvl, maxConnIntvl, slaveLatency, supTimeout);
         break;
      }
      case MCU_EVT_SAVE_CONFIG: {
         MCU_EVT_SAVE_CONFIG_t *evt = (MCU_EVT_SAVE_CONFIG_t *)evtBuf;
         DBG_Evt("MCU_EVT_SAVE_CONFIG.  Status:x%X (%s)\n", evt->status, BLEModule_GetStatusString(evt->status));
         break;
      }
      default: {
         DBG_Evt("%s() Error. Unknown event: x%02X\n", __func__, evtBuf[0]);
         break;
      }
   }

   lastEvt = evtBuf[0];
}

/**
 * @brief  Call to get the description of the node type
 * @param  nodeType - the node type
 * @return String with description of node type if known
 */
const char *BLEModule_GetNodeType(NodeType_t nodeType)
{
   const char *ret;
   switch (nodeType)
   {
      case CONFIG_NODE_TYPE_NONE:
         ret = "None";
         break;

      case CONFIG_NODE_TYPE_STIMULATOR_PRIMARY:
         ret = "Stim1";
         break;

      case CONFIG_NODE_TYPE_STIMULATOR_SECONDARY:
         ret = "Stim2";
         break;

      case CONFIG_NODE_TYPE_FOOTSWITCH:
         ret = "Foot";
         break;

      case CONFIG_NODE_TYPE_PC_DONGLE:
         ret = "Dongle";
         break;

      case CONFIG_NODE_TYPE_SMARTPHONE:
         ret = "Phone";
         break;

      case CONFIG_NODE_TYPE_MCU_UPGRADE_TARGET:
         ret = "MCU Upgrade Mode";
         break;

      default:
         ret = "Unknown";
         break;
   }

   return ret;
}

/**
 * @brief  Call to get the description of the node role
 * @param  nodeRole - the node role
 * @return String with description of node role if known
 */
const char *BLEModule_GetNodeRole(NodeRole_t nodeRole)
{
   const char *ret;
   switch (nodeRole)
   {
      case CONFIG_ROLE_CENTRAL:
         ret = "Central";
         break;

      case CONFIG_ROLE_PERIPHERAL:
         ret = "Periph";
         break;

      default:
         ret = "Unknown";
         break;
   }
   return ret;
}

/**
 * @brief  Call to get the description of the disconnect reason
 * @param  reason - the disconnect reason
 * @return String with description of disconnect reason if known
 */
const char *BLEModule_GetDisconnectReason(uint8_t reason)
{
   const char *ret;
   switch (reason)
   {
      case BLE_HCI_STATUS_CODE_SUCCESS:
         ret = "BLE_HCI_STATUS_CODE_SUCCESS";
         break;
      case BLE_HCI_STATUS_CODE_UNKNOWN_BTLE_COMMAND:
         ret = "BLE_HCI_STATUS_CODE_UNKNOWN_BTLE_COMMAND";
         break;
      case BLE_HCI_STATUS_CODE_UNKNOWN_CONNECTION_IDENTIFIER:
         ret = "BLE_HCI_STATUS_CODE_UNKNOWN_CONNECTION_IDENTIFIER";
         break;
      case BLE_HCI_AUTHENTICATION_FAILURE:
         ret = "BLE_HCI_AUTHENTICATION_FAILURE";
         break;
      case BLE_HCI_STATUS_CODE_PIN_OR_KEY_MISSING:
         ret = "BLE_HCI_STATUS_CODE_PIN_OR_KEY_MISSING";
         break;
      case BLE_HCI_MEMORY_CAPACITY_EXCEEDED:
         ret = "BLE_HCI_MEMORY_CAPACITY_EXCEEDED";
         break;
      case BLE_HCI_CONNECTION_TIMEOUT:
         ret = "BLE_HCI_CONNECTION_TIMEOUT";
         break;
      case BLE_HCI_STATUS_CODE_COMMAND_DISALLOWED:
         ret = "BLE_HCI_STATUS_CODE_COMMAND_DISALLOWED";
         break;
      case BLE_HCI_STATUS_CODE_INVALID_BTLE_COMMAND_PARAMETERS:
         ret = "BLE_HCI_STATUS_CODE_INVALID_BTLE_COMMAND_PARAMETERS";
         break;
      case BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION:
         ret = "BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION";
         break;
      case BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES:
         ret = "BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES";
         break;
      case BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF:
         ret = "BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF";
         break;
      case BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION:
         ret = "BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION";
         break;
      case BLE_HCI_UNSUPPORTED_REMOTE_FEATURE:
         ret = "BLE_HCI_UNSUPPORTED_REMOTE_FEATURE";
         break;
      case BLE_HCI_STATUS_CODE_LMP_RESPONSE_TIMEOUT:
         ret = "BLE_HCI_STATUS_CODE_LMP_RESPONSE_TIMEOUT";
         break;
      case BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION:
         ret = "BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION";
         break;
      case BLE_HCI_STATUS_CODE_LMP_PDU_NOT_ALLOWED:
         ret = "BLE_HCI_STATUS_CODE_LMP_PDU_NOT_ALLOWED";
         break;
      case BLE_HCI_INSTANT_PASSED:
         ret = "BLE_HCI_INSTANT_PASSED";
         break;
      case BLE_HCI_PAIRING_WITH_UNIT_KEY_UNSUPPORTED:
         ret = "BLE_HCI_PAIRING_WITH_UNIT_KEY_UNSUPPORTED";
         break;
      case BLE_HCI_DIFFERENT_TRANSACTION_COLLISION:
         ret = "BLE_HCI_DIFFERENT_TRANSACTION_COLLISION";
         break;
      case BLE_HCI_PARAMETER_OUT_OF_MANDATORY_RANGE:
         ret = "BLE_HCI_PARAMETER_OUT_OF_MANDATORY_RANGE";
         break;
      case BLE_HCI_CONTROLLER_BUSY:
         ret = "BLE_HCI_CONTROLLER_BUSY";
         break;
      case BLE_HCI_CONN_INTERVAL_UNACCEPTABLE:
         ret = "BLE_HCI_CONN_INTERVAL_UNACCEPTABLE";
         break;
      case BLE_HCI_DIRECTED_ADVERTISER_TIMEOUT:
         ret = "BLE_HCI_DIRECTED_ADVERTISER_TIMEOUT";
         break;
      case BLE_HCI_CONN_TERMINATED_DUE_TO_MIC_FAILURE:
         ret = "BLE_HCI_CONN_TERMINATED_DUE_TO_MIC_FAILURE";
         break;
      case BLE_HCI_CONN_FAILED_TO_BE_ESTABLISHED:
         ret = "BLE_HCI_CONN_FAILED_TO_BE_ESTABLISHED";
         break;
      default:
         ret = "Unknown";
         break;
   }
   return ret;
}

/**
 * @brief  Call to get the description of the status
 * @param  status - the status
 * @return String with description of status if known
 */
const char *BLEModule_GetStatusString(Status_t status)
{
   const char *ret;
   switch (status)
   {
      case STATUS_SUCCESS:
         ret = "STATUS_SUCCESS";
         break;
      case STATUS_ERROR:
         ret = "STATUS_ERROR";
         break;
      case STATUS_NOT_INIT:
         ret = "STATUS_NOT_INIT";
         break;
      case STATUS_BUSY:
         ret = "STATUS_BUSY";
         break;
      case STATUS_NVM_FAIL:
         ret = "STATUS_NVM_FAIL";
         break;
      case STATUS_CONFIG_CRC_FAIL:
         ret = "STATUS_CONFIG_CRC_FAIL";
         break;
      case STATUS_CONFIG_NOT_FOUND:
         ret = "STATUS_CONFIG_NOT_FOUND";
         break;
      case STATUS_UART_INIT:
         ret = "STATUS_UART_INIT";
         break;
      case STATUS_UART_TX_OVERFLOW:
         ret = "STATUS_UART_TX_OVERFLOW";
         break;
      case STATUS_UART_OFF:
         ret = "STATUS_UART_OFF";
         break;
      case STATUS_UART_BUSY:
         ret = "STATUS_UART_BUSY";
         break;
      case STATUS_USB_NOT_CONNECTED:
         ret = "STATUS_USB_NOT_CONNECTED";
         break;
      case STATUS_USB_TX_OVERFLOW:
         ret = "STATUS_USB_TX_OVERFLOW";
         break;
      case STATUS_USB_ERROR:
         ret = "STATUS_USB_ERROR";
         break;
      case STATUS_USB_NOT_SUPPORTED:
         ret = "STATUS_USB_NOT_SUPPORTED";
         break;
      case STATUS_MCU_BAD_INDEX:
         ret = "STATUS_MCU_BAD_INDEX";
         break;
      case STATUS_MCU_UNKNOWN_COMMAND:
         ret = "STATUS_MCU_UNKNOWN_COMMAND";
         break;
      case STATUS_MCU_UNSUPPORTED_COMMAND:
         ret = "STATUS_MCU_UNSUPPORTED_COMMAND";
         break;
      case STATUS_MCU_NOT_SUPPORTED_IN_THIS_ROLE:
         ret = "STATUS_MCU_NOT_SUPPORTED_IN_THIS_ROLE";
         break;
      case STATUS_MCU_CHECK_BYTE_FAIL:
         ret = "STATUS_MCU_CHECK_BYTE_FAIL";
         break;
      case STATUS_MCU_BAD_PARAM:
         ret = "STATUS_MCU_BAD_PARAM";
         break;
      case STATUS_MCU_PAIR_TABLE_FULL:
         ret = "STATUS_MCU_PAIR_TABLE_FULL";
         break;
      case STATUS_MCU_PAIRING_ENTRY_NOT_FOUND:
         ret = "STATUS_MCU_PAIRING_ENTRY_NOT_FOUND";
         break;
      case STATUS_MCU_ALREADY_PAIRED:
         ret = "STATUS_MCU_ALREADY_PAIRED";
         break;
      case STATUS_MCU_BAD_NODE_TYPE:
         ret = "STATUS_MCU_BAD_NODE_TYPE";
         break;
      case STATUS_MCU_UART_OFF:
         ret = "STATUS_UART_LINK_OFF";
         break;
      case STATUS_NET_PEER_NOT_CONNECTED:
         ret = "STATUS_NET_PEER_NOT_CONNECTED";
         break;
      case STATUS_NET_CENTRAL_NOT_CONNECTED:
         ret = "STATUS_NET_CENTRAL_NOT_CONNECTED";
         break;
      case STATUS_NET_BAD_CONNECTION_HANDLE:
         ret = "STATUS_NET_BAD_CONNECTION_HANDLE";
         break;
      case STATUS_NET_UNKNOWN_CMD:
         ret = "STATUS_NET_UNKNOWN_CMD";
         break;
      case STATUS_NET_UNSUPPORTED_CMD:
         ret = "STATUS_NET_UNSUPPORTED_CMD";
         break;
      case STATUS_NET_NOT_SUPPORTED_IN_THIS_ROLE:
         ret = "STATUS_NET_NOT_SUPPORTED_IN_THIS_ROLE";
         break;
      case STATUS_NET_BAD_PARAM:
         ret = "STATUS_NET_BAD_PARAM";
         break;
      case STATUS_NET_PAIR_TABLE_FULL:
         ret = "STATUS_NET_PAIR_TABLE_FULL";
         break;
      case STATUS_NET_NODE_NOT_FOUND:
         ret = "STATUS_NET_NODE_NOT_FOUND";
         break;
      case STATUS_NET_ALREADY_PAIRED:
         ret = "STATUS_NET_ALREADY_PAIRED";
         break;
      case STATUS_NET_CONNECTION_NOT_FOUND:
         ret = "STATUS_NET_CONNECTION_NOT_FOUND";
         break;
      case STATUS_NET_SECURITY_FAILED:
         ret = "STATUS_NET_SECURITY_FAILED";
         break;
      case STATUS_NET_ROUTING_ERROR:
         ret = "STATUS_NET_ROUTING_ERROR";
         break;
      case STATUS_NET_MGT_PAYLOAD_CANNOT_BE_FORWARDED:
         ret = "STATUS_NET_MGT_PAYLOAD_CANNOT_BE_FORWARDED";
         break;
      case STATUS_NET_MGT_PAYLOAD_CANNOT_BE_BROADCAST:
         ret = "STATUS_NET_MGT_PAYLOAD_CANNOT_BE_BROADCAST";
         break;
      case STATUS_BLE_ERROR:
         ret = "STATUS_BLE_ERROR";
         break;
      case STATUS_BLE_NOT_SUPPORTED:
         ret = "STATUS_BLE_NOT_SUPPORTED";
         break;
      case STATUS_BLE_NOT_SUPPORTED_IN_THIS_ROLE:
         ret = "STATUS_BLE_NOT_SUPPORTED_IN_THIS_ROLE";
         break;
      case STATUS_BLE_NODE_NOT_CONNECTED:
         ret = "STATUS_BLE_NODE_NOT_CONNECTED";
         break;
      case STATUS_BLE_NODE_ALREADY_CONNECTED:
         ret = "STATUS_BLE_NODE_ALREADY_CONNECTED";
         break;
      case STATUS_BLE_CONNECTION_IN_PROGRESS:
         ret = "STATUS_BLE_CONNECTION_IN_PROGRESS";
         break;
      case STATUS_BLE_BAD_PARAMETER:
         ret = "STATUS_BLE_BAD_PARAMETER";
         break;
      case STATUS_BLE_TX_POWER_NOT_SUPPORTED:
         ret = "STATUS_BLE_TX_POWER_NOT_SUPPORTED";
         break;
      case STATUS_BLE_TEST_MODE_BAD_PATTERN:
         ret = "STATUS_BLE_TEST_MODE_BAD_PATTERN";
         break;
      case STATUS_BLE_TEST_MODE_BAD_CHANNEL:
         ret = "STATUS_BLE_TEST_MODE_BAD_CHANNEL";
         break;
      default:
         ret = "Unknown status";
         break;
   }

   return ret;
}

/**********************************************************************************************
 * Module static functions
 **********************************************************************************************/

/**
 * @brief  Utility function to get data as a hex string
 * @param  addr - the bt address
 * @return None
 */
static void GetDataAsHex(const void *const data, size_t len, char *const buffer)
{
   buffer[0] = 0;
   char temp[4];

   const uint8_t *val = (uint8_t *)data;

   for (size_t count = 0; count < len; count++)
   {
      (void)snprintf(temp, sizeof(temp), "%02x", *val++);
      (void)strcat(buffer, temp);
   }
}

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
