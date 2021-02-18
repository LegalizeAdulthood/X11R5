/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/fad/RCS/fad.ch,v 2.6 1991/09/12 19:39:32 bobg Exp $ */
/* $ACIS:fad.ch 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/fad/RCS/fad.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidfad_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/fad/RCS/fad.ch,v 2.6 1991/09/12 19:39:32 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#ifndef FAD_DEFS
#define FAD_DEFS
#define fad_NEWFAD 10000
#define OLD 1
#define NEW 0
#define PFUG 4
#define min(a,b) (a < b) ? a:b
#define ICONFLAG -10
#define LABELFLAG -10000
#define ANIMATEFLAG -9999
#define ISICONORLABEL(A) ((A) < 0)
#define ISICON(A) ((A) < 0 && (A) != LABELFLAG)
#define ISICONVEC(V) (ISICON(V->p2->x))
#define MAXWIDTH 20000
#define MAXHEIGHT 20000
#define LINEMODE 0
#define BOXMODE 1
#define ANIMATEMODE 2
#define FRTIME 30 	/* default minimum number of milliseconds between frames */
struct anivect {
	float x1,y1,y2,x2,dx1,dx2,dy1,dy2;
	char *label;
	char mode;
	};
struct fadpoint {
	long x,y;
	struct fadpoint *p;
	};
struct vector {
	struct fadpoint *p1,*p2;
	struct vector *v;
	char *label;
	char stat;
	char mode;
	};
struct fad_frame {
	struct vector *v;
	struct fadpoint *p;
	struct fad_frame *f;
	struct anivect *av;
	};
#endif /* FAD_DEFS */
class fad :dataobject[dataobj] {
overrides:
    Read(FILE *f, long id) returns long;
    Write(FILE *f, long writeid, int level) returns long;
    SetAttributes(struct attributes *attributes);
methods:
    newpoint(long x,long y) returns struct fadpoint *;
    newframe() returns struct fad_frame *;
    SetName(char *name);
    setpoint(long x,long y, int type,struct fad_frame *f) returns struct fadpoint *;
    setvector(struct fadpoint *pp1,struct fadpoint *pp2,struct fad_frame *f) returns struct vector *;
    delvector(struct fad_frame *f);
    iconnum(char *s) returns short;
    freeframe(struct fad_frame *ff);	
    flipicons()returns int;
    unflipicons()returns int;
    SetReadOnly(boolean readOnly);
classprocedures:
    InitializeObject(struct fad *self) returns boolean;
    FinalizeObject(struct fad *self);
data:
    short ox,oy,w,h,pltnum,desw,desh;
    struct fad_frame *f,*bf,*deleated;
    struct fadpoint *fp,*lp;
    struct vector *veclist[64];
    short  Frames,iconmode,currentfontindex,topinmp ,fad_square,frtime,readonly,initializedfonts ;
    char cfname[128],currentistr[128], currenticon,fadname[256], iconnamebuf[512],*inmp[15],labelfontname[256];
    struct fontdesc *fontpt[15], *currentfont,*labelfont;
    struct fadpoint *iconpoints[200],**iconpointend, **iconpointnow;
    struct cursor *cursor;
    char mode;
};
