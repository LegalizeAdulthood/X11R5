/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*  $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/runadewa.ch,v 1.3 1991/09/12 19:19:30 bobg Exp $ */
/* $ACIS:eza.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/runadewa.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrunadewa_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/runadewa.ch,v 1.3 1991/09/12 19:19:30 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


struct runadewapp_fileList {
    struct runadewapp_fileList *next;
    char *filename;
    boolean newWindow;
    boolean readOnly;
};

class runadewapp[runadewa]: application[app] {
    overrides:
	ParseArgs(int argc, char **argv) returns boolean;
	Start() returns boolean;
    classprocedures:
        InitializeClass() returns boolean;
        GetArguments(int *argc) returns char **;
    data:
        struct runadewapp_fileList *files,**fileLink;
        boolean initFile;
        boolean haveBufferInWindow;
        struct im *im;
        struct frame *frame;
        struct buffer *buffer;
	char *title, *cls,*func;
	char *objectname;
	};
