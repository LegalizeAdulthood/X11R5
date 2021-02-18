#ifndef lint
static char *rcsid = "$Header: /usr3/Src/emu/canvas/RCS/motion.c,v 1.9 91/09/23 17:37:11 jkh Exp Locker: me $";
#endif

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Functions for parsing and executing motion state machines.
 *
 * Author: Jordan K. Hubbard
 * Date: July 29th, 1991.
 * Description: Routines for handling movement within the canvas in
 *		a user-configurable fashion.
 *
 * Revision History:
 *
 * $Log:	motion.c,v $
 * Revision 1.9  91/09/23  17:37:11  jkh
 * *** empty log message ***
 * 
 * Revision 1.8  91/08/16  07:59:48  jkh
 * Michael's changes to make this stuff actually work.
 * 
 * Revision 1.7  91/08/05  13:55:22  me
 * Fixed canvasDoMotion and getMotion
 * 
 * Revision 1.6  91/08/05  10:09:00  me
 * Jordan's futile tries to get it working
 * 
 * Revision 1.5  91/08/02  18:07:21  jkh
 * Changed MOTION_CONTINUE to more proper MOTION_CONT;  My brain is asleep.
 * 
 * Revision 1.4  91/08/02  17:54:46  jkh
 * Added the idea of explicit MOTION_CONTINUE instead of blind character
 * reads.  Added '&' as a synonym for '|'.
 * 
 * Revision 1.3  91/08/02  16:03:08  jkh
 * Added the concept of special attribute masks, tightened up the code
 * and added more error checking.
 * 
 * Revision 1.2  91/08/01  19:57:20  me
 * More elaborate motion description stuff
 * 
 * Revision 1.1  91/08/01  17:47:40  jkh
 * Initial revision
 * 
 *
 */

#include "canvas.h"
#include "bitstring.h"

/* How many bits to reserve for characters */
#define C_SIZE		256

#define WHITE	"\n\t\r \014"
#define PUNCT	"!@#$%^&*()_-+={}[]:;'|\\~`'<>,./?\042"
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890"

struct _motion {
     bit_decl(pat, C_SIZE);
     unsigned int mask;
     int yes, no;
};
typedef struct _motion Motion, *MotionPtr;

struct _motionList {
     struct _motionList *next;
     String name;
     MotionPtr m;
};
typedef struct _motionList MotionList, *MotionListPtr;

Local MotionPtr getMotion();
Local MotionPtr findMotion();
Local String getEntry();

/*
 * cfunc takes a parameter: ScreenCursorPtr scpt
 * that tells it from where to move.  It returns the new position there too.
 */
Export ScreenCursor
canvasDoMotion(w, name, cfunc, sc)
TermCanvasWidget w;
String name;
int (*cfunc)();
ScreenCursor sc;
{
     MotionPtr m;
     ScreenCursor actsc;
     ScreenCursor retsc;
     int attr,ch, act_pos = 0, new_pos = 0;

     actsc = retsc = sc;
     
     m = getMotion(w, name);
     if (!m)
	  warn("Couldn't find motion type '%s'", name);
     else {
	  ch = (*cfunc)(w, &sc, &attr);
	  while (new_pos != MOTION_STOP && ch != EOF) {
	       if (ch >= 0) {
		    if (bit_test(m[act_pos].pat, ch) && (!m[act_pos].mask ||
			(attr & m[act_pos].mask)))
			 new_pos = m[act_pos].yes;
		    else
			 new_pos = m[act_pos].no;
		    if (new_pos >= 0)
			 act_pos = new_pos;
	       }
	       if (new_pos == MOTION_CONT) {
		    retsc = actsc;
		    actsc = sc;
		    ch = (*cfunc)(w, &sc, &attr);
	       }
	  }
     }
     return retsc;
}

Export void
canvasFreeMotions(w)
TermCanvasWidget w;
{
     MotionPtr m;
     MotionListPtr q;

     q = (MotionListPtr)w->term.motionHead;
     while (q) {
	  MotionListPtr next = q->next;

	  XtFree((caddr_t)q->name);
	  XtFree((caddr_t)q->m);
	  XtFree((char *)q);
	  q = next;
     }
}

Local String
skip_space(s)
String s;
{
     while (isspace(*s))
	  ++s;
     return s;
}

Local MotionPtr
findMotion(w, mtype)
TermCanvasWidget w;
String mtype;
{
     MotionListPtr p = (MotionListPtr) w->term.motionHead;

     while (p != NULL && strcmp(p->name, mtype) != 0) {
	  p = p->next;
     }
     if (p)
         return p->m;
     else
	 return NULL;
}

