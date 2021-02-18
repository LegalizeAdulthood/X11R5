/*
    t_Enum.c
    Unit test program for XrmEnumerateDatabase (and it's back-port to X11R4)
    Martin Brunecky, 09/10/91
    Portions of this code come from MIT X11 Xrm.c

    Use this program to verify that your PRE-X11R5 X11 implemntation
    is similar enough to the prevois MIt X11 sample implementations
    so that you can use my back-implemantation of XrmEnumerateDatabase
    (in file PreR5Enum.c).

    This program accepts the following three arguments:
    aa.bb.cc   -  enumeration selection resource path names
    Aa.Bb.Cc   -  enumeration selection resource class names
    filename   -  resource file pathname ( default=test.db).

    Note: arg1 must have the same number of names as arg2

    Example usage:
    
    File tight.db:
	a:   		line for "a"
	a.a: 		line for "a.a"
	a.a.a: 		line for "a.a.a"
	b:   		line for "b"
	b.b: 		line for "b.b"
	b.b.b: 		line for "b.b.b"
	A:   		line for "A"
	a*a: 		line for "a*a"
	a*b: 		line for "a*b"
	a.a*x.y.z: 	line for "a.a*x.y.z"


    Test run:
	$ t_Enum a.a A.A tight.db
	requested name >a.a< class >A.A< file >tight.db<

	The original database dump:
	a.a.a:  line for "a.a.a"
	a.a*x.y.z:      line for "a.a*x.y.z"
	a.a:    line for "a.a"
	a*a:    line for "a*a"
	a*b:    line for "a*b"
	a:      line for "a"
	b.b.b:  line for "b.b.b"
	b.b:    line for "b.b"
	b:      line for "b"
	A:      line for "A"

	Resources selected by XrmEnumAllLevels
	a.a.a:  line for "a.a.a"
	a.a*x.y.z:      line for "a.a*x.y.z"
	a*a:    line for "a*a"
	a*b:    line for "a*b"

	Resources selected by XrmEnumOneLevel
	a.a.a:  line for "a.a.a"
	a*a:    line for "a*a"
	a*b:    line for "a*b"

     Note that the order in which resources are reported does not
     match the original resource file - that is OK.

*/
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <stdio.h>

#ifdef  DEBUG
#define ONDEBUG(x) {x}
#else
#define ONDEBUG(x) {/* x */}
#endif

/* Definitions not available in X11R4 (for backport of XrmEnumerateDatabase)*/

#ifndef XrmEnumAllLevels 
#define XrmEnumAllLevels 0
#define XrmEnumOneLevel  1
typedef void* XPointer;
#endif


/* Xrm Database Dump routine data */

typedef struct _DumpData
        {
        FILE        *file;
        }
        DumpData;
/*
  -- PrintBindingQuarkList in the X resource specification format: a.b*c 
*******************************************************************************
*/
static void PrintBindingQuarkList(bindings, quarks, stream)
    XrmBindingList      bindings;
    XrmQuarkList        quarks;
    FILE                *stream;
{
    Bool        firstNameSeen;

    for (firstNameSeen = False; *quarks; bindings++, quarks++) {
        if (*bindings == XrmBindLoosely) {
            (void) fprintf(stream, "*");
        } else if (firstNameSeen) {
            (void) fprintf(stream, ".");
        }
        firstNameSeen = True;
        (void) fputs(XrmQuarkToString(*quarks), stream);
    }
}
/*
   -- DumpEntry - Dump the X resource Database entry in the correct syntax
*****************************************************************************
   This function is to be called by XrmEnumerateDatabase
   Note this incarnation copied from MIT Xrm.c does NOT handle other
   than String resource types - conversions are badly needed
*/
/*ARGSUSED*/
static Bool DumpEntry(db, bindings, quarks, type, value, data)
    XrmDatabase         *db;
    XrmBindingList      bindings;
    XrmQuarkList        quarks;
    XrmRepresentation   *type;
    XrmValuePtr         value;
    XPointer            data;
{
    static XrmQuark             XrmQString = NULLQUARK;
    DumpData*                   dd         = (DumpData*) data;
    FILE                        *stream    = dd->file;
    register unsigned int       i;
    register char               *s;
    register char               c;

    if ( XrmQString == NULLQUARK ) XrmQString = XrmStringToQuark("String");

    if (*type != XrmQString)
        (void) putc('!', stream);   /* what a solution ... */

    PrintBindingQuarkList(bindings, quarks, stream);
    s = value->addr;
    i = value->size;
    if (*type == XrmQString) {
        (void) fputs(":\t", stream);
        if (i)
            i--;
    }
    else
        fprintf(stream, "=%s:\t", XrmRepresentationToString(*type));
    if (i && (*s == ' ' || *s == '\t'))
        (void) putc('\\', stream); /* preserve leading whitespace */
    while (i--) {
        c = *s++;
        if (c == '\n') {
            if (i)
                (void) fputs("\\n\\\n", stream);
            else
                (void) fputs("\\n", stream);
        } else if (c == '\\')
            (void) fputs("\\\\", stream);
        else if ((c < ' ' && c != '\t') ||
                 ((unsigned char)c >= 0x7f && (unsigned char)c < 0xa0))
            (void) fprintf(stream, "\\%03o", (unsigned char)c);
        else
            (void) putc(c, stream);
    }
    (void) putc('\n', stream);
    return False;
}

/*
   Main Test program...
************************************************************************
*/
main (argc,argv)
int argc;
char *argv[];
{
   XrmDatabase db;
   XrmQuark   empty = NULLQUARK;
   char*      name  = argv[1];
   char*      class = argv[2];
   char*      file  = argv[3];
   XrmQuark   name_pref[20];
   XrmQuark   class_pref[20];
   DumpData   dd;

   if ( argc < 1 || !name ) name  = "oops";
   if ( argc < 2 || !class) class = "Oops";
   if ( argc < 3 || !file ) file  = "test.db";


   printf ("requested name >%s< class >%s< file >%s<\n",name,class,file );

   XrmInitialize();
   db = XrmGetFileDatabase ( file );

   if (!db)
     { 
     printf("...failed to load file %s\n", file);
     exit(1);
     }

   XrmStringToQuarkList ( name,  name_pref  );
   XrmStringToQuarkList ( class, class_pref );


   printf("\nThe original database dump:\n");
   dd.file = stdout;

   (void) XrmEnumerateDatabase ( db, &empty, &empty, XrmEnumAllLevels,
                               DumpEntry, (XPointer) &dd );

   printf("\nResources selected by XrmEnumAllLevels\n");

   (void) XrmEnumerateDatabase ( db, name_pref, class_pref, XrmEnumAllLevels,
                               DumpEntry, (XPointer) &dd );

   printf("\nResources selected by XrmEnumOneLevel\n");

   (void) XrmEnumerateDatabase ( db, name_pref, class_pref, XrmEnumOneLevel,
                               DumpEntry, (XPointer) &dd );


} 
