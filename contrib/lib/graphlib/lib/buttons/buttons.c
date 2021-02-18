/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Jeffrey Hutzelman, Eric Anderson 
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


/*	GraphLib: An X Windows Programming Library
	Developed at CMU Fall 1990 - Spring 1991

	Eric Anderson (ea08@andrew.cmu.edu, eanders@sura.net)
	Jeffrey Hutzelman (jh4o@andrew.cmu.edu, jhutz@drycas.club.cc.cmu.edu)

	gl_buttons.c: Button Creation and Handler Code
	Button creation handlers
	  _GL_VA_Init_Button
	  _GL_VA_Handle_Button
	  _GL_VA_Execute_Button
	  _GL_VA_Destroy_Button
	Button event handlers
	  _GL_Button_Click
	  _GL_Button_Release
	  _GL_Button_Expose
	Button manipulation
	  GL_DestroyButton
	  GL_CreateRadGrp
	  GL_DestroyRadGrp

	Revision History
	Date     Who	What
	-------- ---	------------------------------
	03/08/91 jth	Created
	04/01/91 jth	Updated to use xmalloc, XError

*/

#include "X11/gl/buttons.h"
#include <malloc.h>
#include <stdio.h>

GC _GL_ButtonGC;		/* Global GC for use by buttons */
int _GL_ButtonInitFlag=0;	/* Button initialization flag */

#ifdef GL_DEBUG
void _GL_Button_Debug();
#endif

static _GL_Error(message)
char *message;
{
  if (message)
    puts(message,stderr);
  exit (-1);
}

static void _GL_InitButtons()
{
  GL_GetGC(RootWindow(display,screen),&_GL_ButtonGC,GL_Done);
  _GL_ButtonInitFlag=1;
#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Buttons initialized.\n");
#endif
}

static void _GL_CheckButtonInit()
{
  if (!_GL_ButtonInitFlag) _GL_InitButtons();
}

int GL_SrchRadio(grpinfo,btninfo)
GL_RadioGroup *grpinfo;
GL_ButtonInfo *btninfo;
{
  int i;
#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Searching radio list %#lx for button %#lx...",grpinfo,btninfo);
#endif
  for (i=0;i<grpinfo->num;i++)
    if (grpinfo->members[i]==btninfo)
      {
#ifdef GL_DEBUG_FULL
	fprintf(stderr,"Found in item %d.\n",i);
#endif
	return i;
      }
#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Not found.\n");
#endif
  return -1;
}
    
void _GL_VA_Init_Button(list)
GL_DataList list;
{
  GL_ButtonInfo *btninfo;
  GL_WindowInfo *wininfo;

  _GL_CheckButtonInit();

#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Creating new button\n");
#endif

  _GL_VA_Init_Window(list);	/* Initialize my parent! */
  btninfo=(GL_ButtonInfo *)xmalloc(sizeof(GL_ButtonInfo));
  wininfo=(GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);

#ifdef GL_DEBUG
  wininfo->borderwidth=1;
#endif

  btninfo->width=btninfo->height = -1;
  wininfo->width=wininfo->height = -1;
  btninfo->state=0;
  btninfo->flags=0;
  btninfo->bnum=0;
  btninfo->bflag=0;
  btninfo->action=0;
  btninfo->draw[0]=0;
  btninfo->draw[1]=0;
  btninfo->draw[2]=0;
  btninfo->draw[3]=0;
  btninfo->fgc=BlackPixel(display,screen);
  btninfo->bgc=WhitePixel(display,screen);
  btninfo->txc=BlackPixel(display,screen);
  wininfo->CWattr.backing_store = WhenMapped;
  wininfo->CWattr.background_pixmap = ParentRelative;
  GL_Add_To_DataList(list,_GL_VA_Init_Button,btninfo);
}