Local MotionPtr
getMotion(w, mtype)
TermCanvasWidget w;
String mtype;
{
     MotionPtr ret;
     char name[128];
     char class[128];
     String mdesc;

     ret = findMotion(w, mtype);
     if (ret == NULL) {
	  MotionListPtr p = (MotionListPtr)XtMalloc(sizeof(MotionList));

	  /* Set up the resource name */
	  sprintf(name, "motion-%s", mtype);
	  sprintf(class, "Motion-%s", mtype);
     
	  mdesc = get_sub_resource_string(w, NULL, name, class);
	  if (!mdesc) {
	       warn("getMotion: Couldn't get motion description for '%s'",
		    mtype);
	       XtFree((caddr_t)p);
	  }
	  else {
	       mdesc = skip_space(mdesc);
	       if (!*mdesc) {
		    warn("Invalid motion description for %s.", mtype);
		    XtFree((char *)p);
		    return NULL;
	       }
	       else {
		    int mcount = 0;

		    p->name = XtNewString(mtype);
		    while (mdesc && *mdesc) {
			 int i, len, yes, no;
			 unsigned int mask;
			 bit_decl(tmp, C_SIZE);

			 mdesc = getEntry(mdesc, mtype, tmp, &mask, &yes, &no);
			 if (mdesc) {
			      if (!mcount)
				   p->m = (MotionPtr)XtMalloc(sizeof(Motion));
			      else
				   p->m = (MotionPtr)XtRealloc((caddr_t)p->m,
							       sizeof(Motion)
							       * mcount);
			      bcopy(tmp, p->m[mcount].pat,
				    _bit_size(C_SIZE) * 4);
			      p->m[mcount].mask = mask;
			      p->m[mcount].yes = yes;
			      p->m[mcount].no = no;
			      ++mcount;
			 }
		    }
		    ret = p->m;
	       }
	       if (!w->term.motionHead)
		    p->next = NULL;
	       else
		    p->next = (MotionListPtr)w->term.motionHead;
	       w->term.motionHead = (Generic)p;
	  }
     }
     return ret;
}

Local String
match_token(s)
String s;
{
     static struct {
	  String from, to;
     } tokens[] = {
	  { "ALPHA",	ALPHA		},
	  { "PUNCT",	PUNCT		},
	  { "WHITE",	WHITE		},
	  { NULL, NULL			},
     };
     int i = 0;

     while (tokens[i].from) {
	  if (!strcmp(s, tokens[i].from))
	       return tokens[i].to;
	  ++i;
     }
     return NULL;
}
    
Local String
getEntry(m, name, bits, maskp, yp, np)
String m, name;
bit_ref(bits);
unsigned int *maskp;
int *yp, *np;
{
     String str;
     char token[C_SIZE];
     int len, i = 0;

     /* Start clean */
     *maskp = 0;
     bit_clearall(bits, C_SIZE);

     m = skip_space(m);
     if (!*m)
	  return NULL;
get_token:
     if (isdigit(*m)) {
	  *maskp &= atoi(m);
	  while (isdigit(*m))
		++m;
     }
     else if (*m != '"') {
	  String tmp;

	  while (*m && !isspace(*m)) {
	       token[i++] = *(m++);
	       if (i == (C_SIZE - 1)) {
		    warn("Motion entry %s too long!", name);
		    return NULL;
	       }
	  }
	  token[i] = '\0';
	  if (!strcmp(token, "all") || !strcmp(token, "ALL")) {
	       bit_setall(bits, C_SIZE);
	  }
	  else {
	       str = match_token(token);
	       if (!str) {
		    warn("Motion entry %s has bogus token '%s'", name, token);
		    return NULL;
	       }
	  }
     }
     else {
	  ++m;
	  while (*m && *m != '"') {
	       token[i++] = *(m++);
	       if (i == (C_SIZE - 1)) {
		    warn("Motion entry %s too long!", name);
		    return NULL;
	       }
	  }
	  token[i] = '\0';
	  str = backslash_convert(token);
     }
     len = strlen(str);
     for (i = 0; i < len; i++)
	  bit_set(bits, str[i]);
     m = skip_space(m);

     if (*m == '|' || *m == '&') {
	  m = skip_space(m);
	  goto get_token;
     }

     if (!(isdigit(*m) || *m == '-' || *m == '+')) {
	  warn("Missing 'yes' index for '%s' motion entry.", name);
	  return NULL;
     }
     else {
	  *yp = atoi(m);
	  while (isdigit(*m) || *m == '-' || *m == '+')
	       ++m;
	  m = skip_space(m);
	  if (!(isdigit(*m) || *m == '-' || *m == '+')) {
	       warn("Missing 'no' index for '%s' motion entry.", name);
	       return NULL;
	  }
	  else {
	       *np = atoi(m);
	       while (isdigit(*m) || *m == '-' || *m == '+')
		    ++m;
	  }
     }
     return skip_space(m);
}
