/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *whichdo_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/cmd/RCS/whichdo.c,v 2.5 1991/09/12 16:58:44 bobg Exp $";

/*
whichdo.c. Searches the CLASSPATH and reports on which .do module will be loaded by ez.

Author: Tom Neuendorffer
*/

#include <stdio.h>
#include <andrewos.h> /* sys/file.h */

char *getenv();

struct PathEntry {
    struct PathEntry *next;
    char name[1];
};
static struct PathEntry *globalPath = 0;
char *pathopen(),*rindex();

main(argc, argv)
int argc;
char *argv[];
{
    char *s;
    Initialize();
    argv++;
    while(--argc) {
	if((s = pathopen(*argv)) == NULL){
	    struct PathEntry *tpe;
	    if(rindex(*argv,'.') == NULL)
		printf("no %s.do in ",*argv);
	    else
		printf("no %s in ",*argv);
	    for(tpe=globalPath;tpe;tpe=tpe->next){
		fputs(tpe->name,stdout);
		putchar(' ');
	    }
	    putchar('\n');
	}
	else puts(s);
	argv++;
    }
}


static char *pathopen (aname)
register char *aname;
{/* open it along the path CLASSPATH */
    register struct PathEntry *tpe;
    register int fn;
    char *tail;
    static char tname[256];
    for(tpe=globalPath;tpe;tpe=tpe->next)
    {/* check out if this file exists */
	strcpy (tname,tpe->name);
	strcat (tname,"/");
	strcat (tname,aname);
	if((tail = rindex(aname,'.')) == NULL)
	    strcat (tname, ".do");
	fn = access (tname,R_OK);
	if (fn == 0) return tname;
    }
    return NULL;
}

void myclass_Init(defaultPath)
char *defaultPath;
{
    char *envString;
    register char *st1, *st2;
    struct PathEntry **lpath, *tpath;

    /* setup and parse path */
    envString = getenv("CLASSPATH");
    if (!envString) {
	if (defaultPath == NULL)
	    envString = "";
	else
	    envString = defaultPath;
    }
    st1 = envString;
    lpath = &globalPath;
    while (1) {
	st2 = st1;
	while (*st2 != '\000' && *st2 != ':') st2++;
	/* allocate one extra byte in case we convert null string to "." */
	tpath = (struct PathEntry *) malloc(sizeof (struct PathEntry) + 1 + st2-st1);
	tpath->next = NULL;
	strncpy(tpath->name, st1, st2-st1);
	tpath->name[st2-st1]=0;
	if (st1 == st2) {
	    /* if string is 0 length==> wdir */
	    strcpy(tpath->name,".");
	}
	*lpath = tpath;
	lpath = &tpath->next;
	if (*st2 == 0) break;
	st1 = st2+1;
    }

}

Initialize()
{
    myclass_Init((char *)AndrewDir("/dlib/atk"));
}

