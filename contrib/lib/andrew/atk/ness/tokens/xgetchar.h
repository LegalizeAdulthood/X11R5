/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/xgetchar.h,v 1.6 1991/09/12 19:45:01 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/xgetchar.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/xgetchar.h,v 1.6 1991/09/12 19:45:01 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
fixGetChar.h

	simpletext declares self->string as 'char' instead of 'unsigned char',
	so GetChar does sign extension on certain unmentionable hardware

	This file redefines the character getting macros.
*/
/*
 *    $Log: xgetchar.h,v $
 * Revision 1.6  1991/09/12  19:45:01  bobg
 * Update copyright notice
 *
 * Revision 1.5  1989/02/17  16:51:45  ghoti
 * ifdef/endif,etc. label fixing - courtesy of Ness
 *
 * Revision 1.4  89/02/08  16:30:32  ghoti
 * change copyright notice
 * 
 * Revision 1.3  89/02/07  14:20:34  ghoti
 * first pass porting changes: filenames and references to them
 * 
 * Revision 1.2  88/11/02  14:47:58  wjh
 * added ExtendShortSign
 * 
 * Revision 1.1  88/10/21  10:38:48  wjh
 * Initial revision
 *  
 * Revision 1.0  88/10/20  wjh
 * Created by WJHansen
 */

#include <smpltext.ih>
#include <text.ih>

#ifndef ibm032

#define 	simpletext_CorrectGetChar(s,i) (0xFF & simpletext_GetChar(s,i))
#define 	text_CorrectGetChar(s,i) (0xFF & text_GetChar(s,i))

#else /* ibm032 */

#define 	simpletext_CorrectGetChar(s,i) (simpletext_GetChar(s,i))
#define 	text_CorrectGetChar(s,i) (text_GetChar(s,i))

#endif /* ibm032 */

#define  	ExtendShortSign(x)  	(((x) ^ 0x8000) - 0x8000)
