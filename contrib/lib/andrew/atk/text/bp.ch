/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
 
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/bp.ch,v 1.3 1991/09/12 19:51:22 bobg Exp $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_bp_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/bp.ch,v 1.3 1991/09/12 19:51:22 bobg Exp $ ";
#endif

class bp : dataobject [dataobj] {
overrides:
      ViewName() returns char *;
};

