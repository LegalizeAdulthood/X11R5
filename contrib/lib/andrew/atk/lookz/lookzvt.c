/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookzvt.c,v 1.3 1991/09/12 16:24:29 bobg Exp $ */
/* $ACIS:testv.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookzvt.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookzvt.c,v 1.3 1991/09/12 16:24:29 bobg Exp $";
#endif /* lint */

/* testv.c

	test the lookzview object
*/

#include <stdio.h>

#include <andyenv.h>
#include <class.h>
#include <im.ih>
#include <view.ih>
#include <frame.ih>
#include <lpair.ih>
#include <stylesht.ih>
#include <style.ih>

#include <lookz.ih>
#include <lookzv.ih>

/* include the ones utilized, but not by this .c file itself */
#define class_StaticEntriesOnly

/* required classes */
#include <environ.ih>
#include <graphic.ih>
#include <fontdesc.ih>
#include <cursor.ih>
#ifdef WM_ENV
#include <wgraphic.ih>
#include <wim.ih>
#include <wcursor.ih>
#include <wfontd.ih>
#include <mrl.ih>
#endif /* WM_ENV */

/* classes that are anyway dynamically loaded by the
	required classes */
#include <dataobj.ih>
#include <event.ih>
#include <filetype.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <proctbl.ih>
#include <keyrec.ih>
#include <menulist.ih>
#include <message.ih>
#include <msghndlr.ih>
#include <observe.ih>
#include <updlist.ih>
#include <bind.ih>

#undef class_StaticEntriesOnly

printdata(dobj)
	register struct lookz *dobj;
{
	printf("Image is %s\n", (lookz_GetVisibility(dobj) ? "visible" : "hidden"));
	fflush(stdout);
}

static boolean
PrintAStyle(s, v)
	struct style *s;
	register struct lookzview *v;
{
	printf("%s\n", style_GetMenuName(s)); fflush(stdout);
	return FALSE;
}

/* findDefine - parse file looking for "\define{" */
boolean
findDefine(f)
	register FILE *f;
{
	register c;
	register char *s;
	while (TRUE) {
		while ((c=getc(f)) != EOF  && c != '\\') 
			{}
		if (c == EOF) return FALSE;
		s = "define{";
		while (*s && (c=getc(f)) != EOF  && c == *s++) 
			{}
		if (c == EOF) return FALSE;
		if (*s == '\0') return TRUE;
		ungetc(c, f);		
	}
}

main( argc, argv )
	register int	  argc;
	register char  **argv;
{
	register struct lookz *dobj;
	register struct lookzview *dview;
	register struct stylesheet *ss;

	struct frame *frame;
	FILE *f;
	struct im *im;

	printf("Start\n"); fflush(stdout);
	class_Init(".");		/* set up classpath */
	printf("Init done\n"); fflush(stdout);

	im_StaticEntry;
	view_StaticEntry;
	frame_StaticEntry;
	lpair_StaticEntry;
	stylesheet_StaticEntry;
	style_StaticEntry;
/* try making it dynamic
	lookz_StaticEntry;
	lookzview_StaticEntry;
*/
	environ_StaticEntry;
	graphic_StaticEntry;
	fontdesc_StaticEntry;
	cursor_StaticEntry;
#ifdef WM_ENV
	wmgraphic_StaticEntry;
	wmim_StaticEntry;
	wmcursor_StaticEntry;
	wmfontdesc_StaticEntry;
	mrl_StaticEntry;
#endif /* WM_ENV */
	dataobject_StaticEntry;
	event_StaticEntry;
	filetype_StaticEntry;
	keymap_StaticEntry;
	keystate_StaticEntry;
	proctable_StaticEntry;
	keyrec_StaticEntry;
	menulist_StaticEntry;
	message_StaticEntry;
	msghandler_StaticEntry;
	observable_StaticEntry;
	updatelist_StaticEntry;
	bind_StaticEntry;


	im_SetProgramName("lookzView");

	printf("About to New\n"); fflush(stdout);

	dobj = lookz_New();
	printdata(dobj);
	dview = lookzview_New();
	lookzview_SetDataObject(dview, dobj);

	printf("Get style\n"); fflush(stdout);

	/* get the style sheet from default.tpl */
	ss = stylesheet_New();
	f = fopen ("/usr/andy/lib/tpls/default.tpl", "r");
	if (f) while (findDefine(f)) 
			stylesheet_Read(ss, f, TRUE);
/*	stylesheet_EnumerateStyles(ss, PrintAStyle, NULL); */
	lookzview_SetStyleSheet(dview, ss);


	if((im = im_Create(NULL)) == NULL) {	/* (argument can be hostname) */
	    fprintf(stderr,"Could not create new window; exiting.\n");
	    exit(-1);
	}
	if((frame = frame_New()) == NULL) {	/* use a frame to get message line */
	    fprintf(stderr,"Could not allocate enough memory; exiting.\n");
	    exit(-1);
	}
	im_SetView(im, frame);
	frame_PostDefaultHandler( frame, "message", 
			frame_WantHandler( frame, "message" ) );

		/* in general, the application layer may have a surround (eg. scrollbar) 
			for lookzview, GetApplicationLayer returns its argument */

	printf("Frame built\n"); fflush(stdout);

	frame_SetView(frame, lookzview_GetApplicationLayer(dview));

	printf("Blastoff !!\n"); fflush(stdout);

	im_KeyboardProcessor();		/* Do it */
}
