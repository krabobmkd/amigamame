/***************************************************************************

    timer.h

    Functions needed to generate timing and synchronization between several
    CPUs.

    Copyright (c) 1996-2006, Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __TIMER_H__
#define __TIMER_H__

#include "mamecore.h"
#include <stdio.h>
#include <stdio.h>
/***************************************************************************
    CONSTANTS
***************************************************************************/

/* subseconds are tracked in attosecond (10^-18) increments */
// krb: now consider subseconds a
//#define MAX_SUBSECONDS				((subseconds_t)1000000000 * (subseconds_t)1000000000)
#define MAX_SECONDS					((seconds_t)1000000000)
#define MAX_SUBSECONDSM 0xffffffffUL
/* effective now/never values as doubles */
#define TIME_NOW             		(0.0)
#define TIME_NEVER            		(1.0e30)



/***************************************************************************
    MACROS
***************************************************************************/

/* convert between a double and subseconds */

//#define SUBSECONDS_TO_DOUBLE(x)		((double)(x) * (1.0 / (double)18446744073709551615.0))
//#define DOUBLE_TO_SUBSECONDS(x)		((subseconds_t)((x) * (double)18446744073709551615.0))

//#define SUBSECONDS_TO_DOUBLE(x)		((double)(x) * (1.0 / (double)MAX_SUBSECONDS))
//#define DOUBLE_TO_SUBSECONDS(x)		((subseconds_t)((x) * (double)MAX_SUBSECONDS))
// 18,446,744,073,709,551,615
// 18446744073709551615.0
/* convert cycles on a given CPU to/from mame_time */
//#define MAME_TIME_TO_CYCLES(cpu,t)	((t).seconds * cycles_per_second[cpu] + (t).subseconds / subseconds_per_cycle[cpu])


/* useful macros for describing time using doubles */
#define TIME_IN_HZ(hz)        (1.0 / (double)(hz))
#define TIME_IN_CYCLES(c,cpu) ((double)(c) * cycles_to_sec[cpu])
#define TIME_IN_SEC(s)        ((double)(s))
#define TIME_IN_MSEC(ms)      ((double)(ms) * (1.0 / 1000.0))
#define TIME_IN_USEC(us)      ((double)(us) * (1.0 / 1000000.0))
#define TIME_IN_NSEC(us)      ((double)(us) * (1.0 / 1000000000.0))

/* convert a double time to the number of cycles on a given CPU */
#define TIME_TO_CYCLES(cpu,t) ((int)((t) * sec_to_cycles[cpu]))

/* macro for the RC time constant on a 74LS123 with C > 1000pF */
/* R is in ohms, C is in farads */
#define TIME_OF_74LS123(r,c)	(0.45 * (double)(r) * ((double)(c))

/* macros that map all allocations to provide file/line/functions to the callee */
#define mame_timer_alloc(c)				_mame_timer_alloc(c, __FILE__, __LINE__, #c)
#define mame_timer_alloc_ptr(c,p)		_mame_timer_alloc_ptr(c, p, __FILE__, __LINE__, #c)
#define mame_timer_pulse(e,p,c)			_mame_timer_pulse(e, p, c, __FILE__, __LINE__, #c)
#define mame_timer_pulse_ptr(e,p,c)		_mame_timer_pulse_ptr(e, p, c, __FILE__, __LINE__, #c)
#define mame_timer_set(d,p,c)			_mame_timer_set(d, p, c, __FILE__, __LINE__, #c)
#define mame_timer_set_ptr(d,p,c)		_mame_timer_set_ptr(d, p, c, __FILE__, __LINE__, #c)

