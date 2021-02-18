/*
 * xlogin - X login manager
 *
 * $Id: util.c,v 1.2 1991/10/04 17:06:01 stumpf Exp stumpf $
 *
 * Copyright 1989, 1990, 1991 Technische Universitaet Muenchen (TUM), Germany
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TUM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  TUM makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 *
 * Author:  Markus Stumpf, Technische Universitaet Muenchen
 *		<Markus.Stumpf@Informatik.TU-Muenchen.DE>
 *
 * Filename: util.c
 * Date:     Mon 24 Jun 1991  15:14:41
 * Creator:  Markus Stumpf <Markus.Stumpf@Informatik.TU-Muenchen.DE>
 *
 * $Source: /usr/wiss/stumpf/R4/local/clients/xlogin/RCS/util.c,v $
 *
 */


#include "xlogin.h"

#include <pwd.h>

/*
 * external functions
 */
extern char    *strtok();
extern void     Debug();

/*
 * forward definitions
 */
void AddMotdInfo _P((Widget, Widget *, MotdListPtr));
void ResizeAndFreeMotdList _P((MotdSortTreePtr, Cardinal *, Cardinal *));
void RelayoutMotdList _P((void));
void GenerateMotdList _P((void));
Boolean MakeValidLabel _P((String));


extern MotdWidgetClassRec mwc;
extern Arg      label_args[5];
extern Arg      text_args[9];
extern char     Myname[];
extern struct passwd *pwd;
extern xlogin_preresourceRec pre_app_resources;


/* ----------------------------------------------------------------
 * Function Name:
 *	AddMotdInfo ( Widget, Widget *, MotdListPtr)
 * Description:
 *
 * Parameters:
 *	- parent
 *	- queue
 *	- item
 * Returns:
 *	None
 */
#if NeedFunctionPrototypes
void
AddMotdInfo(
	       Widget parent,
	       Widget * queue,
	       MotdListPtr item)
#else
void
AddMotdInfo(parent, queue, item)
    Widget          parent;
    Widget         *queue;
    MotdListPtr     item;
#endif				/* NeedFunctionPrototypes */
{
    char           *the_label, *hc, *hlab;
    Dimension       lwidth;
    Boolean         use_path;

    Debug("AddMotdInfo() entering ...");
    Debug("    queue=0x%x", *queue);

    /*
     * if file has 0 lines, skip it
     */
    if (item->lines == 0) {
	Debug("    file %s has zero lines ... leaving", item->filename);
	return;
    }

    if (item->showit) {
	hlab = mwc.changed_label;
	use_path = mwc.changed_use_path;
    }
    else {
	hlab = mwc.unchanged_label;
	use_path = mwc.unchanged_use_path;
    }

    Debug("    hlab=\"%s\"", hlab);
    if ((hlab == (char *) NULL) || (*hlab == '\0')) {
	the_label = (char *) NULL;
    }
    else {
	if (!use_path) {
	    if ((hc = rindex(item->filename, PATH_DELIMITER[0]))
		== NULL) {
		hc = item->filename;
	    }
	    else
		hc++;
	}
	else
	    hc = item->filename;
	the_label = (char *) malloc(strlen(hlab) + strlen(hc) + 1);
	sprintf(the_label, hlab, hc);
    }

    if (the_label != (char *) NULL) {
	label_args[2].value = (XtArgVal) 0;
	label_args[3].value = (XtArgVal) * queue;
	label_args[4].value = (XtArgVal) the_label;
	*queue = item->label =
	    XtCreateManagedWidget("motdLabel",
				  labelWidgetClass,
				  parent,
				  label_args, XtNumber(label_args));
	Debug("    added label-widget 0x%x for file %s ",
	      *queue, item->filename);
	XtVaGetValues(*queue,
		      XtNwidth, &lwidth,
		      NULL, NULL);

	Debug("    lwidth=%d - Re-positioning to %d", lwidth,
	      (mwc.wid_width - lwidth) / 2);
	XtVaSetValues(*queue,
		      XtNhorizDistance, (mwc.wid_width - lwidth) / 2,
		      NULL, NULL);
    }
    else {
	Debug("    skipped adding empty label");
    }

    if (item->showit) {
	text_args[0].value = (XtArgVal) item->filename;
	text_args[1].value = (XtArgVal) * queue;
	text_args[2].value = (XtArgVal) mwc.wid_width;
	text_args[3].value = (XtArgVal) item->lines * mwc.font_height + 4;
	*queue = item->motd =
	    XtCreateManagedWidget("motdText",
				  asciiTextWidgetClass,
				  parent,
			       (ArgList) text_args, XtNumber(text_args));
	Debug("    added text-widget 0x%x for file %s ",
	      *queue, item->filename);
    }

    Debug("AddMotdInfo() leaving ...");
}				/* AddMotdInfo */


/* ----------------------------------------------------------------
 * Function Name:
 *	ResizeAndFreeMotdList ( MotdSortTreePtr, Cardinal *, Cardinal *)
 * Description:
 *
 * Parameters:
 *	- treeItem
 *	- n_lines
 *	- n_wids
 * Returns:
 *	None
 */
