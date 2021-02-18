/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

extern int     zoomscale;
extern int     zoomxoff;
extern int     zoomyoff;

#define ZOOMX(x) (zoomscale*(x-zoomxoff))
#define ZOOMY(y) (zoomscale*(y-zoomyoff))
#define BACKX(x) ((int) (x/(float)zoomscale+zoomxoff+0.5))
#define BACKY(y) ((int) (y/(float)zoomscale+zoomyoff+0.5))

#define zXDrawPoint(d,w,gc,x,y) XDrawPoint(d,w,gc,ZOOMX(x),ZOOMY(y))
#define zXDrawArc(d,w,gc,x,y,d1,d2,a1,a2)\
    XDrawArc(d,w,gc,ZOOMX(x),ZOOMY(y),zoomscale*(d1),zoomscale*(d2),\
	      a1,a2)
#define zXFillArc(d,w,gc,x,y,d1,d2,a1,a2)\
    XFillArc(d,w,gc,ZOOMX(x),ZOOMY(y),zoomscale*(d1),zoomscale*(d2),\
	      a1,a2)
#define zXDrawLine(d,w,gc,x1,y1,x2,y2)\
    XDrawLine(d,w,gc,ZOOMX(x1),ZOOMY(y1),ZOOMX(x2),ZOOMY(y2))
#define zXDrawString(d,w,gc,x,y,s,l)\
    XDrawString(d,w,gc,ZOOMX(x),ZOOMY(y),s,l)
#define zXFillRectangle(d,w,gc,x1,y1,x2,y2)\
    XFillRectangle(d,w,gc,ZOOMX(x1),ZOOMY(y1),zoomscale*(x2),zoomscale*(y2))
#define zXDrawRectangle(d,w,gc,x1,y1,x2,y2)\
    XDrawRectangle(d,w,gc,ZOOMX(x1),ZOOMY(y1),zoomscale*(x2),zoomscale*(y2))
#define zXDrawLines(d,w,gc,p,n,m)\
    {int i;\
     for(i=0;i<n;i++){p[i].x=ZOOMX(p[i].x);p[i].y=ZOOMY(p[i].y);}\
     XDrawLines(d,w,gc,p,n,m);\
     for(i=0;i<n;i++){p[i].x=BACKX(p[i].x);p[i].y=BACKY(p[i].y);}\
    }

#define zXFillPolygon(d,w,gc,p,n,m,o)\
    {int i;\
     for(i=0;i<n;i++){p[i].x=ZOOMX(p[i].x);p[i].y=ZOOMY(p[i].y);}\
     XFillPolygon(d,w,gc,p,n,m,o);\
     for(i=0;i<n;i++){p[i].x=BACKX(p[i].x);p[i].y=BACKY(p[i].y);}\
    }
