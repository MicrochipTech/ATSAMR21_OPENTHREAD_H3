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
 *   This file implements the OpenThread platform abstraction for the alarm.
 *
 */

#include "definitions.h"

#include <openthread/platform/alarm-milli.h>
#include <stdio.h>
#include "configuration.h"

#include "utils/uart.h"
static volatile uint32_t sTime      = 0;
static uint32_t          sDeltaTime = 0;
static uint32_t          sStartTime = 0;

#pragma GCC optimize "O0"
void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
//        //PRVN    
//    char buffer[128];
//    uint16_t len = 0;
    OT_UNUSED_VARIABLE(aInstance);
//    LED0_Toggle(); 

    sDeltaTime = aDt;
    sStartTime = aT0;
    //PRVN 
//    if (sDeltaTime >= 14500 )
//    {
////        len = snprintf((char*)&buffer,128,"%ld %ld %ld\n" ,sTime,sDeltaTime,sStartTime );
//        len = snprintf((char*)&buffer,128," sTime = %ld sDeltaTime = %ld sStartTime = %ld\n" ,sTime,sDeltaTime,sStartTime );
//        otPlatUartSend((uint8_t *)buffer, len );
//    }
    
//    if((sStartTime < sTime))
//    {
//        LED0_Toggle();        
//    }
//    else
//    {
//        while(1);
//    }
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sDeltaTime = 0;
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    return sTime;
}

void Tick_Handler(void)
{
    sTime++;
//    
//    if (!(sTime % 333))
//    {
//        LED0_Toggle();
//    }
}

void samr21AlarmInit(void)
{
    /*Configure system tick to generate periodic interrupts */
    SysTick_Config(CPU_CLOCK_FREQUENCY / 1000);
//    TC4_TimerInitialize();
//    TC4_TimerCallbackRegister ((TC_TIMER_CALLBACK) Tick_Handler, 
//                                (uintptr_t) NULL);
//    TC4_TimerStart();
}

void SysTick_Handler(void)
{
    sTime++;
    if (0 == (sTime % 333))
    {
        LED0_Toggle();
    }
}

void samr21AlarmProcess(otInstance *aInstance)
{
    if ((sDeltaTime != 0) && ((sTime - sStartTime) >= sDeltaTime))
    {
        sDeltaTime = 0;

#if OPENTHREAD_CONFIG_DIAG_ENABLE

        if (otPlatDiagModeGet())
        {
            otPlatDiagAlarmFired(aInstance);
        }
        else
#endif
        {
            otPlatAlarmMilliFired(aInstance);
            
        }
    }
}
