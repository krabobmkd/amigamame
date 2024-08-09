
#include "amiga_locale.h"

#include <proto/locale.h>

// please include that generated horror ony here:
#define CATCOMP_BLOCK
#include "messages.h"

struct LocaleInfo   LocaleInfo ={NULL,NULL};


// do not use the catcomp generated one.
STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);
// STRPTR  __stdargs GetCatalogStr( struct Catalog *catalog, LONG stringNum, STRPTR defaultString );
//#define XLocaleBase LocaleBase
//#define LocaleBase li->li_LocaleBase
// STRPTR  __stdargs GetCatalogStr( struct Catalog *catalog, LONG stringNum, STRPTR defaultString );
    if (li->li_LocaleBase)
        return(GetCatalogStr((struct Catalog *)li->li_Catalog,stringNum,builtIn));
//#define LocaleBase XLocaleBase
//#undef XLocaleBase

    return(builtIn);
}

STRPTR GetMessage(LONG id)
{
    return(GetString(&LocaleInfo, id));
}

