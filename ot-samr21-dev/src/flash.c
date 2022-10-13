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
 *   This file implements the OpenThread platform abstraction for the non-volatile storage.
 */

#include <string.h>

#include <openthread/instance.h>

#include "definitions.h"
#include "utils/uart.h"
#include <stdio.h>

#include "uart.h"

extern uint32_t __d_nv_mem_start;
extern uint32_t __d_nv_mem_end;

#define OT_FLASH_BASE_ADDRESS ((uint32_t)&__d_nv_mem_start)
#define OT_FLASH_PAGE_SIZE    (0x100)

/*
 * This value should not exceed:
 *     (((uint32_t)&__d_nv_mem_end - (uint32_t)&__d_nv_mem_start) / OT_FLASH_PAGE_SIZE)
 *
 * __d_nv_mem_start and __d_nv_mem_end is defined in linker script.
 * The size of NVRAM region is 4k. Page size is 256 bytes. Maximum OT_FLASH_PAGE_NUM
 * should be equal or less than 16.
 *
 */
#define OT_FLASH_PAGE_NUM 16

#define OT_FLASH_SWAP_SIZE (OT_FLASH_PAGE_SIZE * (OT_FLASH_PAGE_NUM / 2))

static uint32_t mapAddress(uint8_t aSwapIndex, uint32_t aOffset)
{
    uint32_t address = OT_FLASH_BASE_ADDRESS + aOffset;

    if (aSwapIndex)
    {
        address += OT_FLASH_SWAP_SIZE;
    }

    return address;
}

static inline uint32_t getRowStartAddr(uint32_t memAddr)
{
    /* 
     * Row runs from 00--FF; so mask them out in a given address to
     * get the row offset.
     * i.e., 0x80040088 shall return 0x80040000
     */
    return (memAddr & ~((NVMCTRL_FLASH_ROWSIZE) - 1));
}

void otPlatFlashInit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

uint32_t otPlatFlashGetSwapSize(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return OT_FLASH_SWAP_SIZE;
}

void otPlatFlashErase(otInstance *aInstance, uint8_t aSwapIndex)
{
#if 1
    uint32_t address = mapAddress(aSwapIndex, 0);

    for (uint8_t i = 0; i < (OT_FLASH_SWAP_SIZE / NVMCTRL_FLASH_ROWSIZE); i++)
    {
        NVMCTRL_RowErase(address + (i * NVMCTRL_FLASH_ROWSIZE));
        while (NVMCTRL_IsBusy());
    }
#endif
    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatFlashWrite(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, const void *aData, uint32_t aSize)
{
#if 1
    uint8_t rowData[NVMCTRL_FLASH_ROWSIZE];
    uint32_t addr = mapAddress(aSwapIndex, aOffset);
    uint32_t rowStartAddr = getRowStartAddr(addr);   
    uint32_t offset = addr - rowStartAddr;   
    
    uint32_t tempOffset = offset;
    uint32_t remSize = aSize;
    uint32_t tempSize = 0;
    uint32_t nextRowIndex = 0;
    uint32_t nextDataIndex = 0;
    
    bool setNext = false;
    
    while(remSize)
    {
        if((tempOffset + remSize) > NVMCTRL_FLASH_ROWSIZE)
        {
            tempSize = NVMCTRL_FLASH_ROWSIZE - tempOffset;
            setNext = true;
        }
        else
        {
            tempSize = remSize;
            remSize = 0;
        }
        rowStartAddr += nextRowIndex;
        aData += nextDataIndex;
        
        memset(rowData, 0, sizeof(rowData));
        
        NVMCTRL_Read((uint32_t *)rowData, NVMCTRL_FLASH_ROWSIZE, rowStartAddr);
        while (NVMCTRL_IsBusy());
        
        memcpy((rowData + tempOffset) , aData, tempSize);
        
        /* Flush the updated data into NVM by pages */
        for (uint8_t i = 0; i < NVMCTRL_ROW_PAGES; i++)
        {
            NVMCTRL_PageBufferWrite((uint32_t *)&(rowData[(i * NVMCTRL_FLASH_PAGESIZE)]), (rowStartAddr + (i * NVMCTRL_FLASH_PAGESIZE)));
            NVMCTRL_PageBufferCommit((rowStartAddr + (i * NVMCTRL_FLASH_PAGESIZE)));
            while(NVMCTRL_IsBusy());
        }
        if(setNext)
        {
            tempOffset = 0;
            remSize -= tempSize;
            nextRowIndex += NVMCTRL_FLASH_ROWSIZE;
            nextDataIndex += tempSize;
            setNext = false;
        }
    }
#endif
    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatFlashRead(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, void *aData, uint32_t aSize)
{
    uint8_t rowData[NVMCTRL_FLASH_ROWSIZE];
    uint32_t addr = mapAddress(aSwapIndex, aOffset);
    uint32_t rowStartAddr = getRowStartAddr(addr);
    uint32_t offset = addr - rowStartAddr; 

    uint32_t tempOffset = offset;
    uint32_t remSize = aSize;
    uint32_t tempSize = 0;
    uint32_t nextRowIndex = 0;
    uint32_t nextDataIndex = 0;

    bool setNext = false;
    
    while(remSize)
    {
        if((tempOffset + remSize) > NVMCTRL_FLASH_ROWSIZE)
        {
            tempSize = NVMCTRL_FLASH_ROWSIZE - tempOffset;
            setNext = true;
        }
        else
        {
            tempSize = remSize;
            remSize = 0;
        }
        rowStartAddr += nextRowIndex;
        aData += nextDataIndex;        
        
        memset(rowData, 0, sizeof(rowData));
        
        NVMCTRL_Read((uint32_t *)rowData, NVMCTRL_FLASH_ROWSIZE, rowStartAddr);
        while (NVMCTRL_IsBusy());
        
        memcpy(aData, (rowData + tempOffset), tempSize);       
        
        if(setNext)
        {
            tempOffset = 0;
            remSize -= tempSize;
            nextRowIndex += NVMCTRL_FLASH_ROWSIZE;
            nextDataIndex += tempSize;
            setNext = false;
        }                
    }
//    NVMCTRL_Read((uint32_t *)rowData, NVMCTRL_FLASH_ROWSIZE, rowStartAddr);
//    while (NVMCTRL_IsBusy());
//
//    memcpy(aData, &(rowData[offset]), aSize);

    OT_UNUSED_VARIABLE(aInstance);
}
