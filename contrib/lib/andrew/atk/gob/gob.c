/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gob.c,v 1.7 1991/09/12 16:21:56 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gob.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *gob_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gob.c,v 1.7 1991/09/12 16:21:56 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* gob.c		

	Code for the gob data object
*/

#include "class.h"
#include "dataobj.ih"
#include <graphic.ih>
#include <fontdesc.ih>
#include "gob.eh"
/* $$$ include  xxx.ih  for any routine called below as xxx_routine() */


#define MAXFILELINE 255

/* $$$ */
gob_FreeOb(ob)
struct oblist *ob;
{

    if(ob->ptlist) free(ob->ptlist);
    if(ob->label) free(ob->label);
    free(ob); 
}
struct oblist *gob_NewOblist(self)
struct gob *self;
{
    struct oblist *ob;
    ob =(struct oblist *) malloc(sizeof(struct oblist));
    ob->type = 0;   
    ob->ptlist = NULL;
    ob->label = NULL;
    gettime(&(ob->modtime));
    ob->fd = NULL;
    ob->next = NULL;
    ob->mode = graphic_COPY;
    return ob;
}
gob_WriteOb(self,f,ob)
struct gob *self;
FILE *f;
struct oblist *ob;
{
    int i;
    struct point *pt;
    fprintf(f,"$O %d,%d,%d,%d\n",ob->type,ob->size,ob->gray,ob->mode);
    for(pt = ob->ptlist,i = ob->size; i;pt++, i--){
	fprintf(f,"$P %d,%d\n",point_X(pt),point_Y(pt));
    }
    if(ob->label)
	fprintf(f,"$C %s\n",ob->label);
    if(ob->fd)
	fprintf(f,"$F %d,%d,%s\n",
		fontdesc_GetFontSize(ob->fd),
		fontdesc_GetFontStyle(ob->fd),
		fontdesc_GetFontFamily(ob->fd));
    fprintf(f,"$E\n");
}
void gob__SetLogicalRect(self,x,y,width,height)
struct gob *self;
long x,y,width,height;
{
    self->xmin = x;
    self->ymin = y;
    self->xmax = x + width;
    self->ymax = y + height;
    gettime(&(self->modtime)); 
    gob_NotifyObservers(self, gob_DATACHANGED);
    gob_SetModified(self);	/* tell the enclosing document I have changed */
}
	void
gob__ClearAllObs(self)
	register struct gob *self;
{
	register struct oblist *d;
	register struct oblist *t;
	for (d = gob_GetFirstOb(self);  d != NULL; )  {
		t = gob_GetNextOb(self, d);
		gob_FreeOb(d);
		d = t;
	}
	self->obs = NULL;
	gettime(&(self->modtime)); 
	gob_NotifyObservers(self, gob_DATACHANGED);
	gob_SetModified(self);	/* tell the enclosing document I have changed */
}
void gob__ClearOb(self,ob)
	register struct gob *self;
        register struct oblist *ob;
{
	register struct oblist *d,*last;
	register struct oblist *t;
	last = NULL;
	for (d = gob_GetFirstOb(self);  d != NULL; )  {
		t = gob_GetNextOb(self, d);
		if(ob == d){
		    if(self->obs == ob){
			self->obs = t;
		    }
		    else last->next = t;
		    gob_FreeOb(d);
		    break;
		}
		last = d;
		d = t;
	}
	gettime(&(self->modtime)); 
	gob_NotifyObservers(self, gob_DATACHANGED);
	gob_SetModified(self);	/* tell the enclosing document I have changed */
}

