/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/stylesht.ch,v 2.7 1991/09/12 19:49:22 bobg Exp $ */
/* $ACIS:stylesht.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/stylesht.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidstylesheet_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/stylesht.ch,v 2.7 1991/09/12 19:49:22 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* stylesht.H
 * class header file for stylesht.
 *
 */

#define stylesheet_VERSION 1

class stylesheet[stylesht] : observable[observe] {
methods:
    FreeStyles();			/* clear out all styles in the stylesheet */
    Add(struct style *styleptr);	/* add a style to the stylesheet */
    Delete(struct style *styleptr);	/* delete a style from the stylesheet */
    Find(char *name) returns struct style *;	/* find a style, given it's name */
    GetMenuList(procedure procname, struct classinfo *infotype) returns struct menulist *;
					/* get a menulist for the styles */
    Read(FILE *fileptr, boolean template) returns long;
					/* parse the contents of \\define{} */
    Write(FILE *fileptr);               /* write all externally defined styles */
    SetTemplateName(char *name);	/* save the name of the template being used */
    GetTemplateName() returns char *;	/* retrieve the name of the template being used */
    EnumerateStyles(/* struct stylesheet *self, */ procedure func, long data) 
		returns struct style *;   /* calls func(style, data) for each style in self.  
			The boolean value returned by func is True if the function is
			through enumerating;  EnumerateStyles then returns the last style 
			processed, othewise it returns NULL */
    GetGlobalStyle() returns struct style *;
classprocedures:
    FinalizeObject(struct stylesheet *self);
    InitializeObject(struct stylesheet *self) returns boolean;
data:
    char *templateName;			/* name of the template used, if any */
    long nstyles;			/* number of styles in this stylesheet */
    long maxStyles;			/* number of styles currently allocated */
    long version;			/* version used for updating menulist */
    struct style **styles;		/* array of the styles themselves */
    struct menulist *styleMenu;		/* current menulist for the styles */
};
