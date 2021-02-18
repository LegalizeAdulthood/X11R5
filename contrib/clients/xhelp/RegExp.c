/****************************************************************************
Copyright 1990 by Brian Totty, Free Widget Foundation.

	RegExp.c

	This file contains the C code for the regular expression
	matching code.

	The routines supported act as a more friendly, user level
	interface to the regexp regular expression matching system.

 ****************************************************************************/

#include "RegExp.h"
#include <regexp.h>

void RegExpCompile(regexp,fsm_ptr,fsm_length)
char *regexp,*fsm_ptr;
int fsm_length;
{
#ifndef NO_REGEXP
	compile(regexp,fsm_ptr,&(fsm_ptr[fsm_length]),'\0');
#endif
} /* End RegExpCompile */


int RegExpMatch(string,fsm_ptr)
char *string,*fsm_ptr;
{
#ifndef NO_REGEXP
	if (advance(string,fsm_ptr) != 0)
		return(TRUE);
	    else
		return(FALSE);
#else
	return(TRUE);
#endif
} /* End RegExpMatch */


void _RegExpError(val)
int val;
{
	fprintf(stderr,"Regular Expression Error %d\n",val);
	exit(-1);
} /* End _RegExpError */


void RegExpPatternToRegExp(pattern,reg_exp)
char *pattern,*reg_exp;
{
	int in_bracket;

	in_bracket = 0;
	while (*pattern != '\0')
	{
		if (in_bracket)
		{
			if (*pattern == ']') in_bracket = 0;
			*reg_exp++ = *pattern++;
		}
		    else
		{
			switch (*pattern)
			{
			    case '[':
				in_bracket = 1;
				*reg_exp++ = '[';
				break;
			    case '?':
				*reg_exp++ = '.';
				break;
			    case '*':
				*reg_exp++ = '.';
				*reg_exp++ = '*';
				break;
			    case '.':
				*reg_exp++ = '\\';
				*reg_exp++ = '.';
				break;
			    default:
				*reg_exp++ = *pattern;
				break;
			}
			++ pattern;
		}
	}
	*reg_exp++ = '$';
	*reg_exp++ = '\0';
} /* End RegExpPatternToRegExp */
