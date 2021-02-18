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


/*  gl_eventmgr.c
    Handles events.  Passes them to where they need to go.
*/

#include <stdio.h>
#include "X11/gl/limits.h"
#include <errno.h>
#include "X11/gl/eventmgr.h"
#include "X11/gl/stddef.h"

#ifndef FD_SET
#define        FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define        FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFBITS)))
#define        FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define NFDBITS        (sizeof(fd_mask) * NBBY)
#define NBBY    8
typedef long    fd_mask;
#define FD_ZERO(p)      bzero((char *)(p), sizeof(*(p)))
#endif

static char *_gle_version = "\nEVENT MANAGER VERSION 1.0\n";
static GL_EventMgrInfo *GL_winlist=NULL;
Display *display;
int screen;
int _GL_initialized=FALSE;
static struct timeval call_interval,last_call_time;
static VPTF timeoutfunc=NULL;
static Window timeoutassocwindow=0;
int s_argc;/* save_ */
char **s_argv;
char *argv_used;
VPTF s_helpfunction;

void GL_MarkArgument(argnum,value)
int argnum,value;
{
  if (argnum<1||argnum>=s_argc) {
    fprintf(stderr,"Argument #%d out of range.\n",argnum);
    exit(-1);
  }
  if (value!=GL_ARG_USED&&value!=GL_ARG_SHARED) {
    fprintf(stderr,"Value #%d for argument marking invalid.\n",value);
    exit(-1);
  }
  if (argv_used[argnum]==GL_ARG_UNUSED||
      (argv_used[argnum]==GL_ARG_SHARED&&value==GL_ARG_SHARED))
    argv_used[argnum]=value;
  else {
    fprintf(stderr,"Collision on argument #%d,%s\n",argnum,s_argv[argnum]);
    fprintf(stderr,"Argument marked %s tried to be set to %s\n",
	    argv_used[argnum]==GL_ARG_USED?"Used":"Shared",
	    value==GL_ARG_USED?"Used":"Shared");
    exit(-1);
  }
}

void _checkargsused()
{
  int loop;

  for (loop=1;loop<s_argc;loop++)
    if(argv_used[loop]==GL_ARG_UNUSED) {
      fprintf(stderr,"Bad command line argument:%s\n",s_argv[loop]);
      s_helpfunction();
      exit(2);
    }
}


static void GL_Err_XServerConnectionBad()
{
  fputs("Way un-cool. Your connection to the X server is pretty dead.\n",stderr);
  fputs("Which is like really coincidental, cause so are we.\n",stderr);
  fputs("Tell some wizard type person(person with pointy not dunce hat),\n",stderr);
  fprintf(stderr,"that you got error number %d on a select.\n",errno);
  exit(-1);
}

static void GL_Err_InsaneEventType()
{
  fprintf(stderr,"GraphLib : sanity check on restricted area event");
  fprintf(stderr," type failed.\n  Legitimate event types are");
  fprintf(stderr,"R_ButtonPress,R_ButtonRelease,R_EnterNotify,\n");
  fprintf(stderr,"R_LeaveNotify,R_KeyPress,R_KeyRelease,");
  fprintf(stderr,"R_MotionNotify.\n");
  exit(-1);
}

void checkinit()
{
  if (_GL_initialized==FALSE) {
    fprintf(stderr,"Arnold thinks you should go learn to program.\n");
    fprintf(stderr,"Arnold thinks you learned to program at ");
    fprintf(stderr,"Montgomery Community College.\n");
    fprintf(stderr,"Arnold thinks you should go to a real school.\n");
    fprintf(stderr,"There you will learn to initialize toolkits.\n");
    fprintf(stderr,"We will now PUMP YOU OUT!\n");
    exit(-1);
  }
}

void GL_SetTimeOut(atimeout,atimeoutfunc,assocwindow)
struct timeval *atimeout;
VPTF atimeoutfunc;
Window assocwindow;
{
  timeoutfunc = atimeoutfunc;
  if (timeoutfunc!=NULL) {
    call_interval = *atimeout;
    timeoutassocwindow = assocwindow;
    (void)gettimeofday(&last_call_time,NULL);
  }
}

