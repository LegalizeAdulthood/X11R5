/*
** Copyright (c) 1990 David E. Smyth
**
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation, advertising
** materials, and other materials related to such distribution and use
** acknowledge that the software was developed by David E. Smyth.  The
** name of David E. Smyth may not be used to endorse or promote products
** derived from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/

/******************************************************************************
** SCCS_data: @(#)WcName.c 1.1 ( 19 Nov 90 )
**
** Description:	Implements several name-to-widget and widget-to-name
**		functions which are generally useful, especially a name 
**		to widget function which really works:
**
**			Widget WcFullNameToWidget(char* widget_name)
**
**		The widget names understood by WcFullNameToWidget() are
**		a superset of those understood by XtNameToWidget in
**		the X11R4 version of libXt.  XtNameToWidget only knows
**		how to find children of a reference widget, and so the
**		names start _below- the refernce widget.  However,
**		WcFullNameToWidget knows how to find widgets anywhere
**		in the widget tree: it normally starts the name search
**		from the root of the widget tree, but it can also
**		perform the name search relatively from the reference
**		widget, both up and down the widget tree.
**
**		When name searches start at the root of the widget
**		tree, the same syntax as that understood by Xrm is
**		used.  Below are four examples of acceptable names:
**
**			*foo			*foo.XmRowColumn*glorp
**			Mri.some*other.foo	*Form.glorp
**
**		Note that components may be class names, such as
**		XmRowColumn, or may be instance names of the widgets.
**		Ambiguous names are resolved exactly as done by 
**		XtNameToWidget: shallowest wins, `.' binds tighter
**		than `*', instance names bind tighter than class
**		names.
**
**		In addition to resolving names from the root of the
**		widget tree, WcFullNameToWidget also can find widget
**		using a relative root prefix.  Three special characters
**		are used:
**
**			^	means "parent"
**			~	means the closest shell ancestor
**			.	means start at the reference widget
**                              (missing prefix implies ".")
**
**              Note:
**              For compatibility with the Xmu resource converter, the
**              specification starting with pure name start at widget's
**              parent, thus "name" = "^name". Starting at the current
**              widget has become impossible, as the X11R5 Xt has a
**              bug - unitialized popup list when the converter is.
**
**		The relative root prefix characters are exactly that:
**		a prefix of a name which will then be passed to
**		XtNameToWidget.  Some examples:
**
**			.foo	a child of the reference widget
**			^foo	a sibling of the reference widget
**			^^foo	a sibling of the ref' widgets's parent
**			~foo	a child of the shell ancestor.
**			~~*foo	some child of the shell's shell ancestor.
**
**		The ^ and ~ prefix characters are only valid at the 
**		beginning.  They effectively operate on the reference
**		widget.
**
**		In all cases, the characters are scanned left to right.
**		So, the first character is acted upon, then the second,
**		and so on.
**		
** Notes:	Most of the "private" part of this file goes away when 
**		the bug in the Xt Intrinsics is fixed which causes 
**		XtNameToWidget() to dump core whenever a Gadget exists 
**		in the widget heirarchy...
**
******************************************************************************/

/******************************************************************************
* Include_files.
******************************************************************************/

#include <ctype.h>      /* isupper() and tolower macros */

/*  -- X Window System includes */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/*  -- Widget Creation Library includes */
#include "WcCreate.h"
#include "WcCreateP.h"

/*
*******************************************************************************
* Private_data_definitions.
*******************************************************************************
*/

/* shared error message and name buffers
*******************************************************************************
** NOTE: These are shared arrays because they are large: i.e.,
** this is a performacne optimization intended to reduce page
** faults which can occur while making large extensions to the
** stack space.  Wait a minute: wouldn't this just happen
** once, and then the stack space is alloc'd?  Yes, but a
** huge stack space runs the risk of getting swapped, which causes
** page faults.  This is probably a nit-picky sort of optimization.
** Remember that D.Smyth is an old sys programmer from the stone
** ages, and pity him instead of flaming him.
** Be careful when filling msg not to call any funcs in here,
** so the message does not get garbled.
*/

static char     msg[MAX_ERRMSG];
static char     cleanName[MAX_PATHNAME];

/* Private Data involving the root widget list
*******************************************************************************
*/

