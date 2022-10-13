/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 * @file
 *   This file implements the OpenThread platform abstraction for UART communication.
 *
 */

#include "definitions.h"

#include "utils/uart.h"

enum
{
    kBaudRate          = 115200,
    kReceiveBufferSize = 128,
};

typedef struct RecvBuffer
{
    // The data buffer
    uint8_t mBuffer[kReceiveBufferSize];
    // The offset of the first item written to the list.
    uint16_t mHead;
    // The offset of the next item to be written to the list.
    uint16_t mTail;
} RecvBuffer;

static RecvBuffer sReceive;

static bool sTransmitDone = false;

static DRV_HANDLE uartHandle;
static DRV_USART_BUFFER_HANDLE uartWriteBufferHandle;
static DRV_USART_BUFFER_HANDLE uartReadBufferHandle;

static void uartBufferEventHandler(DRV_USART_BUFFER_EVENT event, DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t context)
{
    if (event == DRV_USART_BUFFER_EVENT_COMPLETE)
    {
        if (bufferHandle == uartWriteBufferHandle)
        {
            sTransmitDone = true;
        }
        else if (bufferHandle == uartReadBufferHandle)
        {
            if (sReceive.mHead != (sReceive.mTail + 1) % kReceiveBufferSize)
            {
                sReceive.mTail = (sReceive.mTail + 1) % kReceiveBufferSize;
            }
            DRV_USART_ReadBufferAdd(uartHandle, (uint8_t *)&sReceive.mBuffer[sReceive.mTail], sizeof(uint8_t), &uartReadBufferHandle);
        }
    }
}

static void processReceive(void)
{
    // Copy tail to prevent multiple reads
    uint16_t tail = sReceive.mTail;

    // If the data wraps around, process the first part
    if (sReceive.mHead > tail)
    {
        otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, kReceiveBufferSize - sReceive.mHead);

        // Reset the buffer mHead back to zero.
        sReceive.mHead = 0;
    }

    // For any data remaining, process it
    if (sReceive.mHead != tail)
    {
        otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, tail - sReceive.mHead);

        // Set mHead to the local tail we have cached
        sReceive.mHead = tail;
    }
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NOT_IMPLEMENTED;
}

static void processTransmit(void)
{
    if (sTransmitDone)
    {
        sTransmitDone = false;
        otPlatUartSendDone();
    }
}

void samr21UartProcess(void)
{
    processReceive();
    processTransmit();
}

otError otPlatUartEnable(void)
{
    sReceive.mHead = 0;
    sReceive.mTail = 0;
    
    uartHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_READWRITE);
    DRV_USART_BufferEventHandlerSet(uartHandle, uartBufferEventHandler, (uintptr_t)0);
    DRV_USART_ReadBufferAdd(uartHandle, (uint8_t *)&sReceive.mBuffer[sReceive.mTail], sizeof(uint8_t), &uartReadBufferHandle);
    
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    DRV_USART_Close(uartHandle);
    
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error = OT_ERROR_NONE;

    DRV_USART_WriteBufferAdd(uartHandle, (uint8_t *)aBuf, (size_t)aBufLength, &uartWriteBufferHandle);

    return error;
}
