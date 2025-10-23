#ifndef AMIGA_INPUTS_H
#define AMIGA_INPUTS_H

extern "C" {
    #include <exec/types.h>
    #include <exec/ports.h>
}

//void Inputs_AllocAndInitCodes();
//void Inputs_PostScreenOpenInit();

void Inputs_FrameUpdate();


void Inputs_Free();


#endif
