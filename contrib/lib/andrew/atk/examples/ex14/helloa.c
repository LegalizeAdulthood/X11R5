/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *helloa_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex14/RCS/helloa.c,v 2.6 1991/09/12 16:17:27 bobg Exp $";

 /* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex14/RCS/helloa.c,v 2.6 1991/09/12 16:17:27 bobg Exp $ */

#include <class.h>

#include "helloa.eh"

#include "im.ih"
#include "frame.ih"
#include "text.ih"
#include "dataobj.ih"
#include "view.ih"

static boolean makeWindow(dobj)
struct dataobject *dobj;
{
    struct view *v;
    struct view *applayer;
    struct frame *frame;
    struct im *im;

    v=(struct view *)class_NewObject(dataobject_ViewName(dobj));
    if(v==NULL)
	return FALSE;

    applayer=view_GetApplicationLayer(v);
    if(applayer==NULL) {
	view_Destroy(v);
	return FALSE;
    }

    frame=frame_New();
    if(frame==NULL) {
	view_DeleteApplicationLayer(v,applayer);
	return FALSE;
    }

    im=im_Create(NULL);
    if(im==NULL) {
	frame_Destroy(frame);
	return FALSE;
    }

    view_SetDataObject(v,dobj);
    frame_SetView(frame,applayer);
    im_SetView(im,frame);

    view_WantInputFocus(v,v);

    return TRUE;

}

boolean helloworldapp__Start(hwapp)
struct helloworldapp *hwapp;
{
    struct text *t;

    if(!super_Start(hwapp))
	return FALSE;

    t=text_New();
    if(t==NULL)
	return FALSE;

    text_InsertCharacters(t,0,"Hello world!",sizeof("Hello world!")-1);

    if(!makeWindow((struct dataobject *)t) ||
       !makeWindow((struct dataobject *)t)){
	text_Destroy(t);
	return FALSE;
    }

    return TRUE;
}
