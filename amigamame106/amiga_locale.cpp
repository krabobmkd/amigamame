/*
 * amiga_locale.cpp
 * Purpose: Localization and internationalization support (placeholder)
 *
 * ╔════════════════════════════════════════════════════════════════════════╗
 * ║                   🌍 LOCALE/i18n HANDLER 🗣️                            ║
 * ║  ┌──────────────────────────────────────────────────────────────┐    ║
 * ║  │                                                               │    ║
 * ║  │   English  ──┐                                                │    ║
 * ║  │   Français ──┤                                                │    ║
 * ║  │   Deutsch  ──┤──►  Translation Map  ──► UI Text              │    ║
 * ║  │   Italiano ──┤                                                │    ║
 * ║  │   Español  ──┘                                                │    ║
 * ║  │                                                               │    ║
 * ║  │   Ready for future localization support!                      │    ║
 * ║  │   Currently returns English strings as-is                     │    ║
 * ║  └──────────────────────────────────────────────────────────────┘    ║
 * ║         One day, many languages! For now, English only!               ║
 * ╚════════════════════════════════════════════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "amiga_locale.h"
#include <map>
#include <string>
using namespace std;

//re static map<string,string> _locale;

/*
"Quit","Quitter"
"About","A propos"
"About MUI...","A propos de MUI..."

*/

void initLocale()
{
    // one day or never.
    //  Note: also could use the lowlevel simple function instead of locale.library.
}

const char *GetMessagec(const char *pEnglishOrKey)
{
    return pEnglishOrKey;
//    map<string,string>::iterator fit = _locale.find(pEnglishOrKey);
//    if(fit == _locale.end()) return pEnglishOrKey;
//    return fit->second.c_str();
}

/*
 * Universal language: The language of gaming! 🌍
 *       ___
 *      /o o\  <- This globe-trotting elephant speaks all languages!
 *     ( === )
 *      )   (
 *     /`---'\
 */
