/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txttroff.ch,v 2.8 1991/09/12 20:03:25 bobg Exp $ */
/* $ACIS:txttroff.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txttroff.ch,v $ */

#ifndef lint
static char *rcsidtexttroff_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txttroff.ch,v 2.8 1991/09/12 20:03:25 bobg Exp $";
#endif /* lint */

/* Flags for various things texttroff should or should not do. (each should be a power of 2) */
#define	texttroff_Revert 1	/* if set texttroff will revert to the parents state after processing, otherwise the state will be left however it ended up. */
				  
package texttroff[txttroff] {
classprocedures:
    WriteSomeTroff(struct view *view,struct dataobject *dd,FILE *f,int toplevel,unsigned long revert);
    WriteTroff(struct view *view,struct dataobject *dd,FILE * f,int toplevel);

    /* BeginDoc / EndDoc
	These functions are called by standalone views which wish to use the
	troff environment as is established by text.
	They are called if the "toplevel" (fifth) parameter to xxx_Print is TRUE.
	The text generated before calling EndDoc should end with a newline;
	a .br should have been issued to send the last line to the output.
    */
    BeginDoc(FILE *f);
    EndDoc(FILE *f);


    /* BeginPS / EndPS
	These functions supply the transition for including a postscript rectangle
		in a troff page.  
	BeginPS sets up a postscript coordinate system for an image of size 
		(width x height) in postscript units (1/72 in.) 
		with the origin in the lower left corner.
	At the end of the postscript, the routine EndPS must be called.
	Each line between the two must begin with the two characters:  \!
		(backslash followed by exclamation mark)
*/
    BeginPS(FILE *f, long width, long height);
    EndPS(FILE *f, long width, long height);

};
