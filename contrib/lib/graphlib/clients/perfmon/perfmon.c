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
#include "X11/gl/stripchart.h"
#include <signal.h>
#include <X11/Xlibint.h>
#include "moniterable.h"
#include "X11/gl/limits.h"
#include "X11/gl/patchlevel.h"
#include "X11/gl/resmgr.h"
#include "X11/gl/label.h"
#include "X11/gl/menus.h"

char PatchLevel_Perfmon[] = PATCHLEVEL_PERFMON;

Window aroundwin,statwin,labelwin,menuwin;
int curstat;
char hostname[32];
char *labelformat="%n %m %h";

struct Statistic {
  char *name;
  VPTF initproc;
  LPTF updatefunc;
  int startscale,minscale,maxscale,deltascale,multiplier;
};

static struct Statistic Moniterable[] = {
  {"cpu",cpu_init,cpu_getvalue,100,100,100,100,1},
  {"disk",disk_bps_init,disk_bps_getvalue,25,25,INT_MAX,25,1},
  {"packets",if_pks_init,if_pks_getvalue,25,25,INT_MAX,25,1},
  {"paging",page_init,page_getvalue,25,25,INT_MAX,25,1},
  {"swapping",swap_init,swap_getvalue,25,25,INT_MAX,25,1},
  {"interrupts",interrupt_init,interrupt_getvalue,5,5,INT_MAX,5,1},
  {"context",context_init,context_getvalue,5,5,INT_MAX,5,1},
  {"load",load_init,load_getvalue,5,5,INT_MAX,5,LOAD_SCALE},
  {"collisions",collisions_init,collisions_getvalue,5,5,INT_MAX,5,1},
  {"errors",errors_init,errors_getvalue,5,5,INT_MAX,5,1},
  {"forks",forkstat_init,forkstat_getvalue,5,5,INT_MAX,5,1},
};
#define NR_STATS (sizeof(Moniterable)/sizeof(struct Statistic))

#define INFOWINHEIGHT 15

void AroundConfigureProc(report)
XConfigureEvent *report;
{
  XResizeWindow(display,statwin,report->width,report->height);
}

/* Format Specifiers
   %m Max Value
   %n Type Monitered
   %h Host Name
   %Hx Host name truncated to X periods. (Single Character)
*/

void perfmon_help()
{
  int loop;

  GL_StripChart_Help();
  fputs("   -nolabel   Turns of the label\n",stderr);
  fputs("   -format    Format of the label\n",stderr);
  fputs("\nValid Format Specifiers:\n",stderr);
  fputs("    %m  Current Maximum\n",stderr);
  fputs("    %n  Name of statistic monitered\n",stderr);
  fputs("    %h  Full hostname\n",stderr);
  fputs("    %Hx Hostname truncated to x periods. [0-9]\n",stderr);
  fputs("    %%  Percent character (%)\n",stderr);
  fputs("\nMoniterable Statistics :\n",stderr);
  for(loop=0;loop<NR_STATS;loop++)
    fprintf(stderr,"    -%s\n",Moniterable[loop].name);
}

void copyhostname(target,periods)
char *target;
int periods;
{
  char *loop=hostname;
  int periodcount=0;

  for(;;) {
    if (*loop=='.')
      periodcount++;
    if (*loop=='\0'||periodcount>=periods)
      break;
    *target= *loop;
    target++;
    loop++;
  }
}

char *newlabel(themax,multiplier)
int themax,multiplier;
{
  static char newstring[200];
  char *loop=labelformat,*target=newstring;
  int clearloop;

  for (clearloop=0;clearloop<200;clearloop++)
    newstring[clearloop]='\0';
  for (;*loop!='\0';loop++) {
    switch (*loop) {
    case '%' : loop++;
      switch (*loop) {
      case 'h' : strcat(target,hostname);
	target = target+strlen(target);
	break;
      case 'H' : loop++;
	if (*loop<'0'||*loop>'9')
	  fprintf(stderr,"%H modifier out of range :%c\n",*loop);
	else
	  copyhostname(target,*loop-'0');
	target += strlen(target);
	break;
      case 'm' : 
	if (multiplier==1)
	  sprintf(target,"%d",themax/multiplier);
	else
	  sprintf(target,"%.2f",(float)themax/(float)multiplier);
	target = target+strlen(target);
	break;
      case 'n' : strcat(target,Moniterable[curstat].name);
	target = target+strlen(target);
	break;
      case '%' : strcat(target,"%");
	target = target+strlen(target);
	break;
      default: fprintf(stderr,"Unknown % modifier :%c\n",*loop);
      }
      break;
      default : *target = *loop;
      target++;
    }
  }
  return newstring;
}

void ChangeScaleMaxFunc(themax,multiplier)
int themax,multiplier;
{
  Change_Label(labelwin,newlabel(themax,multiplier));
}

void MainMenuProc(report)
XEvent *report;
{
  GL_HandleMenu(report,menuwin);
}

void Switchstat(statnum)
int statnum;
{
  close_any();
  curstat = statnum;
  Moniterable[curstat].initproc();
  Change_Label(labelwin,newlabel(Moniterable[curstat].startscale,
				 Moniterable[curstat].multiplier));
  GL_Modify(GL_StripChart, GL_GetDataList(statwin),
	    GLSC_GetFunc, Moniterable[curstat].updatefunc,
	    GLSC_CurScale, Moniterable[curstat].startscale,
	    GLSC_MinScale, Moniterable[curstat].minscale,
	    GLSC_MaxScale, Moniterable[curstat].maxscale,
	    GLSC_DeltaScale, Moniterable[curstat].deltascale,
	    GLSC_Multiplier, Moniterable[curstat].multiplier,
	    GL_Done);
  GL_ResetStripChart(statwin);
}