static void addtowinlist(wintoadd)
/* wintoadd should stay around after this call. */
GL_EventMgrInfo *wintoadd;
{
  GL_EventMgrInfo *pos;
  
  wintoadd->next = NULL;
  if (GL_winlist==NULL)
    GL_winlist=wintoadd;
  else {
    for (pos=GL_winlist;pos->next!=NULL;pos = pos->next);
    pos->next = wintoadd;
  }
}

GL_EventMgrInfo *SearchWinList(target)
Window target;
{
  register GL_EventMgrInfo *look=GL_winlist;

  for(;look!=NULL&&look->winid!=target;look=look->next);
  
  if (look==NULL) {
    fputs("You have asked for a window that does not exist.\n",stderr);
    fputs("This program now does not exist.\n",stderr);
    exit(-1);
  }
  return look;
}
  
GL_DataList GL_GetDataList(target)
Window target;
{
  register GL_EventMgrInfo *look;

  look = SearchWinList(target);
  return look->AssocList;
}

void GL_AddWindowAssocInfo(target,key,associnfo)
Window target;
VPTF key;
GLPointer associnfo;
{
  GL_EventMgrInfo *thewin;

  thewin=SearchWinList(target);
  
  GL_Add_To_DataList(thewin->AssocList,key,associnfo);
}

int GL_XDeleteAssocInfo(list,key) /* X for eXecute */
GL_DataList *list;
VPTF key;
{
  GL_DataList work,prev=NULL;

  for(work= *list;work!=NULL&&work->key!=key;prev=work,work=work->next);

}

static void AddToFunctionList(list,functoadd,flags,top,bottom,left,right)
GL_FuncListPtr *list;
VPTF functoadd;
short flags;
int top,bottom,left,right;
{
  GL_FuncListPtr pos,add;
  
  if (functoadd!= NULL) {
    add = (GL_FuncListPtr)xmalloc(sizeof(GL_FuncList));
    add->next = NULL;
    add->aFunc=functoadd;
    add->flags=flags;
    add->top=top;
    add->bottom=bottom;
    add->left=left;
    add->right=right;
    if (*list==NULL)
      *list=add;
    else {
      pos=(*list);
      while (pos->next!=NULL)
	pos=pos->next;
      pos->next=add;
    }
  }
}

#define _GL_SaneModType(x) (x)==ButtonPress||(x)==ButtonRelease|| \
	    (x)==EnterNotify||(x)==LeaveNotify|| \
	    (x)==KeyPress||(x)==KeyRelease|| \
	    (x)==MotionNotify 

static void ProcessFunction(pvar,list,modtype)
va_list *pvar;
GL_FuncListPtr *list;
int modtype;
{
  int top,bottom,left,right;
  short func_flags;
  
  top=SHRT_MIN;bottom=SHRT_MAX;left=SHRT_MIN;right=SHRT_MAX;
  func_flags=0;
  if (modtype&GL_Restricted) {
    if ( _GL_SaneModType(modtype&127) ) {
      func_flags |=GL_Restricted;
      top = va_arg(*pvar,int)-1; /* -1&+1 are to adjust so that ptinrect
				   can do < and > instead of <= which
				   didn't seem to work right. */
      bottom = va_arg(*pvar,int)+1;
      left = va_arg(*pvar,int)-1;
      right = va_arg(*pvar,int)+1;
      if (bottom<top||right<left) {
	fprintf(stderr,"You have insane values for your restriction");
	fprintf(stderr,"rect.\n your bottom<top or right<left.");
	fprintf(stderr,"Fix it!\n");
	exit(-1);
      }
    } else GL_Err_InsaneEventType();
  }
  modtype&=127;
  AddToFunctionList(list,va_arg(*pvar,VPTF),func_flags,top,bottom,left,right);
}

