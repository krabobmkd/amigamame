/***************************************************************************

    osdepend.h

    OS-dependent code interface.

    Copyright (c) 1996-2006, Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __MAMEENUMS_H__
#define __MAMEENUMS_H__

/* These values are returned as error codes by osd_fopen() */
enum _osd_file_error
{
	FILEERR_SUCCESS,
	FILEERR_FAILURE,
	FILEERR_OUT_OF_MEMORY,
	FILEERR_NOT_FOUND,
	FILEERR_ACCESS_DENIED,
	FILEERR_ALREADY_OPEN,
	FILEERR_TOO_MANY_FILES
};



#endif	/* __OSDEPEND_H__ */
