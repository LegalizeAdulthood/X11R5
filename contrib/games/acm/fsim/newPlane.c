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

#include <math.h>
#include "pm.h"
#include <stdio.h>
#include <string.h>

static craftType ctype[MAXCRAFTTYPES];

extern FILE * acm_fopen ();

craftType *lookupCraft (name)
char * name; 
{
	int	i;

	for (i=0; i<MAXCRAFTTYPES; ++i)
	    if (ctype[i].name != (char *) NULL)
		if (strcmp(name, ctype[i].name) == 0)
			return &ctype[i];

	return (craftType *) NULL;
}

craftType *newCraft () {

	int	i;
	
	for (i=0; i<MAXCRAFTTYPES; ++i)
		if (ctype[i].name == (char *) NULL) {
			ctype[i].name = "*allocated*";
			return &ctype[i];
		}

	return (craftType *) NULL;
}

#include "f16.tables"

/*
 *  Body and wave drag coefficient (a function of the mach number).
 */

static double CDb (c)
craft *c; {

	return interpolate (&TF16_CDb, c->mach);

}


/*
 *  Calculate the slope of yaw due to sideslip.
 */

static double F16_CnBeta(c)
craft	*c; {

	return interpolate (&TF16_CnBeta, fabs(c->alpha));
}

/*
 *  Calculate the slope of roll due to sideslip.
 */

static double F16_ClBeta(c)
craft	*c; {

	return interpolate (&TF16_ClBeta, fabs(c->alpha));
}

static double genericThrust (c)
craft *c; {

	register double t, ts;

	if (c->flags & FL_AFTERBURNER) { 
		t = interpolate (&TGeneric_ABThrust, c->mach) *
			c->cinfo->maxABThrust;
	}
	else {
		t = interpolate (&TGeneric_Thrust, c->mach) *
			c->cinfo->maxThrust;
	}

	ts = c->throttle / 32768.0;

	return t * ts * ts * c->rho / 0.002377;
}

void genericResupply (c)
craft	*c; {

	c->fuel = c->cinfo->maxFuel;

	c->station[0].type = WK_M61A1;
	c->station[0].info = 500;
	c->station[0].info2 = 0;
	c->station[0].info3 = 0;
	c->station[1].type = WK_AIM9M;
	c->station[2].type = WK_AIM9M;
	c->station[3].type = WK_AIM9M;
	c->station[4].type = WK_AIM9M;
	c->station[5].type = WK_AIM9M;
	c->station[6].type = WK_AIM9M;
	c->station[7].type = WK_AIM9M;
	c->station[8].type = WK_AIM9M;
	initDamage (c);
}

