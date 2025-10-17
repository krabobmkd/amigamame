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
#include <exec/types.h>

// some interface are one entry in an existing port
// some interface are a new port with a ist of entries.
//typedef void (*fAddAsEntry)(const char *pPortName,int );
//typedef void (*fAddAsPort)(const char *pPortName,int );

typedef void (*fAddOsCode)(void *m,const char *keyname,ULONG oscode ,ULONG interfcode);

struct sMameInputsInterface
{
//    const char *Name; // general interface name

//    int _nbHardwareInputs; // used by config

//    // for each <_nbHardwareInputs
//    void (*AddConfig)(int iHardwareInput,fAddAsEntry addAsEntry, fAddAsPort addAsPort);

//    // In order they are used
////    void (*AddConfigSetup)();

    void *(*Create)();
    // only accept 0-1023 oscode range.
    void (*InitOsCodeMap)(void *o,void *m,fAddOsCode addOsCode);

    // optional, can be null
    void (*FrameUpdate)(void *o);
    // mandatory, used by real time.
    int (*GetCode)(void *o, ULONG oscode);
    // mandatory, must free.
    void (*Close)(void *o);
};

#ifdef __cplusplus
}
#endif

#endif
