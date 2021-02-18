/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	btwp.c
	Common procedures and data for manipulating the fields and B-tree
	making up the WP database.
	For use only internally to the WP library.
\* ************************************************************ */

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/btwp.c,v 2.13 1991/09/12 17:24:27 bobg Exp $";

#include <andrewos.h>	/* strings */
#include <andyenv.h>
#include <stdio.h>
#include <ctype.h>
#include <util.h>
#include <pwd.h>
#ifdef WHITEPAGES_ENV /* avoid makedepend "errors" */
#include <wp.h>
#include <btwp.h>
#endif /* WHITEPAGES_ENV  */
int bwDebugging = 0;

#ifndef _IBMR2
extern char *malloc(), *realloc();
#endif /* _IBMR2 */

/* IF YOU CHANGE THE FOLLOWING, CHANGE ``makeboth.c'' ALSO. */
char BTIxTags[NumBTIndices] = {KeyTagTk, KeyTagNI, KeyTagID, KeyTagN,
			  KeyTagSk, KeyTagCS, KeyTagCG, KeyTagNk, KeyTagOv};
int BTIndices[NumBTIndices] = {FldTk, FldNI, FldID, FldN, FldN, FldD, FldX, -1, -1};
int BTAuxIndices[NumBTIndices] = {-1, -1, -1, FldWN, FldWN, -1, -1, -1, -1};
/* IF YOU CHANGE wpFieldName, CHANGE ``btwp.h'' AND ``makeboth.c'' ALSO. */
char *wpFieldName[FldCOUNT+1] = {
	"N",
	"Tk",
	"WN",
	"ID",
	"EK",
	"NI",
	"GI",
	"PW",
	"HD",
	"Sh",
	"Af",
	"Fwd",
	"DK",
	"DP",
	"D",
	"X",
	"SI",
	"NPA",
	"Dt",
	"A2",
	"HA",
	"HP",
	"CA",
	"CX",
	"OA",
	"OP",
	"FAX",
	"HFX",
	0};

/* IF YOU CHANGE btIndexNames, CHANGE ``btwp.h'' ALSO. */
char *btIndexNames[BTIxCOUNT] = {
	"Tk",
	"NI",
	"ID",
	"NWN",
	"SK",
	"CS",
	"CG",
	"Nk",
	"Ov"
};

/* The TokenChar table tells whether we believe that a given character should be a searchable part of a person's name. */
char	wpTokenChar[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0000-0017 ^@^a^b^c^d^e^f^g^h^i^j^k^l^m^n^o*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0020-0037 ^p^q^r^s^t^u^v^w^x^y^z^[^\^]^^^_*/
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1,	/* 0040-0057  !"#$%&'()*+,-./ */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,	/* 0060-0077 0123456789:;<=>? */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0100-0117 @ABCDEFGHIJKLMNO */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,	/* 0120-0137 PQRSTUVWXYZ[\]^_ */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0140-0157 `abcdefghijklmno */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,	/* 0160-0177 pqrstuvwxyz{|}~^? */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0200-0217 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0220-0237 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0240-0257 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0260-0277 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0300-0317 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0320-0337 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0340-0357 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* 0360-0377 */
};

