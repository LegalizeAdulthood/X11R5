/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/fonts/lib/RCS/fntnamng.c,v 2.5 1991/09/12 17:13:16 bobg Exp $ */
/* $ACIS:fntnamng.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/fonts/lib/RCS/fntnamng.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/fonts/lib/RCS/fntnamng.c,v 2.5 1991/09/12 17:13:16 bobg Exp $";
#endif /* lint */

/*********************************************************\
* 							  *
* 	File: fntnamng.c				  *
* 							  *
* Routines for manipulating font names.			  *
* 							  *
* HISTORY						  *
* 							  *
\*********************************************************/

#include "font.h"
#include <ctype.h>


/* ************************************************************ */
/*								*/
/*  FormatFontname						*/
/*								*/
/* ************************************************************ */

/*
	Create a FileName from a FontName structure 
*/

char *FormatFontname (n)
register struct FontName  *n;
{
   static char buf[128];
   char  rbuf[5];
   register char *p;

   /* first create the rotation substring */
   if (n->rotation)
      sprintf(rbuf, "r%d", n->rotation);
   else
      rbuf[0] = '\0';

   /* format the various pieces together */
   sprintf(buf, "%s%d%s%s%s%s%s",
	 n->FamilyName,
	 n->height,
	 rbuf,
	 n->FaceCode & BoldFace ? "b" : "",
	 n->FaceCode & ItalicFace ? "i" : "",
	 n->FaceCode & FixedWidthFace ? "f" : "",
	 n->FaceCode & ShadowFace ? "s" : "");

   /* make the file name all lower case */
   for (p = buf; *p; p++)
      if (isupper(*p))
	 *p = tolower(*p);

   return(buf);
}


/* ************************************************************ */
/*								*/
/*  parsefname							*/
/*								*/
/* ************************************************************ */

/*
	Take a file name (FileName) and parse it to create
	a FontName structure (Fontname)
*/

parsefname(FileName, Fontname)
register char *FileName;
register struct FontName  *Fontname;
{
   register char *p;
   register int   i;
   int   err = 0;

   /* strip off Family Name (e.g. TimesRoman); copy to struct */
   p = Fontname->FamilyName;
   i = sizeof(Fontname->FamilyName);
   while (isalpha(*FileName))
      if (--i > 0)
	 *p++ = *FileName++;
      else
	 FileName++;
   *p = '\0';
   if (FileName[0] == '\0')
      return(1) /* no Family Name */ ;

   /* now pick up the point size */
   Fontname->height = 0;
   while (isdigit(*FileName))
      Fontname->height = Fontname->height * 10 + (*FileName++ - '0');
   if (Fontname->height == 0)
      return(1) /* no point size */ ;

   /* rotation and facecodes are optional */
   Fontname->rotation = 0;
   Fontname->FaceCode = 0;
   while (*FileName != '\0')
      switch (*FileName++)
	 {
	    case 'b': 
		  Fontname->FaceCode |= BoldFace;
		  break;
	    case 'i': 
		  Fontname->FaceCode |= ItalicFace;
		  break;
	    case 'f': 
		  Fontname->FaceCode |= FixedWidthFace;
		  break;
	    case 'r': 
		  while (isdigit(*FileName))
		     Fontname->rotation = Fontname->rotation * 10
			+ *FileName++ - '0';
		  break;
	    case 's': /* Shadow font */
		  Fontname->FaceCode |= ShadowFace;
		  break;

	    case '.': /* ignore extensions of .fwm and .fdb */
		  if (strcmp(FileName, "fwm") == 0
			|| strcmp(FileName, "fdb") == 0)
		     FileName += 3;
		  break;

	    default: 
		  err++;
		  break;
	 }
   return(err);
}
