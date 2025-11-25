/******************************************************************************
 * amiga_inputs_midiimpl.h
 *
 *       ╔════════════════════════════════╗
 *       ║  ♪  MIDI IMPLEMENTATION  ♫     ║
 *       ║  ═══════════════════════       ║
 *       ║   [IN] ●─────● [OUT]           ║
 *       ║                                ║
 *       ║   Note On / Note Off           ║
 *       ║   Control Change Magic         ║
 *       ║   Music IS the controller!     ║
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

#ifndef AMIGA_MIDIIMPL_H
#define AMIGA_MIDIIMPL_H

/**
    Link the independent MIDI input extension controller source
    as a C++ inputs module.
*/

struct sMameInputsInterface;
extern struct sMameInputsInterface g_ipt_MidiIn;

#endif

/* MIDI bridged to C++! Notes flowing perfectly!
 *       /\_/\
 *      ( ^.^ )  <-- Playful kitten plays the keys
 *       (") (")
 */