/* $$$ */
void
gob_AddOb(self, d)
register struct gob *self;
struct oblist *d;
{
    register struct oblist *dp;
    d->next = NULL;
    for(dp = gob_GetFirstOb(self); dp != NULL && dp->next != NULL; dp = dp->next);
    if(dp == NULL){
	self->obs = d;
    }
    else {
	dp->next = d;
    }
    gob_NotifyObservers(self, gob_DATACHANGED);
    gob_SetModified(self);	/* tell the enclosing document I have changed */
}
struct oblist *gob__AddPathOb(self,pointlist,size,type,color,mode)
struct gob *self;
struct point *pointlist;
short size;
short type;
short color;
short mode;
{
    struct oblist *ob;
    struct point *pt;
    ob = gob_NewOblist(self);
    ob->type = type | gob_PATH;
    ob->ptlist = (struct point *)malloc(sizeof(struct point) * size);
    ob->size = size;
    for(pt = ob->ptlist;size--; pt++ , pointlist++){
	*pt = *pointlist;
    }
    ob->gray = color;
    ob->mode = mode;
    gob_AddOb(self,ob);
    return ob; 
}
struct oblist *gob__AddOvalOb(self,x,y,width,height,type,color,mode)
struct gob *self;
long x, y, width, height;
short type, color, mode;
{
    struct oblist *ob;
    struct point *pt;
    ob = gob_NewOblist(self);
    ob->type = type | gob_OVAL;
    ob->ptlist = (struct point *)malloc(sizeof(struct point) * 2);
    pt = ob->ptlist;
    point_SetPt(pt,x,y + height);
    pt++;
    point_SetPt(pt,x + width,y);
    ob->size = 2;
    ob->gray = color;
    ob->mode = mode;
    gob_AddOb(self,ob);
    return ob; 
}
struct oblist *gob__AddArcOb(self,x,y,width,height,StartAngle,OffsetAngle,type,color,mode)
struct gob *self;
long x; long y; long width;long height;
short StartAngle; short OffsetAngle; short type;short color;short mode;
{
    struct oblist *ob;
    struct point *pt;
    ob = gob_NewOblist(self);
    ob->type = type | gob_ARC;
    ob->ptlist = (struct point *)malloc(sizeof(struct point) * 3);
    pt = ob->ptlist;
    point_SetPt(pt,x,y + height);
    pt++;
    point_SetPt(pt,x + width,y);
    pt++;
    StartAngle += (StartAngle > 180) ? -180: 180;
    point_SetPt(pt,StartAngle,OffsetAngle);
    ob->size = 3;
    ob->gray = color;
    ob->mode = mode;
    gob_AddOb(self,ob);
    return ob; 
}


struct oblist *gob__AddLabelOb(self,x,y,label,fd,op,mode)
struct gob *self;
long x,y;
char *label;
struct fontdesc *fd;
short op,mode;
{
    struct oblist *ob;
    struct point *pt;
    ob = gob_NewOblist(self);
    ob->type = gob_LABEL;
    ob->ptlist = (struct point *)malloc(sizeof(struct point));
    pt = ob->ptlist;
    point_SetPt(pt,x,y);
    ob->size = 1;
    ob->fd = fd;
    if(label){
	ob->label = malloc(strlen(label) + 1);
	strcpy(ob->label,label);
    }
    gob_AddOb(self,ob);
    ob->mode = mode;
    ob->gray = op;
    return ob; 
}

	boolean
gob__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	/* initialize any global data used in this module */
	return TRUE;
}

	boolean
gob__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct gob  *self;
{
	/* here we give initial values to any fields that need them */
	
	self->obs = NULL;	/* $$$ */
	self->xmin = self->ymin = -100;
	self->xmax = self->ymax = 100;
	gettime(&(self->modtime)); 

	return TRUE;
}

	void 
gob__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct gob  *self;
{
	/* free any storage allocated to self */
	gob_ClearAllObs(self);	/* $$$ */
}

/* gob__Read(self, file, id)
	Reads the data for 'self' from 'file',  see format in gob.H
	If 'id' is 0, the data stream is at the outermost level and thus might
	be in an alternate format (see gob__Write).  In this data object,
	the format is always the same.

	If 'id' is non-zero, the first line of the data stream will have been read
	by the caller and the value found in the line is passed as 'id'.  This must
	match the id value found in the enddata line.

	Returns a boolean indication of success.  Fails if syntax of headers
	is incorrect.
*/
	long