/* macros that map double time functions to mame_time functions */
#define timer_alloc(c)					mame_timer_alloc(c)
#define timer_alloc_ptr(c,p)			mame_timer_alloc_ptr(c,p)
#define timer_adjust(w,d,p,e)			mame_timer_adjust(w, double_to_mame_time(d), p, double_to_mame_time(e))
#define timer_adjust_ptr(w,d,e)			mame_timer_adjust_ptr(w, double_to_mame_time(d), double_to_mame_time(e))
#define timer_pulse(e,p,c)				mame_timer_pulse(double_to_mame_time(e), p, c)
#define timer_pulse_ptr(e,p,c)			mame_timer_pulse_ptr(double_to_mame_time(e), p, c)
#define timer_set(d,p,c)				mame_timer_set(double_to_mame_time(d), p, c)
#define timer_set_ptr(d,p,c)			mame_timer_set_ptr(double_to_mame_time(d), p, c)
#define timer_reset(w,d)				mame_timer_reset(w, double_to_mame_time(d))
#define timer_enable(w,e)				mame_timer_enable(w,e)
#define timer_timeelapsed(w)			mame_time_to_double(mame_timer_timeelapsed(w))
#define timer_timeleft(w)				mame_time_to_double(mame_timer_timeleft(w))
#define timer_get_time()				mame_time_to_double(mame_timer_get_time())
#define timer_starttime(w)				mame_time_to_double(mame_timer_starttime(w))
#define timer_firetime(w)				mame_time_to_double(mame_timer_firetime(w))



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

/* opaque type for representing a timer */
typedef struct _mame_timer mame_timer;

/* these core types describe a 96-bit time value */
typedef UINT32 subseconds_t;
typedef INT32 seconds_t;

typedef struct _mame_time mame_time;
struct _mame_time
{
    union {
        INT64 _t;
        struct {
#ifdef LSB_FIRST
	subseconds_t	subseconds;
	seconds_t		seconds;
#else
	seconds_t		seconds;
	subseconds_t	subseconds;
#endif
        };
    };

};

static inline double SUBSECONDS_TO_DOUBLE(subseconds_t x)
{
    double d = (double)(x) * (1.0 / (double)/*18446744073709551615.0*/(1ULL<<32));
    return d;
}
static inline subseconds_t DOUBLE_TO_SUBSECONDS(double d)
{
    subseconds_t x = ((subseconds_t)((d) * (double)/*18446744073709551615.0*/(1ULL<<32)));
    return x;
}




/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

/* constant times for zero and never */
extern mame_time time_zero;
extern mame_time time_never;

/* arrays containing mappings between CPU cycle times and timer values */
extern subseconds_t subseconds_per_cycle[];
extern INT64 subseconds_per_cycle64[];
extern UINT32 cycles_per_second[];
extern double cycles_to_sec[];
extern double sec_to_cycles[];



/***************************************************************************
    PROTOTYPES
***************************************************************************/

void timer_init(void);
void timer_free(void);
int timer_count_anonymous(void);

mame_time mame_timer_next_fire_time(void);
void mame_timer_set_global_time(mame_time newbase);
mame_timer *_mame_timer_alloc(void (*callback)(int), const char *file, int line, const char *func);
mame_timer *_mame_timer_alloc_ptr(void (*callback)(void *), void *param, const char *file, int line, const char *func);
void mame_timer_adjust(mame_timer *which, mame_time duration, INT32 param, mame_time period);
void mame_timer_adjust_ptr(mame_timer *which, mame_time duration, mame_time period);
void _mame_timer_pulse(mame_time period, INT32 param, void (*callback)(int), const char *file, int line, const char *func);
void _mame_timer_pulse_ptr(mame_time period, void *param, void (*callback)(void *), const char *file, int line, const char *func);
void _mame_timer_set(mame_time duration, INT32 param, void (*callback)(int), const char *file, int line, const char *func);
void _mame_timer_set_ptr(mame_time duration, void *param, void (*callback)(void *), const char *file, int line, const char *func);
void mame_timer_reset(mame_timer *which, mame_time duration);
int mame_timer_enable(mame_timer *which, int enable);
mame_time mame_timer_timeelapsed(mame_timer *which);
mame_time mame_timer_timeleft(mame_timer *which);
mame_time mame_timer_get_time(void);
mame_time mame_timer_starttime(mame_timer *which);
mame_time mame_timer_firetime(mame_timer *which);



/***************************************************************************
    static inlineS
***************************************************************************/

/*-------------------------------------------------
    make_mame_time - assemble a mame_time from
    seconds and subseconds components
-------------------------------------------------*/

static inline mame_time make_mame_time(seconds_t _secs, subseconds_t _subsecs)
{
	mame_time result;
	result.seconds = _secs;
	result.subseconds = _subsecs;
	return result;
}


