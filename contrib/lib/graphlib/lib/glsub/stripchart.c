/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Eric Anderson 
 * Buttons and menus mostly by Jeffrey Hutzelman
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */

#include "X11/gl/stripchart.h"
#include "X11/gl/limits.h"
#include "X11/gl/resmgr.h"
#include <stdio.h>
#include "memory.h"

#define SCALE_AMOUNT(scinfo) ((float)(scinfo)->curscale/(scinfo)->winheight)
#define SCALE(x,scinfo) ((x)/SCALE_AMOUNT(scinfo))
#define STAT_TO_PIXEL(x,scinfo) ((int)((scinfo)->winheight-SCALE(x,scinfo)))
#define POS(x,scinfo) (STAT_TO_PIXEL((scinfo)->stats[(x)],scinfo))

static void GraphExposeProc(report,assoclist)
XExposeEvent *report;
GL_DataList assoclist;
{
  register int loop,xmax,ymax,tmp,tmp2;
  GL_StripChartInfo *scinfo;

  scinfo = (GL_StripChartInfo *)GL_Search_DataList(assoclist,_GL_VA_Init_StripChart);

  xmax = (report->x+report->width)>scinfo->drawpos
    ? scinfo->drawpos+1 : report->x+report->width;
  ymax = report->y+report->height;

  for(loop=report->x;loop<xmax;loop++) {
    if (scinfo->drawpos==(scinfo->winwidth-1))
      /* If wrapped. */	
      tmp = POS((loop+scinfo->curstat)%scinfo->winwidth,scinfo);
    else
      tmp = POS(loop,scinfo);
    if (tmp<ymax) {
      tmp2=report->y>tmp?report->y:tmp;
      XFillRectangle(display,report->window,scinfo->gc,loop,tmp2,1,ymax-tmp2);
    }
  }
  XFlush(display);
}

static void CopyLatest(old,new,oldw,neww)
long *old,*new;
int oldw,neww;
{
  int start,loop;

  start= neww<oldw?oldw-neww:0;

  for(loop=start;loop<oldw;loop++)
    new[loop-start]=old[loop];
}

static void Possible_Scale_Down(scinfo,window)
GL_StripChartInfo *scinfo;
Window window;
{
  int loop,newscale,oldscale=scinfo->curscale;
  
  scinfo->realmax=0;
  for (loop=0;loop<(scinfo->drawpos+1);loop++)
    if (scinfo->stats[loop]>scinfo->realmax)
      scinfo->realmax=scinfo->stats[loop];

  while((newscale=scinfo->curscale-scinfo->deltascale)>scinfo->realmax&&
	newscale>=scinfo->minscale)
   scinfo->curscale=newscale;
  if (scinfo->curscale!=oldscale) {
    XClearArea(display,window,0,0,0,0,True);
    if (scinfo->changescalemaxfunc!=NULL) 
      scinfo->changescalemaxfunc(scinfo->curscale,scinfo->multiplier);
  }
}

static void Possible_Scale_Up(scinfo,newval,window)
GL_StripChartInfo *scinfo;
int newval;
Window window;
{
  int oldmax = scinfo->curscale;

  scinfo->realmax=newval;
  while (scinfo->curscale<scinfo->realmax&&
	 scinfo->curscale<=(scinfo->maxscale-scinfo->deltascale))
    scinfo->curscale+=scinfo->deltascale;
  if (scinfo->curscale!=oldmax) {
    XClearArea(display,window,0,0,0,0,True);
    if (scinfo->changescalemaxfunc!=NULL) 
      scinfo->changescalemaxfunc(scinfo->curscale,scinfo->multiplier);
  }
}

