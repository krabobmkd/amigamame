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

