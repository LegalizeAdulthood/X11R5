/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/bargrphv.c,v 2.10 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/bargrphv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/bargrphv.c,v 2.10 1991/09/30 18:26:45 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <bargrphv.eh>
#include <sbuttonv.ih>
#include <sliderv.ih>
#include <sbutton.ih>

#define FUDGE 2
#define FUDGE2 4
#define FUDGE4 8
#define MAXWID 36

#ifndef MIN
#define MIN(A,B) ((A > B)? B:A)
#endif
static DrawKnurl(self,fullupdate,rr)
struct bargraphV * self;
boolean fullupdate;
struct rectangle *rr;

{
    long start,height;
    struct rectangle r,clipper;
    struct sliderV *ss;
    int offset;
    ss = (struct sliderV *) self;
    offset = FUDGE2;
    bargraphV_SetTransferMode(self, graphic_COPY);
    height =  ((ss->tmpval - ss->minval)* (rectangle_Height(rr) - FUDGE2)) /(ss->maxval - ss->minval) ;
    start = rectangle_Height(rr) - height - FUDGE2; 
    rectangle_SetRectSize(&r,rectangle_Left(rr) + FUDGE,
			  rectangle_Top(rr) +  start, 
			  rectangle_Width(rr) - FUDGE2,height);
    if(!fullupdate){
	clipper.top = MIN(r.top,self->lasttop)- FUDGE2 ;
	clipper.left = rr->left;
	clipper.height = abs(r.top - self->lasttop)+ FUDGE4;
	clipper.width = rr->width + FUDGE4;
	sbuttonv_SetClippingRect(self,&(clipper));
    }
    bargraphV_EraseRectSize(self,rr->left,rr->top,rr->width + FUDGE4,rr->height);
    bargraphV_FillTrapezoid(self,r.left,r.top + r.height, r.width,
			 r.left	+ offset, r.top	+ r.height + offset, r.width,
			 bargraphV_GrayPattern(self,8,16));
    bargraphV_FillTrapezoid(self,r.left + r.width ,r.top , 0,
			 r.left + r.width,  r.top + offset,offset,
			 bargraphV_GrayPattern(self,12,16));
    bargraphV_FillTrapezoid(self,r.left + r.width ,r.top + r.height , offset,
			 r.left + r.width +offset , r.top + r.height + offset,0,
			 bargraphV_GrayPattern(self,12,16));
    bargraphV_FillRectSize(self,r.left + r.width ,r.top + offset,
			offset,r.height - offset,bargraphV_GrayPattern(self,12,16));
    bargraphV_MoveTo(self,r.left + r.width,r.top );
    bargraphV_DrawLine(self,offset,offset);bargraphV_DrawLine(self,0,r.height);
    bargraphV_DrawLine(self,-r.width,0);bargraphV_DrawLine(self,-offset,-offset);
    bargraphV_MoveTo(self,r.left + r.width,r.top + r.height);
    bargraphV_DrawLine(self,offset,offset);
    bargraphV_DrawRect(self, &r);
    if(!fullupdate)
	sbuttonv_ClearClippingRect(self);
    self->lasttop = r.top;
}

void bargraphV__Drawslider(self,fullupdate,rr)
struct bargraphV * self;
boolean fullupdate;
struct rectangle *rr;
{
    int start,height;
    struct rectangle interior,exterior,cl,clside,clipper;
    struct sliderV *ss;
    struct sbuttonv_view_info vi;
    boolean pushed = TRUE; /* should be TRUE, change when sbutton is fixed */
    ss = (struct sliderV *) self;
    if( ss->prefs->style == 2) ss->prefs->style = 4;
    if( ss->prefs->style != 4){
	DrawKnurl(self,fullupdate,rr);
	return;
    }
    exterior = *rr;
    clside = *rr;
    cl = *rr;
    height =  ((ss->tmpval - ss->minval )* (rectangle_Height(&exterior))) / (ss->maxval - ss->minval);
    height += (4 *  ( rr->height - height)) / rr->height ;
    start = rectangle_Top(&exterior) + rectangle_Height(&exterior) - height;
    exterior.height = height;
    exterior.top = start;
    cl.height = cl.height + 1 -  exterior.height ;
    sbuttonv_DrawBorder(self, exterior.left,exterior.top,
			 exterior.width, exterior.height,
			 ss->prefs,pushed,FALSE,&interior);
    clside = exterior;
    clside.width = interior.left - exterior.left;
    if(!fullupdate){
	clipper.top = MIN(exterior.top,self->lasttop)- FUDGE2 ;
	clipper.left = exterior.left;
	clipper.height = abs(exterior.top - self->lasttop)+ FUDGE4;
	clipper.width = exterior.width;
	sbuttonv_SetClippingRect(self,&(clipper));
    }
    if(fullupdate || (self->lasttop > exterior.top)){
	sbuttonv_SaveViewState( self, &vi);
	sbuttonv_DrawBorder(self, exterior.left,exterior.top,
			    exterior.width, exterior.height,
			    ss->prefs,!pushed,TRUE,&interior);
	sbuttonv_RestoreViewState( self, &vi);
    }
    if(cl.height > 0) sbuttonv_EraseRect(self,&cl);
    if(clside.width > 0)  sbuttonv_EraseRect(self,&clside);
    bargraphV_FillTrapezoid(self,interior.left + interior.width,interior.top - 1,exterior.left + exterior.width - (interior.left + interior.width),exterior.left + exterior.width,interior.top + (interior.top  - exterior.top),0,bargraphV_WhitePattern(self));
    if(!fullupdate)
	sbuttonv_ClearClippingRect(self);

    self->lasttop = exterior.top;

}
boolean bargraphV__InitializeObject(classID, self )
struct classheader *classID;
struct bargraphV * self;
{
    struct sliderV *ss;
    ss = (struct sliderV *) self;
    ss->readonlydefault = TRUE;
    return TRUE;
}
void bargraphV__DrawFromScratch(self,x,y,width,height)
struct bargraphV * self;
long x,y,width,height;
{
    struct sliderV *ss;
    ss = (struct sliderV *) self;

    ss->sliderwidth = width - FUDGE4 - x - x;
    if( ss->sliderwidth > MAXWID){
	ss->sliderwidth = MAXWID;
    }
    super_DrawFromScratch(self,x,y,width,height);
}
