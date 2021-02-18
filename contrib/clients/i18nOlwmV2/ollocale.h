/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 */

/* @(#)  ollocale.h 1.4 90/12/12; SMI */

#ifndef _OLWMOLLOCALEH
#define _OLWMOLLOCALEH

/*
 * OPEN LOOK Locale Categories.  Basic Locale must be first item
 * (least number).
 */
#define	OLLC_LC_BASIC_LOCALE	0
#define	OLLC_LC_DISPLAY_LANG	1
#define	OLLC_LC_INPUT_LANG	2
#define	OLLC_LC_NUMERIC		3
#define	OLLC_LC_TIME_FORMAT	4
#define	OLLC_LC_MAX		5

/*
 * OPEN LOOK Locale priority (small number has higher priority).
 * Could not use enum data type, since we will do the comparison among
 * this numbers.
 */
#define	OLLC_SRC_PROGRAM	0
#define	OLLC_SRC_COMMAND_LINE	1
#define	OLLC_SRC_RESOURCE	2
#define	OLLC_SRC_POSIX		3

typedef struct _OLLCItem {
	char	*locale;
	int	priority;
	int	posix_category;		/* Will initialize in InitGRVLV() */
} OLLCItem;

/*
 * This structure sometimes has been access as array.  Watch out!
 */
typedef struct _OLLC {
	OLLCItem	BasicLocale;
	OLLCItem	DisplayLang;
	OLLCItem	InputLang;
	OLLCItem	Numeric;
	OLLCItem	TimeFormat;
} OLLC;

extern int	OLLCUpdated;

#endif /* _OLWMOLLOCALEH */
