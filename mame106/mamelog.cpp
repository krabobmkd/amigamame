#include "mamelog.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static int doStdOut = 0;
static logCallbcack logcb = NULL;
static int max_iwe=2;
static int min_iwe=1;
// make it simple....
void log_enableStdOut(int e)
{
    doStdOut = e;
    max_iwe = 2;
    min_iwe = 0;
}

void log_setCallback(logCallbcack cb)
{
    logcb = cb;
}

void vloginfo(int i_w_e, const char * format, va_list args )
{
    if(i_w_e>max_iwe || i_w_e<min_iwe) return;

    if(doStdOut)
    {
        vprintf(format,args);
    }

    if(logcb)
    {
        char tmp[128];
        vsnprintf(tmp, 127, format, args);
        tmp[127]=0;
        logcb(i_w_e,tmp);
    }
}
void loginfo(int i_w_e,const char *format, ...)
{
    if(i_w_e>max_iwe || i_w_e<min_iwe) return;

    va_list args;
    va_start(args, format);
        vloginfo(i_w_e,format,args);
    va_end(args);
}
void logerror( const char *format, ...)
{
 //krb: I use this print  to watch which driver is crying out too much logs.
    // this is sometimes the cause for slowness.
   printf("vloginfo: %s\n",format);
    if(2>max_iwe || 2<min_iwe) return;
//    printf("prout\n");
    va_list args;
    va_start(args, format);
        vloginfo(2,format,args);
    va_end(args);
}
