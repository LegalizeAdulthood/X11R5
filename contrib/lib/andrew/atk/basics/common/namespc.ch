/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/namespc.ch,v 2.7 1991/09/12 19:22:47 bobg Exp $ */
/* $ACIS:namespc.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/namespc.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidnamespace_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/namespc.ch,v 2.7 1991/09/12 19:22:47 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 *
 */

#include <atom.ih>


struct namespace_entry
{
  struct atom * name;
  long binding;
  short boundp;
};


class namespace[namespc] { 
methods:
  Enumerate( procedure proc, long data ) returns int;

  Clear();

  Lookup(struct atom * name) returns int;
  LookupCreate(struct atom * name ) returns int;

  BoundpAt( int index ) returns short;
  ValueAt( int index ) returns long;
  UnbindAt( int index );
  NameAt( int index ) returns struct atom *;
  SetValueAt( int index, long value );

  SetValue( struct atom * name, long value );
  GetValue( struct atom * name ) returns long;
  Boundp( struct atom * name, long * data ) returns short;
  Unbind( struct atom * name);

  WhereIsValue( long value ) returns struct atom *;

data:
  struct namespace_entry * namespace;
  short namespaceSize;
};


