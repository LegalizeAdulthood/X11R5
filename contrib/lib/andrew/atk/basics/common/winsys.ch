/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/winsys.ch,v 2.8 1991/09/12 19:23:53 bobg Exp $ */
/* $ACIS:winsys.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/winsys.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidwindowsystem_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/winsys.ch,v 2.8 1991/09/12 19:23:53 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* Window system package.
 * Provides an abstraction for dealing with multiple window management systems
 * using a singe executable.
 */

class windowsystem[winsys] {
    methods:
        CreateIM() returns struct im *;
        CreateGraphic() returns struct graphic *;
        CreateCursor() returns struct cursor *;
        CreateFontdesc() returns struct fontdesc *;
	CreateOffScreenWindow(char * host, long width, long height) returns struct offscrwin *;
        FlushAllWindows();
        HandleFiles(long WaitTime, boolean beCheap) returns boolean;
};