void initf16 () {

	FILE    	*f;
	craftType	*c;

	c = newCraft();
	c->name = strdup("f-16c");
	c->CLOrigin = 0.129871;

	c->CLSlope = 4.0;

	c->CLNegStall = DEGtoRAD (-17.0);
	c->CLPosStall = DEGtoRAD (30.0);

	c->aspectRatio = 3.0;

	c->CDOrigin = 0.03642;
	c->CDb = CDb;
	c->CnBeta = F16_CnBeta;
	c->ClBeta = F16_ClBeta;
	c->CDFactor = -2.54200;
	c->CDPhase = 0.0349066;

	c->CDBOrigin = 1.0;
	c->CDBFactor = -1.0;
	c->CDBPhase = 0.0;

	c->CYbeta = -1.5;

	c->effElevator = 0.50;
	c->effAileron = 0.20;
	c->effRudder = 0.35;
	c->SeTrimTakeoff = 0.0;

	c->maxFlap = DEGtoRAD (40.0);	/* 40 degree maximum flap setting */
	c->cFlap = 0.64279;		/* yields Clift of 1.0 at max extension */
	c->cFlapDrag = 0.0467;		/* yields 0.03 of extra drag at max ext */
	c->flapRate = 0.1745;		/* about 4 seconds to fully extend flaps */

	c->cGearDrag = 0.03;
	c->gearRate = DEGtoRAD (30.0);	/* 3 seconds to extend ldg gear */

	c->maxSpeedBrake = DEGtoRAD (80.0);
	c->cSpeedBrake = 0.03;		/* drag coefficient added by speed brakes */
	c->speedBrakeRate = DEGtoRAD (40.0); /* about 2.0 seconds to fully extend */
	c->speedBrakeIncr = DEGtoRAD (80.0); /* one keystoke means full extension */

	VIdentMatrix(&(c->I));

	c->I.m[0][0] =  7240.7;
	c->I.m[1][1] = 49785.9;
	c->I.m[2][2] = 54389.5;

	c->LDamp = 40000.0;
	c->MDamp = -0.6;
	c->NDamp = -0.6;

	c->cmSlope = -2.00;
	c->cmFactor = -1.30;
	c->betaStall = DEGtoRAD (15.0);

	c->wingS = 300.0;
	c->wings = 16.0;
	c->emptyWeight = 14576.0;
	c->maxFuel = 6972.0;

/*
 * Engine data for F100-PW-100
 * source "Modern Combat Aircraft Design", Klaus Huenecke
 */

	c->maxThrust = 14080.0;
	c->maxABThrust = 23540.0;
	c->thrust = genericThrust;
	c->spFuelConsump = 0.68;
	c->spABFuelConsump = 2.55;

	c->groundingPoint.x = 0.0;
	c->groundingPoint.y = 0.0;
	c->groundingPoint.z = 6.63;
	c->viewPoint.x = 12.5;
	c->viewPoint.y = 0.0;
	c->viewPoint.z = -3.18;
	c->crashC = 90.0;

	c->sCount = 9;
	c->wStation[0].x = 7.0;
	c->wStation[0].y = -4.0;
	c->wStation[0].z = 0.0;
	c->wStation[1].x = 0.357;
	c->wStation[1].y = 15.6;
	c->wStation[1].z = 0.0;
	c->wStation[2].x = 0.357;
	c->wStation[2].y = -15.6;
	c->wStation[2].z = 0.0;
	c->wStation[3].x = 1.5;
	c->wStation[3].y = 9.0;
	c->wStation[3].z = 2.0;
	c->wStation[4].x = 1.5;
	c->wStation[4].y = -9.0;
	c->wStation[4].z = 2.0;
	c->wStation[5].x = 1.5;
	c->wStation[5].y = 8.0;
	c->wStation[5].z = 1.5;
	c->wStation[6].x = 1.5;
	c->wStation[6].y = -8.0;
	c->wStation[6].z = 1.5;
	c->wStation[7].x = 1.5;
	c->wStation[7].y = 10.0;
	c->wStation[7].z = 1.5;
	c->wStation[8].x = 1.5;
	c->wStation[8].y = -10.0;
	c->wStation[8].z = 1.5;


	c->muStatic = 0.08;
	c->muKinetic = 0.05;
	c->muBStatic = 0.7;
	c->muBKinetic = 0.6;

	c->maxNWDef = 1.27; /* 72.7 degrees */
	c->NWIncr = 1.27 / 32.0;
	c->maxNWS = 85.0 * 6076.0 / 3600.0;
	c->gearD1 = 13.5;
	c->gearD2 = -2.5;
	f = acm_fopen ("f16", "r");
	c->object = VReadObject(f);
	fclose (f);

	c->TEWSThreshold = 0.0;		/* zero, for now */
	c->radarOutput = 15000.0;	/* estimated */
	c->radarDRange = 80.0 * NM;
	c->radarTRange = 60.0 * NM;

	c->placeProc = NULL;

	c->damageBits = SYS_NODAMAGE;
	c->damageBits &= ~ SYS_ENGINE2;
	c->structurePts = 15;

	c->resupply = genericResupply;

}

