/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/entrstrv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/entrstrv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/entrstrv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */
#include <class.h>
#include <lpair.ih>
#include <entrtext.ih>
#include <etextv.ih>
#include <observe.ih>
#include <value.ih>
#include <valuev.ih>

#include <entrstrv.eh>



struct buttonV *enterstrV__DoHit( self,type,x,y,hits )
     struct enterstrV * self;
     enum view_MouseAction type;
     long x,y,hits;
{
  if (self->etext &&(type == view_RightUp || type == view_LeftUp))
      entertext_updatebuf(self->etext);
  return (struct buttonV *)self;
}
void enterstrV__ObservedChanged(self,changed,value)
struct enterstrV *self;
struct observable *changed;
long value;
{
    struct value *val ;
    char *str,*os;
    val = enterstrV_Value(self);
    if( changed == (struct observable *) self->etext){
	if(value == observable_OBJECTDESTROYED){
	    self->etext = NULL;
	    value_SetString(val,NULL);
	}
	else if(value == entertext_BUFCHANGEDFLAG && self->etext){
	    char *buf = entertext_GetString(self->etext);
	    struct value *val = enterstrV_Value(self);
	    value_SetNotify(val,FALSE);
	    value_SetValue(val,atoi(buf));
	    value_SetNotify(val,TRUE);
	    value_SetString(val,buf);
	}
    }
    else if(self->etext){
	if(val != (struct value *)self->header.view.dataobject){
	    /* ERROR */
	    fflush(stdout);
	    val = (struct value *)self->header.view.dataobject;
	}
	str = value_GetString(val);
	os = entertext_GetString(self->etext);
	if(str == NULL) str = "";
	if(os == NULL) os = "";
	if(str != os ){
	    if(strcmp(str,os) != 0){
		entertext_SetChars(self->etext,str,strlen(str));
		entertext_updatebuf(self->etext);
		return;
	    }
	}
	super_ObservedChanged(self,changed,value);
    }
}
struct view *enterstrV__GetApplicationLayer(self)
struct enterstrV *self;
{
    struct lpair *lp;
    struct etextview *ev;
    long w,h;
    if((self->etext = entertext_New()) == NULL) return (struct view *)self;
    if((ev = etextview_New()) == NULL) return (struct view *)self;
    etextview_SetDataObject(ev,self->etext);
    entertext_AddObserver(self->etext,self);
    h = 40;
    if(((struct view *)self)->parent != NULL){
	/* can't call desired size on unlinked text */
	etextview_LinkTree(ev,self);
	etextview_DesiredSize(ev,500,500,0,&w,&h);
	etextview_UnlinkTree(ev);
    }
    lp = lpair_Create(self,ev,h);
    self->etextview = ev;
    etextview_SetValueview(ev,(struct valueview *)self);
    return (struct view *)lp;
}
void enterstrV__RemoveApplicationLayer(self,vw)
struct enterstrV *self;
struct lpair *vw;
{
    struct value *val;
    struct etextview *ev;
    struct entertext *etext = self->etext;
    val = enterstrV_Value(self);
    ev = self->etextview ;
    if(etext && val && ev){
	entertext_RemoveObserver(self->etext,self);
	self->etext = NULL;
	self->etextview = NULL;
	value_SetString(val,NULL);
	lpair_Destroy(vw);
	entertext_Destroy(etext);
    }
}
boolean enterstrV__InitializeObject(classID,self)
struct classheader *classID;
struct enterstrV *self;
{
    self->etext = NULL;
    self->etextview = NULL;
    return TRUE;
}
boolean enterstrV__FinalizeObject(classID,self)
struct classheader *classID;
struct enterstrV *self;
{
    if(self->etext)
	entertext_RemoveObserver(self->etext,self);
    return TRUE;
}
void enterstrV__WantInputFocus(self)
struct enterstrV *self;
{
    if(self->etextview) 
	etextview_WantInputFocus(self->etextview,self->etextview);
}
