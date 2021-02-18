#include "manifest.h"
#include "eng.xbm"
#include "flaps0.xbm"
#include "flaps1.xbm"
#include "flaps2.xbm"
#include "flaps3.xbm"
#include "handleUp.xbm"
#include "handleDn.xbm"
#include "gearUp.xbm"
#include "gearTran.xbm"
#include "gearDown.xbm"
#include "scale.h"
#include "damage.h"
#include "interpolate.h"
#include <Vlib.h>

#define MAXSURFACE	8
#define MAXPLAYERS	16
#define MAXPROJECTILES	(MAXPLAYERS * 8)
#define MAXEXPLOSIONS	MAXPROJECTILES
#define MAXCRAFTTYPES	16
#define STATIONS	9
#define WEAPONTYPES	4

typedef struct _viewer {
	struct _viewer *next;
	Display	        *dpy;		/* display for this user */
	Window		win;		/* window for this user */
	Window		twin, rwin;	/* threat window and radar window */
	GC		gc;		/* GC for drawing */
	XFontStruct	*font;		/* HUD font */
	XFontStruct	*rfont;		/* radar font */
	Viewport	*v;		/* Viewport for out-of-cockpit views */
	Pixmap		eng;		/* Engine "RPM" gauge */
	Pixmap		flap[5];	/* flap setting indicators */
	Pixmap		handle[2];	/* ldg gear handle indicators */
	Pixmap		gearLight[3];	/* ldg gear status lights */
	int		cn;		/* X connection fd */
	int		width, height;	/* width & height of viewing window */
	int		xCenter, yCenter; /* center of viewing window */
	int		ftw, fth;	/* HUD font width & height (pixels) */
	int		rftw, rfth;	/* radar font width & height */
	int		radarWidth;	/* width of radar screen (pixels) */
	int		radarHeight;	/* height of radar screen (pixels) */
	scaleControl	altScale;	/* altitude scale control */
	scaleControl	velScale;	/* airspeed scale control */
	scaleControl	hdgScale;	/* heading scale control */
	int		TEWSx, TEWSy;	/* location of center of TEWS display */
	int		TEWSSize;	/* size of TEWS display (pixels) */
	int		panelx, panely;	/* location of top/right corner */
	double		scaleFactor;	/* overall scaling factor for display */
	}		viewer;

/*
 *  This structure describes a class of aircraft (e.g. an F-16C).
 */

typedef struct {
	char	*name;		/* name of aircraft class */

	double	aspectRatio;	/* wing aspect ratio */

	double	CLOrigin, CLSlope; /* Defines the CL characteristic eqn */
	double	CLNegStall, CLPosStall;
	double	betaStall;	/* Stall angle for rudder */
	double  CDOrigin, CDFactor; /* Defines the CD Characteristic eqn */
	double	(*CDb)();	/* compute body + wave drag coefficient */
	double	(*CnBeta)();	/* compute slope of yaw due to sideslip */
	double	(*ClBeta)();	/* compute slope of roll due to sideslip */
	double  CDPhase;
	double  CDBOrigin, CDBFactor; /* Defines the CD Characteristic eqn */
	double  CDBPhase;
	double	CYbeta;		/* Side-force from side-slip (dCY/dBeta) */

	double	effAileron;	/* Aileron effectiveness */
	double	effElevator;	/* Elevator effectiveness */
	double	effRudder;	/* Rudder effectiveness */
	double	SeTrimTakeoff;	/* Takeoff elevator trim setting */

	VMatrix I;		/* Moments of Inertia about CG in [xyz] */
	double	LDamp;		/* roll damping factor */
	double	MDamp;		/* pitch damping factor */
	double	NDamp;		/* yaw damping factor */
	double	cmSlope; 	/* CmAlpha curve slope */
	double  cmFactor;	/* CmAlpha factor when stalled */

	double	maxFlap;	/* maximum flap setting (radians) */
	double	cFlap;		/* lift coefficient of flaps */
	double	cFlapDrag;	/* drag coefficient of lowered flaps */
	double	flapRate;	/* flap movement rate (radians/sec) */

	double	cGearDrag;	/* drag coefficient of lowered gear */
	double  gearRate;	/* landging gear movement rate (rad/sec) */

	double	maxSpeedBrake;	/* maximum speed brake setting (radians) */
	double	cSpeedBrake;	/* drag coefficient of 90 degree speed brake */
	double	speedBrakeRate;	/* rate of speed brake movement (radians/sec) */
	double	speedBrakeIncr;	/* number of radians than one keystroke moves brake */

	double	wingS;		/* wing area (ft^2) */
	double	wings;		/* wing half-span (ft) */
	double	emptyWeight;	/* empty weight (lbs.) */
	double	maxFuel;	/* maximum internal fuel (lbs.) */

	double  maxThrust;	/* max static thrust, military power (lb) */
	double  maxABThrust;	/* max static thrust, afterburner on  (lb) */
	double  (*thrust)();	/* computes current thrust */
	double  spFuelConsump;	/* specific fuel consump(lb fuel/lb T x hr)*/
	double	spABFuelConsump;
	VPoint	groundingPoint;	/* hypothetical single pt of contact w/ground */
	VPoint  viewPoint;	/* pilot's viewing location wrt CG */
	double	crashC;		/* maximum tolerable touchdown z component */

	double	muStatic;	/* static coefficient of friction no-brakes */
	double	muKinetic;	/* moving coefficient of friction no-brakes */
	double	muBStatic;	/* static brakes-on */
	double  muBKinetic;	/* moving brakes-on */

	double  maxNWDef;	/* maximum nosewheel deflection (radians) */
	double  NWIncr;		/* deflection for each unit */
	double  maxNWS;		/* maximum NWS velocity */
	double	gearD1;		/* x station wrt nose gear of main gear */
	double  gearD2;		/* x station wrt CG of main gear */
	double	armDelay;	/* arming delay for missiles */

	long	damageBits;	/* initial bit mask of damaged systems */
	long	structurePts;	/* maximum structural damage */

	double	radarOutput;	/* radar output (watts) */
	double	radarTRange;	/* tracking radar range */
	double	radarDRange;	/* detection radar range */
	double	TEWSThreshold;

	int	sCount;		/* number of weapon stations */
	VPoint	wStation[STATIONS]; /* weapon's stations (launch points) */

	int	(*placeProc)();	/* object placement procedure (for special craft) */
	VObject *object;	/* what it looks like */
	void	(*resupply)();	/* the plane's rearm & refuel procedure */
	}	craftType;

