/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xws.ch,v 1.6 1991/09/12 19:24:55 bobg Exp $ */
/* $ACIS:ws.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xws.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxws_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xws.ch,v 1.6 1991/09/12 19:24:55 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* X implementation of the windowsystem class.
 */

class xws : windowsystem[winsys] {
    overrides:
        CreateCursor() returns struct xcursor *;
        CreateFontdesc() returns struct xfontdesc *;
        CreateGraphic() returns struct xgraphic *;
        CreateIM() returns struct xim *;
	CreateOffScreenWindow(char * host, long width, long height) returns struct xoffscrwin *;
        FlushAllWindows();
        HandleFiles(long WaitTime, boolean beCheap) returns boolean;
    classprocedures:
        InitializeClass() returns boolean;
};
