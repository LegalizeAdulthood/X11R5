/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *canon_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/mail/lib/RCS/canon.c,v 2.5 1991/09/12 17:14:29 bobg Exp $";

/*
	canon.c -- canonicalize a person's name
*/

#include <ctype.h>

#define Ctxt 0
#define Cnul 1
#define Cwsp 2
#define Cdot 3
static char CharClass[0200] = {
	Cnul, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0000 - 0007 */
	Ctxt, Cwsp, Cwsp, Cwsp, Cwsp, Cwsp, Ctxt, Ctxt,	/* 0010 - 0017 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0020 - 0027 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0030 - 0037 */
	Cwsp, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0040 - 0047 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Cdot, Ctxt,	/* 0050 - 0057 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0060 - 0067 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0070 - 0077 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0100 - 0107 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0110 - 0117 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0120 - 0127 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Cwsp,	/* 0130 - 0137 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0140 - 0147 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0150 - 0157 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt,	/* 0160 - 0167 */
	Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt, Ctxt };	/* 0170 - 0177 */
void CanonicalizePersonName(LocalPart)
char *LocalPart;
{/* Overwrites the storage in LocalPart with a canonicalized version--normalizing dots and spaces. */
/* Turns dots to spaces unless they look like an abbreviation. */
    char *SPtr, *DPtr, Chr;
    enum {Ignore, Copying, SeeDot, DblDot, EndScan, STOP} State;
    int Class;

    SPtr = DPtr = LocalPart;
    State = Ignore;
    while (State != STOP) {
	Chr = *SPtr++;
	Class = CharClass[toascii(Chr)];
	switch (State) {
	    case Ignore:
		if (Class == Cnul) State = STOP;
		else if (Class == Ctxt || Class == Cdot) {*DPtr++ = Chr; State = Copying;}
		break;
	    case Copying:
		if (Class == Ctxt) *DPtr++ = Chr;
		else if (Class == Cnul) State = STOP;
		else if (Class == Cwsp) State = EndScan;
		else State = SeeDot;
		break;
	    case SeeDot:
		if (Class == Ctxt) {*DPtr++ = ' '; *DPtr++ = Chr; State = Copying;}
		else if (Class == Cnul) {*DPtr++ = '.'; State = STOP;}
		else if (Class == Cwsp) {*DPtr++ = '.'; State = EndScan;}
		else State = DblDot;
		break;
	    case DblDot:
		if (Class == Ctxt) {*DPtr++ = '.';*DPtr++ = ' ';*DPtr++ = Chr; State = Copying;}
		else if (Class == Cnul) {*DPtr++ = '.'; State = STOP;}
		else if (Class == Cwsp) {*DPtr++ = '.'; State = EndScan;}
		break;
	    case EndScan:
		if (Class == Ctxt || Class == Cdot) {*DPtr++=' '; *DPtr++=Chr; State=Copying;}
		else if (Class == Cnul) State = STOP;
		break;
	}
    }
    *DPtr++ = '\0';	/* terminate it */
}