gob__Read(self, file, id)
	register struct gob  *self;
	register FILE  *file;
	register long  id;
{
	long result = dataobject_BADFORMAT;
	char s[MAXFILELINE + 2];
	long len;
	long sid;
	char buf[1024],*bp,*dp;
	struct oblist *ob = NULL;
	struct point *pt = NULL;
	long px,py;
	long p1,p2,p3,p4;
	if (id == 0) {
		if (fscanf(file, "\\begindata{gob,%ld", &sid) != 1
				|| getc(file) != '}' || getc(file) != '\n') 
			return dataobject_NOTBE2DATASTREAM;
	}
	else {
		long c;
		sid = id;

		/*  The parent is supposed to have read the entire 
			header including the final \n.
		    Some ancient insets did not read the final \n,
			so we check and discard it here.
		    This must be removed if the internal data stream,
			might begin with \n.
		*/
		if ((c=getc(file)) != '\n')
			ungetc(c, file);
	}

	gob_ClearAllObs(self);	/* $$$  initialize data structure */
	bp = buf + 1;
	dp = buf + 3;
	result = dataobject_NOREADERROR;
	while (TRUE) {
	    /* read the lines of the data stream */
	    if(fgets(buf,1024,file)== NULL)
		return dataobject_MISSINGENDDATAMARKER;
	    if(*buf == '\\') break;
	    if(*buf != '$') continue;
	    switch(*bp){
		case 'G':
		    sscanf(dp,"%d,%d,%d,%d\n",&(self ->xmin), &(self->xmax), &(self->ymin),&(self->ymax));
#ifdef DEBUG
printf("G> %d,%d,%d,%d\n",(self ->xmin), (self->xmax),
 (self->ymin),(self->ymax));
#endif /* DEBUG */
		    break;
		case 'O':
		    ob = gob_NewOblist(self);
		    sscanf(dp,"%d,%d,%d,%d\n",&p1,&p2,&p3,&p4);
		    ob->type = p1;
		    ob->size = p2;
		    ob->gray = p3;
		    ob->mode = p4;
#ifdef DEBUG
printf("O> %d,%d,%d,%d\n",(ob->type),(ob->size), (ob->gray),(ob->mode));
#endif /* DEBUG */
		    if(ob->size){
			ob->ptlist = (struct point *)malloc(sizeof(struct point ) * ob->size);
			pt = ob->ptlist;
		    }
		    else pt = NULL;
		    break;
		case 'P':
		    if(pt == NULL){
			fprintf(stderr,"Bad Read Of gob\n");
			return dataobject_BADFORMAT;
		    }
		    sscanf(dp,"%d,%d\n",&px,&py);
		    point_SetPt(pt,px,py);
		    pt++;
		    break;
		case 'C':
		    len = strlen(dp);
		    ob->label = malloc(len);
		    dp[len - 1] = '\0'; /* get rid of newline */
		    strcpy(ob->label,dp);
		    break;
		case 'F':
		    sscanf(dp,"%d,%d,%s\n",&px,&py,s);
		    ob->fd = fontdesc_Create(s,py,px);
		    break;
		case 'E': 
		    gob_AddOb(self,ob);
	    }
	}
	gob_SetModified(self);	/* tell the enclosing document I have changed */
	return result;
	}


/* gob__Write(self, file, writeID, level)
	Writes a data stream representation of 'self' to 'file'
	The same 'writeID' value is passed in for all writes during a given
	datastream construction.  Once this object has been written, it need 
	not be written again.

	The 'level' can be ignored.  (A level of 0 indicates that the
	output is to be the sole contents of a file, so the format could be 
	non-datastream.  This is only used to support data streams 
	defined prior to the base editor.)
	
	Returns the identifying number written into the header.  The caller will 
	use this number in referring to the object from the dict.
*/
	long
gob__Write(self, file, writeID, level)
	register struct gob  *self;
	FILE  *file;
 	long  writeID;
	int  level;
{
	char head[50];
	long id = gob_UniqueID(self);
	if (self->header.dataobject.writeID != writeID) {
		/* new instance of write, do it */
		register struct oblist *d;
		self->header.dataobject.writeID = writeID;
		sprintf(head, "data{%s, %d}\n", class_GetTypeName(self), id);
		fprintf(file, "\\begin%s", head);
		fprintf(file,"$G %d,%d,%d,%d\n",self ->xmin, self->xmax, self->ymin,self->ymax);
		/* $$$ output lines of data stream */
		for (d = gob_GetFirstOb(self);  d != NULL; 
				d = gob_GetNextOb(self, d))  
		    gob_WriteOb(self,file,d);
		fprintf(file, "\\end%s", head);
	}
	return id;
}
