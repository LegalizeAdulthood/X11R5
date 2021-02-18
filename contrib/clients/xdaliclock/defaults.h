/* Resource and command-line junk
 *
 * xdaliclock.c, Copyright (c) 1991 Jamie Zawinski <jwz@lucid.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */


char *xdaliclockDefaults = "\
*name:			XDaliClock\n\
*mode:			12\n\
*seconds:		True\n\
*cycle:			False\n\
*shape:			False\n\
*memory:		Low\n\
*title:			Dali Clock\n\
*foreground:		black\n\
*background:		white\n\
XDaliClock*font:	BUILTIN\n\
";

/* The font resource above has the class specified so that if the user
   has "*font:" in their personal resource database, XDaliClock won't
   inherit it unless its class is specified explicitly.  To do otherwise
   would be annoying.
 */

XrmOptionDescRec xdaliclock_options [] = {
  { "-12",		".mode",		XrmoptionNoArg, "12"  },
  { "-24",		".mode",		XrmoptionNoArg, "24"  },
  { "-seconds",		".seconds",		XrmoptionNoArg, "on"  },
  { "-noseconds",	".seconds",		XrmoptionNoArg, "off" },
  { "-cycle",		".cycle",		XrmoptionNoArg, "on"  },
  { "-nocycle",		".cycle",		XrmoptionNoArg, "off" },
  { "-reverse",		".reverseVideo",	XrmoptionNoArg, "on"  },
  { "-rv",		".reverseVideo",	XrmoptionNoArg, "on"  },
  { "-synchronous",	".synchronous",		XrmoptionNoArg, "on"  },
  { "-oink",		".memory",		XrmoptionNoArg, "medium" },
  { "-oink-oink",	".memory",		XrmoptionNoArg, "high"   },
#ifdef BUILTIN_FONT
  { "-builtin",		".font",		XrmoptionNoArg, "BUILTIN" },
#endif
#ifdef SHAPE
  { "-shape",		".shape",		XrmoptionNoArg, "on"  },
  { "-noshape",		".shape",		XrmoptionNoArg, "off" },
#endif
  { "-memory",		".memory",		XrmoptionSepArg, 0 },
  { "-sleaze-level",	".memory",		XrmoptionSepArg, 0 },
  { "-name",		".name",		XrmoptionSepArg, 0 },
  { "-display",		".display",		XrmoptionSepArg, 0 },
  { "-geometry",	".geometry",		XrmoptionSepArg, 0 },
  { "-geom",		".geometry",		XrmoptionSepArg, 0 },
  { "-foreground",	".foreground",		XrmoptionSepArg, 0 },
  { "-background",	".background",		XrmoptionSepArg, 0 },
  { "-fg",		".foreground",		XrmoptionSepArg, 0 },
  { "-bg",		".background",		XrmoptionSepArg, 0 },
  { "-border",		".border",		XrmoptionSepArg, 0 },
  { "-borderwidth",	".borderWidth",		XrmoptionSepArg, 0 },
  { "-bw",		".borderWidth",		XrmoptionSepArg, 0 },
  { "-title",		".title",		XrmoptionSepArg, 0 },
  { "-font",		".font",		XrmoptionSepArg, 0 },
  { "-fn",		".font",		XrmoptionSepArg, 0 },
  { "-xrm",		0,			XrmoptionResArg, 0 }
};

int num_options = sizeof (xdaliclock_options) / sizeof (XrmOptionDescRec);
