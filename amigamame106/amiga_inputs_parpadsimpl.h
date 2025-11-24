/******************************************************************************
 * amiga_inputs_parpadsimpl.h
 *
 *       ╔════════════════════════════════╗
 *       ║  PARALLEL PORT CONTROLLER      ║
 *       ║  ═══════════════════           ║
 *       ║   25 PIN CONNECTOR             ║
 *       ║  [D0][D1][D2][D3][D4][D5]      ║
 *       ║  [D6][D7][GND][GND][...]       ║
 *       ║                                ║
 *       ║  Extra controllers galore!     ║
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

#ifndef AMIGA_PARPADSIMPL_H
#define AMIGA_PARPADSIMPL_H

/**
    Link the independent C parallel pads extension controller source
    as a C++ inputs module.
*/

struct sMameInputsInterface;
extern struct sMameInputsInterface g_ipt_ParallelPads;

#endif

/* Parallel is parallel-awesome! More players, more fun!
 *        /\
 *       /  \
 *      / /\ \  <-- Turtle carries all the data
 *     /______\
 */
