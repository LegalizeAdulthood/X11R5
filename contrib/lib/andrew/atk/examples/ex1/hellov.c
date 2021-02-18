/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *hellov_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex1/RCS/hellov.c,v 2.6 1991/09/12 16:16:29 bobg Exp $";

#include <class.h>

#include "graphic.ih"
#include "hellov.eh"

void helloworldview__FullUpdate(hwv, type, left, top, width, height)
    struct helloworldview *hwv;
    enum view_UpdateType type;
    long left;
    long top;
    long width;
    long height;
{
    int x,y;
    struct rectangle VisualRect;		

    helloworldview_GetVisualBounds(hwv,&VisualRect);
    x = rectangle_Left(&VisualRect) + rectangle_Width(&VisualRect)/2;
    y = rectangle_Top(&VisualRect) + rectangle_Height(&VisualRect)/2;

    helloworldview_MoveTo(hwv,x,y);
    helloworldview_DrawString(hwv,"hello world",
	graphic_BETWEENTOPANDBASELINE | graphic_BETWEENLEFTANDRIGHT);    

}

    
