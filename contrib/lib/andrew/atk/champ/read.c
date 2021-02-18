/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *read_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/champ/RCS/read.c,v 2.6 1991/09/12 16:04:08 bobg Exp $";

#include <andrewos.h>
#include "stdio.h"
#include "ctype.h"
#include "champ.h"
#include "sys/param.h"
#include "pwd.h"
#include "util.h"
#include "errno.h"

extern char *index();
extern int errno;
struct eventnode *readdateintoeventnode();
struct eventnode *RootEventNode = NULL;

int ReadDatesFromChampPath(champpath) 
char *champpath;
{
    char *colon, FBuf[1+MAXPATHLEN];
    FILE *fp;

    if (!champpath) champpath = getprofile("champpath");
    if (!champpath) champpath = "~/events.champ:~/.events.champ";
    while(champpath) {
	colon = index(champpath, ':');
	if (colon) *colon++ = NULL;
	ResolveTildes(champpath, FBuf);
	champpath = colon;
	fp = fopen(FBuf, "r");
	if (!fp) {
	    if (errno != ENOENT) perror(FBuf);
	    continue;
	}
	ReadDatesFromFile(fp);
	fclose(fp);
    }
    return(0);
}

ResolveTildes(in, out)
char   *in, *out;
{
    char   *t, user[1000];
    struct passwd *p;

    while (in && *in && (*in == ' ' || *in == '\t' || *in == '\n')) {
	++in;
    }
    if (*in != '~') {
	strcpy(out, in);
	return;
    }
    if (*++in == '/') {
	p = getvpwuid(getvuid());
	if (p) {
	    sprintf(out, "%s/%s", p->pw_dir, ++in);
	} else {
	    strcpy(out, in);
	}
	return;
    }
    for (t = user; *in && *in != '/'; ++in, ++t) {
	*t = *in;
    }
    *t = '\0';
    if (*in) ++in;
    p = getvpwnam(user);
    if (!p) {
	strcpy(out, in);
	return;
    }
    sprintf(out, "%s/%s", p->pw_dir, in);
}

int ReadDatesFromFile(fp)
FILE *fp;
{
    char Buf[1000];
    struct eventnode *allocevent;

    while (1) {
	if (!fgets(Buf, sizeof(Buf)-1, fp)) {
	    return(0);
	}
	allocevent = readdateintoeventnode(Buf);
	if (allocevent) {
	    allocevent->next = RootEventNode;
	    RootEventNode = allocevent;
	}
    }
}

struct eventnode *readdateintoeventnode(Buf)
char *Buf;
{
    char *event, *s;
    int rval;
    struct datespec ds;
    struct eventnode *newevent;

    if (Buf[0] == '#') return(NULL); /* a comment */
    event = index(Buf, '#');
    if (event) {
	*event++ = NULL;
	s = index(event, '\n');
	if (s) *s = NULL;
	while (*event && isspace(*event)) ++event;
    } else {
	event = "null event";
    }
    for (s=Buf; *s && isspace(*s); ++s) {
	;
    }
    if (*s == '\0') return(NULL); /* empty line */
    rval = readdate(s, &ds);
    if (rval != CHAMPERR_NOERR) {
	printf("readdate error %d on %s\n", rval, event);
	return(NULL);
    }
    newevent = (struct eventnode *) malloc(sizeof(struct eventnode));
    if (!newevent) {
	printf("Out of Memory\n");
	return(NULL);
    }
    bcopy(&ds, &newevent->ds, sizeof(struct datespec));
    newevent->event = (char *) malloc(1+strlen(event));
    if (!newevent->event) {
	free(newevent);
	printf("Out of Memory\n");
	return(NULL);
    }
    strcpy(newevent->event, event);
    newevent->flagged = 0;
    return(newevent);
}

int readdate(datebuf, datespec)
char *datebuf;
struct datespec *datespec;
{
    char *s;
    int calsys;

    for (s=datebuf; *s && !isspace(*s); ++s) {
	;
    }
    if (!*s) return(CHAMPERR_BADFORMAT);
    *s++ = NULL;
    calsys = atoi(datebuf);
    switch(calsys) {
	case CALSYS_GREGORIAN:
	    datespec->calsys = calsys;
	    return(readgregoriandate(s, &datespec->sys.gd));
	case CALSYS_HEBREW:
	    datespec->calsys = calsys;
	    return(readhebrewdate(s, &datespec->sys.hd));
	case CALSYS_ECCLESIASTICAL:
	    datespec->calsys = calsys;
	    return(readecclesiasticaldate(s, &datespec->sys.ed));
	default:
	    return(CHAMPERR_BADCALSYS);
    }
}

int readgregoriandate(db, ds)
char *db;
struct gregoriandatespec *ds;
{
    ds->year = -1;
    ds->month = -1;
    ds->day = -1;
    ds->hour = -1;
    ds->min = -1;
    ds->wkday = -1;
    ds->wkdayselector = -1,
    (void) sscanf(db, "%d %d %d %d %d %d %d", &ds->year, &ds->month, &ds->day, &ds->wkday, &ds->wkdayselector, &ds->hour, &ds->min);
    return(CHAMPERR_NOERR);
}

int readhebrewdate(db, ds)
char *db;
struct hebrewdatespec *ds;
{
    ds->year = -1;
    ds->month = -1;
    ds->day = -1;
    (void) sscanf(db, "%d %d %d", &ds->year, &ds->month, &ds->day);
    return(CHAMPERR_NOERR);
}

int readecclesiasticaldate(db, ds)
char *db;
struct ecclesiasticaldatespec *ds;
{
    ds->year = -1;
    ds->landmark = -1;
    ds->offset = 0;
    ds->hour = -1;
    ds->min = -1;
    (void) sscanf(db, "%d %d %d %d %d", &ds->year, &ds->landmark, &ds->offset, &ds->hour, &ds->min);
    return(CHAMPERR_NOERR);
}
