/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *hellov_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex3/RCS/hellov.c,v 2.6 1991/09/12 16:18:44 bobg Exp $";

#include <class.h>
#include "hellov.eh"
#include "graphic.ih"

#define POSUNDEF -1

boolean helloworldview__InitializeObject(classID, hwv)
struct classheader *classID;
struct helloworldview *hwv;
{
    hwv->x = POSUNDEF;
    hwv->y = POSUNDEF;
    hwv->HaveDownTransition = FALSE;
    return TRUE;
}


void helloworldview__FullUpdate(hwv, type, left, top, width, height)
    struct helloworldview *hwv;
    enum view_UpdateType type;
    long left;
    long top;
    long width;
    long height;
{
    struct rectangle myVisualRect; 

    if (hwv->x == POSUNDEF)  {
	helloworldview_GetVisualBounds(hwv,&myVisualRect);
	hwv->x = rectangle_Left(&myVisualRect) + rectangle_Width(&myVisualRect)/2;
	hwv->y = rectangle_Top(&myVisualRect) + rectangle_Height(&myVisualRect)/2;
    }

    helloworldview_MoveTo(hwv,hwv->x,hwv->y);
    helloworldview_DrawString(hwv,"hello world",
	graphic_BETWEENTOPANDBASELINE | graphic_BETWEENLEFTANDRIGHT);    
}

    
void helloworldview__Update(hwv)
struct helloworldview *hwv;
{
   
    if (hwv->newX != hwv->x ||  hwv->newY != hwv->y) {
        helloworldview_SetTransferMode(hwv,graphic_INVERT);

        helloworldview_MoveTo(hwv, hwv->x , hwv->y);
        helloworldview_DrawString(hwv, "hello world", graphic_BETWEENTOPANDBASELINE | graphic_BETWEENLEFTANDRIGHT);

        hwv->x = hwv->newX;
        hwv->y = hwv->newY;

        helloworldview_MoveTo(hwv, hwv->x , hwv->y);
        helloworldview_DrawString(hwv, "hello world", graphic_BETWEENTOPANDBASELINE | graphic_BETWEENLEFTANDRIGHT);
    
    }
}


struct view *helloworldview__Hit(hwv,action,x,y,numberOfClicks)
struct helloworldview *hwv;
enum view_MouseAction action;
long x;
long y;
long numberOfClicks;
{
    if(hwv->HaveDownTransition)
	switch(action){
	    case view_RightUp:
		hwv->HaveDownTransition=FALSE;
		/* fall through */
	    case view_RightMovement:
		hwv->newX=x-hwv->distX;
		hwv->newY=y-hwv->distY;
		break;
	    case view_LeftUp:
		hwv->HaveDownTransition=FALSE;
		hwv->newX=x;
		hwv->newY=y;
		break;
	    case view_LeftMovement:
		/* do nothing */
		break;
	    default:
		/* re-synchronize mouse */
		hwv->HaveDownTransition=FALSE;
	}

    if(!hwv->HaveDownTransition)
	switch(action){
	    case view_RightDown:
		hwv->distX=x-hwv->x;
		hwv->distY=y-hwv->y;
		/* fall through */
	    case view_LeftDown:
		hwv->HaveDownTransition=TRUE;
		helloworldview_WantInputFocus(hwv,hwv);
		break;
	}

    helloworldview_WantUpdate(hwv,hwv);

    return (struct view *)hwv;
}