void initmig23 () {

	FILE    	*f;
	craftType	*c;

	c = newCraft();
	c->name = strdup("mig-23");
	c->CLOrigin = 0.129871;
	c->CLSlope = 3.9;
	c->CLNegStall = -17.0 * pi / 180.0;
	c->CLPosStall =  26.0 * pi / 180.0;

	c->aspectRatio = 2.27;

	c->CDOrigin = 0.03642;
	c->CDb = CDb;
	c->CnBeta = F16_CnBeta;		/* for now, at least */
	c->ClBeta = F16_ClBeta;
	c->CDFactor = -2.54200;
	c->CDPhase = 0.0349066;

	c->CDBOrigin = 1.0;
	c->CDBFactor = -1.0;
	c->CDBPhase = 0.0;

	c->CYbeta = -1.3;

	c->effElevator = 0.50;
	c->effAileron = 0.20;
	c->effRudder = 0.35;
	c->SeTrimTakeoff = 0.0;

	c->maxFlap = 40.0 * pi / 180.0;	/* 40 degree maximum flap setting */
	c->cFlap = 0.64279;		/* yields Clift of 1.0 at max extension */
	c->flapRate = 0.1745;		/* about 4 seconds to fully extend flaps */

	c->cGearDrag = 0.03;
	c->gearRate = DEGtoRAD (30.0);	/* 3 seconds to extend ldg gear */

	c->maxSpeedBrake = DEGtoRAD (80.0);
	c->cSpeedBrake = 0.03;		/* drag coefficient added by speed brakes */
	c->speedBrakeRate = DEGtoRAD (40.0); /* about 0.25 seconds to fully extend */
	c->speedBrakeIncr = DEGtoRAD (80.0); /* one keystoke means full extension */

	VIdentMatrix(&(c->I));
	c->I.m[0][0] =  9000.0;
	c->I.m[1][1] = 75000.0;
	c->I.m[2][2] = 95000.0;

	c->LDamp =  50000.0;
	c->MDamp = -0.5;
	c->NDamp = -0.4;

	c->cmSlope = -1.80;
	c->cmFactor = -1.30;
	c->betaStall = DEGtoRAD (15.0);

	c->wingS = 325.0;
	c->wings = 13.5;
	c->emptyWeight = 25000.0;
	c->maxFuel = 9914.0;

/*
 * Engine data for Tumansky R-29B
 * source "Modern Fighter Combat"
 */

	c->maxThrust = 17635.0;
	c->maxABThrust = 25350.0;
	c->thrust = genericThrust;
	c->spFuelConsump = 0.68;
	c->spABFuelConsump = 2.55;

	c->groundingPoint.x = 0.0;
	c->groundingPoint.y = 0.0;
	c->groundingPoint.z = 6.0;
	c->viewPoint.x = 12.5;
	c->viewPoint.y = 0.0;
	c->viewPoint.z = -3.0;
	c->crashC = 90.0;

	c->sCount = 9;
	c->wStation[0].x = 7.0;
	c->wStation[0].y = -4.0;
	c->wStation[0].z = 0.0;
	c->wStation[1].x = 0.357;
	c->wStation[1].y = 15.6;
	c->wStation[1].z = 0.0;
	c->wStation[2].x = 0.357;
	c->wStation[2].y = -15.6;
	c->wStation[2].z = 0.0;
	c->wStation[3].x = 1.5;
	c->wStation[3].y = 9.0;
	c->wStation[3].z = 2.0;
	c->wStation[4].x = 1.5;
	c->wStation[4].y = -9.0;
	c->wStation[4].z = 2.0;
	c->wStation[5].x = 1.5;
	c->wStation[5].y = 8.0;
	c->wStation[5].z = 1.5;
	c->wStation[6].x = 1.5;
	c->wStation[6].y = -8.0;
	c->wStation[6].z = 1.5;
	c->wStation[7].x = 1.5;
	c->wStation[7].y = 10.0;
	c->wStation[7].z = 1.5;
	c->wStation[8].x = 1.5;
	c->wStation[8].y = -10.0;
	c->wStation[8].z = 1.5;

	c->muStatic = 0.08;
	c->muKinetic = 0.05;
	c->muBStatic = 0.7;
	c->muBKinetic = 0.6;

	c->maxNWDef = 1.27; /* 72.7 degrees */
	c->NWIncr = 1.27 / 32.0;
	c->maxNWS = 100.0 * 6076.0 / 3600.0;
	c->gearD1 = 13.5;
	c->gearD2 = -2.5;
	f = acm_fopen ("mig23", "r");
	c->object = VReadObject(f);
	fclose (f);
	
	c->TEWSThreshold = 0.0;		/* zero, for now */
	c->radarOutput = 15000.0;	/* estimated */
	c->radarDRange = 80.0 * NM;
	c->radarTRange = 30.0 * NM;

	c->placeProc = NULL;

	c->damageBits = SYS_NODAMAGE;
	c->damageBits &= ~ SYS_ENGINE2;
	c->structurePts = 19;

	c->resupply = genericResupply;

}

