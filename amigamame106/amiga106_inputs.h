#ifndef AMIGA_INPUTS_H
#define AMIGA_INPUTS_H

extern "C" {
    #include <exec/types.h>
    #include <utility/tagitem.h>
    #include <devices/inputevent.h>
    #include <devices/gameport.h>
}

//olde, now done in ConfigureLowLevelLib() if needed. void InitLowLevelLib();
void ConfigureLowLevelLib();
// r16, now to be used at each game closing.(FreeInputs)
void CloseLowLevelLib();

void AllocInputs();
void FreeInputs();
void UpdateInputs(struct MsgPort *pMsgPort);

#endif
