/*------------------------------ libXO.h -------------------------------*/

/*
 *  header for libXO
 *
 *  Author:  Miles O'Neal
 *
 *  Release: 1.0 meo - FCS release
 *
 *  COPYRIGHT 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA
 *
 *  Permission is hereby granted to use, copy, modify and/or distribute
 *  this software and supporting documentation, for any purpose and
 *  without fee, as long as this copyright notice and disclaimer are
 *  included in all such copies and derived works, and as long as
 *  neither the author's nor the copyright holder's name are used
 *  in publicity or distribution of said software and/or documentation
 *  without prior, written permission.
 *
 *  This software is presented as is, with no warranties expressed or
 *  implied, including implied warranties of merchantability and
 *  fitness. In no event shall the author be liable for any special,
 *  direct, indirect or consequential damages whatsoever resulting
 *  from loss of use, data or profits, whether in an action of
 *  contract, negligence or other tortious action, arising out of
 *  or in connection with the use or performance of this software.
 *  In other words, you are on your own. If you don't like it, don't
 *  use it!
 */

int XONormalizeColorName(
#ifdef __STDC__

char *color		/* unnormalized color name */

#endif
);



int XOGetStdColorNames(
#ifdef __STDC__

char *colors[];		/* array to hold normalized color names */
int maxcolors;		/* # of elements in colors */
int *maxlen;		/* max # of chars in a normalized name */
int showgray;		/* 1 if grays wanted, else 0 */
char *namefile;		/* file from which to read color names */

#endif
);



Widget XOCreateHelpBtn(
#ifdef __STDC__

Widget	parent;		/* parent of help button widget */
Widget	root;		/* root of help button widget */
void	(*cb) ();	/* user-supplied callback (if any) */
Arg	*res;		/* additional resources (if any) */
int	nres;		/* # of additional resources (if any) */
char	*helpText;	/* user-supplied help text (they'd better!) */

#endif
);



int
XOAddIconToWidget(
#ifdef __STDC__

Widget w;		/* shellClass or other class that takes iconBitmap */
char *bits;		/* xbm format _bits variable name */
int width;		/* xbm format _width field */
int height;		/* xbm format _height field */

#endif
);



Widget
XOCreateCancelBtn(
#ifdef __STDC__

Widget	parent;		/* parent widget of cancel button widget */
Widget	root;		/* root widget of cancel button widget */
void	(*cb) ();	/* user-supplied callback (if any) */
Arg	*res;		/* additional resources (if any) */
int	nres;		/* # of additional resources (if any) */

#endif
);



Widget
XOCreateQuitBtn(
#ifdef __STDC__

Widget	parent;		/* parent widget of quit button widget */
Widget	root;		/* root widget of quit button widget */
void	(*cb) ();	/* user-supplied callback (if any) */
Arg	*res;		/* additional resources (if any) */
int	nres;		/* # of additional resources (if any) */

#endif
);
