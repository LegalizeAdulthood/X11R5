/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* 
 * Control inclusion of debugging code in object code
 */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/vuidebug.h,v 2.4 1991/09/12 19:17:34 bobg Exp $ */

#ifndef DEBUG
#define debug(xxx_foo)
#endif /* DEBUG */
#ifdef DEBUG
#define debug(xxx_foo) debugrtn xxx_foo
#endif /* DEBUG */
