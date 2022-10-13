/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file logging.c
 * Platform abstraction for the logging
 *
 */

#include "openthread-core-config.h"

#include <utils/code_utils.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/toolchain.h>

#include "definitions.h"

#include <stdarg.h>
#include <stdio.h>

#include "uart.h"

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED)

#if BOARD == SAMR21_XPLAINED_PRO

#define LOG_PARSE_BUFFER_SIZE 128
#define LOG_TIMESTAMP_ENABLE 1

char sLogString[LOG_PARSE_BUFFER_SIZE + 1];

static void logOutput(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, va_list ap)
{
    int len = 0;

    len = vsnprintf(sLogString, LOG_PARSE_BUFFER_SIZE, aFormat, ap);

    otEXPECT(len >= 0);

exit:

    if (len >= LOG_PARSE_BUFFER_SIZE)
    {
        len = LOG_PARSE_BUFFER_SIZE - 1;
    }

    sLogString[len++] = '\n';
    
//    otPlatUartSend((uint8_t *)sLogString, len);

//    LOGSPI_CS_Clear();
//    
//    for (uint8_t i = 0; i < len; i++)
//    {
//        uint8_t rx_byte = 0;
//        uint8_t tx_byte = sLogString[i];
//        SERCOM5_SPI_WriteRead((uint8_t *)&tx_byte, 1, (uint8_t *)&rx_byte, 1);
//    }    
//    
//    LOGSPI_CS_Set();

    return;
}

#endif

void samr21LogInit(void)
{
    //SERCOM5_SPI_Initialize();
}

OT_TOOL_WEAK void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    va_list ap;

    va_start(ap, aFormat);

    logOutput(aLogLevel, aLogRegion, aFormat, ap);

    va_end(ap);
}

#endif
