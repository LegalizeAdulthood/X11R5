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
#include <sys/time.h>
#include "X11/gl/label.h"
#include "X11/gl/resmgr.h"
#include "X11/gl/patchlevel.h"

/* Formatting foo :
   %d day
   %m month        - three char
   %M month        - written out
   %y year         - two char
   %Y year         - written out
   %w weekday      - three char
   %W weekday      - written out
   %% %            - percent sign

   @h Hours
   @H Hours Military Time
   @m Minutes
   @s Seconds
   @p AM/PM
   @: Blinking :
   @@ @ at sign.
*/

static char *format;
static int blinkstate=0;

char PatchLevel_GLClock[] = PATCHLEVEL_GLCLOCK;

char *endofstring(what)
char *what;
{
  while(*what!='\0')
    what++;
  return what;
}

char *ThreeCharMonth(month)
int month;
{
  switch(month) {
  case 0  : return "Jan";
  case 1  : return "Feb";
  case 2  : return "Mar";
  case 3  : return "Apr";
  case 4  : return "May";
  case 5  : return "Jun";
  case 6  : return "Jul";
  case 7  : return "Aug";
  case 8  : return "Sep";
  case 9  : return "Oct";
  case 10 : return "Nov";
  case 11 : return "Dec";
  }
  return "Bogus";
}

char *Month(month)
int month;
{
  switch(month) {
  case 0  : return "January";
  case 1  : return "February";
  case 2  : return "March";
  case 3  : return "April";
  case 4  : return "May";
  case 5  : return "June";
  case 6  : return "July";
  case 7  : return "August";
  case 8  : return "September";
  case 9  : return "October";
  case 10 : return "November";
  case 11 : return "December";
  }
  return "Bogus";
}

char *ThreeCharWeekday(day)
int day;
{
  switch(day) {
  case 0 : return "Sun";
  case 1 : return "Mon";
  case 2 : return "Tue";
  case 3 : return "Wed";
  case 4 : return "Thu";
  case 5 : return "Fri";
  case 6 : return "Sat";
  }
  return "Bogus";
}

char *Weekday(day)
int day;
{
  switch(day) {
  case 0 : return "Sunday";
  case 1 : return "Monday";
  case 2 : return "Tuesday";
  case 3 : return "Wednesday";
  case 4 : return "Thursday";
  case 5 : return "Friday";
  case 6 : return "Saturday";
  }
  return "Bogus";
}

void HandlePercent(key,timestruct,target)
char key;
struct tm *timestruct;
char *target;
{
  switch (key) {
  case 'd' :
    if (timestruct->tm_mday<10)
      strcat(target," ");
    sprintf(target,"%d",timestruct->tm_mday);
    break;
  case 'm' :
    strcat(target,ThreeCharMonth(timestruct->tm_mon));
    break;
  case 'M' :
    strcat(target,Month(timestruct->tm_mon));
    break;
  case 'y' :
    if (timestruct->tm_year%100<100)
      strcat(target,"0");
    sprintf(target,"%d",timestruct->tm_year%100);
    break;
  case 'Y' :
    sprintf(target,"%d",timestruct->tm_year+1900);
    break;
  case 'w' :
    strcat(target,ThreeCharWeekday(timestruct->tm_wday));
    break;
  case 'W' :
    strcat(target,Weekday(timestruct->tm_wday));
    break;
  case '%' :
    strcat(target,"%");
    break;
    default :
    fprintf(stderr,"Unrecognized %% modifier:%c\n",key);
    exit(-1);
  }
}

void HandleAt(key,timestruct,target)
char key;
struct tm *timestruct;
char *target;
{
  switch (key) {
    case 'h' : 
      if(timestruct->tm_hour%12==0)
	strcat(target,"12");
      else
	sprintf(target,"%d",timestruct->tm_hour%12);
      break;
    case 'H' :
      sprintf(target,"%d",timestruct->tm_hour);
      break;
    case 'm' :
      if (timestruct->tm_min<10) {
	*target = '0';
	target++;
      }
      sprintf(target,"%d",timestruct->tm_min);
      break;
    case 's' :
      if (timestruct->tm_sec<10) {
	*target = '0';
	target++;
      }
      sprintf(target,"%d",timestruct->tm_sec);
      break;
    case 'p' :
      if (timestruct->tm_hour>11)
	strcat(target,"P.M.");
      else
	strcat(target,"A.M.");
      break;
    case '@' :
      strcat(target,"@");
      break;
    case ':' :
      strcat(target,blinkstate?":":" ");
      break;
      default :
      fprintf(stderr,"Unrecognized @ modifier:%c\n",key);
      exit(-1);
  }
}

char *makestring()
{
  struct tm *timestruct;
  long clock;
  static char curtime[200];
  char *loop,*target;

  for(clock=0;clock<200;clock++)
    curtime[clock]='\0';
  clock = time(NULL);
  timestruct = localtime(&clock);
  for (loop = format,target = curtime;*loop!='\0';loop++) {
    switch (*loop) {
    case '%' : loop++;
      HandlePercent(*loop,timestruct,target);
      break;
    case '@' : loop++;
      HandleAt(*loop,timestruct,target);
      break;
    default : 
      *target= *loop;
      target++;
      *target = '\0';
      break;
    }
    target=endofstring(target);
  }
  return curtime;
}

void IntervalProc(window)
Window window;
{
  blinkstate= !blinkstate;
  Change_Label(window,makestring());
}

void KeyPressProc(report)
XKeyPressedEvent *report;
{
  if (XKeycodeToKeysym(display,report->keycode,0)=='q')
    exit(1);
}

void help()
{
  GL_Label_Help();
  fputs("   -format <str>    Specify format string to use\n",stderr);
  fputs("\n",stderr);
  fputs("Valid format specifiers:\n",stderr);
  fputs("    %d day\n",stderr);
  fputs("    %m month (three char)           @h Hours (12 hour)\n",stderr);
  fputs("    %M month (written out)          @H Hours (24 hour)\n",stderr);
  fputs("    %y year (last 2 digits)r        @m Minutes\n",stderr);
  fputs("    %Y year (written out)           @s Seconds\n",stderr);
  fputs("    %w weekday (three char)         @p AM/PM\n",stderr);
  fputs("    %W weekday (written out)        @: Blinking colon (:)\n",stderr);
  fputs("    %% percent sign (%)             @@ at sign (@)\n",stderr);
}

main(argc,argv)
int argc;
char **argv;
{
  Window mainwin;
  struct timeval Interval;
  char *fontname=NULL;
  XFontStruct *font = NULL;

  format = "%w %m %d @H:@m:@s %Y";
  GL_InitGraphics(argc,argv,"glclock",help);

  GL_Create(GL_Resources,
	    GL_RES_STRING, "format", "format", &format,
	    GL_Done);
  if (fontname!=NULL) {
    font = XLoadQueryFont(display,fontname);
    if (font==NULL) 
      fprintf(stderr,"Warning, font %s not found.\n",fontname);
  }
  mainwin = (Window)GL_Create(GL_Label,
			      GLL_InitialLabel, makestring(),
			      GLL_Font, font==NULL?default_font:font,
			      GL_WinName, "glclock",
			      GL_PrimaryWindow,
			      KeyPress, KeyPressProc,
			      GL_Done);
  
  Interval.tv_usec=0;
  Interval.tv_sec=1;
  GL_SetTimeOut(&Interval,(VPTF)IntervalProc,mainwin);
  GL_MainLoop(mainwin);
}
