/******************************************************************************
 * amiga_locale.h
 *
 *
 *     ╔═══════════════════════════════════════╗
 *     ║  Retro Computing for the Masses!      ║
 *     ║  [A] [M] [I] [G] [A]  Power!          ║
 *     ╚═══════════════════════════════════════╝
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

#ifndef AMIGA_LOCALE_H
#define AMIGA_LOCALE_H

#ifdef __cplusplus
extern "C" {
#endif

void initLocale();
const char *GetMessagec(const char *pEnglishOrKey);

#ifdef __cplusplus
}
#endif

#endif

/* The End - May your strings always be localized!
 *    /\_/\
 *   ( o.o )  <-- Cat says: "Meow in any language!"
 *    > ^ <
 */
