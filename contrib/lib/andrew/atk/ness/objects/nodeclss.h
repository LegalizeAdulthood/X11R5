/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nodeclss.h,v 1.5 1991/09/12 19:44:36 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nodeclss.h,v $ */

/* nodeclass.h - global definition for the nodeclass system */

#ifndef _nodeclass_h_
#define _nodeclass_h_
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *nodeclass_h_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nodeclss.h,v 1.5 1991/09/12 19:44:36 bobg Exp $";
#endif

/* there is no defining instance of QTnode.  Its uses are always local, so it does not
matter if more than one copy is allocated. */

struct node *QTnode;

#endif _nodeclass_h_
