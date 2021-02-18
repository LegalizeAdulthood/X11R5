/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *dofix_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/sgi_mips/RCS/dofix.c,v 1.2 1991/10/03 15:58:42 bobg Exp $";

/* 
	dofix.c - convert .o file into .do file

	Author:  Zalman Stern July 1989
 */

/* This version of dofix is much more simpler than ones for other machine types
 * since MIPS' ld does what we want...
 */

#include <stdio.h> /* For NULL in absence of stddef.h */

#include <sys/fcntl.h>
#include <filehdr.h>
#include <scnhdr.h>

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

static char *argv0 = "dofix" ;

static void
ErrorExit( char *msg )
{
	(void) fprintf( stderr, "%s: %s\n", argv0, msg ) ;
	exit( -1 ) ;
	/*NOTREACHED*/
}

static unsigned long int
ComputeBindingAddress( InputFileName )
	char *InputFileName ;
{
	unsigned long int coreaddr ;
	int fd ;
	struct filehdr fh ;
	struct scnhdr sh ;
	int hash ;
	char *ptr ;

	fd = open( InputFileName, O_RDONLY ) ;
	if ( fd < 0 )
		ErrorExit( "Can't open Input File" ) ;

	if ( read( fd, &fh, sizeof fh ) != sizeof fh )
		ErrorExit( "Can't read file header" ) ;
	lseek( fd, fh.f_opthdr, 1 ) ;	/* past optional hdr */
	if ( read( fd, &sh, sizeof sh ) != sizeof sh )
		ErrorExit( "Can't read section header" ) ;

	(void) close( fd ) ;

	/* Calculate "hashed" address for binding */
	for ( hash = 0, ptr = InputFileName ; *ptr ; ++ptr ) {
		hash += hash >> 4 ; /* Really ( x * 17 ) / 16 */
		hash += *ptr ;
	}
	/* Allow the section to range from 4M to 192M in 4M increments */
	hash %= 47 ;

	return ( ( hash + 1 ) * 0x00400000 ) + sh.s_scnptr ;
}

main(argc, argp)
int argc;
char **argp;
{
    char *InputFileName;
    char *OutputFileName;
    char *EntryPointName = NULL;
    int gotcha = 0;
    int textaddr ;
    char CommandBuffer[1024];
    char *debugsymflag = "-x" ; /* Strip locals by default */

	argv0 = *argp ;

    while (--argc > 0) {
	if (**++argp == '-') {
	    switch (*++*argp) {
	    case 'T' :
		(void) sscanf( *++argp, "%x", &textaddr ) ;
		argc--;
		break;
	    case 'g':
		debugsymflag = "-g" ;
		break;
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
    else if ( textaddr == 0 )
        textaddr = ComputeBindingAddress( InputFileName ) ;

    (void) sprintf( CommandBuffer, "ld -jumpopt %s -r -T %x -e %s %s -o %s",
		    debugsymflag, /* "-x" or "-g" -- Strip locals by default */
		    textaddr, EntryPointName,
		    InputFileName, OutputFileName ) ;
    system(CommandBuffer);

    exit(0);
}