static int    numRoots = 0;
static Widget rootWidgets[MAX_ROOT_WIDGETS];

/*
*******************************************************************************
* Private_function definitions.
*******************************************************************************

/*
    The following implements XtNameToWidget() in a way which really works.

    Note: the #if defined... assumes a FIXED version of R4.  The version
    even as of 19 June 1990 still is not correct (dumps core on Gadgets).
    Note: even with R5 we get into troubles here ....
*/

#if defined(XtSpecificationRelease) && XtSpecificationRelease > 4

Widget WcChildNameToWidget( ref, name )  /* was XtNameToWidget */
    Widget ref;
    char*  name;
{
    return XtNameToWidget( ref, name );
}

#else

/* NOTE: The Motif 1.0 XtNameToWidget is broken: it cannot find
** names with wild cards.  The R4 XtNameToWidget is also broken: it
** cannot handle encounters with Gadgets.
**
** Below is the code extracted from the X11R4 distribution, with very
** minor changes to make it independent from the rest of the R4 Intrinsics, 
** and to fix the bug in encountering Gadgets.
** 
** Fixes: Added the two lines following this comment block.
**	  Renamed XtNameToWidget to WcChildNameToWidget to avoid warning.
**	  Removed "register" from arg type decls, as dbx does these
**	  incorrectly, and a decent compiler (gcc) does this anyway.
** -->	  Before looking for children, see if a widget is a gadget.
**	     Gadgets can't have children, in fact those fields are
**	     something else entirely!!!
**
** Here is the Copyright notice in the X11R4 files from which the
** following section of code, through the line containing
**		#endif from X11R4
***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************
*/

/*************** Begin source from X11R4 Xtos.h ***************/

#ifndef ALLOCATE_LOCAL
#define ALLOCATE_LOCAL(size) XtMalloc((unsigned long)(size))
#define DEALLOCATE_LOCAL(ptr) XtFree((caddr_t)(ptr))
#endif /* ALLOCATE_LOCAL */

/*************** End source from X11R4 Xtos.h ***************/

#define _XtAllocError		XtError

/*************** Begin source from X11R4 lib/Xt/Intrinsics.c ***************/

static Widget NameListToWidget();

typedef Widget (*NameMatchProc)();

static Widget MatchExactChildren(names, bindings, children, num,
        in_depth, out_depth, found_depth)
    XrmNameList     names;
    XrmBindingList  bindings;
    WidgetList children;
    int num;
    int in_depth, *out_depth, *found_depth;
{
    Cardinal   i;
    XrmName    name = *names;
    Widget w, result = NULL;
    int d, min = 10000;

    for (i = 0; i < num; i++) {
        if (name == children[i]->core.xrm_name) {
            w = NameListToWidget(children[i], &names[1], &bindings[1],
                    in_depth+1, &d, found_depth);
            if (w != NULL && d < min) {result = w; min = d;}
        }
    }
    *out_depth = min;
    return result;
}

static Widget MatchWildChildren(names, bindings, children, num,
        in_depth, out_depth, found_depth)
    XrmNameList     names;
    XrmBindingList  bindings;
    WidgetList children;
    int num;
    int in_depth, *out_depth, *found_depth;
{
    Cardinal   i;
    Widget w, result = NULL;
    int d, min = 10000;

    for (i = 0; i < num; i++) {
        w = NameListToWidget(children[i], names, bindings,
                in_depth+1, &d, found_depth);
        if (w != NULL && d < min) {result = w; min = d;}
    }
    *out_depth = min;
    return result;
}

static Widget SearchChildren(root, names, bindings, matchproc,
        in_depth, out_depth, found_depth)
    Widget root;
    XrmNameList     names;
    XrmBindingList  bindings;
    NameMatchProc matchproc;
    int in_depth, *out_depth, *found_depth;
{
    Widget w1, w2;
    int d1, d2;

    if (!XtIsWidget(root)) {
	*out_depth = 10000;	/* I don't know what this should be */
	return (Widget)NULL;
    }
    if (XtIsComposite(root)) {
        w1 = (*matchproc)(names, bindings,
                ((CompositeWidget) root)->composite.children,
                ((CompositeWidget) root)->composite.num_children,
                in_depth, &d1, found_depth);
    } else d1 = 10000;
    w2 = (*matchproc)(names, bindings, root->core.popup_list,
            root->core.num_popups, in_depth, &d2, found_depth);
    *out_depth = (d1 < d2 ? d1 : d2);
    return (d1 < d2 ? w1 : w2);
}

