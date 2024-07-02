
extern "C"
{
    #include <proto/exec.h>
    #include <proto/graphics.h>
    #include <dos/dos.h>
}
#include <stdlib.h>
#include <stdio.h>

#include "amiga_parallelpads.h"

ParallelPads *g_pParPads=NULL;

void close()
{
    if(g_pParPads) closeParallelPads(g_pParPads);
}

int main(int argc, char **argv)
{
    atexit(&close);
    g_pParPads = createParallelPads();
    ParallelPads *ppads = g_pParPads;

    if(!g_pParPads)
    {
        printf("init failed\n");
        return 1;
    }
    printf("init ok\n");
    int signal;
//    while( signal = Wait(ppads->_ppidata->_Signal | SIGBREAKF_CTRL_C))
//    {
//        if(signal &SIGBREAKF_CTRL_C) break;
//        printf("aprb: %02x bpra:%02x c:%d\n",(int)ppads->_ppidata->_ciaaprb_directions,(int)ppads->_ppidata->_ciabpra_fires,
//               (int)ppads->_ppidata->_counter
//               );
//    }

    printf("closing\n");
    return 0;
}