void _GL_VA_Handle_Button(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_ButtonInfo *btninfo;
  GL_RadioGroup *grpinfo;
  GL_WindowInfo *wininfo;

#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Parsing button option #%ld\n",modifier);
#endif

  btninfo=(GL_ButtonInfo *)GL_Search_DataList(list,_GL_VA_Init_Button);
  switch (modifier)
    {
    case GL_BtnSwitch: btninfo->bnum=va_arg(*pvar,int);break;
    case GL_BtnAction: btninfo->action=va_arg(*pvar,VPTF);break;
    case GL_BtnDraw+0: btninfo->draw[0]=va_arg(*pvar,VPTF);break;
    case GL_BtnDraw+1: btninfo->draw[1]=va_arg(*pvar,VPTF);break;
    case GL_BtnDraw+2: btninfo->draw[2]=va_arg(*pvar,VPTF);break;
    case GL_BtnDraw+3: btninfo->draw[3]=va_arg(*pvar,VPTF);break;
    case GL_BtnFG: btninfo->fgc=va_arg(*pvar,unsigned long);break;
    case GL_BtnBG: btninfo->bgc=va_arg(*pvar,unsigned long);break;
    case GL_BtnTX: btninfo->txc=va_arg(*pvar,unsigned long);break;
    case GL_BtnRadGrp:
      grpinfo=va_arg(*pvar,GL_RadioGroup *);
      if (grpinfo->num<grpinfo->size)
	{
	  GL_Add_To_DataList(list,GL_CreateRadGrp,grpinfo);
	  grpinfo->members[grpinfo->num++]=btninfo;
	  btninfo->flags |= GL_Bradio;
	}
      else
	{
	  _GL_Error("Too many members in radio group.",0);
	}
      break;
      
    case GL_BtnFlags:
      btninfo->flags = (btninfo->flags & GL_Bradio) |
	(va_arg(*pvar,int) & ~GL_Bradio);
      break;
      
    default:			/* Pass it on if noone else took it */
	_GL_VA_Handle_Window(list,modifier,pvar);
    }
}