static Widget NameListToWidget(root, names, bindings,
        in_depth, out_depth, found_depth)
    Widget root;
    XrmNameList     names;
    XrmBindingList  bindings;
    int in_depth, *out_depth, *found_depth;
{
    Widget w1, w2;
    int d1, d2;

    if (in_depth >= *found_depth) {
        *out_depth = 10000;
        return NULL;
    }

    if (names[0] == NULLQUARK) {
        *out_depth = *found_depth = in_depth;
        return root;
    }

    if (*bindings == XrmBindTightly) {
        return SearchChildren(root, names, bindings, MatchExactChildren,
                in_depth, out_depth, found_depth);

    } else {    /* XrmBindLoosely */
        w1 = SearchChildren(root, names, bindings, MatchExactChildren,
                in_depth, &d1, found_depth);
        w2 = SearchChildren(root, names, bindings, MatchWildChildren,
                in_depth, &d2, found_depth);
        *out_depth = (d1 < d2 ? d1 : d2);
        return (d1 < d2 ? w1 : w2);
    }
} /* NameListToWidget */

Widget WcChildNameToWidget( ref, name )  /* was XtNameToWidget */
    Widget ref;
    char*  name;
{
    XrmName *names;
    XrmBinding *bindings;
    int len, depth, found = 10000;
    Widget result;

    len = strlen(name);
    if (len == 0) return NULL;

    names = (XrmName *) ALLOCATE_LOCAL((unsigned) (len+1) * sizeof(XrmName));
    bindings = (XrmBinding *)
        ALLOCATE_LOCAL((unsigned) (len+1) * sizeof(XrmBinding));
    if (names == NULL || bindings == NULL) _XtAllocError("alloca");

    XrmStringToBindingQuarkList(name, bindings, names);
    if (names[0] == NULLQUARK) {
        DEALLOCATE_LOCAL((char *) names);
        DEALLOCATE_LOCAL((char *) bindings);
        return NULL;
    }

    result = NameListToWidget(ref, names, bindings, 0, &depth, &found);

    DEALLOCATE_LOCAL((char *) names);
    DEALLOCATE_LOCAL((char *) bindings);
    return result;
} /* WcChildNameToWidget */

/*************** End of source from X11R4 lib/Xt/Intrinsics.c ***************/
#endif /* from X11R4 */

/******************************************************************************
** Public functions
******************************************************************************/

/*******************************************************************************
** Allocate and return a lower case copy of the input string.
** Caller must free output string!
*******************************************************************************/

char* WcLowerCaseCopy( in )
    char* in;
{
    char* retVal = (char*)XtMalloc( 1 + strlen ( in ) );
    char* cp = retVal;

    while (*in)
    {
	*cp = (isupper(*in) ? tolower(*in) : *in );
	cp++ ; in++ ;
    }
    *cp = NUL;
    return retVal;
}

/******************************************************************************
**  Return "clean" widget name, resource, or value from string
*******************************************************************************
    This function strips leading and trailing whitespace from the
    passed in char*.  Note that the caller must allocate and free
    the returned character buffer.
******************************************************************************/

char* WcSkipWhitespace( cp )
    char* cp;
{
    while ( *cp && *cp <= ' ' )
        cp++;
    return cp;
}

char* WcSkipWhitespace_Comma( cp )
    char* cp;
{
    while ( *cp && *cp <= ' ' )		/* cp = WcSkipWhitespace ( cp ); */
        cp++;
    if ( *cp == ',' )
        cp++;
    return cp;
}

char* WcCleanName( in, out )
    char* in;
    char* out;
{
    /* copy from in[] into out[],
    ** ignore initial whitespace,
    ** stop at trailing whitespace or comma.
    ** Returns pointer to whitespace or comma following name.
    */
    while ( *in && *in <= ' ' )		/* in = WcSkipWhitespace( in ); */
	in++;
    for ( ; (*in > ' ' && *in != ',') ; in++ )
        *out++ = *in;
    *out = NUL;
    return in;  /* this points at 1st whitespace or comma following "out" */
}

