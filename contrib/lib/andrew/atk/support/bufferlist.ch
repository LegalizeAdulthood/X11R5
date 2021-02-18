/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/bufferlist.ch,v 1.5 1991/09/12 19:48:36 bobg Exp $ */
/* $ACIS:bufferlist.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/bufferlist.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidbufferlist_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/bufferlist.ch,v 1.5 1991/09/12 19:48:36 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class bufferlist : observable[observe] {
    overrides:
	ObservedChanged (struct observable *changed, long value);
    methods:
        AddBuffer(struct buffer *buffer);
        RemoveBuffer(struct buffer *buffer);
        Enumerate(procedure mapFunction, long functionData) returns struct buffer *;
        CreateBuffer(char *bufferName, char *fileName, char *objetName, struct dataobject *data) returns struct buffer *;
	
/* Lookup functions */
        FindBufferByName(char *bufferName) returns struct buffer *;
        FindBufferByFile(char *filename) returns struct buffer *;
        FindBufferByData(struct dataobject *data) returns struct buffer *;

/* File functions. */
        GetBufferOnFile(char *filename, long flags) returns struct buffer *;
        GuessBufferName( char *filename, char *bufferName, int nameSize);
        GetUniqueBufferName( char *proposedBufferName, char *bufferName, int nameSize);
        SetDefaultObject(char *objectname);
    classprocedures:
        FinalizeObject(struct bufferlist *self);
        InitializeObject(struct bufferlist *self) returns boolean;
    data:
	struct listentry *head;
};