void _GL_VA_Execute_Button(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;
  GL_ButtonInfo *btninfo;
  GL_EventMgrInfo *eventmgrinfo;
  GL_RadioGroup *grpinfo;

  wininfo=(GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  btninfo=(GL_ButtonInfo *)GL_Search_DataList(list,_GL_VA_Init_Button);
  eventmgrinfo = (GL_EventMgrInfo *)GL_Search_DataList(list,_GL_VA_Init_EventMgr);
  
#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Executing button %#lx:\n",btninfo);
  fprintf(stderr,"    Window Name Pointer:     %#lx\n",wininfo->winname);
  fprintf(stderr,"    Window Name:             %s\n",wininfo->winname);
  fprintf(stderr,"    Size:                    %d x %d\n",
    btninfo->width,btninfo->height);
  fprintf(stderr,"    State:                   %d\n",btninfo->state);
  fprintf(stderr,"    Flags:                   %#x\n",btninfo->flags);
  fprintf(stderr,"    Switch:                  %d [%#x]\n",
    btninfo->bnum,btninfo->bflag);
  fprintf(stderr,"    Action:                  %s\n",btninfo->action?"yes":"no");
  fprintf(stderr,"    Draw:                    %-3s %-3s %-3s %-3s\n",
    btninfo->draw[0]?"yes":"no",
    btninfo->draw[1]?"yes":"no",
    btninfo->draw[2]?"yes":"no",
    btninfo->draw[3]?"yes":"no");
  fprintf(stderr,"    Colors:                  %lu %lu %lu\n",
    btninfo->fgc,btninfo->bgc,btninfo->txc);
#endif

  if (!wininfo->winname)
    {
      wininfo->winname=(char *)xmalloc(strlen("Button")+1);
      btninfo->name=(char *)xmalloc(strlen("Button")+1);
      strcpy(wininfo->winname,"Button");
      strcpy(btninfo->name,"Button");
    }
  else
    {
      btninfo->name=(char *)xmalloc(strlen(wininfo->winname)+1);
      strcpy(btninfo->name,wininfo->winname);
    }
  if (wininfo->width>=0)
      btninfo->width=wininfo->width;
  else
      wininfo->width=btninfo->width =
	XTextWidth(default_font,btninfo->name,strlen(btninfo->name))+10;
  if (wininfo->height>=0)
      btninfo->height=wininfo->height;
  else
    wininfo->height=btninfo->height = 
      default_font->max_bounds.ascent+default_font->max_bounds.descent+2;

#ifdef GL_DEBUG
  wininfo->x--;
  wininfo->y--;
#endif

  wininfo->CWattr.event_mask = ButtonPressMask | ButtonReleaseMask |
#ifdef GL_DEBUG
      KeyPressMask |
#endif
    EnterWindowMask | LeaveWindowMask | ExposureMask;

  _GL_VA_Execute_Window(list);
  btninfo->winid=eventmgrinfo->winid;
  XSetWindowBackgroundPixmap(display, btninfo->winid, ParentRelative);
  if (btninfo->flags&GL_Bradio)
    {
      grpinfo=(GL_RadioGroup *)GL_Search_DataList(list,GL_CreateRadGrp);
      if (grpinfo->active==GL_SrchRadio(grpinfo,btninfo))
	{
	  btninfo->flags |= GL_Bchecked;
	}
      else
	{
	  btninfo->flags &= ~GL_Bchecked;
	}
      GL_AddWindowAssocInfo(eventmgrinfo->winid,(VPTF)GL_CreateRadGrp,
			    (GLPointer)grpinfo);
    }
  GL_AddWindowAssocInfo(eventmgrinfo->winid,_GL_VA_Init_Button,
			(GLPointer)btninfo);
  GL_AddFuncPtr(eventmgrinfo->winid,
    ButtonPress, _GL_Button_Click,
    ButtonRelease, _GL_Button_Release,
    EnterNotify, _GL_Button_MouseEnter,
    LeaveNotify, _GL_Button_MouseLeave,
    Expose, _GL_Button_Expose,
#ifdef GL_DEBUG
    KeyPress, _GL_Button_Debug,
#endif
    GL_Done);  
}

void _GL_VA_Destroy_Button(list)
GL_DataList list;
{
  _GL_VA_Destroy_Window(list);
}

static void _GL_Button_Click(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *btninfo;
  int ostate;
  
#ifdef GL_DEBUG_FULL
  fprintf(stderr, "Clicked in button %lx (window %lx)\n", btninfo, btninfo->winid);
#endif

  btninfo=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  ostate=btninfo->state;
  if (btninfo->bnum>0 && btninfo->bnum != report->xbutton.button) return;
  if (!btninfo->bflag)
    {
      btninfo->state=2;		/* Guaranteed */
      btninfo->flags |= GL_Binuse;
      if (btninfo->flags&GL_BaAction && btninfo->action)
	(btninfo->action)(report,alist,btninfo->winid);
	if ((btninfo->draw[btninfo->state] != btninfo->draw[ostate] || (btninfo->flags&GL_BaUpdate)) && !(btninfo->flags&GL_Bdestroy))
	  {
	    if (btninfo->draw[btninfo->state]) (btninfo->draw[btninfo->state])(report,alist,btninfo->winid);
	    else XClearWindow(display,btninfo->winid);
	  }
      btninfo->flags &= ~GL_Binuse;
    }
  btninfo->bflag |= 1<<(report->xbutton.button-1);
  if (btninfo->flags & GL_Bdestroy) GL_DestroyButton(btninfo->winid);
}

static void _GL_Button_Release(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *btninfo,*auxinfo;
  GL_RadioGroup *grpinfo;
  int ostate,ochecked;
  
#ifdef GL_DEBUG_FULL
  fprintf(stderr, "Released in button %lx (window %lx)\n", btninfo, btninfo->winid);
#endif

  btninfo=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  ostate=btninfo->state;
  ochecked=btninfo->flags & GL_Bchecked;
  if (btninfo->bnum>0 && btninfo->bnum != report->xbutton.button) return;
  btninfo->bflag &= ~(1<<(report->xbutton.button-1));
  if (btninfo->bflag) return;
  btninfo->state=(btninfo->state==2);
  btninfo->flags |= GL_Binuse;
  if ((btninfo->state || btninfo->flags&GL_BaAction) && btninfo->action)
    {
#ifdef GL_DEBUG
      fprintf(stderr,"Acting on button %lx (window %lx)\n",btninfo,btninfo->winid);
#endif
      (btninfo->action)(report,alist,btninfo->winid);
    }
  if (btninfo->state && btninfo->flags&GL_Bautocheck)
    btninfo->flags^=GL_Bchecked;
  if (btninfo->state && btninfo->flags&GL_Bradio)
    {
      if (!(btninfo->flags&GL_Bchecked))
	{
	  btninfo->flags|=GL_Bchecked;
	}
      else
	{
	  grpinfo=(GL_RadioGroup *)GL_Search_DataList(alist,GL_CreateRadGrp);
	  if (grpinfo==0)
	    {
	      btninfo->flags &= ~GL_Bchecked;
	    }
	  else
	    {
	      if (grpinfo->active>=0 && grpinfo->active < grpinfo->num)
		{
		  auxinfo=grpinfo->members[grpinfo->active];
		  if (auxinfo)
		    {
		      auxinfo->flags |= GL_Binuse;
		      if (auxinfo->action)
			(auxinfo->action)(report,GL_GetDataList(auxinfo->winid),auxinfo->winid);
		      auxinfo->flags &= ~GL_Bchecked;
		      if (!(auxinfo->flags & GL_Bdestroy))
			{
			  if (auxinfo->draw[auxinfo->state])
			    (auxinfo->draw[auxinfo->state])(report,GL_GetDataList(auxinfo->winid),auxinfo->winid);
			  else
			    XClearWindow(display,auxinfo->winid);
			  grpinfo->oactive=grpinfo->active;
			}
		      else
			grpinfo->oactive= -1;
		      auxinfo->flags &= ~GL_Binuse;
		      if (auxinfo->flags & GL_Bdestroy)
			GL_DestroyButton(auxinfo->winid);
		    }
		}
	      else
		{
		  grpinfo->oactive = -1;
		}
	      grpinfo->active=GL_SrchRadio(grpinfo,btninfo);
	      if (grpinfo->action)
		(grpinfo->action)(report,alist,btninfo->winid);
	    }
	}
    }
  if ((btninfo->draw[btninfo->state] != btninfo->draw[ostate] ||
    btninfo->flags&GL_Bchecked != ochecked || (btninfo->flags&GL_BaUpdate)) && !(btninfo->flags & GL_Bdestroy))
    {
      if (btninfo->draw[btninfo->state])
	(btninfo->draw[btninfo->state])(report,alist,btninfo->winid);
      else
	XClearWindow(display,btninfo->winid);
    }
  btninfo->flags &= ~GL_Binuse;
  if (btninfo->flags & GL_Bdestroy)
    GL_DestroyButton(btninfo->winid);
}

static void _GL_Button_MouseEnter(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *btninfo=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int ostate=btninfo->state;

#ifdef GL_DEBUG_FULL
  fprintf(stderr, "Entered button %lx (window %lx)\n", btninfo, btninfo->winid);
#endif

  btninfo->state=(ostate==3||ostate==2)?2:1;
  if (btninfo->draw[btninfo->state] != btninfo->draw[ostate] ||
    (btninfo->flags&GL_BaUpdate))
    {
      if (btninfo->draw[btninfo->state]) (btninfo->draw[btninfo->state])(report,alist,btninfo->winid);
      else XClearWindow(display,btninfo->winid);
    }
}

static void _GL_Button_MouseLeave(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *btninfo=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int ostate=btninfo->state;

#ifdef GL_DEBUG_FULL
  fprintf(stderr, "Left button %lx (window %lx)\n", btninfo, btninfo->winid);
#endif

  btninfo->state=(ostate==3||ostate==2)?3:0;
  if (btninfo->draw[btninfo->state] != btninfo->draw[ostate] ||
    (btninfo->flags&GL_BaUpdate))
    {
      if (btninfo->draw[btninfo->state]) (btninfo->draw[btninfo->state])(report,alist,btninfo->winid);
      else XClearWindow(display,btninfo->winid);
    }
}

static void _GL_Button_Expose(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *btninfo=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);

#ifdef GL_DEBUG_FULL
  fprintf(stderr, "Exposed button %lx (window %lx)\n", btninfo, btninfo->winid);
#endif

  if (btninfo->draw[btninfo->state])
    (btninfo->draw[btninfo->state])(report,alist,btninfo->winid);
  else
    XClearWindow(display,btninfo->winid);
}

