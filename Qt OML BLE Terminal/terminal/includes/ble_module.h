/**
 *  @File: ble_module.h
 *
 *  *******************************************************************************************
 *
 *  @file      ble_module.h
 *
 *  @brief     Defines the BLE Module API
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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
 * Module exported defines
 **********************************************************************************************/
#define BLE_HCI_STATUS_CODE_SUCCESS                         0x00 /**< Success. */
#define BLE_HCI_STATUS_CODE_UNKNOWN_BTLE_COMMAND            0x01 /**< Unknown BLE Command. */
#define BLE_HCI_STATUS_CODE_UNKNOWN_CONNECTION_IDENTIFIER   0x02 /**< Unknown Connection Identifier. */
#define BLE_HCI_AUTHENTICATION_FAILURE                      0x05 /**< Authentication Failure. */
#define BLE_HCI_STATUS_CODE_PIN_OR_KEY_MISSING              0x06 /**< Pin or Key missing. */
#define BLE_HCI_MEMORY_CAPACITY_EXCEEDED                    0x07 /**< Memory Capacity Exceeded. */
#define BLE_HCI_CONNECTION_TIMEOUT                          0x08 /**< Connection Timeout. */
#define BLE_HCI_STATUS_CODE_COMMAND_DISALLOWED              0x0C /**< Command Disallowed. */
#define BLE_HCI_STATUS_CODE_INVALID_BTLE_COMMAND_PARAMETERS 0x12 /**< Invalid BLE Command Parameters. */
#define BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION           0x13 /**< Remote User Terminated Connection. */
#define BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES 0x14 /**< Remote Device Terminated Connection due to low resources.*/
#define BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF     0x15 /**< Remote Device Terminated Connection due to power off. */
#define BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION            0x16 /**< Local Host Terminated Connection. */
#define BLE_HCI_UNSUPPORTED_REMOTE_FEATURE                  0x1A /**< Unsupported Remote Feature. */
#define BLE_HCI_STATUS_CODE_LMP_RESPONSE_TIMEOUT            0x22 /**< LMP Response Timeout. */
#define BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION 0x23 /**< LMP Error Transaction Collision/LL Procedure Collision. */
#define BLE_HCI_STATUS_CODE_LMP_PDU_NOT_ALLOWED             0x24 /**< LMP PDU Not Allowed. */
#define BLE_HCI_INSTANT_PASSED                              0x28 /**< Instant Passed. */
#define BLE_HCI_PAIRING_WITH_UNIT_KEY_UNSUPPORTED           0x29 /**< Pairing with Unit Key Unsupported. */
#define BLE_HCI_DIFFERENT_TRANSACTION_COLLISION             0x2A /**< Different Transaction Collision. */
#define BLE_HCI_PARAMETER_OUT_OF_MANDATORY_RANGE            0x30 /**< Parameter Out Of Mandatory Range. */
#define BLE_HCI_CONTROLLER_BUSY                             0x3A /**< Controller Busy. */
#define BLE_HCI_CONN_INTERVAL_UNACCEPTABLE                  0x3B /**< Connection Interval Unacceptable. */
#define BLE_HCI_DIRECTED_ADVERTISER_TIMEOUT                 0x3C /**< Directed Advertisement Timeout. */
#define BLE_HCI_CONN_TERMINATED_DUE_TO_MIC_FAILURE          0x3D /**< Connection Terminated due to MIC Failure. */
#define BLE_HCI_CONN_FAILED_TO_BE_ESTABLISHED               0x3E /**< Connection Failed to be Established. */

/**********************************************************************************************
 * Module exported types
 **********************************************************************************************/

/**********************************************************************************************
 * Module exported functions
 **********************************************************************************************/
void BLEModule_Init(void);
void BLEModule_OnRx(const uint8_t ch);
void BLEModule_Tx(const void *payload, size_t payloadLen);
void BLEModule_Handler(const uint8_t *buf, size_t bufLen);
void BLEModule_RspHandler(const uint8_t *buf, size_t bufLen);
void BLEModule_EvtHandler(const uint8_t *buf, size_t bufLen);
const char *BLEModule_GetNodeType(NodeType_t nodeType);
const char *BLEModule_GetNodeRole(NodeRole_t nodeRole);
const char *BLEModule_GetDisconnectReason(uint8_t reason);
const char *BLEModule_GetStatusString(Status_t status);

/**********************************************************************************************
 * Module exported variables
 **********************************************************************************************/

#ifdef __cplusplus
}
#endif

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
