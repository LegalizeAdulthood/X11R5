/*
 *	acm : an aerial combat simulator for X
 *
 *	Written by Riley Rainey,  riley@mips.com
 *
 *	Permission to use, copy, modify and distribute (without charge) this
 *	software, documentation, images, etc. is granted, provided that this 
 *	comment and the author's name is retained.
 *
 */
 
#include "pm.h"
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define MAX_MAPPED_STRING_LEN	10
#define MAX_POPUP_STRING_LEN	40

extern int cur;

extern void resizePlayerWindow(), redrawPanel();
extern int debug;

void doEvents(c)
craft *c; 
{

	XEvent		ev;
	viewer		*u;
	int		player;

/*
 *  Process events for each player and any "back seaters"
 */

	player = 1;
	for (u=c->vl; u != (viewer *) NULL; u=u->next) {


#define mask 	(-1L)

		if (cur % REDRAW_EVERY == 0)
			getStick (c, u);

		while (XEventsQueued(u->dpy, QueuedAlready)) {

			XWindowEvent (u->dpy, u->win, mask, &ev);

			switch (ev.type) {

			case KeyPress:
				if (doKeyEvent (c, u, &ev, player) < 0)
					return;
				break;

			case ButtonPress:
				if (doButtonEvent (c, u, &ev, player) < 0)
					return;
				break;

			case ButtonRelease:
				if (doButtonReleaseEvent (c, u, &ev, player) < 0)
					return;
				break;

			case ConfigureNotify:
				resizePlayerWindow (c, u, ev.xconfigure.width,
					ev.xconfigure.height, 0);
				VResizeViewport (u->v, 12.0*25.4/1000.0,
					0.40, u->scaleFactor, u->width,
					u->height);
				break;

			case Expose:
				redrawPanel (c);
				break;

			default:
				break;
			}

		}

		player = 0;

	}
}

/*ARGSUSED*/
int doButtonEvent (c, u, ev, player)
craft	*c;
viewer	*u;
XEvent	*ev;
int	player; {

	if (ev->xbutton.button == Button2)
		fireWeapon (c);

	else if (ev->xbutton.button == Button3)
		selectWeapon (c);

	return 0;
}

/*ARGSUSED*/
int doButtonReleaseEvent (c, u, ev, player)
craft	*c;
viewer	*u;
XEvent	*ev;
int	player; {

	if (ev->xbutton.button == Button2)
		ceaseFireWeapon (c);

	return 0;
}