typedef struct _wdsc {
	int		mask;		/* key mask */
	int		(*select)();	/* weapon select procedure */
	int		(*update)();	/* per tick update procedure */
	int		(*display)();	/* display update procedure */
	int		(*firePress)();	/* fire button pressed procedure */
	int		(*fireRelease)(); /* fire button released */
	craftType	*w;		/* description of this weapon */
	}		weaponDesc;

typedef struct _wctl {
	int		type;		/* type of weapon at this station */
	int		info;		/* extra info (weapon specific) */
	int		info2;		/* "      " */
	int		info3;		/* "      " */
	}		weaponStation;

#define WK_M61A1	0x01		/* M-61A1 Vulcan 20mm cannon */
#define WK_AIM9M	0x02		/* Sidewinder missile */

/*
 *  This structure describes a particular instance of aircraft.
 */

typedef struct _craft {
	int	index;		/* index number of this element in the vector */
	int	type;		/* craft type */
	int	team;		/* team number */
	double	createTime;	/* creation time of this craft */
	viewer	*vl;		/* list of viewers */
	VMatrix trihedral;  	/* transforms [x, y, z] to Geoparallel */
				/* x = forward, y=right wing, z=down */
				/* does not include origin translation */
	VMatrix	Itrihedral;	/* transforms geoparallel to [x, y, z] */
	VPoint	Cg;		/* Velocity vector in Geoparallel system */
	VPoint	Sg;		/* Position in Geoparallel system */
	double	rho;		/* current rho value */
	double  mach1;		/* current value of mach 1.0 (fps) */
	double	mach;		/* current mach number */
	VPoint	G;		/* g-force vector in acft system */
	VPoint	prevSg;		/* last interval's Sg value (ft) */
	double	p, q, r;	/* roll, pitch, and yaw rates (rad/sec) */
	double	Se, Sa, Sr;	/* control settings (-1 <= x <= 1) */
	double	SeTrim;		/* elevator trim setting */
	double	curHeading, curPitch, curRoll; /* Euler angles for acft */
	double	curThrust;	/* Current thrust value (lbs) */
	double	curFlap;	/* current flap setting (radians) */
	double	flapSetting;	/* current target flap setting (radians) */
	double	curSpeedBrake;	/* current speed brake position (radians) */
	double	speedBrakeSetting; /* current target speed brake setting(rad) */
	double	curGear[3];	/* current ldg gear location (0.0 = up) */
				/* [0] - nose, [1] - right, [2] - left */
	int	throttle;	/* thrust setting 0 - 32768 */
	double	alpha, beta;	/* angles of attack and sideslip (rad) */
	double	fuel;		/* current fuel on board (lbs) */
	int	flags;		/* flag word */

	long	damageBits;	/* bit flags of damaged subsystems */
	long	structurePts;	/* damage pts that can be absorbed */
	double	leakRate;	/* fuel leakage rate (lbs/second) */
	double	damageCL;	/* damage induced roll */
	double	damageCM;	/* damage induced pitch */

	int	radarMode;	/* radar mode */
	double  curNWDef;	/* Current nosewheel deflection (radians) */
	int	armFuse;	/* missile arming countdown */
	craftType *cinfo;	/* General craft information */
	double	order;		/* temporary value used to sort craft */
	struct  _craft *next;	/* next craft in sorted list */
	VPoint	viewDirection;	/* where the pilot is currently looking */
	VPoint	viewUp;		/* the "up" direction of the pilot's view */

	char	name[32];	/* logname of player */
	char	display[32];	/* display name of player */

	short	curRadarTarget;	/* our primary "threat" */
	double	targetDistance; /* distance to primary target */
	double	targetClosure;	/* closure rate on primary target */
	short	relValid[MAXPLAYERS];
	VPoint	relPos[MAXPLAYERS];
	double	rval[MAXPLAYERS];/* radar strength seen by us from other craft */
				/* relative positions of possible targets */
	char	*leftHUD[3];	/* strings in lower left corner of HUD */
				/* (reserved for weapons' status */
	char	*rightHUD[3];	/* strings in lower right corner of HUD */
				/* (reserved for future use) */
	int	curWeapon;	/* index of currently selected weapon */
	weaponStation station[STATIONS]; /* what's at each weapon station */
	}	craft;

