
#include "amiga_locale.h"
#include <map>
#include <string>
using namespace std;

static map<string,string> _locale;

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
    map<string,string>::iterator fit = _locale.find(pEnglishOrKey);
    if(fit == _locale.end()) return pEnglishOrKey;
    return fit->second.c_str();
}
