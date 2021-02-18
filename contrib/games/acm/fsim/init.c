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
 
#include <string.h>
#include "pm.h"

extern craftType *newCraft();
extern void initaim9(), initm61a1();
extern void initf16(), initmig23();
extern void buildExplosion();

FILE *acm_fopen (name, access)
char *name, *access; {

	FILE	*f;
	char	libname[256];

	if ((f = fopen (name, access)) == (FILE *) NULL) {
		strcpy (libname, ACM_LIBRARY);
		strcat (libname, name);
		if ((f = fopen (libname, access)) == (FILE *) NULL) {
			fprintf (stderr, "Sorry, I cannot find the graphics \
file %s in . or %s\n", name, ACM_LIBRARY);
			exit (1);
		}
	}

	return f;
}

int init () {

	FILE	*f;
	static	VObject	*o2;
	int	i;
	craft	*p;

	srandom (time(0));

/*
 *  Build an explosion object
 */

	buildExplosion ();

        for ((i=0, p=stbl); i<MAXSURFACE; (++i, ++p))
		p->type = CT_FREE;

        for ((i=0, p=ptbl); i<MAXPLAYERS; (++i, ++p)) {
		p->index = i;
		p->type = CT_FREE;
	}

        for ((i=0, p=mtbl); i<MAXPROJECTILES; (++i, ++p))
		p->type = CT_FREE;

/*	HUDColor = VAllocColor ("#ffc800"); */
	HUDColor = VAllocColor ("white");
/*	groundColor = VAllocColor ("#29350B"); */
/*	groundColor = VAllocColor ("#717844"); */
	groundColor = VAllocColor ("#879349");
	whiteColor = VAllocColor ("white");
	blackColor = VAllocColor ("black");

/*
 * Read in the runway and place it.
 */

	teamLoc[1].x = 1.0 * NM;
	teamLoc[1].y = 0.0 * NM;
	teamLoc[1].z = 0.0 * NM;

	f = acm_fopen ("rwy", "r");
	stbl[1].type = CT_SURFACE;
	stbl[1].cinfo = newCraft();
	stbl[1].cinfo->object = VReadObject(f);
	fclose (f);
	stbl[1].Sg.x = 0.0;
	stbl[1].Sg.y = 0.0;
	stbl[1].Sg.z = 0.0;
	stbl[1].curHeading = stbl[1].curPitch = stbl[1].curRoll = 0.0;

	f = acm_fopen ("rwy2", "r");
	stbl[0].type = CT_SURFACE;
	stbl[0].cinfo = newCraft();
	stbl[0].cinfo->object = VReadObject(f);
	fclose (f);
	stbl[0].Sg.x = 4000.0;
	stbl[0].Sg.y = 3000.0;
	stbl[0].Sg.z = 0.0;
	stbl[0].curHeading = 300.0 * 3.14159 / 180.0;
	stbl[0].curPitch = stbl[0].curRoll = 0.0;

	f = acm_fopen ("tower", "r");
	stbl[2].type = CT_SURFACE;
	stbl[2].cinfo = newCraft();
	stbl[2].cinfo->object = VReadObject(f);
	fclose (f);
	stbl[2].Sg.x = 4000.0;
	stbl[2].Sg.y = -700.0;
	stbl[2].Sg.z = 0.0;
	stbl[2].curHeading = stbl[2].curPitch = stbl[2].curRoll = 0.0;

	f = acm_fopen ("mtn", "r");
	stbl[3].type = CT_SURFACE;
	stbl[3].cinfo = newCraft();
	stbl[3].cinfo->object = VReadObject(f);
	fclose (f);
	stbl[3].Sg.x = 20.0 * NM;
	stbl[3].Sg.y = 6.0 * NM;
	stbl[3].Sg.z = 0.0;
	stbl[3].curHeading = DEGtoRAD (30.0);
	stbl[3].curPitch = stbl[3].curRoll = 0.0;

	stbl[4].type = CT_SURFACE;
	stbl[4].cinfo = stbl[3].cinfo;
	stbl[4].Sg.x = -2.0 * NM;
	stbl[4].Sg.y = 40.0 * NM;
	stbl[4].Sg.z = 0.0;
	stbl[4].curHeading = DEGtoRAD (160.0);
	stbl[4].curPitch = stbl[4].curRoll = 0.0;

	stbl[5].type = CT_SURFACE;
	stbl[5].cinfo = stbl[3].cinfo;
	stbl[5].Sg.x = -2.6 * NM;
	stbl[5].Sg.y = 43.0 * NM;
	stbl[5].Sg.z = 0.0;
	stbl[5].curHeading = DEGtoRAD (160.0);
	stbl[5].curPitch = stbl[4].curRoll = 0.0;

	teamLoc[2].x = 0.4 * NM;
	teamLoc[2].y = 49.0 * NM;
	teamLoc[2].z = 0.0 * NM;

	stbl[6].type = CT_SURFACE;
	stbl[6].cinfo = stbl[1].cinfo;
	stbl[6].Sg.x = -0.6 * NM;
	stbl[6].Sg.y = 49.0 * NM;
	stbl[6].Sg.z = 0.0;
	stbl[6].curHeading = DEGtoRAD (0.0);
	stbl[6].curPitch = stbl[6].curRoll = 0.0;

	initf16();
	initmig23();
	initf16();
	initmig23();
	initaim9();
	initm61a1();

/*
 *  Set time intervals.
 */

	deltaT = (double) UPDATE_INTERVAL / 1000000.0;

	halfDeltaTSquared = 0.5 * deltaT * deltaT;

	return 0;
}
