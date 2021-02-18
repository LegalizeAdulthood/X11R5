#include "Vlib.h"

void VDrawString(v, win, gc, p, str, len)
Viewport *v;
Window	 win;
GC	 gc;
VPoint	 *p;
char	 *str;
int	 len; {

	VPoint 	TmpPt;
	register int	x, y;

	if (v->flags & VPPerspective) {
		TmpPt.x = v->Middl.x + v->Scale.x * p->x / p->z;
		TmpPt.y = v->Middl.y - v->Scale.y * p->y / p->z;
	}
	else {
		TmpPt.x = v->Middl.x + v->Scale.x * p->x;
		TmpPt.y = v->Middl.y - v->Scale.y * p->y;
	}

	x = TmpPt.x + 0.5;
	y = TmpPt.y + 0.5;

	XDrawString (v->dpy, win, gc, x, y, str, len);

}
