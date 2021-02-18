/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/cmerror.c,v 2.5 1991/09/12 17:09:24 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/cmerror.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/cmerror.c,v 2.5 1991/09/12 17:09:24 bobg Exp $";
#endif /* lint */

#include <cmerror.h>

/*
 * _cmErrorCode - Global cmenu error code.
 */
int _cmErrorCode = cmE_NO_ERROR;

/*
 * _cmErrorList - Global cmenu error code discription strings.
 */
char *
_cmErrorList[cmE_CODE_COUNT] = {
    "No error",				/* cmE_NO_ERROR */
    "Menu not initialized",		/* cmE_NOT_INIT */
    "Argument out of bounds",		/* cmE_ARG_BOUNDS */
    "Pane not found",			/* cmE_P_NOT_FOUND */
    "Selection not found",		/* cmE_S_NOT_FOUND */
    "Unable to create graphics context",/* cmE_CREATE_GC */
    "Unable to calloc memory",		/* cmE_CALLOC */
    "Unable to create XAssocTable",	/* cmE_CREATE_ASSOC */
    "Unable to make pixmap",		/* cmE_MAKE_PIXMAP */
    "Unable to create cursor",		/* cmE_CREATE_CURSOR */
    "Unable to open font",		/* cmE_OPEN_FONT */
    "Unable to create windows",		/* cmE_CREATE_WINDOW */
    "Unable to create InputOnly window",/* cmE_CREATE_INPUTONLY */
};

char *cmenuError()
{
    char message[128];		/* Error message buffer. */

    if ((_cmErrorCode < cmE_CODE_COUNT) && (_cmErrorCode >= 0)) {
	return(_cmErrorList[_cmErrorCode]);
    }
    sprintf(message, "Unknown _cmErrorCode: %d", _cmErrorCode);
    return(message);
}

