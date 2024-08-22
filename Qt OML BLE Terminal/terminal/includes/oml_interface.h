/**
 *  @File: oml_interface.h
 *
 *  *******************************************************************************************
 *
 *  @file      oml_interface.h
 *
 *  @brief     Defines the physical layer interface to OML BLE module
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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
 * Module exported defines
 **********************************************************************************************/

/**********************************************************************************************
 * Module exported types
 **********************************************************************************************/

/**********************************************************************************************
 * Module exported functions
 **********************************************************************************************/
bool OMLInterface_Open(uint8_t port);
void OMLInterface_Purge(void);
void OMLInterface_Close(void);
void OMLInterface_Process(void);
void OMLInterface_Transmit(const void *const data, size_t len);
bool OMLInterface_Wake(void);

/**********************************************************************************************
 * Module exported variables
 **********************************************************************************************/

#ifdef __cplusplus
}
#endif

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
