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

#include <stdio.h>
#include "X11/gl/window.h"
#include "icon_bitmap"
#include "X11/gl/stddef.h"
#include "X11/gl/resmgr.h"
#include "X11/gl/resfns.h"
#define DEFAULT_FONT "fixed"

char *s_classname;
Pixmap default_icon,default_grey,default_foreground,default_background;
XFontStruct *default_font;
GC default_gc,default_clip_gc,default_reverse_gc,default_xor_gc;

int GL_Debug = 0;

void load_default_font(fontname)
char *fontname;
{
  /* Access font */
  if ((default_font = XLoadQueryFont(display,fontname)) == NULL) {
    (void) fprintf( stderr, "Graphics Lib: Text is outa here.  Tough Luck Sucker!\n");
    (void) fprintf( stderr, "Unable to open default font fixed.");
    exit( -1 );
  }
}

void GL_InitGraphics(argc,argv,classname,helpfunction)
int argc;
char **argv;
char *classname;
VPTF helpfunction;
{
  char dispmach[30];
  int index;
  extern int _GL_initialized;
  Boolean reverse=False;
  Window dummy;
  XGCValues values;
  unsigned long valuemask;
  char *default_font_name = DEFAULT_FONT;

  strcpy(dispmach,"");
  _GL_initialized = TRUE;
  s_argc=argc;
  s_argv=argv;
  s_helpfunction=helpfunction;
  argv_used = xmalloc(argc);
  for(index=0;index<argc;index++)
    argv_used[index]=GL_ARG_UNUSED;
  for (index=0;index<argc;index++) {
    if (strcmp("-help",argv[index])==0) {
      s_helpfunction();
      exit(1);
    }
    if (strcmp("-display",argv[index])==0) {
      GL_MarkArgument(index,GL_ARG_USED);
      index++;
      GL_MarkArgument(index,GL_ARG_USED);
      strcpy(dispmach,argv[index]);
    }
    if (strcmp("-debug",argv[index])==0) {
      GL_Debug=1;
      GL_MarkArgument(index,GL_ARG_USED);
    }
  }
  if ((display=XOpenDisplay(dispmach))==NULL)
    {
      (void)fprintf(stderr,"%s: the X server named %s hates you, and so do I.\n",argv[0],XDisplayName(dispmach));
      (void)fprintf(stderr,"%s:See, you have to understand, to run this program, you have to be\nwilling to have X with your computer\n",argv[0]);
      exit(-1);
    }
  screen = DefaultScreen(display);
  s_classname = (char *)xmalloc(strlen(classname)+1);
  strcpy(s_classname,classname);

  default_icon=XCreateBitmapFromData(display,RootWindow(display,screen),icon_bitmap_bits,icon_bitmap_width,icon_bitmap_height);
  GL_Create(GL_Resources,
	    GL_RES_BOOLEAN, "reverseVideo", "rv", &reverse,
	    GL_RES_STRING, "Font", "fn", &default_font_name,
	    GL_Done);
  if (reverse) {
    default_foreground=WhitePixel(display,screen);
    default_background=BlackPixel(display,screen);
  } else {
    default_foreground=BlackPixel(display,screen);
    default_background=WhitePixel(display,screen);
  }
  default_grey=XCreatePixmapFromBitmapData(display,RootWindow(display,screen),
					   grey_bitmap_bits,grey_bitmap_width,
					   grey_bitmap_height,
					   BlackPixel(display,screen),
					   WhitePixel(display,screen),
					   DefaultDepth(display,screen));
  load_default_font(default_font_name);
  dummy = (Window)GL_Create(GL_Window,
			    GL_Done);
  GL_GetGC(dummy,&default_gc,GL_Done);
  GL_GetGC(dummy,&default_clip_gc,GL_Done);
  GL_GetGC(dummy,&default_reverse_gc,GL_Done);
  XSetForeground(display,default_reverse_gc,default_background);
  XSetBackground(display,default_reverse_gc,default_foreground);
  GL_GetGC(dummy,&default_xor_gc,GL_Done);
  values.function = GXxor;
  values.foreground = 1;
  values.background = 0;
  values.plane_mask = 1;
  valuemask = GCFunction | GCForeground | GCBackground | GCPlaneMask;
  XChangeGC(display,default_xor_gc,valuemask,&values);
  XDestroyWindow(display,dummy);
}