void GL_AddFuncPtr(win, va_alist)
Window win;
va_dcl
{
  va_list pvar;
  GL_FuncListPtr list;
  int modtype;
  GL_EventMgrInfo *thiswin;

  for(thiswin=GL_winlist;(thiswin!=NULL)&&(thiswin->winid!=win);
      thiswin=thiswin->next);

  va_start(pvar);
  for (modtype=va_arg(pvar,int);modtype;modtype=va_arg(pvar,int))
    {
      if (modtype<2||modtype>GL_MaxFunction) {
	fprintf(stderr,"There was a goofy modifier passed to this function.\n");
	fprintf(stderr,"I really don't know what to do with it.\n");
	fprintf(stderr,"I see two possiblilities.  One, the user of the library\n");
	fprintf(stderr,"barfed, and did their call wrong.  Alternatively, some\n");
	fprintf(stderr,"function which called me didn't handle what it was\n");
	fprintf(stderr,"supposed to.  Either way, the number was %d\n",modtype);
	fprintf(stderr,"and I'm so confused I'm going to exit.  Bye Bye Bucko.\n");
	exit(-1);
      }
      /* We know the modifier is o.k. now. */
      if (!((modtype&127)<LASTEvent)) {
	fprintf(stderr,"That is not an event, That is not an event.\n");
	fprintf(stderr,"EVENTually, you will learn to program.\n");
	fprintf(stderr,"But until then, X11R4 event #%d is illegal.\n",modtype&127);
	fprintf(stderr,"On the off chance that this is not X11R4,\n");
	fprintf(stderr,"You should recompile me, because I'm old and decrepid.\n");
	fprintf(stderr,"But most likely, you are old and decrepid.\n");
	fprintf(stderr,"Therefore, you shall die.");
	exit(-1);
      }
      /* We know it is legal to take the arrary reference here. */
      ProcessFunction(&pvar,&(thiswin->FuncLists[modtype&127]),modtype);
    }
}

#define POINTINRECT(x,y,top,bottom,left,right) ((y)>=(top)&&(y)<=(bottom)&&(x)>=(left)&&(x)<=(right))
/* POINTINRECT is inclusive eg, the border is inside the rectangle. */

void GL_ProcessEvent(report)
XEvent *report;
{
  GL_EventMgrInfo *thiswin;
  GL_FuncListPtr afunclist,work;
  register int call;

  for(thiswin=GL_winlist;
      (thiswin!=NULL)&&(thiswin->winid!=report->xany.window);
      thiswin=thiswin->next)
    ;
  if (thiswin==NULL) {
    fputs("I refuse to believe in the window the X server says I have.\n",stderr);
    exit(-1);
  }
  afunclist=thiswin->FuncLists[report->type];
  while (afunclist!=NULL) {
    call = 1;
    if (afunclist->flags&GL_Restricted)
      call = POINTINRECT(report->xbutton.x,report->xbutton.y,afunclist->top,
			 afunclist->bottom,afunclist->left,afunclist->right);
    if (call)
      afunclist->aFunc(report,thiswin->AssocList);
    afunclist=afunclist->next;
  }
}

static void GL_ProcessEvents()
{
  XEvent report;

  do {
    XNextEvent(display,&report);
    GL_ProcessEvent(&report);
  } while (XPending(display));
}

/*Computes a=a-b;*/
#define SUBTIME(a,b) (a).tv_sec-=(b).tv_sec;\
  (a).tv_usec-=(b).tv_usec;\
  if((a).tv_usec<0) \
    {(a).tv_sec--;(a).tv_usec+=1000000;}

static void _GL_HandleTimeOutFunc(max_delay)
struct timeval *max_delay;
{
  struct timeval curtime;
  
  (void)gettimeofday(&curtime,NULL);
  SUBTIME(curtime,last_call_time);
  /* difference between last_call_time and curtime is in curtime. */
  /* now check to see if we missed a call. */

  if (timercmp(&curtime,&call_interval,>)) {
    /* missed a call, call, and set last to now, maxdelay to interval*/
    (void)gettimeofday(&last_call_time,NULL);
    timeoutfunc(timeoutassocwindow);
    *max_delay=call_interval;
  } else {
    /* didn't miss call, max delay is call_interval-curtime */
    /* as curtime is currently time since last call.*/
    *max_delay=call_interval;
    SUBTIME(*max_delay,curtime);
  }
}

