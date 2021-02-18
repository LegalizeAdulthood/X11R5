/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *month_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/champ/RCS/month.c,v 2.4 1991/09/12 16:03:59 bobg Exp $";

#include <andrewos.h>	/* time.h */
#include <stdio.h>
#include <class.h>
#include <month.eh>

boolean month__InitializeObject(c, self)
struct classheader *c;
struct month *self;
{
    long clock = time(0);
    struct tm *thisdate = localtime(&clock);

    self->mon = thisdate->tm_mon;
    self->year = thisdate->tm_year;
    return(TRUE);
}

long month__Write(self, fp, id, level)
struct month *self;
FILE *fp;
long id;
int level;
{
    long clock;
    struct tm *thisdate;

    if (id != month_GetWriteID(self)) {
	/* New write operation */
	month_SetWriteID(self, id);
	clock = time(0);
	thisdate = localtime(&clock);
	fprintf(fp, "\\begindata{%s,%d}\n%d\n%d\n\\enddata{%s,%d}\n",
		class_GetTypeName(self), month_UniqueID(self),
		self->mon - thisdate->tm_mon, self->year - thisdate->tm_year,
		class_GetTypeName(self), month_UniqueID(self));
    }
    return(month_UniqueID(self));
}

long month__Read(self, fp, id)
struct month *self;
FILE *fp;
long id;
{
    char LineBuf[250];
    long clock = time(0);
    struct tm *thisdate = localtime(&clock);

    month_SetID(self, month_UniqueID(self));
    if (fgets(LineBuf,sizeof(LineBuf), fp) == NULL) {
	return(dataobject_PREMATUREEOF);
    }
    self->mon = thisdate->tm_mon + atoi(LineBuf);
    if (fgets(LineBuf,sizeof(LineBuf), fp) == NULL) {
	return(dataobject_PREMATUREEOF);
    }
    self->year = thisdate->tm_year + atoi(LineBuf);
    /* Now read in the \enddata line */
    if (fgets(LineBuf,sizeof(LineBuf), fp) == NULL) {
	return(dataobject_PREMATUREEOF);
    }
    if (strncmp(LineBuf, "\\enddata", 8)) {
	return(dataobject_MISSINGENDDATAMARKER);
    }
    return(dataobject_NOREADERROR);
}

void month__SetMonthAndYear(self, mon, year)
struct month *self;
int mon, year;
{
    self->mon = mon;
    self->year = year;
    month_NotifyObservers(self, 0);
}