/* This function is necessary because XtNameToWidget cannot really
** take a widget name which begins at the top level shell, but rather
** only names which pretend the widget BELOW the top level shell is
** the top level shell.  I have no idea why some thought the
** Xt implementation is correct.  Quoting from the Xt manual:
**
**   XtNameToWidget returns the descendent [of root] ... according to
**   the following rules, ... :
**
**   o	... qualifying the name of each object with the names of all
**	its ancestors up to _but_not_including_ the reference widget.
**
** Since this is not useful for our purposes, we need to first do some 
** screwing around to see if the user specified the widget name like one 
** would specify any other widget name in the resource file.
*/

char* WcStripWhitespaceFromBothEnds( name )
    char* name;
{
    char* first; 
    char* last;
    char* buff;
    char* bp;

    for ( first = name ; *first <= ' ' ; first ++ )
	;
    for ( last = first ; *last ; last++ )
	;
    for ( last-- ; *last <= ' ' ; last-- )
	;
    buff = (char*)XtMalloc( (last - first) + 2 );
    for ( bp = buff ; first <= last ; bp++, first++ )
	*bp = *first;
    *bp = NUL;

    return buff;
}

/*
    -- Find the named widget
*******************************************************************************

    This function uses WcChildNameToWidget to search a widget tree for a
    widget with the given name.  WcChildNameToWidget is basically
    XtNameToWidget from X11R4, but hacked so it works when there are
    Gadgets in the widget tree.

    WcChildNameToWidget, like XtNameToWidget, starts searching for children
    of a reference widget.  WcFullNameToWidget examines the first few
    characters of the `name' argument in order to determine which widget is
    the reference whidget where the search will begin.

    The possibilities are these:  The `name' can begin with one or more of
    the relative prefix characters: ^ or ~, which means the reference
    widget will be some relative node in the widget tree, such as the
    parent or the shell ancestor.  Otherwise, the  root widget will be the
    starting point.
*/