/*
 *  We'll use some defines to reduce the storage required for the craft
 *  (unions would be cleaner, perhaps).
 */

#define	rounds		curRadarTarget
#define	tracerMod	structurePts
#define tracerVal	radarMode
#define owner		curWeapon
#define duration	radarMode
#define flameDuration	flags
#define	escale		SeTrim

/*
 *  Craft type definitions
 */

#define CT_FREE		0	/* an unused craft entry */
#define CT_PLANE	1	/* a player */
#define CT_MISSILE	2	/* an air to air missile */
#define CT_CANNON	3	/* a stream of cannon fire */
#define CT_SURFACE	4	/* surface object (e.g. a runway) */
#define CT_OBSERVER	5	/* a non-player "watcher" */
#define CT_DRONE	6	/* a target drone */
#define CT_EXPLOSION	7	/* an explosion */

#define FL_RECORD	0x01	/* activate recording function */
#define FL_AFTERBURNER	0x02	/* afterburner state */
#define FL_BRAKES	0x04	/* wheel brake state */
#define	FL_NWS		0x08	/* nose-wheel steering mode */
#define FL_HAS_GYRO	0x10	/* missile is gyroscope equiped */
#define FL_GHANDLE_DN	0x20	/* landing gear handle state (1 = DOWN) */
#define FL_GEAR0_UP	0x40	/* set when nose gear is completely up */
#define FL_GEAR1_UP	0x80	/* set when right gear is completely up */
#define FL_GEAR2_UP	0x100	/* set when left gear is completely up */
#define FL_GND_CONTACT	0x200	/* set when we're touching the ground */

#define RM_OFF		0	/* radar is off */
#define RM_STANDBY	1	/* standby */
#define RM_GUN_DIRECTOR	2	/* gun director mode */
#define RM_HUD		3	/* 20 degree scan */
#define RM_NORMAL	4	/* 65 degree scan */

#ifdef notdef
typedef struct {
	VPoint	Sg;		/* location */
	double	size;		/* size scaling factor */
	int	duration;	/* number of ticks left in visible life */
	int	flameDuration;	/* number of ticks left of flame */
	}	explosion;
#endif

#ifndef ALLOCATE_SPACE
#define _var_type extern
#else
#define _var_type
#endif

_var_type  int		recordCount;	/* number of players recording info */
_var_type  double		curTime;	/* current time */

_var_type  craft		stbl[MAXSURFACE]; /* table of surface objects */
_var_type  craft		ptbl[MAXPLAYERS]; /* table of player aircraft */
_var_type  craft		mtbl[MAXPROJECTILES]; /* table of missiles and cannon streams */
_var_type  weaponDesc	wtbl[WEAPONTYPES]; /* descriptions of different weapons */

_var_type  VPoint		teamLoc[3];	/* the centers of each team's airport */