void GL_DestroyButton(winid)
Window winid;     
{
  GL_DataList alist=GL_GetDataList(winid);
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  GL_RadioGroup *grpinfo;
  int i,n;

  if (info->flags & GL_Binuse)
    {
      info->flags |= GL_Bdestroy;
      return;
    }
  if (info->flags & GL_Bradio)
    {
      grpinfo=(GL_RadioGroup *)GL_Search_DataList(alist,GL_CreateRadGrp);
      grpinfo->members[GL_SrchRadio(grpinfo,info)]=0;
      for (n=0,i=0;i<grpinfo->num;i++)
	if (grpinfo->members[i]) n++;
      GL_XDeleteAssocInfo(&alist,GL_CreateRadGrp);
      if (n) GL_DestroyRadGrp(grpinfo);
    }
  GL_XDeleteAssocInfo(&alist,_GL_VA_Init_Button);
  XDestroyWindow(display,info->winid);
  free(info);
}

GL_RadioGroup *GL_CreateRadGrp(size,active,action)
int size,active;
VPTF action;
{
  GL_RadioGroup *grpinfo;
  int i;

#ifdef GL_DEBUG
  fprintf(stderr,"Creating Radio Group\n");
#endif

  grpinfo=(GL_RadioGroup *)xmalloc(sizeof(GL_RadioGroup));
  grpinfo->size=size;
  grpinfo->oactive=grpinfo->active=(active<size)?active:0;
  grpinfo->num=0;
  grpinfo->members=(GL_ButtonInfo **)xmalloc(size*sizeof(GL_ButtonInfo *));
  for (i=0;i<grpinfo->size;i++) grpinfo->members[i]=0;
  grpinfo->action=action;
  return grpinfo;
}

