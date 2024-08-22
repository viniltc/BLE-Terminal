/**
 *  @File: crc8.h
 *
 *  *******************************************************************************************
 *
 *  @file      crc8.h
 *
 *  @brief     Defines the CRC-8-CCITT API https://crccalc.com/
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

/**********************************************************************************************
 * Module exported types
 **********************************************************************************************/

/**********************************************************************************************
 * Module exported functions
 **********************************************************************************************/
uint8_t crc8ccitt_block(uint8_t val, const void *data, size_t size);

/**********************************************************************************************
 * Module exported variables
 **********************************************************************************************/

#ifdef __cplusplus
}
#endif

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
