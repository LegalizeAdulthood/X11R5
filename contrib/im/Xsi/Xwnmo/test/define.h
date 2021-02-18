/*
 * $Id: define.h,v 1.1 1991/09/11 06:48:26 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * define.h v 1.0   Fri Mar  8 18:21:25 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */


/***************************/
/* definition of XIMTEST.C */
/***************************/

/* definition of error */
#define	ENV_ERR	1

/* definition of buffer size */
#define MAX_BUF		256	/* MAXimum BUFfer size */

/* definition of judge flag about IC mode */
#define	ONSP	0x0001		/* mode of ON-the-SPot */
#define	OVERSP	0x0002		/* mode of OVER-the-SPot */
#define	OFFSP	0x0004		/* mode of OFF-the-SPot */
#define	ROOTW	0x0008		/* mode of ROOT-Window */
#define	ENDAT	0x000F		/* END of All Tests */

/* definition of judge flag about test result of change IC */
#define COMP    0x0001		/* COMPleted test */
#define DIC     0x0002		/* Destroy IC and continue test */
#define MENU    0x0004		/* return main MENU */

/* definition of judge flag OK or NG */
#undef	OK
#define OK	0x0000		/* result is OK */
#define NG	0x0001		/* result is No Good */

/*****************************/
/* definition of XRESOURCE.C */
/*****************************/

/* definition of geometry about bitmap data */
#define BIT_WIDTH       10	/* BITmap data WIDTH */
#define BIT_HEIGHT      10	/* BITmap data HEIGHT */

/* definition of number of data on FontSet, Pixmap, Colormap .... */
#define ARRAY_MAX       2	/* ARRAY MAXimum */

/* definition of pixel value */
#define PLANE_MAX	1	/* PLANE MAXimum */
#define PIX_MAX         4	/* PIXel MAXimum */

/* definition of bitmap element value */
#define BITS_MAX	20	/* BITS MAXimum */

/* definition of number of menu windows */
#define MENU_MAX	20

/* definition of geometry on test window */
#define W1_X		680	/* Window1 X axis */
#define W23_X		0	/* Window2,3 X axis */
#define W1_Y		480	/* Winddow1 Y axis */
#define W2_Y		200     /* Window2, Y axis */
#define W3_Y		50	/* Window3 Y axis */
#define W1_WIDTH	600	/* Winddow1 WIDTH */
#define W2_WIDTH	400	/* Window2, WITDH */
#define W3_WIDTH	550	/* Window3 WIDTH */
#define W1_HEIGHT	450	/* Window1 HEIGHT */
#define W23_HEIGHT	150	/* Window2,3 HEIGHT */
#define W_BORDER	2	/* Window BORDER */

/* definition of geometry on name window of test window */
#define NW_WIDTH	80      /* Name Window WIDTH */
#define NW_HEIGHT	30	/* Name Window HEIGHT */
#define WIN_ITV		10	/* WINdow InTerVal */

/* definition of geometry on menu window */
#define MROOT_X		20	/* Menu ROOT window X axis */
#define MROOT_Y		930	/* Menu ROOT window Y axia */
#define MSUB_Y          30	/* Menu SUB window Y axis */
#define MSUB_WIDTH      90	/* Menu SUB window WIDTH */
#define MSUB_HEIGHT     30	/* Menu SUB window HEIGHT */
#define M_BORDER     	2	/* Menu window BORDER_width*/

/*************************/
/* definition of CHKIC.C */
/*************************/

/* definition of mode numer */
#define MODENUM		5 /* MODE NUMber */

/* definition of error code for XCreateIC and XGetICValues */
#define	NOERR		0x0000 /* NO ERRor */
#define OPIMERR		0x0001 /* OPen IM ERRor */
#define GIMERR		0x0002 /* GetIMvalues ERRor */
#define	CRICERR		0x0004 /* CReate IC ERRor */
#define	GICERR		0x0008 /* GetICvalues ERRor */

/* definition of item-numbers of valuables */
#define ONMUST		15 	/* ON-the-spot (MUST) */
#define ONMAY		17	/* ON-the-spot (MAY) */
#define OVERMUST	5  	/* OVER-the-spot (MUST) */
#define	OVERMAY		28 	/* OVER-the-spot (MAY) */
#define	OFFMUST		3  	/* OFF-the-spot (MUST) */
#define	OFFMAY	   	25 	/* OFF-the-spot (MAY) */
#define	ROOTMUST	1  	/* ROOT-window (MUST) */
#define	ROOTMAY		17 	/* ROOT-window (MAY) */

#define V_NUM           3

/* definition of dummy valuable for XVaCreateNestedList */
#define DUMMY           0

