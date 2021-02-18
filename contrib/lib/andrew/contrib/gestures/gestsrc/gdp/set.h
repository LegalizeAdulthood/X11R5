/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/

/*
  This file implements 'undoable' sets

  A Set supports the operations
	s = EmptySet(..);
	e = AddElement(s, data);
	DeleteElement(e);
	Map(s, fp, arg)   [calls (*fp)(ElementPointer(e, arg)) for each
			element e in s]
	MapE(s, fp, arg)   [calls (*fp)(e, arg) for each element e in s]
	ElementPointer(e)  [returns the data associated with e.]

  Sets are grouped together into 'set group'.  Each set group has a group
  leader which is a Set used to refer the group.  Groupleaders are
  distinguished by having NULL as the groupleader to EmptySet() - other members
  in the group have the groupleader as this parameter:
	s1 = EmptySet(NULL, NULL, NULL);
	s2 = EmptySet(NULL, NULL, s1);
	s3 = EmptySet(NULL, NULL, s1);
	s4 = EmptySet(NULL, NULL, NULL);
  Here s1 and s4 are groupleaders, with [s1,s2,s3] forming a group, and [s4]
  by itself as the other group.

  CheckpointSetGroup(groupleader) marks the current state of all the sets in
  the group with the current version number, which is returned.  Further
  AddElement and DeleteElement operations which take place on any of the sets
  in the group will effect subsequent versions, but not effect the version
  returned by CheckpointSetGroup.  The state of the all the sets in the group
  may be restored by UndoSetGroup(groupleader, versionnumber) which returns
  a new version number.

 */


typedef struct set *Set;
typedef struct dll *Element;
typedef char *Pointer;
typedef int VersionNumber;

Set	EmptySet();	/* void (*when_added)(Set, Element, Pointer),
			   void (*when_deleted)(Set, Element, Pointer)
			   Set groupleader;
			   */

Element	AddElement();		/* Set, Pointer */
void	DeleteElement();	/* Set, Element */
void	Map();			/* Set, function(Pointer), Pointer*/
void	MapE();			/* Set, function(Element), Pointer*/
Pointer	ElementPointer();	/* Element */
Element	AnElement();		/* Set */

/* undo functions */

/* functions with Sets in their name take an arrays of sets, followed by
   a count of how may sets are in the array */

VersionNumber	CheckpointSetGroup();	/* Set groupleader; */
void		UndoSetGroup();		/* Set groupleader; VersionNumber v; */

void	DiscardOldVersions();	/* Set, VersionNumber */

/* debugging */

void	DumpSet();		/* Set, PrintPointerFunction(Pointer) */


void	IterateSet();		/* Set */
Element	NextElement();		/* Set */

