/* $Header: /h2c/truth/server/ddx/dec/ws/RCS/ws.h,v 1.8 91/09/10 14:21:10 edg Exp $ */

/* $Log:	ws.h,v $
 * Revision 1.8  91/09/10  14:21:10  edg
 * cleanup
 * 
 * Revision 1.7  91/09/10  12:58:37  edg
 * added more -tx options
 * 
 * Revision 1.6  91/08/08  20:31:11  edg
 * added command line options for TX
 * 
 * Revision 1.5  90/07/25  17:35:19  gringort
 * added new field for monitor type arg
 * 
 * Revision 1.4  90/07/16  17:45:16  gringort
 * added edge attachment command arg support
 * 
 * Revision 1.3  90/06/21  18:56:20  gringort
 * added wsMotionEvents
 * 
 * Revision 1.2  90/06/07  13:31:33  gringort
 * changes to init
 * 
 * Revision 1.1  90/04/23  14:45:52  gringort
 * Initial revision
 * 
 *  
 */

#define NOMAPYET        (ColormapPtr) 1

#define  ARG_DPIX	(1 << 0)
#define  ARG_DPIY	(1 << 1)
#define  ARG_DPI	(1 << 2)
#define  ARG_BLACKVALUE	(1 << 3)
#define  ARG_WHITEVALUE	(1 << 4)
#define	 ARG_CLASS	(1 << 5)
#define	 ARG_EDGE_L	(1 << 6)
#define	 ARG_EDGE_R	(1 << 7)
#define	 ARG_EDGE_T	(1 << 8)
#define	 ARG_EDGE_B	(1 << 9)
#define  ARG_MONITOR	(1 << 10)
#define  ARG_TXRVISUAL	(1 << 11)	/* for TX */
#define  ARG_TXXORFIX	(1 << 12)	/* for TX */
#define  ARG_TXBANKSW	(1 << 13)	/* for TX */
#define  ARG_TXBSTORE	(1 << 14)	/* for TX */

typedef struct  {
	int flags;
	int dpix;
	int dpiy;
	int dpi;
	int class;
	char  *blackValue;
	char  *whiteValue;
	int edge_left;
	int edge_right;
	int edge_top;
	int edge_bottom;
	ws_monitor monitor;
	int txRootDepth;	/* for TX */
	int txRootClass;	/* for TX */
	int txXorFix;		/* for TX */
	int txBankSwitch;	/* for TX */
	int txBStore;		/* for TX */
} ScreenArgsRec;

typedef struct {
	unsigned int		currentmask; 	/* saved plane mask */
	BoxPtr			cursorConstraint;
	ws_screen_descriptor	*screenDesc;
	ColormapPtr		pInstalledMap;
	ScreenArgsRec 		*args;
} wsScreenPrivate;

typedef struct {
	char *moduleID;	/* graphic module ID */
	Bool (*createProc)();	/* create procedure for this hardware type */
} wsAcceleratorTypes;

extern void wsStoreColors();
extern void wsInstallColormap();
extern void wsUninstallColormap();
extern int wsListInstalledColormaps();
extern int wsScreenPrivateIndex;
extern Bool wsSaveScreen();
extern int dpix, dpiy, dpi;

extern ScreenArgsRec screenArgs[];

extern ScreenPtr wsScreens[];
extern int class;
extern int fdPM;   /* this is the file descriptor for screen so
		    can do IOCTL to colormap */
extern int ws_cpu;

