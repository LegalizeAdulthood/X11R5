/*
 * $Id: func.h,v 1.1 1991/09/11 06:48:31 ohm Exp $
 */
/* using functions on xresource.c */
void	createxres();		/* CREATE X RESources */
void	mktwin();		/* MaKe Test WINdow */
void	mkmwin();		/* MaKe Menu WINdow */
void 	mapwin();		/* MAP WINdow */
void	dstrwin();		/* Draw STRing on WINdow */
void	cwcolor();		/* Change Window COLOR */
void	freexres();		/* FREE X RESources */

/* using function on chkim.c */
XIM	openim();		/* check of XOpenIM */

/* using functions on chkic.c */
XIC	createic();		/* check of XCreateIC */
XIC	onmust();		/* for ON-the-spot mode (MUST) */
XIC	onmay();		/* for ON-the-spot mode (MAY) */
XIC	overmust();		/* for OVER-the-spot mode (MUST) */
XIC	overmay();		/* for OVER-the-spot mode (MAY) */
XIC	offmust();		/* for OFF-the-spot mode (MUST) */
XIC	offmay();		/* for OFF-the-spot mode (MAY) */
XIC	rootmust();		/* for ROOT-window mode (MUST) */
XIC	rootmay();		/* for ROOT-window mode (MAY) */

/* using function on chkutil.c */
void	chkutil();		/* CHecK UTILity functions */

/* using function on keyev.c */
FLAG	keyev();		/* transaction of KEY EVent */

/* using function on chkfe.c */
FLAG	fltev();		/* check of FiLTer EVent */

/* using function on chkxls.c */
void	xlstr();		/* check of X*LookupSTRing */

/* using functions on chkcset.c */
FLAG	xsuicf();		/* check of XSet/UnsetICFocus */
FLAG	sfocus();		/* Set FOCUS window */
FLAG	sresource();		/* Set RESOURCE name/class */

/* using functions on chkpsset.c */
FLAG	sarea();		/* Set AREA */
FLAG	sneeded();		/* Set area-NEEDED */
FLAG	sspot();		/* Set SPOTlocation */
FLAG	scmap();		/* Set ColorMAP */
FLAG	sground();		/* Set fore/backGROUND */
FLAG	spixmap();		/* Set bg/PIXMAP */
FLAG	sfontset();		/* Set FONTSET */
FLAG	slinespc();		/* Set LINESPaCing */
FLAG	scursor();		/* Set CURSOR */

/* using function on callback.c */
void	geometry();
int	pe_start();
void	pe_done();
void	pe_draw();
void 	pe_caret();
void	st_start();
void	st_done();
void	st_draw();
void	wlogcb();
void	wlogtext();
void	chgcaretval();
int	trnpebuf();

/* using function on chkreset.c */
FLAG	preset();		/* Preedit area RESET */

/* using function on final.c */
void	dtictoex();		/* DesTroy IC TO EXit */
void	dticonly();		/* DesTroy IC ONLY */
void	dtictofc();		/* DesTroy IC TO File Close */
void	fcictoex();		/* log File Close for IC TO EXit */
void	climtoex();		/* CLose IM TO EXit */
void	fcimtoex();		/* log File Close for IM TO EXit */

/* using functions on curses.c */
void	initmwin();		/* INITialization of Main WINdow */
void	prtitle();		/* PRint TITLE */
u_short	psmenu();		/* Print and Selest main MENU */
void	prprint();		/* PRint string on PRINT part */
void	prerrfunc();		/* PRint ERRor message for FUNCtion */
void	prerrld();		/* PRe-ERRor for Log file Directory */
void	prveres();		/* PRint VErify RESult */
void	prvelog();		/* PRint VErify result for LOG file */
void	prtorm();		/* PRint string of Title OR Menu on print part */
void	prmode();		/* PRint string on MODE part */
int	outchar();		/* OUTput CHARacter */
void	prstatus();		/* PRint string on STATUS part */
void	prpok();		/* PRint Prompt for OK */
void	prpng();		/* PRint Prompt for No Good */
void	cls();			/* CLear Screen */
void	freemwin();		/* FREE Main WINdow */

/* using functions on funcv.c */
VALUABLE	*mkstruct();	/* MaKe STRUCT valuable */
FLAG		verval();	/* VERify VALuable */

















