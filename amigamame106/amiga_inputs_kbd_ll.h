/******************************************************************************
 * amiga_inputs_kbd_ll.h
 *
 *       ╔════════════════════════════════╗
 *       ║  [ESC][F1][F2][F3][F4][F5]...  ║
 *       ║  [~][1][2][3][4][5][6][7][8]   ║
 *       ║  [TAB][Q][W][E][R][T][Y][U]    ║
 *       ║  [CTRL][A][S][D][F][G][H]      ║
 *       ║  [SHIFT][Z][X][C][V][B]        ║
 *       ║   [====SPACE BAR====]          ║
 *       ║  Low Level Keyboard Magic!     ║
 *       ╚════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 *****************************************************************************/

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

void Inputs_Keyboard_ll_Update(struct MsgPort *pMsgPort);

#endif

/* Clickety-clack! Keys mapped, inputs captured!
 *       ()()
 *      (='.'=)  <-- Rabbit typed everything perfectly
 *      (")_(")
 */