Widget WcFullNameToWidget( w, name )
    Widget w;
    char*  name;
{
    Widget retWidget;
    char  *widgetName;
    char  *lowerName;
    int    i;

    widgetName = WcStripWhitespaceFromBothEnds( name );	/* must be XtFree'd */

    if ( widgetName[0] == '*' )
    {
	retWidget = WcChildNameToWidget( WcRootWidget(w), widgetName );
	XtFree( widgetName );
	return retWidget;
    }

    if (widgetName[0] == '^' 
     || widgetName[0] == '~' 
     || widgetName[0] == '.')
    {
	i = 0;
	while (widgetName[i] == '^' 	/* parent */
	    || widgetName[i] == '~' 	/* shell ancestor */
	    || widgetName[i] == '.')	/* eaten and ignored */
	{
	    if (widgetName[i] == '^')
	    {
                Widget parent = XtParent(w);
                if (parent)
                {
                   w = parent;
                }
                else 
                {
                sprintf( msg,
                    "WcFullNameToWidget cannot convert `%s' to widget \n\
	    Problem: Widget name starts with '^', but widget has no parent",
                    widgetName );
                XtWarning( msg );
                XtFree (widgetName);
                return NULL;
                }
	    }
	    else if (widgetName[i] == '~')
	    {
		/* There is a bug in /usr/include/X11/IntrinsicP.h, in
		** the XtIsShell() macro.  It does not parenthesize its
		** argument when it uses it.  Therefore, the extra 
		** parens are necessary here!
		*/
                if (XtParent(w))
		   while (! XtIsShell( (w = XtParent(w)) ) )
		     /* noop */ ;
	    }
	    i++;
	}
	if (widgetName[i] == '\0')
	    retWidget = w;
	else
	    retWidget = WcChildNameToWidget( w, &(widgetName[i]) );
	XtFree( widgetName );
	return retWidget;
    }

    lowerName  = WcLowerCaseCopy( widgetName );   /* must be XtFree'd */

    if ( 0 == strcmp( "this", lowerName ) )
    {
	XtFree( widgetName );
	XtFree( lowerName  );
	return w;
    }

    /* To obey Xrm naming conventions, "name" should mean ".name"
    ** But the Xmu converter takes "name" as "^name". I think Xmu is wrong
    ** To make it worse, when the widget is being created the num_popups
    ** seems to be bogus when the converter is called (X11R5/Motif1.1.3) 
    */
    if ( (retWidget = WcChildNameToWidget(XtParent(w), widgetName) ) )
       {
          XtFree( widgetName );
          return ( retWidget );
       }

    /* Apparently, the widget name starts with a name.  We need to find
    ** which root widget has this name.  We need to go down the list of
    ** root widgets maintained by WcRootWidget().
    */

    {
	Widget	root;
	Widget  root_of_w;
	char*	rootName;
	char*	lowerRootName;
	int     widgetNameLen = strlen(lowerName);
	int	rootNameLen;
	int	startsWithRootName;
	char*	stripped;

	/* most of the time, a widget names something else in its
	** own widget heirarchy.  Therefore, see if the naming starts
	** at the root widget of `w' but don't check that widget again.
	*/
	root_of_w = root = WcRootWidget( w ) ;
	i = -1;

	while(1)
	{
	    rootName = XrmQuarkToString( root->core.xrm_name );
	    lowerRootName = WcLowerCaseCopy( rootName );       /* XtFree this */
	    rootNameLen = strlen( lowerRootName );
	    startsWithRootName = !strncmp(lowerName,lowerRootName,rootNameLen);

	    if ( startsWithRootName && widgetName[rootNameLen] == '*' )
	    {
	        /* the root widget name is followed by a `*' so strip the
		** root name, but keep the star as it implies loose binding.
		*/
		stripped = &widgetName[rootNameLen];
                retWidget = WcChildNameToWidget( root, stripped );
		XtFree( widgetName    );
		XtFree( lowerName     );
		XtFree( lowerRootName );
		return retWidget;
	    }

	    else if ( startsWithRootName && widgetName[rootNameLen] == '.' )
	    {
		/* the root widget name is followed by a `.' so strip the
		** root name and the period to imply tight binding.
		*/
		stripped = &widgetName[++rootNameLen];
		retWidget = WcChildNameToWidget( root, stripped );
		XtFree( widgetName    );
		XtFree( lowerName     );
		XtFree( lowerRootName );
		return retWidget;
	    }

	    else if ( startsWithRootName && (widgetNameLen == rootNameLen) )
	    {
		/* widgetName is the root widget. */
		XtFree( widgetName    );
                XtFree( lowerName     );
                XtFree( lowerRootName );
                return root;
	    }

	    /* Did not find the root name.  Try the next, but skip the
	    ** root_of_w which we checked first.
	    */
	    if (++i == numRoots)
		break;
	    if (root_of_w == (root = rootWidgets[i]) )
	    {
		if (++i == numRoots)
		    break;
	        root = rootWidgets[i];
	    }
	}

	/* Completely unsucessful in parsing this name. */
	sprintf( msg,
	    "WcFullNameToWidget cannot convert `%s' to widget \n\
	     Problem: Widget name must start with `*' or `.' or `~' or `^'\n\
	              or `<aRoot>*' or `<aRoot>.' or be `this'" ,
	     widgetName );
	XtWarning( msg ); 

	XtFree( widgetName );
	XtFree( lowerName );
	XtFree( lowerRootName );
	return NULL;
    }
}

/*
    -- Names to Widget List
******************************************************************************
    This routine converts a string of comma separated widget names
    (or widget paths) into a list of widget id's. Blank space ignored.
    If a NULL string is provided, NULL is put on the list.

    The return value is the list of names which could NOT be
    converted.  Note that this list is fixed size, and is re-used.
*/

char* WcNamesToWidgetList ( w, names, widget_list, widget_count )
    Widget      w;                  /* reference widget */
    char*       names;              /* string of widget names */
    Widget      widget_list[];      /* returned widget list */
    int	       *widget_count;       /* in widget_list[len], out widget count */
{
    static char ignored[MAX_XRMSTRING];
    char*	next   = names;
    int		max    = *widget_count;

/*  -- parse the input names "widgetpath [, widgetpath] ..." */
    ignored[0] = NUL;
    *widget_count = 0;

    do 
    {
	next = WcCleanName ( next, cleanName );

	if ( widget_list[*widget_count] = WcFullNameToWidget ( w, cleanName ) )
	    (*widget_count)++;
	else
	{
	    if (ignored[0] == NUL)
		strcpy(ignored, cleanName);
	    else
	    {
		strcat(ignored, ", ");
		strcat(ignored, cleanName);
	    }
	}
	next = WcSkipWhitespace_Comma ( next );

    } while ( *next && *widget_count < max) ;

    return ignored;
}