void GL_MainLoop(mainwin)
Window mainwin;
{
  register int nfound;
  fd_set rmaskfd,emaskfd;
  struct timeval max_delay;

  (void)gettimeofday(&last_call_time,NULL);
  checkinit();
  _checkargsused();
  XMapSubwindows(display,mainwin);
  XMapWindow(display,mainwin);
  while (1)  {
    XFlush(display);
    FD_ZERO(&rmaskfd);
    FD_SET(ConnectionNumber(display),&rmaskfd);
    FD_ZERO(&emaskfd);
    FD_SET(ConnectionNumber(display),&emaskfd);
    if (timeoutfunc==NULL)
      nfound = select(getdtablesize(),&rmaskfd,NULL,&emaskfd,NULL);
    else {
      _GL_HandleTimeOutFunc(&max_delay);
      nfound = select(getdtablesize(),&rmaskfd,NULL,&emaskfd,&max_delay);
    };
    if (nfound==0&&timeoutfunc!=NULL) {
      (void)gettimeofday(&last_call_time,NULL);
      timeoutfunc(timeoutassocwindow);
    }
    if (nfound==-1/* && errno != EINTR*/ ||
	FD_ISSET(ConnectionNumber(display),&emaskfd))
      GL_Err_XServerConnectionBad();
    if (FD_ISSET(ConnectionNumber(display),&rmaskfd))
      GL_ProcessEvents();
  }
}


void _GL_VA_Init_EventMgr(list)
GL_DataList list;
{
  GL_EventMgrInfo *data;
  int loop;

  data = (GL_EventMgrInfo *)xmalloc(sizeof(GL_EventMgrInfo));
  data->next = NULL;
  data->winid = 0;
  data->AssocList = GL_Init_DataList();
  for (loop=0;loop<LASTEvent;loop++)
    data->FuncLists[loop]=NULL;
  GL_Add_To_DataList(list,_GL_VA_Init_EventMgr,data);
}

void _GL_VA_Handle_EventMgr(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_EventMgrInfo *data;
  long key;

  data = (GL_EventMgrInfo *)GL_Search_DataList(list,_GL_VA_Init_EventMgr);
  if (modifier==GL_AssocData) {
    key = va_arg(*pvar,long);
    GL_Add_To_DataList(data->AssocList,key,va_arg(*pvar,GLPointer));
    return;
  }
  if (modifier<2||modifier>GL_MaxFunction) {
    fprintf(stderr,"There was a goofy modifier passed to this function.\n");
    fprintf(stderr,"I really don't know what to do with it.\n");
    fprintf(stderr,"I see two possiblilities.  One, the user of the library\n");
    fprintf(stderr,"barfed, and did their call wrong.  Alternatively, some\n");
    fprintf(stderr,"function which called me didn't handle what it was\n");
    fprintf(stderr,"supposed to.  Either way, the number was %d\n",modifier);
    fprintf(stderr,"and I'm so confused I'm going to exit.  Bye Bye Bucko.\n");
    exit(-1);
  }
  /* We know the modifier is o.k. now. */
  if (!((modifier&127)<LASTEvent)) {
    fprintf(stderr,"That is not an event, That is not an event.\n");
    fprintf(stderr,"EVENTually, you will learn to program.\n");
    fprintf(stderr,"But until then, X11R4 event #%d is illegal.\n",modifier&127);
    fprintf(stderr,"On the off chance that this is not X11R4,\n");
    fprintf(stderr,"You should recompile me, because I'm old and decrepid.\n");
    fprintf(stderr,"But most likely, you are old and decrepid.\n");
    fprintf(stderr,"Therefore, you shall die.");
    exit(-1);
  }
  /* We know it is legal to take the arrary reference here. */
  ProcessFunction(pvar,&(data->FuncLists[modifier&127]),modifier);
}

void _GL_VA_Execute_EventMgr(list)
GL_DataList list;
{
  GL_EventMgrInfo *data;
  data = (GL_EventMgrInfo *)GL_Search_DataList(list,_GL_VA_Init_EventMgr);
  if (data->winid!=0)
    addtowinlist(data);
}

void _GL_VA_Destroy_EventMgr(list)
GL_DataList list;
{
};

void GL_EventMgr_Help()
{
  fputs("Valid options :\n",stderr);
  fputs("   -help                           print out this message\n",stderr);
  fputs("   -help-resources                 lists the resources used.\n",
	stderr);
  fputs("   -display displayname            X server to contact\n",stderr);
}
