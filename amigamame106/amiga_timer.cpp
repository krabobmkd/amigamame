/*
 * amiga_timer.cpp
 * Purpose: High-precision timer management via timer.device
 *
 * ╔════════════════════════════════════════════════════════════════════════╗
 * ║                   ⏱️  TIMER DEVICE HANDLER ⏰                           ║
 * ║  ┌──────────────────────────────────────────────────────────────┐    ║
 * ║  │                                                               │    ║
 * ║  │            ╔═══════════╗                                      │    ║
 * ║  │            ║ TIMER.DEV ║                                      │    ║
 * ║  │            ╚═════╤═════╝                                      │    ║
 * ║  │                  │                                            │    ║
 * ║  │                  ├──► Microsecond precision                   │    ║
 * ║  │                  ├──► Frame timing                            │    ║
 * ║  │                  └──► Performance counters                    │    ║
 * ║  │                                                               │    ║
 * ║  │   UNIT_MICROHZ provides microsecond-level accuracy!           │    ║
 * ║  └──────────────────────────────────────────────────────────────┘    ║
 * ║         Precise timing for perfect emulation speed!                   ║
 * ╚════════════════════════════════════════════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <proto/exec.h>
//#include <proto/dos.h>
#include <proto/timer.h>

extern "C" {
    #include "osdepend.h"
}

extern "C" {
    struct Device *TimerBase = NULL;
    struct timerequest    *TimerIO=NULL;
    struct MsgPort *timer_msgport=NULL;
}

void initTimers()
{
    // - - - - - - - timer init. Most likely to work
    timer_msgport = CreateMsgPort();
    if(timer_msgport)
    {
        TimerIO = (struct timerequest*)CreateIORequest(timer_msgport, sizeof(struct timerequest));
        if(TimerIO)
        {
            if(!OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *) TimerIO, 0))
                TimerBase = TimerIO->tr_node.io_Device;
        }
    }

}
void closeTimers()
{
    if(TimerIO)
    {
        if(TimerBase) {
          CloseDevice((struct IORequest *) TimerIO);
        }
        DeleteIORequest((struct IORequest *) TimerIO);
        if(timer_msgport) { DeleteMsgPort(timer_msgport); }
    }
}

/* return the current number of cycles, or some other high-resolution timer */
cycles_t osd_cycles(void)
{
   // cycles_t
    struct timeval tt;
    GetSysTime( &tt);
    return  (cycles_t)(((long long)tt.tv_secs)*1000000LL + (long long)tt.tv_micro);
}

/* return the number of cycles per second */
cycles_t osd_cycles_per_second(void)
{
    return 1000000LL;
}

/* return the current number of cycles, or some other high-resolution timer.
   This call must be the fastest possible because it is called by the profiler;
   it isn't necessary to know the number of ticks per seconds. */
cycles_t osd_profiling_ticks(void)
{
     struct timeval tt;
     GetSysTime( &tt);

     return  (cycles_t)((long long)tt.tv_secs*1000000LL + (long long)tt.tv_micro);
}

/*
 * Every microsecond counted! Timing is everything! ⏱️
 *      ___
 *     (o,o)  <- This cheetah appreciates precise timing!
 *     {| |}
 *      " "
 *     ~~~~
 */