/*
    -- WidgetToFullName
*******************************************************************************
    Traverse up the widget tree, sprintf each name right up to
    the root of the widget tree.  sprintf the names to buffer.  Use
    recursion so order of names comes out right.  Client MUST free
    the char string alloc's and returned by WcWidgetToFullName().

    Note: If using the Motif widget set, it is likely (almost inavoidable)
    that the "widget" may actually be a Gadget.  Well, Gadgets don't have
    many things, particularly a core.name member.  Therefore, if using
    Motif we must check to see if the "widget" is not actually an XmGadget.
    If is it, then we must use XrmQuarkToString(w->core.xrm_name) rather
    than core.name (unfortunately).  I'd rather not use the xrm name because
    the case has been flattened: everything is lower case.  Name something
    SomeComplexLongName and you get back somecomplexlongname.  The case
    is always insignificant, but the mixed case name is easier to read.
*/

static char* nextChar;

static int FullNameLen( w )
    Widget w;
{
    int len;

    if ( XtIsWidget(w) )
	len = 1 + strlen ( w->core.name );
    else
	len = 1 + strlen ( XrmQuarkToString(w->core.xrm_name) );

    if (w->core.parent)
	len += FullNameLen(w->core.parent);
    return len;
}

static void WidgetToFullName( w )
    Widget w;
{
    char* cp;

    if (w->core.parent)
    {
        WidgetToFullName( w->core.parent );	/* nextChar AFTER parent name */
	*nextChar++ = '.';			/* inter-name `dot' */
    }

    if ( XtIsWidget(w) )
        cp = w->core.name;
    else
	cp = XrmQuarkToString(w->core.xrm_name);

    while (*cp)
	*nextChar++ = *cp++;
}

char* WcWidgetToFullName( w )
    Widget w;
{
    char* buff = XtMalloc( FullNameLen( w ) );

    nextChar = buff;

    WidgetToFullName( w );
    *nextChar = NUL;
			
    return buff;
}

/*
    -- Search widget's resource list for resource_type
*******************************************************************************
    Gets the XtResourceList from the widget, searches the list for
    the resource name to determine the type required, which is then
    returned to the caller.
*/

char* WcGetResourceType( w, res_name )
    Widget w;
    char*  res_name;
{
    XtResource* res_list;
    int         i, num;
    char*	retstr;

    XtGetResourceList( w->core.widget_class, &res_list, &num );

    for ( i = 0 ; i < num ; i++ )
    {
        if (0 == strcmp( res_name, res_list[i].resource_name) 
	 || 0 == strcmp( res_name, res_list[i].resource_class) )
	{
            retstr = XtNewString(res_list[i].resource_type);
	    XtFree( res_list );
	    return retstr;
	}
    }

    w = XtParent( w );
    if (XtIsConstraint( w ))
    {
	XtGetConstraintResourceList( w->core.widget_class, &res_list, &num );

	for ( i = 0 ; i < num ; i++ )
	{
	    if (0 == strcmp( res_name, res_list[i].resource_name)
	     || 0 == strcmp( res_name, res_list[i].resource_class) )
            {
                retstr = XtNewString(res_list[i].resource_type);
                XtFree( res_list );
                return retstr;
            }
	}
    }

    return NULL;
}

/*
    -- Convert resource value from string to whatever the widget needs
*******************************************************************************
    Gets the XtResourceList from the widget, searches the list for
    the resource name to determine the type required, then uses the
    resource manager to convert from string to the required type.
    Calls XtSetValue with converted type.

    Note that if the widget does not have the specified resource
    type, it is not set.  WcGetResourceType() checks for both
    widget resources and constraint resources.

    Note also that no converter-failed behavior is necessary,
    because converters generally give their own error messages.
*/

void WcSetValueFromString( w, res_name, res_val )
    Widget w;		 /* MUST already be init'd */
    char*  res_name;
    char*  res_val;	/* NUL terminated, should NOT have whitespace */
{
    char*	res_type;	/* must be XtFree'd */

    if ( res_type = WcGetResourceType( w, res_name ) )
    {
	/* This widget does know about this resource type */
	WcSetValueFromStringAndType( w, res_name, res_val, res_type );
    }
    XtFree( res_type );
}