extern void initPanel();

int newPlane(planeType)
char *planeType; {

	int     i, j;
	craft	*c;

	for (i=0; i<MAXPLAYERS; ++i) {
		if (ptbl[i].type == CT_FREE) {

			c = &ptbl[i];
			c->type = CT_PLANE;
			c->cinfo = lookupCraft (planeType);

			c->Cg.x = 0.0;
			c->Cg.y = 0.0;
			c->Cg.z = 0.0;
			c->Sg.x = 90.0;
			c->Sg.y = 0.0;
			c->Sg.z = - c->cinfo->groundingPoint.z;
			c->prevSg = c->Sg;

			c->p = c->q = c->r = 0.0;
			c->Se = c->Sr = c->Sa = 0.0;
			c->SeTrim = 0.0;

			c->throttle = 8192;
			c->curThrust = calcThrust (c);
			c->curFlap = 0.0;
			c->flapSetting = 0.0;
			c->curGear[0] = c->curGear[1] =
				c->curGear[2] = pi / 2.0;
			c->curSpeedBrake = 0.0;
			c->speedBrakeSetting = 0.0;
			c->curHeading = c->curPitch = c->curRoll = 0.0;
			VIdentMatrix(&(c->trihedral));
			c->Itrihedral = c->trihedral;
			c->curNWDef = 0.0;
			c->flags = FL_NWS | FL_GHANDLE_DN | FL_GND_CONTACT;
			c->radarMode = RM_NORMAL;
			c->curRadarTarget = -1;
			c->viewDirection.x = 1.0;
			c->viewDirection.y = 0.0;
			c->viewDirection.z = 0.0;
			c->viewUp.x = 0.0;
			c->viewUp.y = 0.0;
			c->viewUp.z = -1.0;
			for (j=0; j<3; ++j) {
				c->leftHUD[j] = Vmalloc (32);
				c->rightHUD[j] = Vmalloc (32);
			}

/*
 *  rearm and fuel the aircraft.
 */

			(*c->cinfo->resupply) (c);

			initPanel (c);

/* a trick to select a valid weapon */

			c->curWeapon = WEAPONTYPES - 1;
			selectWeapon (c);

			break;
			
		}
	}

	if (i == MAXPLAYERS)
		return -1;

	return i;
}

int newDrone(p)
craft *p; {


	int     i, j, droneTeam;
	craft	*c;
	char	*type;
	VPoint	s;

	for (i=0; i<MAXPLAYERS; ++i) {
		if (ptbl[i].type == CT_FREE) {

			if (p->team == 1) {
				type = "mig-23";
				droneTeam = 2;
			}
			else {
				type = "f-16c";
				droneTeam = 1;
			}

			c = &ptbl[i];
			*c = *p;

			c->index = i;
			c->team = droneTeam;

			c->vl = NULL;
			c->type = CT_DRONE;
			c->cinfo = lookupCraft (type);

/*
 *  Position the drone 200 feet ahead of the player's craft.
 */

			s.x = 5400.0;
			s.y = 2030.0;
			s.z = 0.0;
			VTransform (&s, &(p->trihedral), &(c->Sg));
			c->Sg.x += p->Sg.x;
			c->Sg.y += p->Sg.y;
			c->Sg.z += p->Sg.z;
			c->prevSg = c->Sg;

/*			c->p = c->q = c->r = 0.0;
			c->Se = c->Sr = c->Sa = 0.0;

			c->throttle = 8192;
			c->curThrust = calcThrust (c);
			c->curHeading = c->curPitch = c->curRoll = 0.0;
			VIdentMatrix(&(c->trihedral));
			c->Itrihedral = c->trihedral;
*/
			c->curNWDef = 0.0;
			c->flags = 0;
			c->radarMode = RM_NORMAL;

			for (j=0; j<3; ++j) 
				c->leftHUD[j] = c->rightHUD[j] = NULL;

			break;			
		}
	}

	if (i == MAXPLAYERS)
		return -1;

	return i;
}
