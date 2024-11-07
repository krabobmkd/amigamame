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