/*-------------------------------------------------
    mame_time_to_double - convert from mame_time
    to double
-------------------------------------------------*/

static inline double mame_time_to_double(mame_time _time)
{
    double res = (double)_time.seconds + SUBSECONDS_TO_DOUBLE(_time.subseconds);
	return res;
}


/*-------------------------------------------------
    double_to_mame_time - convert from double to
    mame_time
-------------------------------------------------*/

static inline mame_time double_to_mame_time(double _time)
{
	mame_time abstime;

	/* special case for TIME_NEVER */
	if (_time >= TIME_NEVER)
		return time_never;
//optim heavy on 32b
	abstime._t = (INT64)(_time * ((double)(1LL<<32)));

	return abstime;
}


/*-------------------------------------------------
    add_mame_times - add two mame_times
-------------------------------------------------*/

static inline mame_time add_mame_times(mame_time _time1, mame_time _time2)
{
	mame_time result;

	/* if one of the items is time_never, return time_never */
	//OPTIMIZE THIS
	if (_time1.seconds >= MAX_SECONDS || _time2.seconds >= MAX_SECONDS)
		return time_never;

    result._t = _time1._t + _time2._t;
	return result;
}


/*-------------------------------------------------
    add_subseconds_to_mame_time - add subseconds
    to a mame_time
-------------------------------------------------*/

static inline mame_time add_subseconds_to_mame_time(mame_time _time1, subseconds_t _subseconds)
{
	mame_time result;

	/* if one of the items is time_never, return time_never */
	if (_time1.seconds >= MAX_SECONDS)
		return time_never;

    result._t = _time1._t + (INT64)_subseconds;
	return result;
}


/*-------------------------------------------------
    sub_mame_times - subtract two mame_times
-------------------------------------------------*/

static inline mame_time sub_mame_times(mame_time _time1, mame_time _time2)
{
	mame_time result;

	/* if time1 is time_never, return time_never */
	if (_time1.seconds >= MAX_SECONDS)
		return time_never;

    result._t = _time1._t - _time2._t;

	return result;
}


/*-------------------------------------------------
    sub_subseconds_from_mame_time - subtract
    subseconds from a mame_time
-------------------------------------------------*/

static inline mame_time sub_subseconds_from_mame_time(mame_time _time1, subseconds_t _subseconds)
{
	mame_time result;

	/* if time1 is time_never, return time_never */
	if (_time1.seconds >= MAX_SECONDS)
		return time_never;

    result._t = _time1._t - (INT64)_subseconds;

	return result;
}


/*-------------------------------------------------
    compare_mame_times - compare two mame_times
-------------------------------------------------*/

static inline int compare_mame_times(mame_time _time1, mame_time _time2)
{
    //printf("compare_mame_times:\n");

    INT64 dif = _time1._t - _time2._t;
	if (dif > 0LL)
		return 1;
	if (dif< 0LL)
		return -1;
	return 0;

   // if(dif != 0) return dif;
    //return (int)_time1.subseconds - (int)_time2.subseconds;
	// if (_time1.subseconds > _time2.subseconds)
	// 	return 1;
	// if (_time1.subseconds < _time2.subseconds)
	// 	return -1;
	//return 0;
}


static inline int MAME_TIME_TO_CYCLES(int cpunum,mame_time t)
{
    int r = t.seconds * cycles_per_second[cpunum];
    r += t.subseconds / subseconds_per_cycle[cpunum];
    return r;
}

//#define MAME_TIME_IN_CYCLES(c,cpu)	(make_mame_time((c) / cycles_per_second[cpu], (c) * subseconds_per_cycle[cpu]))
static inline mame_time MAME_TIME_IN_CYCLES(UINT32 cycles,int cpunum)
{
    mame_time result;
    result._t = (INT64)cycles * (INT64)subseconds_per_cycle[cpunum];
    // result.seconds = cycles / cycles_per_second[cpunum];
    // printf("MAME_TIME_IN_CYCLES cpu:%d cycles:%08x subsecpc:%08x\n",cpunum,cycles,subseconds_per_cycle[cpunum]);
    // result.subseconds = (cycles * subseconds_per_cycle[cpunum]);
    // printf("result.subseconds:%08x\n",result.subseconds);
    return result;
}


#endif	/* __TIMER_H__ */