/*ARGSUSED*/
int doKeyEvent (c, u, ev, player)
craft	*c;
viewer	*u;
XEvent	*ev;
int	player; 
{

	KeySym		keysym;
	XComposeStatus	compose;
	char		buffer[MAX_MAPPED_STRING_LEN];
	int		buflen = MAX_MAPPED_STRING_LEN;

	(void) XLookupString (&ev->xkey, buffer, buflen, &keysym, &compose);

	if (player) {

		switch (keysym) {

		case XK_Left:
			if (c->flags & FL_NWS) {
				c->curNWDef = c->curNWDef - c->cinfo->NWIncr;
				if (c->curNWDef < - c->cinfo->maxNWDef)
					c->curNWDef = - c->cinfo->maxNWDef;
			}
			else {
				c->Sa =  c->Sa - 0.05;
			}
			break;

		case XK_Right:
			if (c->flags & FL_NWS) {
				c->curNWDef = c->curNWDef + c->cinfo->NWIncr;
				if (c->curNWDef > c->cinfo->maxNWDef)
					c->curNWDef = c->cinfo->maxNWDef;
			}
			else {
				c->Sa = c->Sa + 0.05;
			}
			break;

		case XK_Up:
			c->Se = c->Se - 0.01;
			break;

		case XK_Down:
			c->Se = c->Se + 0.01;
			break;

		case XK_z:
		case XK_Z:
			c->Sr = c->Sr + 0.01;
			break;

		case XK_c:
		case XK_C:
			c->Sr = c->Sr - 0.01;
			break;

		case XK_x:
		case XK_X:
			c->Sr = 0.0;
			break;

		case XK_Home:
			c->Se = 0.0;
			c->Sr = 0.0;
			c->Sa = 0.0;
			break;

/*
 *  J  :  Adjust Elevator Trim
 */
		case XK_J:
		case XK_j:
			c->SeTrim += c->Se;
			break;

/*
 *  U  :  Set Elevator trim to the Takeoff setting
 */

		case XK_U:
		case XK_u:
			c->SeTrim = c->cinfo->SeTrimTakeoff;
#ifdef FLIGHTDEBUG
			debug ^= 1;
#endif
			break;

		case XK_y:
		case XK_Y:
			flapsUp (c);
			break;

		case XK_h:
		case XK_H:
			flapsDown (c);
			break;

		case XK_w:
		case XK_W:
			speedBrakeRetract (c);
			break;

		case XK_s:
		case XK_S:
			speedBrakeExtend (c);
			break;

		case XK_2:
		case XK_KP_Subtract:
			c->throttle = (c->throttle <= 6553) ? 6553 :
				c->throttle - 512;
			break;

		case XK_3:
		case XK_KP_Add:
			c->throttle = (c->throttle >= 32768) ? 32768 :
				c->throttle + 512;
			break;

		case XK_4:
		case XK_Prior:
			c->throttle = 32768;
			break;

		case XK_1:
		case XK_Next:
			c->throttle = 8192;
			break;

		case XK_A:
		case XK_a:
			c->flags ^= FL_AFTERBURNER;
			break;

		case XK_B:
		case XK_b:
			c->flags ^= FL_BRAKES;
			break;

		case XK_G:
		case XK_g:
			c->flags ^= FL_GHANDLE_DN;
			break;

		case XK_L:
		case XK_l:
			newDrone (c);
			break;

		case XK_KP_8:
			c->viewDirection.x = 1.0;
			c->viewDirection.y = 0.0;
			c->viewDirection.z = 0.0;
			c->viewUp.x = 0.0;
			c->viewUp.y = 0.0;
			c->viewUp.z = -1.0;
			break;

/* look right */

		case XK_KP_6:
			c->viewDirection.x = 0.0;
			c->viewDirection.y = 1.0;
			c->viewDirection.z = 0.0;
			c->viewUp.x = 0.0;
			c->viewUp.y = 0.0;
			c->viewUp.z = -1.0;
			break;

/* look left */

		case XK_KP_4:
			c->viewDirection.x = 0.0;
			c->viewDirection.y = -1.0;
			c->viewDirection.z = 0.0;
			c->viewUp.x = 0.0;
			c->viewUp.y = 0.0;
			c->viewUp.z = -1.0;
			break;

/* look back */

		case XK_KP_2:
			c->viewDirection.x = -1.0;
			c->viewDirection.y = 0.0;
			c->viewDirection.z = 0.0;
			c->viewUp.x = 0.0;
			c->viewUp.y = 0.0;
			c->viewUp.z = -1.0;
			break;

/* look up */

		case XK_KP_5:
			c->viewDirection.x = 0.0;
			c->viewDirection.y = 0.0;
			c->viewDirection.z = -1.0;
			c->viewUp.x = -1.0;
			c->viewUp.y = 0.0;
			c->viewUp.z = 0.0;
			break;

		case XK_Q:
		case XK_q:
			c->curRadarTarget = getRadarTarget (c, 1);
			break;

		case XK_R:
		case XK_r:
			c->radarMode = (c->radarMode == RM_STANDBY) ?
				RM_NORMAL : RM_STANDBY;
			break;

#ifdef FLIGHTDEBUG
		case XK_o:
			if (absorbDamage(c, 3) == 0) {
				killPlayer (c);
				return -1;
			}
			break;
#endif

		case XK_P:
		case XK_p:
			killPlayer (c);
			return -1;
		}

	}

	return 0;
}