static void GraphConfigureProc(report,assoclist)
XConfigureEvent *report;
GL_DataList assoclist;
{
  long *replace;
  int loop;
  GL_StripChartInfo *scinfo;

  scinfo = (GL_StripChartInfo *)GL_Search_DataList(assoclist,_GL_VA_Init_StripChart);
  
  if (report->width!=scinfo->winwidth||
      report->height!=scinfo->winheight)
    XClearArea(display,report->window,0,0,0,0,True);

  if (report->width!=scinfo->winwidth) {
    replace = (long *)xmalloc(report->width*sizeof(long));
    if (scinfo->drawpos!=(scinfo->winwidth-1)) {
      /* Haven't wrapped yet, just make a copy,
         make sure to copy most recent data. */
      CopyLatest(scinfo->stats,replace,scinfo->curstat,report->width);
      scinfo->drawpos = scinfo->curstat>report->width 
	? report->width-1 : scinfo->curstat-1;
    } else {
      /* Wrapped, so we need to shift things around. */
      /* For easier coding and understandability, move the data
	 so that the oldest data is in position 0, and the newest
	 is in position n */
      long *work;

      work = (long *)malloc(sizeof(long)*scinfo->winwidth);
/*    memcpy(work,&scinfo->stats[scinfo->curstat],
	     &work[scinfo->winwidth-scinfo->curstat]-work);
      memcpy(&work[loop+scinfo->winwidth-scinfo->curstat],scinfo,
	     &work[scinfo->winwidth-scinfo->curstat]-&work[scinfo->winwidth]);*/
      for (loop=scinfo->curstat;loop<scinfo->winwidth;loop++)
	work[loop-scinfo->curstat]= scinfo->stats[loop];
      for (loop=0;loop<scinfo->curstat;loop++)
	work[loop+scinfo->winwidth-scinfo->curstat]=scinfo->stats[loop];
      CopyLatest(work,replace,scinfo->winwidth,report->width);
      scinfo->drawpos = report->width<scinfo->winwidth
	? report->width-1 : scinfo->winwidth-1;
      free(work);
    }
    scinfo->curstat=(scinfo->drawpos+1)%report->width;
    loop = scinfo->winwidth;
    scinfo->winwidth = report->width;
    free(scinfo->stats);
    scinfo->stats=replace;
    if (loop>scinfo->winwidth)
      Possible_Scale_Down(scinfo,report->window);
  }
  scinfo->winheight = report->height;
  XFlush(display);
}

static void GraphUpdateProc(window)
Window window;
{
  int oldval;
  GL_StripChartInfo *scinfo;

  scinfo = (GL_StripChartInfo *)GL_Search_DataList(GL_GetDataList(window),
						   _GL_VA_Init_StripChart);
  oldval=scinfo->stats[scinfo->curstat];
  if (scinfo->drawpos==(scinfo->winwidth-1)) {
    register int tmp,amt;
    XCopyArea(display,window,window,scinfo->gc,1,0,
	      scinfo->winwidth-1,scinfo->winheight,0,0);
    tmp = scinfo->curstat==0 ? (scinfo->winwidth-1) : scinfo->curstat-1;
    amt=POS(tmp,scinfo);
    if (amt<0)
      amt=0;
    if (amt!=scinfo->winheight)
      XClearArea(display,window,scinfo->winwidth-1,
		 amt,1,scinfo->winheight-amt,False);
  }
  else
    scinfo->drawpos++;

  scinfo->stats[scinfo->curstat] = scinfo->getfunc();

  if (POS(scinfo->curstat,scinfo)!=scinfo->winheight)
    XFillRectangle(display,window,scinfo->gc,scinfo->drawpos,
		   POS(scinfo->curstat,scinfo),1,
		   scinfo->winheight-POS(scinfo->curstat,scinfo));
  if (oldval==scinfo->realmax && 
      scinfo->curscale > scinfo->minscale)
    Possible_Scale_Down(scinfo,window);

  if (scinfo->stats[scinfo->curstat]>scinfo->realmax && 
      scinfo->curscale<scinfo->maxscale)
    Possible_Scale_Up(scinfo,scinfo->stats[scinfo->curstat],window);

  scinfo->curstat++;
  scinfo->curstat%=scinfo->winwidth;
  XFlush(display);
}

void GL_ResetStripChart(window)
Window window;
{
  GL_StripChartInfo *sinfo;

  sinfo = (GL_StripChartInfo *)GL_Search_DataList(GL_GetDataList(window),
						  _GL_VA_Init_StripChart);
  sinfo->curstat=0;
  sinfo->drawpos= -1;
  sinfo->realmax=0;
  XClearWindow(display,window);
}

						    
/*****
 *
 * Varargs Functions
 *
 *****/