void SwitchtoCpu()
{
  Switchstat(0);
}
  
void SwitchtoDisk()
{
  Switchstat(1);
}

void SwitchtoPackets()
{
  Switchstat(2);
}

void SwitchtoPaging()
{
  Switchstat(3);
}

void SwitchtoSwapping()
{
  Switchstat(4);
}

void SwitchtoInterrupts()
{
  Switchstat(5);
}

void SwitchtoContext()
{
  Switchstat(6);
}

void SwitchtoLoad()
{
  Switchstat(7);
}

void SwitchtoCollisions()
{
  Switchstat(8);
}

void SwitchtoErrors()
{
  Switchstat(9);
}

void SwitchtoForks()
{
  Switchstat(10);
}

main(argc,argv)
int argc;
char **argv;
{
  int argloop,loop,argchoice= -1;
  char *windowname;
  char *classname;
  Boolean showname=True;
  VPTF changefunc=NULL;

  curstat = -1;
  for (argloop=1;argloop<argc;argloop++)
    for (loop=0;loop<NR_STATS;loop++)
      if (strcmp(Moniterable[loop].name,argv[argloop]+1)==0) {
	if (curstat != -1) {
	  fprintf(stderr,"More than one moniter type argument given.\n");
	  fprintf(stderr,"%s was the second.\n",argv[argloop]);
	  exit(-1);
	}
	curstat = loop;
	argchoice = argloop;/* to mark used with. */
      }

  if (curstat== -1)
    curstat = 0;/* Default to cpu */

  windowname = Moniterable[curstat].name;

  classname = malloc(strlen(windowname)+9);
  strcpy(classname,"perfmon.");
  strcat(classname,windowname);
  GL_InitGraphics(argc,argv,classname,perfmon_help);
  if (argchoice != -1)
    GL_MarkArgument(argchoice,GL_ARG_USED);
  gethostname(hostname,32);
  GL_Create(GL_Resources,
	    GL_RES_BOOLEAN, "label", "label", &showname,
	    GL_RES_STRING, "format", "format", &labelformat,
	    GL_Done);
  aroundwin = GL_CreateWindow(GL_Window,
			      GL_PrimaryWindow,
			      GL_WinName, windowname,
			      GL_WinHeight,200+(showname?INFOWINHEIGHT:0),
			      ConfigureNotify, AroundConfigureProc,
			      GL_Done);
  loop = Moniterable[curstat].startscale;
  windowname = newlabel(loop,Moniterable[curstat].multiplier);
  if (showname) {
    labelwin = GL_CreateWindow(GL_Label,
			       GLL_InitialLabel, windowname,
			       GL_WinParent, aroundwin,
			       ButtonPress, MainMenuProc,
			       GL_Done);
    changefunc = ChangeScaleMaxFunc;
  }
  Moniterable[curstat].initproc();
  statwin = GL_CreateWindow(GL_StripChart,
			    GL_WinParent, aroundwin,
			    GL_WinY, showname?INFOWINHEIGHT:0,
			    GL_WindowBorderWidth, 0,
			    GLSC_GetFunc, Moniterable[curstat].updatefunc,
			    GLSC_CurScale, Moniterable[curstat].startscale,
			    GLSC_MinScale, Moniterable[curstat].minscale,
			    GLSC_MaxScale, Moniterable[curstat].maxscale,
			    GLSC_DeltaScale, Moniterable[curstat].deltascale,
			    GLSC_Multiplier, Moniterable[curstat].multiplier,
			    GLSC_ChangeScaleMaxFunc, changefunc,
			    ButtonPress, MainMenuProc,
			    GL_Done);
  menuwin = GL_CreateWindow(GL_Menu,
			    GL_ComplexMenuItem,
			      GL_MItemName, "Performance Meter",
			      GL_MItemTitle,
			      GL_MItemSeparator,
			      GL_Done,
			    GL_MenuItem, "Show Cpu", SwitchtoCpu,
			    GL_MenuItem, "Show Disk", SwitchtoDisk,
			    GL_MenuItem, "Show Packets", SwitchtoPackets,
			    GL_MenuItem, "Show Paging", SwitchtoPaging,
			    GL_MenuItem, "Show Swapping", SwitchtoSwapping,
			    GL_MenuItem, "Show Interrupts", SwitchtoInterrupts,
			    GL_MenuItem, "Show Context", SwitchtoContext,
			    GL_MenuItem, "Show Load", SwitchtoLoad,
			    GL_MenuItem, "Show Collisions", SwitchtoCollisions,
			    GL_MenuItem, "Show Errors", SwitchtoErrors,
			    GL_MenuItem, "Show Forks", SwitchtoForks,
			    GL_ComplexMenuItem, 
			      GL_MItemName, "",
			      GL_MItemTitle,
			      GL_MItemSeparator,
			      GL_Done,
			    GL_MenuItem, "Quit", exit,
			    GL_Done);
			    
  GL_MainLoop(aroundwin);
}

