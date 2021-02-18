/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/indexpro.c,v 1.6 1991/09/12 16:35:06 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/indexpro.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/indexpro.c,v 1.6 1991/09/12 16:35:06 bobg Exp $";
#endif /* lint */
#include <stdio.h>
#include <ctype.h>
#include <andrewos.h>
#define GAP 1
#define BUFSIZE 5120
#define NUMSIZE 512
#define DCHAR '+'
static char header[] = "\
.SH \n\
Index\n\
.LP \n\
.nr PS 8\n\
.nr VS 9\n\
.MC 1.9i\n\
.na	\n\
.de XX	\n\
.br	\n\
.ti -.2i\n\
.ne 2	\n\
..\n\
.de YY 	\n\
.sp 1.5\n\
.ne 3\n\
.ce 1\n\
- \\\\$1 -\n\
.sp .5\n\
..\n\
.in .2i	\n\
.hy 0\
";
int cmp(a,b)
int *a,*b;
{
    return(*a - *b);
}
static char lbuf[512];
output(buf,n,np)
char *buf;
int *n,*np;
{
    int *tp;
    static char lastc = ' ';
    char *lc;
    int head = 0;
    if(lastc != *buf) {
	printf(".XX\n.YY %c\n.XX\n",*buf);
	lastc = *buf;
	head++;
    }
    if(np > n + 2) qsort((char *)n,np - n,sizeof(int),cmp);
    if((lc = (char *)index(buf,DCHAR)) != NULL && lc[1] == DCHAR &&
	lc[2] != ' ' && lc[2] != '\0'){
	int len = lc - buf;
	if((strlen(lbuf) == len) && strncmp(lbuf,buf,(len)) == 0){
	    buf = lc + 2;
	}
	else {
	    strncpy(lbuf,buf,len);
	    lbuf[len] = '\0';
	    if (head == 0) puts(".XX\n");
	    puts(lbuf);
	    buf = lc + 2;
	}
	putchar(' ');
    }
    else {
	if (head == 0) puts(".XX\n");
	strcpy(lbuf,buf);
    }
    fputs(buf,stdout);
    putchar(',');putchar(' ');
    while(n < np){
	printf("%d",*n);
	if(n + 1 == np) break;
	for(tp = n + 1;tp != np; tp++){
	    if(*tp == *n) continue;
	    if(*tp <= *(tp - 1) + GAP) continue;
	    break;
	}
	if(tp == n + 1){
	    putchar(',');
	    n++;
	    continue;
	}
	tp--;
	if(*tp != *n) printf("-%d",*(tp));
	n = tp + 1;
	if(n != np) putchar(',');
    }
    putchar('\n');
}

main(argc,argv)
int argc;
char *argv[];
{
    char buf[BUFSIZE],rbuf[BUFSIZE],*begin, *end,*c;
    int num[NUMSIZE],*np;
    puts(header);
    *buf = '\0';
    *lbuf = '\0';
    np = num;
    begin = rbuf + 4; /* skip over "ix: " */
    while(gets(rbuf) != NULL){
	end = rindex(rbuf,'\t');
	if(end == NULL) continue;
	for(c = end ; isspace(*c) && c > rbuf; c--)
	    *c = '\0';
	end++;
	if(islower(*begin)) *begin = toupper(*begin);
	if(*buf == '\0')
	    strcpy(buf,begin);
	else if(strcmp(begin,buf) != 0){
	    output(buf,num,np);
	    np = num;
	    strcpy(buf,begin);
	}
	*np++ = atoi(end);
    }
    if(*buf) output(buf,num,np);
    exit(0);
}

	
