/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *dofix_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/dec_mips_42/RCS/dofix.c,v 1.2 1991/09/12 17:01:27 bobg Exp $";

/* 
	dofix.c - convert .o file into .do file

	Author:  Zalman Stern July 1989
 */

/* This version of dofix is much more simpler than ones for other machine types
 * since MIPS' ld does what we want...
 */

#include <stdio.h> /* For NULL in absence of stddef.h */

static char *ComputeOutputFileName (InputFileName, extension)
char *InputFileName;
char *extension;
{

    static char name[256];
    register char  *p, *q;
    char   *ext;

 /* copy the input name and look for the last '.' */

    for (p = InputFileName, q = name, ext = NULL; *p != '\0';) {
	if (*p == '/')		/* ignore period if '/' follows */
	    p++, q = name, ext = NULL;
	else
	    if ((*q++ = *p++) == '.')
		ext = q - 1;
    }
    if (ext == NULL)
	ext = q;
    *ext = '\0';

 /* overwrite the extension with new extension */

    strncat(name, extension, 255);
    if (strcmp(InputFileName, name) == 0)
	strncat(name, extension, 255);
    return name ;
}

main(argc, argp)
int argc;
char **argp;
{
    char *InputFileName;
    char *OutputFileName;
    char *EntryPointName = NULL;
    int gotcha = 0;
    char CommandBuffer[1024];

    while (--argc > 0) {
	if (**++argp == '-') {
	    switch (*++*argp) {
	    case 'd':
		break;
	    case 'e':
		if (*++*argp)
		    EntryPointName = *argp;
		else {
		    EntryPointName = *++argp;
		    argc--;
		}
                if (EntryPointName[0] == '_')
                    EntryPointName++;
		break;
	    default:
		fprintf(stderr, "dofix:  Unknown switch -%c ignored\n", *argp);
	    }
	}
	else {
	    gotcha++;
            InputFileName = *argp;
	    OutputFileName = ComputeOutputFileName(InputFileName, ".do");
        }
    }
    if (gotcha == 0) {
        InputFileName = "-";
        InputFileName = "-";
    }

    sprintf(CommandBuffer, "ld -x -r -e %s %s -o %s", EntryPointName,
            InputFileName, OutputFileName);
    system(CommandBuffer);

    exit(0);
}
