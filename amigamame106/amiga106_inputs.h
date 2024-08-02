#ifndef AMIGA_INPUTS_H
#define AMIGA_INPUTS_H

extern "C" {
    #include <exec/types.h>
    #include <utility/tagitem.h>
    #include <devices/inputevent.h>
    #include <devices/gameport.h>
}

void InitLowLevelLib();
void ConfigureLowLevelLib();
void CloseLowLevelLib();

void AllocInputs();
void FreeInputs();
void UpdateInputs(struct MsgPort *pMsgPort);

#endif
