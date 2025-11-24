/******************************************************************************
 * amiga_inputs_interface.h
 *
 *      ╔═══════════════════════════════════╗
 *      ║   INPUT PLUGINS INTERFACE         ║
 *      ║   ┌─────┐   ┌─────┐   ┌─────┐    ║
 *      ║   │ KBD │ ─ │ JOY │ ─ │ GUN │    ║
 *      ║   └─────┘   └─────┘   └─────┘    ║
 *      ║        │         │         │      ║
 *      ║        └─────────┴─────────┘      ║
 *      ║           UNIFIED API              ║
 *      ╚═══════════════════════════════════╝
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

#ifndef AMIGA_INPUTSINTERFCE_H
#define AMIGA_INPUTSINTERFCE_H
/**
    Internal plugin interface for abstract hardware controllers...
    each registered controller must declare oscodes between 0-1023.
    (experimental)
*/
#ifdef __cplusplus
extern "C" {
#endif
// Amiga types
#include <exec/types.h>
// Mame's types
#include "osd_cpu.h"
#include "osdepend.h"

#ifndef input_code
/* input code is used to represent an abstracted input type */
typedef UINT32 input_code;
#endif


typedef void (*fAddOsCode)(void *registerer,os_code_info *pFirst, int nbcodes);

struct sMameInputsInterface
{
    // only accept 0-1023 oscode range.
    // if no control for this input module, return NULL. else should register codes.
    void *(*Create)(void *registerer,fAddOsCode addOsCode);
    // optional, per frame.
    void (*FrameUpdate)(void *o);
    // mandatory, used by real time.
    int (*GetCode)(void *o, ULONG oscode);
    // mandatory, must free.
    void (*Close)(void *o);
    // optional.
    void (*PostInputPortInitCheck)(void *o);

};

#ifdef __cplusplus
}
#endif

#endif

/* Plugin it all! Every controller welcome here!
 *      /\_/\
 *     ( ^.^ )  <-- Hamster approves this interface
 *      > ~ <
 *     /|   |\
 */