#if NeedFunctionPrototypes
void
ResizeAndFreeMotdList(
			 MotdSortTreePtr treeItem,
			 Cardinal * n_lines,
			 Cardinal * n_wids)
#else
void
ResizeAndFreeMotdList(treeItem, n_lines, n_wids)
    MotdSortTreePtr treeItem;
    Cardinal       *n_lines, *n_wids;
#endif				/* NeedFunctionPrototypes */
{
    Cardinal        maxLines;

    Debug("ResizeAndFreeMotdList(): entered ...");
    if (treeItem == (MotdSortTreePtr) NULL) {
	Debug("ResizeAndFreeMotdList(): leaving ... nothing to sort");
	return;
    }

    ResizeAndFreeMotdList(treeItem->left, n_lines, n_wids);

    if (*n_wids == 0) {
	Debug("    n_wids == 0 done.");
	return;
    }
    maxLines = *n_lines / *n_wids;
    Debug("    maxLines=%d, n_lines=%d, n_wids=%d, itemlines=%d ",
	  maxLines, *n_lines, *n_wids, treeItem->lines);

    if (treeItem->lines > maxLines) {
	Debug("    resizing 0x%x ...", treeItem->wid);
	XtVaSetValues(treeItem->wid,
		      XtNheight, mwc.font_height * maxLines,
		      NULL, NULL);
	Debug("        done.");
	*n_lines -= maxLines;
    }
    else
	*n_lines -= treeItem->lines;
    (*n_wids)--;

    ResizeAndFreeMotdList(treeItem->right, n_lines, n_wids);
    free(treeItem);
    Debug("ResizeAndFreeMotdList(): leaving ...");
}				/* ResizeAndFreeMotdList */


/* ----------------------------------------------------------------
 * Function Name:
 *	RelayoutMotdList ( void )
 * Description:
 *
 * Parameters:
 *	None
 * Returns:
 *	None
 */
#if NeedFunctionPrototypes
void
RelayoutMotdList(
		    void)
#else
void
RelayoutMotdList()
#endif				/* NeedFunctionPrototypes */
{
    Boolean         found;
    Cardinal        n_lines, n_wids;
    MotdSortTreePtr treeAnchor, treePtr, htreePtr;
    MotdListPtr     listPtr = mwc.listAnchor;

    Debug("RelayoutMotdList(): entered ...");
    treeAnchor = (MotdSortTreePtr) NULL;
    while (listPtr != (MotdListPtr) NULL) {
	if (listPtr->showit == False) {
	    Debug("    skipping %s ", listPtr->filename);
	    listPtr = listPtr->next;
	    continue;
	}
	Debug("    inserting %s ", listPtr->filename);
	if ((htreePtr = (MotdSortTreePtr) malloc(sizeof(MotdSortTree)))
	    == (MotdSortTreePtr) NULL) {
	    fprintf(stderr, "%s: cannot perform malloc \n",
		    Myname);
	    exit(1);
	}

	htreePtr->wid = listPtr->motd;
	htreePtr->lines = listPtr->lines;
	htreePtr->left = htreePtr->right = (MotdSortTreePtr) NULL;
	/*
	 * if we have the first entry only assign; no
	 * sorting needed;
	 */
	if (treeAnchor == (MotdSortTreePtr) NULL) {
	    treeAnchor = htreePtr;
	    listPtr = listPtr->next;
	    continue;
	}

	treePtr = treeAnchor;
	found = False;

	while (!found) {
	    if (treePtr->lines > htreePtr->lines) {
		if (treePtr->left == (MotdSortTreePtr) NULL) {
		    treePtr->left = htreePtr;
		    found = True;
		}
		else {
		    treePtr = treePtr->left;
		}
	    }
	    else {
		if (treePtr->right == (MotdSortTreePtr) NULL) {
		    treePtr->right = htreePtr;
		    found = True;
		}
		else {
		    treePtr = treePtr->right;
		}
	    }
	}
	listPtr = listPtr->next;
    }
    n_lines = mwc.max_lines;
    n_wids = mwc.num_motds;
    ResizeAndFreeMotdList(treeAnchor, &n_lines, &n_wids);
    Debug("RelayoutMotdList(): leaving ...");
}				/* RelayoutMotdList() */


/* ----------------------------------------------------------------
 * Function Name:
 *	GenerateMotdList ( void )
 * Description:
 *	Substitute ":" with \0 in the list of motd_files
 * Parameters:
 *	None
 * Returns:
 *	None
 */
#if NeedFunctionPrototypes
void
GenerateMotdList(
		    void)