void GL_DestroyRadGrp(grpinfo)
GL_RadioGroup *grpinfo;
{
  int i;
  GL_ButtonInfo *btninfo;
  GL_DataList alist;

  for (i=0;i < grpinfo->size;i++)
    {
      btninfo=grpinfo->members[i];
      if (btninfo)
	{
	  btninfo->flags &= ~GL_Bradio;
	  alist=GL_GetDataList(btninfo->winid);
	  if (GL_Search_DataList(alist,GL_CreateRadGrp))
	    GL_XDeleteAssocInfo(&alist,GL_CreateRadGrp);
	}
    }
  free (grpinfo->members);
  free (grpinfo);
}

#ifdef GL_DEBUG
static void _GL_Button_Debug(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *btninfo=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  GL_RadioGroup *grpinfo;
  int memnum;

  switch (XKeycodeToKeysym(display,report->xkey.keycode,0))
    {
    case 'B':
    case 'b':
      fprintf(stderr,"Button %#lx:\n",btninfo);
      fprintf(stderr,"    Name:                    %s\n",btninfo->name);
      fprintf(stderr,"    Size:                    %d x %d\n",
	btninfo->width,btninfo->height);
      fprintf(stderr,"    Window:                  %lx\n",btninfo->winid);
      fprintf(stderr,"    State:                   %d\n",btninfo->state);
      fprintf(stderr,"    Flags:                   %#x\n",btninfo->flags);
      fprintf(stderr,"    Switch:                  %d [%#x]\n",
	btninfo->bnum,btninfo->bflag);
      fprintf(stderr,"    Action:                  %s\n",btninfo->action?"yes":"no");
      fprintf(stderr,"    Draw:                    %-3s %-3s %-3s %-3s\n",
	btninfo->draw[0]?"yes":"no",
	btninfo->draw[1]?"yes":"no",
	btninfo->draw[2]?"yes":"no",
	btninfo->draw[3]?"yes":"no");
      fprintf(stderr,"    Colors:                  %lu %lu %lu\n",
	btninfo->fgc,btninfo->bgc,btninfo->txc);
      break;
    case 'R':
    case 'r':
      if (!(btninfo->flags&GL_Bradio))
	fprintf(stderr,"Button %#lx is not a member of a radio group\n",btninfo);
      else
	{
	  grpinfo=(GL_RadioGroup *)GL_Search_DataList(alist,GL_CreateRadGrp);
	  if (grpinfo==0)
	    {
	      fprintf(stderr,"Button %#lx is no longer a radio group member\n");
	    }
	  else
	    {
	      memnum=GL_SrchRadio(grpinfo,btninfo);
	      fprintf(stderr,"Radio Group %#lx:\n",grpinfo);
	      fprintf(stderr,"    Size:                %d\n",grpinfo->size);
	      fprintf(stderr,"    Number:              %d\n",grpinfo->num);
	      fprintf(stderr,"    Active Member:       %d [%#lx]\n",
		grpinfo->active,(grpinfo->active>=0&&grpinfo->active<grpinfo->size)?grpinfo->members[grpinfo->active]:0);
	      fprintf(stderr,"    Last Active:         %d [%#lx]\n",
		grpinfo->oactive,(grpinfo->oactive>=0&&grpinfo->oactive<grpinfo->size)?grpinfo->members[grpinfo->oactive]:0);
	      fprintf(stderr,"    This Member:         %d [%#lx]\n",memnum,btninfo);
	      fprintf(stderr,"    Action:              %s\n",grpinfo->action?"yes":"no");
	    }
	}
    }
}
#endif