_var_type  double		deltaT;		/* Update interval in seconds */
_var_type  double		halfDeltaTSquared; /* 0.5 * deltaT * deltaT */
_var_type  int		HUDPixel;	/* index of HUD color in viewport(s) */
_var_type  VColor		*HUDColor;
_var_type  int		whitePixel;	/* index of white in viewport(s) */
_var_type  VColor		*whiteColor;	/* white in viewport(s) */
_var_type  int		blackPixel;	/* index of black in viewport(s) */
_var_type  VColor		*blackColor;	/* black in viewport(s) */
_var_type  VColor		*groundColor;	/* ground color in viewport(s) */

#undef _var_type

#define a	32.15		/* acceleration due to gravity (fps^2) */
#define pi	3.14159265358979323846
#define mag(v)	(sqrt (v.x * v.x + v.y * v.y + v.z * v.z))

#define NM		6076.115
#define FPStoMPH(v)	(v / 5280.0 * 3600.0)
#define FPStoKTS(v)	(v / 6076.115 * 3600.0)
#define RADtoDEG(a)	(a * 180.0 / pi)
#define DEGtoRAD(a)	(a * pi / 180.0)
#define fsign(d)	(d < 0 ? -1.0 : 1.0)
#define calcThrust(c)	((*c->cinfo->thrust)(c))
#define fuelUsed(c)	(((c->flags & FL_AFTERBURNER) ? \
	c->cinfo->spABFuelConsump : c->cinfo->spFuelConsump) * c->curThrust * \
	deltaT / 3600.0)

#define VIEW_WINDOW_HEIGHT	500
#define VIEW_WINDOW_WIDTH	1200
#define RADAR_WINDOW_WIDTH	200
#define RADAR_WINDOW_HEIGHT	200
#define RADAR_X			(1200/2-200/2)
#define RADAR_Y			(500)
#define TEWS_X			(1200/2-200/2-100)
#define TEWS_Y			(VIEW_WINDOW_HEIGHT+60)
#define TEWS_SIZE		81
#define FS_WINDOW_WIDTH		(VIEW_WINDOW_WIDTH)	
#define FS_WINDOW_HEIGHT 	(VIEW_WINDOW_HEIGHT+RADAR_WINDOW_HEIGHT)
#define	FLAP_X			(1200/2-200/2-95)
#define FLAP_Y			(FS_WINDOW_HEIGHT-53)
#define DESIGNATOR_SIZE		40
/* PANEL_X is determined based on the size of the panel and TEWS location */
#define PANEL_Y			(RADAR_Y+40)

/*
 *  Location of the center of the engine RPM gauge.
 */

#define ENG_X			(FS_WINDOW_WIDTH-300)
#define ENG_Y			(VIEW_WINDOW_HEIGHT+60)

#define ALT_ORG_X		760
#define ALT_ORG_Y		360
#define ALT_LENGTH		199
#define ALT_ORIENT		orientRight
#define ALT_SCALE		(1700.0 / (double) ALT_LENGTH)
#define ALT_INDEX_SIZE		30
#define ALT_MIN_INTERVAL	100
#define ALT_MIN_SIZE		8
#define ALT_MAJ_INTERVAL	500
#define ALT_MAJ_SIZE		15
#define ALT_DIVISOR		1000.0
#define ALT_FORMAT		"%4.3g"

#define VEL_ORG_X		440
#define VEL_ORG_Y		360
#define VEL_LENGTH		ALT_LENGTH
#define VEL_ORIENT		0
#define VEL_SCALE		(170.0 / (double) VEL_LENGTH)
#define VEL_INDEX_SIZE		30
#define VEL_MIN_INTERVAL	10
#define VEL_MIN_SIZE		8
#define VEL_MAJ_INTERVAL	50
#define VEL_MAJ_SIZE		15
#define VEL_DIVISOR		10.0
#define VEL_FORMAT		"%3.3g"

#define HDG_ORG_X		(VIEW_WINDOW_WIDTH / 2 - (6*30+1) / 2)
#define HDG_ORG_Y		400
#define HDG_LENGTH		(6*30+1)
#define HDG_ORIENT		0
#define HDG_SCALE		(3000.0 / (double) VEL_LENGTH)
#define HDG_INDEX_SIZE		10
#define HDG_MIN_INTERVAL	500
#define HDG_MIN_SIZE		5
#define HDG_MAJ_INTERVAL	1000
#define HDG_MAJ_SIZE		10
#define HDG_DIVISOR		1000.0
#define HDG_FORMAT		"%2.2g"