/* definition of valuables for Set/GetICValues */
#define	IS		"InputStyle"
#define	CW		"CreateWindow"
#define	FW		"FocusWindow"
#define	RN		"ResouceName"
#define	RC		"ResouceClass"
#define	PAX		"Preedit-Area.x"
#define	PAY		"Preedit-Area.y"
#define	PAW		"Preedit-Area.width"
#define	PAH		"Preedit-Area.height"
#define	PNW		"Preedit-AreaNeeded.width"
#define	PNH		"Preedit-AreaNeeded.height"
#define	PSLX		"Preedit-SpotLocation.x"
#define	PSLY		"Preedit-SpotLocation.y"
#define	PCM		"Preedit-Colormap"
#define	PFG		"Preedit-Foreground"
#define	PBG		"Preedit-Background"
#define	PBP		"Preedit-Bg/Pixmap"
#define	PFS		"Preedit-FontSet"
#define	PLS		"Preedit-LineSpacing"
#define	PCU		"Preedit-Cursor"
#define PSL		"Preedit-StartCallbacks.client_data"
#define PSA		"Preedit-StartCallbacks.callback"
#define POL		"Preedit-DoneCallbacks.client_data"
#define POA		"Preedit-DoneCallback.callback"
#define PRL		"Preedit-DrawCallback.client_data"
#define PRA		"Preedit-DrawCallback.callback"
#define PCL		"Preedit-CaretCallback.client_data"
#define PCA		"Preedit-CaretCallback.callback"
#define	SAX		"Status-Area.x"
#define	SAY		"Status-Area.y"
#define	SAW		"Status-Area.width"
#define	SAH		"Status-Area.height"
#define	SNW		"Status-AreaNeeded.width"
#define	SNH		"Status-AreaNeeded.height"
#define	SCM		"Status-Colormap"
#define	SFG		"Status-Foreground"
#define SBG		"Status-Background"
#define	SBP		"Status-Bg/Pixmap"
#define	SFS		"Status-FontSet"
#define	SCU		"Status-Cursor"
#define SSL		"Status-StartCallbacks.client_data"
#define SSA		"Status-StartCallbacks.callback"
#define SOL		"Status-DoneCallbacks.client_data"
#define SOA		"Status-DoneCallbacks.callback"
#define SRL		"Status-DrawCallbacks.client_data"
#define SRA		"Status-DrawCallbacks.callback"

/* definition of flag for output style */
#define DEC		0x000a
#define HEX		0x0010
#define STR		0x0100

/* definition of PreEdit/Status window area */
/* Preedit area on OVER-the-spot mode */
#define	XPOVER		0	/* X axis */
#define	YPOVER		0	/* Y axis */
#define	WPOVER1		400	/* Width pattern 1 */
#define	WPOVER2		150	/* Width pattern 2 */
#define HPOVER		300	/* Height */
#define XPSL1		0	/* X axis of SpotLocation pattern 1 */
#define XPSL2		300	/* X axis of SpotLocation pattern 2 */
#define YPSL		16	/* Y axis of SpotLocation */
#define LSPC1		5	/* LineSpacing pattern1 */
#define LSPC2		10	/* LineSpacing pattern2 */
/* Preedit area on OFF-the-spot mode */
#define	WPOFF1		600	/* Width pattern 1 */
#define	WPOFF2		800	/* Width pattern 2 */
/* Status area on OVer-the-spot mode & OFf-the-spot mode */
#define	XSOVOF		20	/* X axis */
#define	YSOVOF		370	/* Y axis */
#define	WSOVOF1		100	/* Width pattern 1 */
#define	WSOVOF2		50	/* Width pattern 2 */

#define ST_WIDTH	50	/* STatus WIDTH */
#define PS_HEIGHT	20	/* Preedit/Status HEIGHT */

/*************************/
/* definition of KEYEV.C */
/*************************/

/* flags for XSet/UnsetICFocus check */
#define SUICF		0x0001	/* Set/UnsetICFocus */

/* flags for SetICValues check */
#define	FOCUS		0x0002	/* FOCUS window */
#define	RESNC		0x0004	/* RESource Name/RESource Class */
#define	AREA		0x0008	/* preedit/status AREA */
#define	NEEDED		0x0010	/* preedit/status area NEEDED */
#define SPOT		0x0020	/* preedit SPOTlocation */
#define	COLORMAP	0x0040	/* preedit/status COLORMAP */
#define	GROUND		0x0080	/* preedit/status fore GROUND/back GROUND */
#define	PIXMAP		0x0100	/* preedit/status bg/PIXMAP */
#define	FONTSET		0x0200	/* preedit/status FONTSET */
#define	LINESPC		0x0400	/* preedit LINE SPaCing (only over-the-spot) */
#define	CURSOR		0x0800	/* preedit/status CURSOR */
#define	RESET		0x1000	/* xmb/wcRESETic */
#define	CHMOD		0x2000	/* return main menu to CHange MODe */

/* window name geometry */
#define	MSTR_XY		5, 20

/***************************/
/* definition of CHKCSET.C */
/***************************/

#define CHKRES		2	/* CHecK items, array of RESource name/class */

/****************************/
/* definition of CHKPSSET.C */
/****************************/

#define	CHKAREA		8
#define	CHKNEEDED	4
#define CHKSPOT		2
#define	CHKCMAP 	2
#define CHKFBGND	4
#define	CHKPIX		2
#define CHKFONT		2
#define	CHKCUR		2

/**************************/
/* definition of curses.c */
/**************************/
#define	PRDISP_GEOM	22, 80, 0, 1 /* geometry of print part */
#define MODISP_GEOM	1, 80, 22, 1 /* geometry of mode part */
#define STDISP_GEOM	1, 80, 23, 1 /* geometry of status part */

#define	TITLE_YX	6, 26	/* geometry of title(title) */
#define COPYR_YX 	14, 24	/* geometry of title(copyright) */

#define MTITLE_YX	3, 33	/* geometry of menu mode (title) */
#define MON_YX		7, 31	/* geometry of menu mode (mode1) */
#define MOVER_YX	10, 31	/* geometry of menu mode (mode2) */
#define MOFF_YX		13, 31	/* geometry of menu mode (mode3) */
#define MROOT_YX	16, 31	/* geometry of menu mode (mode4) */
#define EXIT_YX		19, 31	/* geometry of menu mode (exit) */
#define	MODISP_YX	22, 1	/* (y, x) of mode part */
#define	STDISP_YX	23, 1	/* (y, x) of status part */

#define SLEEP_TIME	1	/* SLEEP TIME for process */


/* macro */
#define CALCY(winy, winh) ((winy) - ((winh) + (WIN_ITV)))

















