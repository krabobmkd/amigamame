#ifndef _MAMELOG_H_
#define _MAMELOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdarg.h>
// make it simple....
void log_enableStdOut(int e);
typedef void (*logCallbcack)(int i_w_e,const char *);
void log_setCallback(logCallbcack cb);

void loginfo(int i_w_e, const char *format, ...);
void logerror( const char *format, ...);
void vloginfo(int i_w_e, const char * format, va_list args );

#ifdef __cplusplus
}
#endif



#endif