void _GL_VA_Init_Window(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;

  _GL_VA_Init_EventMgr(list);
  wininfo = (GL_WindowInfo *)xmalloc(sizeof(GL_WindowInfo));

  wininfo->parent = RootWindow(display,screen);
  wininfo->width = wininfo->height = 200;
  wininfo->x = wininfo->y = 0;
  wininfo->borderwidth = 0;
  wininfo->primarywindow = 0;

  wininfo->CWattr.background_pixmap = None;
  wininfo->CWattr.background_pixel = default_background;
  wininfo->CWattr.border_pixmap = CopyFromParent;
  wininfo->CWattr.border_pixel = default_foreground;
  wininfo->CWattr.bit_gravity = ForgetGravity;
  wininfo->CWattr.win_gravity = NorthWestGravity;
  wininfo->CWattr.backing_store = NotUseful;
  wininfo->CWattr.backing_planes = ~0;
  wininfo->CWattr.backing_pixel = 0;
  wininfo->CWattr.save_under = False;
  wininfo->CWattr.event_mask = ExposureMask |KeyPressMask|ButtonPressMask|
    StructureNotifyMask;
  wininfo->CWattr.do_not_propagate_mask = 0;
  wininfo->CWattr.override_redirect = False;
  wininfo->CWattr.colormap = CopyFromParent;
  wininfo->CWattr.cursor = None;

  wininfo->size_hints.flags = PPosition | PSize;
  wininfo->size_hints.x = wininfo->size_hints.y = 0;
  wininfo->size_hints.width = wininfo->size_hints.height = 200;
  wininfo->CWvaluemask = CWBackPixmap|CWBackPixel|CWBorderPixmap|CWBorderPixel|
    CWBitGravity|CWWinGravity|CWBackingStore|CWBackingPlanes|
      CWBackingPixel|CWOverrideRedirect|CWSaveUnder|CWEventMask|
        CWDontPropagate|CWColormap|CWCursor;

  wininfo->winname = NULL;
  wininfo->iconname = NULL;
  wininfo->class_hints.res_name = s_argv[0];
  wininfo->class_hints.res_class = s_classname;
  wininfo->wm_hints.flags = InputHint | StateHint;
  wininfo->wm_hints.input = True;
  wininfo->wm_hints.initial_state = NormalState;

  GL_Add_To_DataList(list,_GL_VA_Init_Window,wininfo);
}

void _GL_VA_Handle_Window(list, modifier, pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_WindowInfo *wininfo;
  char *dummy;

  wininfo = (GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  switch (modifier) {
    case GL_WinWidth  : wininfo->width = va_arg(*pvar,int);break;
    case GL_WinHeight : wininfo->height = va_arg(*pvar,int);break;
    case GL_WinParent : wininfo->parent = va_arg(*pvar,int);break;
    case GL_WinX      : wininfo->x = va_arg(*pvar,int);break;
    case GL_WinY      : wininfo->y = va_arg(*pvar,int);break;
    case GL_EventMask : wininfo->CWattr.event_mask = va_arg(*pvar,long);break;
    case GL_OverrideRedirect : wininfo->CWattr.override_redirect=va_arg(*pvar,Boolean);
      break;
    case GL_SaveUnder : wininfo->CWattr.save_under=va_arg(*pvar,Boolean);break;
    case GL_WindowBorderWidth : wininfo->borderwidth=va_arg(*pvar,int);break;
    case GL_WinName : if (wininfo->winname) free(wininfo->winname);
                      dummy = va_arg(*pvar,char *);
                      wininfo->winname=(char *)xmalloc(strlen(dummy)+1);
                      strcpy(wininfo->winname,dummy);
      break;
    case GL_BackgroundPixel : 
      wininfo->CWattr.background_pixel=va_arg(*pvar,unsigned long);
      break;
    case GL_PrimaryWindow : 
      wininfo->primarywindow=1;
      break;
    default : _GL_VA_Handle_EventMgr(list,modifier,pvar);
  }
}

void _GL_VA_Execute_Window(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;
  GL_EventMgrInfo *eventmgrinfo;
  XTextProperty windowName,iconName;

  wininfo = (GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  eventmgrinfo = (GL_EventMgrInfo *)GL_Search_DataList(list,
						       _GL_VA_Init_EventMgr);
  if(wininfo->primarywindow)
    GL_Create(GL_Resources, GL_RES_FUNCTION, "geometry","geometry",
	      wininfo,GL_VA_handle_geometry,
	      GL_Done);
  eventmgrinfo->winid = XCreateWindow(display,wininfo->parent,wininfo->x,
				      wininfo->y,wininfo->width,
				      wininfo->height,
				      wininfo->borderwidth,CopyFromParent,
				      InputOutput,CopyFromParent,
				      wininfo->CWvaluemask,
				      &(wininfo->CWattr));
  if (wininfo->primarywindow) {
    CheckAlloc(XStringListToTextProperty(&wininfo->winname,1,&windowName));
    CheckAlloc(XStringListToTextProperty(&wininfo->iconname,1,&iconName));
    XSetWMProperties(display,eventmgrinfo->winid,&windowName,&iconName,
		     s_argv,s_argc,&wininfo->size_hints,
		     &wininfo->wm_hints,&wininfo->class_hints);
  }
  _GL_VA_Execute_EventMgr(list);
}

void _GL_VA_Destroy_Window(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;

  wininfo = (GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  free(wininfo);
  _GL_VA_Destroy_EventMgr(list);
}

GLPointer _GL_VA_Return_Window(list)
GL_DataList list;
{
  GL_EventMgrInfo *eventmgrinfo;

  eventmgrinfo = (GL_EventMgrInfo *)GL_Search_DataList(list,_GL_VA_Init_EventMgr);
  return (GLPointer)eventmgrinfo->winid;
}

void GL_Window_Help()
{
  GL_EventMgr_Help();
  fputs("   -geometry [=][<width>x<height>][{+-}xoffset{+-}yoffset]\n",stderr);
  fputs("   -rv  specify that the window should be in reverse video\n",stderr);
  fputs("   -fn fontname specify the font that the program should use\n",stderr);
  fputs("   Note: all boolean arguments default to true if you use a -.");
  fputs("         false values may be achieved by +arg or -noarg");
}
