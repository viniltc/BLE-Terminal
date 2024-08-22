/**
 *  @File: oml_interface.c
 *
 *  *******************************************************************************************
 *
 *  @file      oml_interface.c
 *
 *  @brief     Implements the physical layer interface to OML BLE module
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
#include "oml_interface.h"
#include "..\..\OML BLE App\mcu_cmds.h"
#include "..\..\OML BLE App\types.h"
#include "ble_module.h"
#include "serial.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

/**********************************************************************************************
 * Module constant defines
 **********************************************************************************************/
#define MCU_BAUD_RATE 1000000u

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
 * @brief  Open the comm port interface to the OML BLE module
 * @param  port  - enumerated port number
 * @return true if port opened OK, false otherwise
 */
bool OMLInterface_Open(uint8_t port)
{
   if (!SerialOpen(port, MCU_BAUD_RATE))


   {
      return false;
   }

   OMLInterface_Purge();

   return true;
}

/**
 * @brief  Purge the rx buffer
 * @param  None
 * @return None
 */
void OMLInterface_Purge(void)
{
   while (SerialRxPending())
   {
      (void)SerialReadData();
   }
}

/**
 * @brief  Close the comm port interface to the OML BLE module
 * @param  None
 * @return None
 */
void OMLInterface_Close(void)
{
   SerialClose();
}

/**
 * @brief  Run the OML BLE interface
 * @param  None
 * @return None
 */
void OMLInterface_Process(void)
{
   while (SerialRxPending())
   {
      uint8_t ch = SerialReadData();
      BLEModule_OnRx(ch);
   }
}

/**
 * @brief  Send data to the OML BLE module
 * @param  data - data to send
 * @param  len - length of data to send in bytes
 * @return None
 */
void OMLInterface_Transmit(const void *const data, size_t len)
{
   SerialWriteBytes(data, (int)len);
}

/**
 * @brief  Wake the OML BLE module by toggling RTS
 * @param  None
 * @return true if operation successful, false otherwise
 */
bool OMLInterface_Wake(void)
{
   bool ret = false;

   if (SerialClrRts())
   {
      printf("CLRRTS\n");
      Sleep(100u);
      printf("SETRTS\n");
      if (SerialSetRts())
      {
         Sleep(100u);
         printf("CLRRTS\n");
         if (SerialClrRts())
         {
            Sleep(100u);
            printf("AUTORTS\n");
            if (SerialAutoRts())
            {
               ret = true;
            }
         }
      }
   }
   return ret;
}

/**********************************************************************************************
 * Module static functions
 **********************************************************************************************/

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
