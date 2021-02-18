/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/table/RCS/spread.ch,v 1.7 1991/09/12 19:50:43 bobg Exp $ */
/* $ACIS:spread.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/table/RCS/spread.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidspread_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/table/RCS/spread.ch,v 1.7 1991/09/12 19:50:43 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* spread.ch - table view definition and interface */
#define spread_VERSION 1

/* internal dimensions */

#define	spread_LINEWIDTH 1	    /* width of lines between cells */
#define	spread_SPACING 3	    /* total additional space between cells */
#define	spread_CELLMARGIN 1	    /* white border stolen from cell contents */
#define	spread_BORDER 20	    /* width of table border */

/* reference to the associated data object */

#define getView(V) ((V) -> header.view)
#define getIM(V) view_GetIM(&getView(V))
#define getDrawable(V) view_GetDrawable(&getView(V))
#define MyTable(V) ((struct table *)(getView(V).dataobject))

#define localWidth(V) view_GetLogicalWidth(&getView(V))
#define localHeight(V) view_GetLogicalHeight(&getView(V))

#define CtoX(V, j) (Width(V, 0, j) + spread_BORDER - V -> hOffset)
#define RtoY(V, j) (Height(V, 0, j) + spread_BORDER - V -> vOffset)

/* loops over cells */

#define FirstX(V, j, z) j = 0, z = spread_BORDER - V -> hOffset
#define FirstY(V, j, z) j = 0, z = spread_BORDER - V -> vOffset
#define NextX(V, j, z) z += MyTable(V) -> col[j++].thickness + spread_SPACING
#define NextY(V, j, z) z += V -> rowInfo[j++].computedHeight

/* information about row displays */

struct rowInformation {
    int	computedHeight;		/* most recently computed thickness */
    int	biggestCol;		/* column which maximized thickness */
};

/* internal interfaces */

long Width();			/* get width of columns */
long Height();			/* get height of rows */
struct view *FindSubview();	/* find subview associated with cell */

/* Interface definition */

class spread: view {

overrides:
  Print(FILE *f, char *processor, char *format, boolean toplevel);
  FullUpdate(enum view_UpdateType how, long left, long top, long width, long height);
  Update();
  WantUpdate(struct view *requestor);
  Hit(enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
  DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
  WantNewSize(struct view *requestor);
  ReceiveInputFocus();
  LoseInputFocus();
  GetApplicationLayer() returns struct view *;
  GetInterface(char *type) returns struct scrollfns *;
  LinkTree(struct view * parent);
  ObservedChanged(struct observable *changed, long status);
  UnlinkNotification(struct view *unlinkedTree);
methods:

classprocedures:
  InitializeClass() returns boolean;	/* initialize class */
  InitializeObject(struct spread *V) returns boolean;	/* initialize instance */
  FinalizeObject(struct	spread *V);	/* clean up instance */

data:
	boolean hasInputFocus;		/* res ipse dixit */
	int updateRequests;		/* update requests pending */
	boolean borderDrawn;		/* border, etc are visible */
	int rowInfoCount;		/* number of entries in rowInfo */
	struct rowInformation *rowInfo;	/* row display information */
	char movemode;			/* in midst of a line move */
	short currentslice;		/* current (dim ? column : row) index */
        short currentoffset;		/* loc (pels in inset) of current slice */
	short icx,icy;			/* hit location */
	int lastTime;			/* timestamp of last update */
	long vOffset;			/* vertical scroll offset */
	long hOffset;			/* horizontal scroll offset */
	struct chunk anchor;		/* chunk selected by downpress */
	struct chunk selection;		/* currently selected chunk */
	boolean selectionvisible;		/* selection has been drawn */
	char bufferstatus;		/* what is in keysin */
#define BUFFEREMPTY 0			/* nothing there */
#define BUFFERHASINPUT 1		/* reading new formula */
#define BUFFERHASFORMULA 2		/* displaying selected cell formula */
#define BUFFERHASMESSAGE 3		/* displaying message */
#define BUFFERHASPARAM 4		/* reading parameter */
	struct keystate *keystate;	/* keyboard input tracker */
	struct menulist *menulist;	/* menus for this view */
	struct cursor *tableCursor;	/* cursor within the table */

	struct fontdesc *writingFont;	/* font for table entries */
	long standardHeight;		/* height of a digit or letter */
	long zeroWidth;			/* width of zero in writingfont */
	long dotWidth;			/* width of dot in writingfont */

	struct graphic *grayPix;	/* halftone filler */
	struct graphic *blackPix;	/* pure black filler */
	boolean	finalizing;		/* being finalized? */
};

/* end of spread.ch */

