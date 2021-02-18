/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *helloa_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex18/RCS/helloa.c,v 2.6 1991/09/12 16:18:07 bobg Exp $";

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex18/RCS/helloa.c,v 2.6 1991/09/12 16:18:07 bobg Exp $ */

#include <class.h>

#include "helloa.eh"

#include "dataobj.ih"
#include "view.ih"
#include "im.ih"
#include "frame.ih"
#include "lpair.ih"
#include "text.ih"
#include "textv.ih"
#include "style.ih"
#include "fontdesc.ih"
#include "hello.ih"

static struct view *appLayerOrDestroy(v)
struct view *v;
{
    if(v==NULL)
	return NULL;
    else{
	struct view *al=view_GetApplicationLayer(v);

	if(al==NULL){
	    view_Destroy(v);
	    return NULL;
	}

	return al;
    }
}

static boolean makeSplitWindow(dobj1,dobj2)
struct dataobject *dobj1,*dobj2;
{
    struct view *v1,*v2;
    struct view *al1,*al2,*lpAl;
    struct frame *frame;
    struct im *im;
    struct lpair *lp;

    al1=appLayerOrDestroy(v1=(struct view *)class_NewObject(dataobject_ViewName(dobj1)));
    if(al1==NULL)
	return FALSE;

    al2=appLayerOrDestroy(v2=(struct view *)class_NewObject(dataobject_ViewName(dobj2)));
    if(al2==NULL) {
	view_DeleteApplicationLayer(v1,al1);
    	view_Destroy(v1);
	return FALSE;
    }

    lpAl=appLayerOrDestroy((struct view *)(lp=lpair_New()));
    if(lpAl==NULL) {
	view_DeleteApplicationLayer(v2,al2);
    	view_Destroy(v2);
	return FALSE;
    }

    /* this call makes a left/right split, with the given
     * percentage allocated to the left view
     */
    lpair_HSplit(lp,al1,al2,40 /* percent */,TRUE /* moveable boundary */);

    frame=frame_New();
    if(frame==NULL)  {
	lpair_DeleteApplicationLayer(lp,lpAl);
    	lpair_Destroy(lp);
	return FALSE;
    }

    im=im_Create(NULL);
    if(im==NULL) {
	frame_Destroy(frame);
	return FALSE;
    }

    view_SetDataObject(v1,dobj1);
    view_SetDataObject(v2,dobj2);
    frame_SetView(frame,lpAl);
    im_SetView(im,frame);

    view_WantInputFocus(v1,v1);

    return TRUE;

}

boolean helloworldapp__Start(hwapp)
struct helloworldapp *hwapp;
{
    struct helloworld *hw;
    struct text *t;
    struct style *bold,*italic;

    if(!super_Start(hwapp))
	return FALSE;

    hw=helloworld_New();
    if(hw==NULL)
	return FALSE;

    t=text_New();
    if(t==NULL) {
	helloworld_Destroy(hw);
	return FALSE;
    }

    bold=style_New();
    if(bold==NULL) {
	text_Destroy(t);
	return FALSE;
    }
    style_AddNewFontFace(bold,fontdesc_Bold);

    italic=style_New();
    if(italic==NULL) {
	style_Destroy(bold);
	return FALSE;
   }
    style_AddNewFontFace(italic,fontdesc_Italic);

    text_InsertCharacters(t,0,"Hello world!",sizeof("Hello world!")-1);
    text_AddStyle(t,0,5,bold);
    text_AddStyle(t,6,5,italic);

    if(!makeSplitWindow((struct dataobject *)hw,(struct dataobject *)t) ||
       !makeSplitWindow((struct dataobject *)hw,(struct dataobject *)t)) {
	style_Destroy(italic);
	return FALSE;
    }

    return TRUE;

}
