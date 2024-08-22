/**
 *  @File: debug.c
 *
 *  *******************************************************************************************
 *
 *  @file      debug.c
 *
 *  @brief     Implements a debug API to aid development
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
#include "debug.h"
#include "timer.h"
#include <stdarg.h>
#include <stdio.h>

/**********************************************************************************************
 * Module constant defines
 **********************************************************************************************/
#define COLOUR_NONE  "\033[0m"
#define COLOR_GREEN  "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_RED    "\033[0;31m"
#define COLOR_CYAN   "\033[0;36m"

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
 * @brief  Produce timestamped debug, with module name in accordance with the global debug level
 * @param  module - module name
 * @param  level- the debug level
 * @param  format - format string
 * @return None
 */
void DBG(const uint8_t level, const char *format, ...)
{
   if (level >= DEBUG_LEVEL_ENABLED)
   {
      const char *levelStr = "";
      const char *colour = COLOUR_NONE;
      switch (level)
      {
         case DEBUG_LEVEL_INFO:
            levelStr = "I";
            colour = COLOR_GREEN;
            break;

         case DEBUG_LEVEL_WARN:
            levelStr = "W";
            colour = COLOR_YELLOW;
            break;

         case DEBUG_LEVEL_ERROR:
            levelStr = "E";
            colour = COLOR_RED;
            break;

         default:
            break;
      }

      (void)printf("%s%08lld %s: ", colour, TIMER_NowMs(), levelStr);

      va_list paramList;
      va_start(paramList, format);
      (void)vprintf(format, paramList);
      va_end(paramList);

      (void)printf(COLOUR_NONE);
   }
}

/**
 * @brief  Produce timestamped OML Response debug, with colour code based on status
 * @param  status - OML BLE Status_t
 * @param  format - format string
 * @return None
 */
void DBG_Rsp(int status, const char *format, ...)
{
   const char *colour;

   if (0 == status)
   {
      colour = COLOR_GREEN;
   }
   else
   {
      colour = COLOR_RED;
   }

   (void)printf("%s%08lld ", colour, TIMER_NowMs());

   va_list paramList;
   va_start(paramList, format);
   (void)vprintf(format, paramList);
   va_end(paramList);

   (void)printf(COLOUR_NONE);
}

/**
 * @brief  Produce timestamped OML Event debug, with colour code based on status
 * @param  status - OML BLE Status_t
 * @param  format - format string
 * @return None
 */
void DBG_Evt(const char *format, ...)
{
   const char *colour = COLOR_CYAN;

   (void)printf("%s%08lld ", colour, TIMER_NowMs());

   va_list paramList;
   va_start(paramList, format);
   (void)vprintf(format, paramList);
   va_end(paramList);

   (void)printf(COLOUR_NONE);
}

/**
 * @brief  Dump a hex buffer to trace
 * @param  desc - description (can be NULL)
 * @param  addr - the address to start dumping from
 * @param  len - the number of bytes to dump
 * @param  perLine - number of bytes on each output line
 * @return None
 */
void DBG_Hex(const char *desc, const void *addr, const uint16_t len, int perLine)
{
   // Silently ignore silly per-line values.
   if (perLine < 4 || perLine > 64)
      perLine = 16;

   int i;
   const unsigned char *pc = (const unsigned char *)addr;

   // Output description if given.
   if (desc != NULL)
      printf("%s:\n", desc);

   // Length checks.
   if (len == 0)
   {
      printf("  ZERO LENGTH\n");
      return;
   }
   if (len < 0)
   {
      printf("  NEGATIVE LENGTH: %d\n", len);
      return;
   }

   if (perLine > 64u)
   {
      printf("  NEGATIVE LENGTH: %d\n", len);
      return;
   }

   uint8_t buff[65];

   // Process every byte in the data.
   for (i = 0; i < len; i++)
   {
      // Multiple of perLine means new or first line (with line offset).
      if ((i % perLine) == 0)
      {
         // Only print previous-line ASCII buffer for lines beyond first.
         if (i != 0)
            printf("  %s\n", buff);

         // Output the offset of current line.
         printf("  %04x ", i);
      }

      // Now the hex code for the specific character.
      printf(" %02x", pc[i]);

      // And buffer a printable ASCII character for later.
      if ((pc[i] < 0x20) || (pc[i] > 0x7e))
         buff[i % perLine] = '.';
      else
         buff[i % perLine] = pc[i];
      buff[(i % perLine) + 1] = '\0';
   }

   // Pad out last line if not exactly perLine characters.
   while ((i % perLine) != 0)
   {
      printf("   ");
      i++;
   }

   // And print the final ASCII buffer.
   printf("  %s\n", buff);
}

/**********************************************************************************************
 * Module static functions
 **********************************************************************************************/

/**********************************************************************************************
 * End of file
 **********************************************************************************************/
