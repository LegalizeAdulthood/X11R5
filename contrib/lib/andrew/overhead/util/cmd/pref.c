/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: */
/* $ACIS: */
/* $Source: */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <andrewos.h> 
#include <stdio.h>
#include <util.h>

#define boolean short
#define FALSE 0
#define TRUE 1

char ProgramName[100]="pf";

#define IFARG(argv) if(*++*argv=='\0')argv++;if(argv!=NULL && **argv!='\0')
#define DIE(str) (fprintf(stderr,str),exit(-1))
#define USAGE "Usage: pref [-p programname] [-sbi] preferencename [default|value]\n"

boolean atob(str)
char *str;
{
    int i;
    static struct keys {
	char   *name;
	boolean value;
    } keys[] = {
	"true",	    TRUE,
	"false",    FALSE,
	"on",	    TRUE,
	"off",	    FALSE,
	"yes",	    TRUE,
	"no",	    FALSE,
	"1",	    TRUE,
	"0",	    FALSE,
	NULL,	    FALSE
    };

    for(i=0;keys[i].name!=NULL;i++)
	if(FOLDEDEQ(keys[i].name,str))
	    return keys[i].value;

    return FALSE;
}

main(argc,argv)
int argc;
char **argv;
{
    char *name,*def = NULL;
    boolean boolVal=FALSE, intVal=FALSE, set=FALSE;

    while(*++argv!=NULL && **argv=='-')
	switch(*++*argv){
	    case 'p':
		IFARG(argv)
		  strcpy(ProgramName,*argv);
		else
		    DIE("-p needs a program name\n");
		break;
	    case 's':
		set=TRUE; break;
	    case 'b':
		boolVal=TRUE; break;
	    case 'i':
		intVal=TRUE; break;
	}

    if(*argv==NULL)
	DIE(USAGE);

    name= *argv++;

    if(*argv==NULL)
	if(set)
	    DIE("You must supply a value to set\n");
	else
	    def=NULL;
    else
	def= *argv++;

    if(set){
	char *dot=index(name,'.'),*prog;

	/* sigh... setprofilestring doesn't parse things */
	if(dot==NULL)
	    prog=ProgramName;
	else{
	    prog=name;
	    *dot++='\0';
	    name=dot;
	}

	setprofilestring(prog,name,def);
    }
    else if(boolVal)
	puts(getprofileswitch(name,(def==NULL ? FALSE : atob(def))) ? "true" : "false");
    else if(intVal)
	printf("%d\n",getprofileint(name,(def==NULL ? 0 : atoi(def))));
    else{
	char *s=getprofile(name);
	if(s==NULL && def==NULL)
	    exit(1);
	puts(s==NULL ? def : s);
    }

    exit(0);
}
