/*
  Include declarations
*/
#include <stdio.h>
#include <math.h>
#ifdef __STDC__
#include <stdlib.h>
#else
#include <ctype.h>
#ifndef vms
#include <malloc.h>
#include <memory.h>

extern long
  strtol(),
  time();
#endif
#endif
#undef index

/*
  Define declarations for the Display program.
*/
#define False  0
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#define MinInfoSize (1 << 18)
#define True  1
#define Warning(message,qualifier)  \
{  \
  (void) fprintf(stderr,"%s: %s",application_name,message);  \
  if (qualifier != (char *) NULL)  \
    (void) fprintf(stderr," (%s)",qualifier);  \
  (void) fprintf(stderr,".\n");  \
}

/*
  Variable declarations.
*/
char
  *application_name;

#ifndef lint
static char 
  Version[]="@(#)ImageMagick 91/01/01 cristy@dupont.com";
#endif