void WcSetValueFromStringAndType( w, res_name, res_val, res_type )
    Widget w;
    char*  res_name;
    char*  res_val;
    char*  res_type;
{
    XrmValue    fr_val;
    XrmValue    to_val;
    Arg		arg[1];

    fr_val.size = strlen(res_val) + 1;
    fr_val.addr = (caddr_t)res_val;
    to_val.size = 0;
    to_val.addr = NULL;
    XtConvert(
            w,		/* the widget */
            XtRString,	/* from type */
            &fr_val,	/* from value */
            res_type,	/* to type */
            &to_val		/* the converted value */
    );

    if (to_val.addr)
    {
        /* Conversion worked.  */
	if ( 0 == strcmp(res_type, "String"))
	    XtSetArg( arg[0], res_name, to_val.addr );
	else
	{
	    switch(to_val.size)
            {
            case sizeof(char):
                XtSetArg( arg[0], res_name, *(char*)to_val.addr );
                break;
            case sizeof(short):
                XtSetArg( arg[0], res_name, *(short*)to_val.addr );
                break;
            case sizeof(int):
                XtSetArg( arg[0], res_name, *(int*)to_val.addr );
                break;
            default:
	        XtSetArg( arg[0], res_name, to_val.addr );
            }
	}
        XtSetValues( w, arg, 1 );
    }
}

/*
*******************************************************************************
* Private Data involving the root widget list, declared at top of this file
*	static int numRoots = 0;
*	static Widget rootWidgets[MAX_ROOT_WIDGETS];
*******************************************************************************
*/


/*
    -- Forget about a root widget
*******************************************************************************
    When a root widget gets destroyed, we need to take that widget out
    of our list of root widgets.  This is a destroy callback routine
    which is added to a root widget's destroy callback list by WcRootWidget.
*/

static void ForgetRoot ( w, client, call )
    Widget  w;
    caddr_t client;
    caddr_t call;
{
    int i;
    for (i = 0 ; i < numRoots ; i++ )
    {
        if ( w == rootWidgets[i] )
	{
	    /* move all following widgets up to close the gap */
	    for ( ; i < numRoots ; i++ )
	    {
		rootWidgets[i] = rootWidgets[i+1];
	    }
	    numRoots-- ;
	    return;
	}
    }
    /* should never get here */
}

/*
    -- Find root widget
*******************************************************************************
    If a widget is passed, then find the root of that widget.  See if
    it is one of the root widgets we already know about.  Add to list
    if not.  Return the root widget.

    If no widget is passed, then return the first root widget we know
    about.  If we know of no root widgets, then we will return a NULL
    since the rootWidgets[] array starts out filled with nulls, and
    gets re-filled as roots are destroyed.
*/

Widget WcRootWidget( w )
    Widget w;
{
    int i;

    if (w)
    {
	while ( XtParent(w) )
	    w = XtParent(w);

	for (i = 0 ; i < numRoots ; i++)
	{
	    if ( w == rootWidgets[i] )
		return w;
	}

	rootWidgets[i] = w;
	numRoots++;
	XtAddCallback( w, XtNdestroyCallback, ForgetRoot, NULL );
	return w;
    }
    else
    {
	return rootWidgets[0];
    }
}

/*
   -- Equivalent to ANSI C library function strstr()
*******************************************************************************
   This function is only necessary on systems which do not have
   ANSI C libraries.  Soon, it looks like everybody will have
   such libraries, what with the recent SVR4 and OSF efforts
   to include everything for everybody.  In the meantime,
   this will always be included in the library.  Why not put
   #ifdef's around it?  because the problem really arises not
   when the library is built, but when applications are built.
   I can't very well require all application writers in the
   world to know what this library uses...
*/

char* WcStrStr( s1, s2 )
    char* s1;
    char* s2;
{
    while (*s1)
    {
	if (*s1 == *s2)
	{
	    char* start = s1;
	    char* c = s2;
	    while (*++s1 & *++c && *s1 == *c)
		;
	    if (*c == '\0')
		return start;
	    else
		s1 = ++start;
	}
	else
	{
	    s1++ ;
	}
    }
    return (char*)0;
}
