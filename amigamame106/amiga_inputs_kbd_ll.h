#ifndef AMIGA_INPUTS_KBDLL_H
#define AMIGA_INPUTS_KBDLL_H
/*
    Manage Keyboard &
    Lowlevel entries: CD32 pads and mouse.
*/
extern "C" {

}


struct sMameInputsInterface;
// manages both keyboard and lowlevel pads...

extern struct sMameInputsInterface g_ipt_Keyboard;
extern struct sMameInputsInterface g_ipt_LLMouses;
#endif
