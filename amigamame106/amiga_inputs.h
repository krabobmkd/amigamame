/******************************************************************************
 * amiga_inputs.h
 *
 *        ╔════════════════════════════╗
 *        ║    ┌──┐                    ║
 *        ║   ┌┘  └┐  JOYSTICK         ║
 *        ║   │ •• │  ┌──────┐         ║
 *        ║   └┐  ┌┘  │ (A)(B)│         ║
 *        ║    └──┘   └──────┘         ║
 *        ║                            ║
 *        ║  Press Start to Play!      ║
 *        ╚════════════════════════════╝
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

/* Button mashing complete! Great success!
 *       ,_     _,
 *       |\\_,-~/|  <-- Squirrel stored all your inputs
 *       / _ _ _ \
 *      (   @ @   )
 *       \  (_)  /
 */