void _GL_VA_Init_StripChart(list)
GL_DataList list;
{
  GL_StripChartInfo *scinfo;

  _GL_VA_Init_Window(list);
  scinfo = (GL_StripChartInfo *)xmalloc(sizeof(GL_StripChartInfo));
  
  scinfo->exposefunc=GraphExposeProc;
  scinfo->drawfunc=NULL;
  scinfo->getfunc=NULL;
  scinfo->changescalemaxfunc=NULL;
  scinfo->drawpos= -1;
  /* gc initialized in execute func */
  scinfo->winheight=200;
  scinfo->winwidth=200;
  scinfo->realmax=0;
  scinfo->maxscale=0;
  scinfo->curstat=0;
  scinfo->minscale= 0;
  scinfo->maxscale= INT_MAX;
  scinfo->deltascale= 100;
  scinfo->stats= NULL;
  scinfo->interval_sec = 1;
  scinfo->interval_usec = 0;
  scinfo->multiplier = 1;
  GL_Add_To_DataList(list,_GL_VA_Init_StripChart,scinfo);
}

void _GL_VA_Handle_StripChart(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_StripChartInfo *scinfo;

  scinfo = (GL_StripChartInfo *)GL_Search_DataList(list,_GL_VA_Init_StripChart);
  
  switch (modifier) {
    case GLSC_DrawFunc   : scinfo->drawfunc = va_arg(*pvar,VPTF);break;
    case GLSC_GetFunc    : scinfo->getfunc  = va_arg(*pvar,LPTF);break;
    case GLSC_CurScale   : scinfo->curscale = va_arg(*pvar,int);break;
    case GLSC_MinScale   : scinfo->minscale = va_arg(*pvar,int);break;
    case GLSC_MaxScale   : scinfo->maxscale = va_arg(*pvar,int);break;
    case GLSC_DeltaScale : scinfo->deltascale = va_arg(*pvar,int);break;
    case GLSC_Interval_Sec   : scinfo->interval_sec = va_arg(*pvar,int);break;
    case GLSC_Interval_USec  : scinfo->interval_usec = va_arg(*pvar,int);break;
    case GLSC_ChangeScaleMaxFunc : 
      scinfo->changescalemaxfunc = va_arg(*pvar,VPTF);break;
    case GLSC_Multiplier : scinfo->multiplier = va_arg(*pvar,int);
      if (scinfo->multiplier==0) scinfo->multiplier=1;
      break;
    default : _GL_VA_Handle_Window(list,modifier,pvar);
  }
}

void _GL_VA_Execute_StripChart(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;
  GL_StripChartInfo *scinfo;
  GL_EventMgrInfo *eventmgrinfo;
  struct timeval Value;
  long updateval= -1;

  _GL_VA_Execute_Window(list);
  
  eventmgrinfo = (GL_EventMgrInfo *)GL_Search_DataList(list,_GL_VA_Init_EventMgr);
  wininfo = (GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  scinfo  = (GL_StripChartInfo *)GL_Search_DataList(list,_GL_VA_Init_StripChart);

  scinfo->winheight = wininfo->height;
  scinfo->winwidth  = wininfo->width;
  scinfo->stats = (long *)xmalloc(scinfo->winwidth*sizeof(long));
  GL_AddFuncPtr(eventmgrinfo->winid,
		Expose, scinfo->exposefunc,
		GraphicsExpose, scinfo->exposefunc,
		ConfigureNotify, GraphConfigureProc,
		GL_Done);
  GL_Create(GL_Resources,
	    GL_RES_LONG, "update","update", &updateval,
	    GL_Done);
  if (updateval != -1) {
    scinfo->interval_usec = (updateval%100)*10000;/* 1,000,000/100 */
    scinfo->interval_sec = updateval/100;
  }
  timerclear(&Value);
  Value.tv_sec = scinfo->interval_sec;
  Value.tv_usec = scinfo->interval_usec;
  GL_SetTimeOut(&Value,GraphUpdateProc,eventmgrinfo->winid);
  GL_GetGC(eventmgrinfo->winid,&(scinfo->gc),GL_Done);
  GL_AddWindowAssocInfo(eventmgrinfo->winid,_GL_VA_Init_StripChart,
			(void *)scinfo);
}

void GL_StripChart_Help()
{
  GL_Window_Help();
  fprintf(stderr,"   -update <hundredths of a second>  How often to update the display.\n");
}




