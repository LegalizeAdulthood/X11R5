/****************************************************************************
 Copyright 1990 by Brian Totty, Free Widget Foundation

	RegExp.h

	This file contains the C definitions and declarations for
	the regular expression matching code.

	The routines supported act as a more friendly, user level
	interface to the regexp regular expression matching system.

 ****************************************************************************/

#ifndef _REGEXP_H_
#define	_REGEXP_H_

#include <stdio.h>

void		RegExpCompile();
int		RegExpMatch();
void		_RegExpError();
void		RegExpPatternToRegExp();

#ifndef TRUE
#define TRUE				1
#endif

#ifndef FALSE
#define	FALSE				0
#endif

#define	INIT		register char *sp = instring;
#define	GETC()		(*sp++)
#define	PEEKC()		(*sp)
#define	UNGETC(c)	-- sp
#define	RETURN(ptr)	return;
#define	ERROR(val)	_RegExpError(val)

#endif