#else
void
GenerateMotdList()
#endif				/* NeedFunctionPrototypes */
{
    char           *c, *nFileName, cc;
    FILE           *fp;
    struct stat     buf;
    MotdListPtr    *hptr;
    Boolean         found;

    Debug("GenerateMotdList(): entered ...");

    mwc.num_motds = 0;
    mwc.lines_motds = 0;

    if (mwc.motdFileString == (char *) NULL) {
	/*
	 * This should never happen, for we should at least
	 * have /etc/motd
	 */
	Debug("    no files provided! \n");
	return;
    }

#ifndef HAS_NO_SETRE
    /*
     * If this code works for you, *USE IT* !!
     * It fixes a security bug to allow any user to read any
     * file using the 'userFiles' resource.
     */
    setregid(getgid(), pwd->pw_gid);
    /*
     * don't bother to undo initgroups later on
     */
    initgroups(pwd->pw_name, pwd->pw_gid);
    setreuid(getuid(), pwd->pw_uid);
#endif				/* HAS_NO_SETRE */

    for (c = mwc.motdFileString;
	 nFileName = strtok(c, ":");
	 c = (char *) NULL) {
	if (*nFileName == '\0') {
	    Debug("    null length filename found.");
	    continue;
	}

	hptr = &(mwc.listAnchor);
	found = False;
	while (*hptr != (MotdListPtr) NULL) {
	    if (strcmp((*hptr)->filename, nFileName) == 0) {
		Debug("    already had file %s",
		      (*hptr)->filename);
		found = True;
		break;
	    }
	    else
		hptr = &((*hptr)->next);
	}
	if (found)
	    continue;

	if (stat(nFileName, &buf) != NULL) {
	    fprintf(stderr, "%s: WARNING! File %s not found. \n",
		    Myname, nFileName);
	    continue;
	}

	if ((fp = fopen(nFileName, "r")) == NULL) {
	    fprintf(stderr, "%s: WARNING! File %s is unreadable. \n",
		    Myname, nFileName);
	    continue;
	}

	if ((*hptr = (MotdListPtr) malloc(sizeof(MotdList))) == NULL) {
	    fprintf(stderr, "%s - GenerateMotdList(): %s",
		    Myname, " cannot perform malloc \n");
	    fprintf(stderr, "     maybe not all files %s \n",
		    "will be handled.");
	    exit(1);
	}
	(*hptr)->next = (MotdListPtr) NULL;
	(*hptr)->filename = nFileName;
	(*hptr)->lines = 0;
	(*hptr)->label = (Widget) NULL;
	(*hptr)->motd = (Widget) NULL;

	while ((cc = getc(fp)) != EOF)
	    if (cc == '\n')
		(*hptr)->lines++;
	fclose(fp);

	Debug("    file %s has %d lines.", (*hptr)->filename,
	      (*hptr)->lines);

	(*hptr)->showit = (buf.st_mtime > mwc.ll_time)
	    || mwc.motd_always;

	if ((*hptr)->showit) {
	    mwc.num_motds++;
	    mwc.lines_motds += (*hptr)->lines;
	}
#ifndef HAS_NO_SETRE
	setreuid(getuid(), getuid());
	setregid(getgid(), getgid());
#endif				/* HAS_NO_SETRE */
    }
}				/* GenerateMotdList() */


/* ----------------------------------------------------------------
 * Function Name:
 *	MakeValidLabel ( String, Boolean*, Boolean )
 * Description:
 *	Want to have a secure string for my sprintf().
 *	Allowed formats are:
 *		%% - prints %
 *		%F - substitutes to filename only
 *		%P - substitutes to full pathname of file
 *	All other are changed to %% ans result in printed %, with
 *	exception of %\0 which results in ' '\0
 * Parameters:
 *	- s		string to check
 * Returns:
 *	Boolean, set to whether it was %F (False) or %P (True)
 */
#if NeedFunctionPrototypes
Boolean
MakeValidLabel(
		  String s)
#else
Boolean
MakeValidLabel(s)
    String          s;
#endif				/* NeedFunctionPrototypes */
{
    char           *hc;
    char           *c;
    Boolean         had_it;
    Boolean         use_path;

    Debug("MakeValidLabel() entering ...");
    hc = s;
    had_it = False;
    use_path = True;
    while ((c = index(hc, '%')) != NULL) {
	c++;
	switch (*c) {
	case '%':
	    c++;
	    break;
	case 'P':
	    if (had_it)
		*c++ = '%';
	    else {
		had_it = True;;
		use_path = True;
		*c = 's';
	    }
	    break;
	case 'F':
	    if (had_it)
		*c++ = '%';
	    else {
		had_it = True;;
		use_path = False;
		*c = 's';
	    }
	    break;
	case '\0':
	    c[-1] = ' ';
	    break;
	default:
	    *c++ = '%';
	}
	hc = c;
    }
    Debug("MakeValidLabel() leaving ...");
    return (use_path);
}				/* MakeValidLabel() */


void
Debug(fmt, arg1, arg2, arg3, arg4, arg5)
    char           *fmt;
    int             arg1, arg2, arg3, arg4, arg5;
{
    if (pre_app_resources.debug) {
	fprintf(stderr, "Debug: ");
	fprintf(stderr, fmt, arg1, arg2, arg3, arg4, arg5);
	fprintf(stderr, "\n");
	fflush(stderr);
    }
}
