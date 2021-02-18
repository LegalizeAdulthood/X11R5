/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
void StInit();
void StRedraw();
Element StNewObj();	/* ObjectType t; */
void StDelete(); 	/* Element e; */
void StUpdatePoint();	/* Element e; int point_number, x, y; */
void StMove();		/* Element e; int x, y; */
void StTransform();	/* Element e; Transformation t */
Element StPick();	/* int x, y */
Element StEdit();	/* Element e; */
Element StCopyElement();/* Element e; */


Set CopySet();
