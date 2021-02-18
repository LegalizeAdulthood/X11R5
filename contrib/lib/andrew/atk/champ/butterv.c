/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *butterv_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/champ/RCS/butterv.c,v 2.7 1991/10/03 15:56:02 bobg Exp $";

#include "class.h"
#include "butterv.eh"
#include "butter.ih"
#include "fontdesc.ih"
#include "graphic.ih"
#include "cursor.ih"

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

void
butterview__FullUpdate(self, type, left, top, width, height)
struct butterview *self;
enum view_UpdateType type;
long left, top, width, height;
{
    struct rectangle Rect;
    struct butter *b = (struct butter *) butterview_GetDataObject(self);

    if((type == view_LastPartialRedraw) || (type == view_FullRedraw))
	if (b) {
	    if (!butter_GetButtonFont(b)) {
		butter_SetButtonFont(b, fontdesc_Create(b->myfontname ? b->myfontname : "andy", b->myfonttype, b->myfontsize));
	    }
	    if (!b->mycursor) {
		b->mycursor = cursor_Create(self);
		cursor_SetStandard(b->mycursor, Cursor_Octagon);
	    }
	    butterview_GetLogicalBounds(self, &Rect);
	    butterview_SetTransferMode(self, graphic_WHITE);
	    butterview_FillRect(self, &Rect, butterview_GetDrawable(self));
	    butterview_PostCursor(self, &Rect, b->mycursor);
	    butterview_SetFont(self, butter_GetButtonFont(b));
	    butterview_SetTransferMode(self, graphic_BLACK);
	    butterview_MoveTo(self, (Rect.left + Rect.width) / 2, (Rect.top + Rect.height) / 2);
	    butterview_DrawString(self, butter_GetText(b) ? butter_GetText(b) : "<BEEP>", graphic_BETWEENLEFTANDRIGHT | graphic_BETWEENTOPANDBASELINE);
	}
}

void
butterview__Update(self)
struct butterview *self;  
{
    struct rectangle r;

    butterview_GetLogicalBounds(self, &r);
    butterview_FullUpdate(self, 0, r.left, r.top, r.width, r.height);
}

struct view *
butterview__Hit(self, action, x, y, numclicks)
struct butterview *self;
long x, y, action, numclicks;  
{
    struct butter *b = (struct butter *) butterview_GetDataObject(self);

    if (b && butter_GetHitFunction(b)) {
	butter_GetHitFunction(b)
	  (butter_GetRockPtr(b), butter_GetRockInt(b), b, action);
    }
    return((struct view *)self);
}
