/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *helloa_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex6/RCS/helloa.c,v 2.6 1991/09/12 16:19:05 bobg Exp $";

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex6/RCS/helloa.c,v 2.6 1991/09/12 16:19:05 bobg Exp $ */

#include <class.h>

#include "helloa.eh"

#include "im.ih"
#include "scroll.ih"
#include "hellov.ih"

boolean helloworldapp__Start(hwapp)
struct helloworldapp *hwapp;
{
    struct helloworldview *hwv;
    struct im *im;
    struct scroll *scroll;

    if(!super_Start(hwapp))
	return FALSE;

    hwv=helloworldview_New();
    if(hwv==NULL)
	return FALSE;

    scroll=scroll_Create(hwv,scroll_LEFT|scroll_BOTTOM);
    if(scroll==NULL){
	helloworldview_Destroy(hwv);
	return FALSE;
    }

    im=im_Create(NULL);
    if(im==NULL){
	helloworldview_Destroy(hwv);
	scroll_Destroy(scroll);
	return FALSE;
    }

    im_SetView(im,scroll);

    helloworldview_WantInputFocus(hwv,hwv);

    return TRUE;
}
