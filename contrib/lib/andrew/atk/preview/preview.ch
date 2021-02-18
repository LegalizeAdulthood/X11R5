/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/preview/RCS/preview.ch,v 2.5 1991/09/12 19:45:49 bobg Exp $ */
/* $ACIS:preview.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/preview/RCS/preview.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidpreview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/preview/RCS/preview.ch,v 2.5 1991/09/12 19:45:49 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* ************************************************************ */
#define preview_MaxDviFonts 100
#define preview_MaxWMFonts 200

#define preview_DISPLAY_RESOLUTION 72

#define preview_MAXPageTable 1024

typedef char Preview_Line[256];
typedef short preview_pagenumber;
typedef long preview_coordinate;
typedef short preview_pagetableindex;

#define Boolean boolean
struct preview_fontname
{
   short    number;
   struct fontdesc *font;
};

enum preview_ScrollDirection  {
    preview_NoScroll,
    preview_ScrollForward,
    preview_ScrollBackward,
    preview_ScrollRight,
    preview_ScrollLeft,
    preview_MultipleScroll
};

class preview:view {
overrides:
    GetApplicationLayer() returns struct view *;
    DeleteApplicationLayer(struct view *applicationLayer);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct preview *;
/*     DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes; */
/*     GetOrigin(long width, long height, long *originX, long *originY);
 */    ReceiveInputFocus();
    LoseInputFocus();
    GetInterface(char *interfaceName) returns char *;
    WantUpdate(struct view *requestor);
methods:
    DviToDisplay();
    ReadyToQuit() returns int;
classprocedures:
    Create(FILE *f,char *fname,char *fbase,boolean compleated,boolean scale) returns struct preview *;
    InitializeObject(struct preview *self) returns boolean;
    FinalizeObject(struct preview *self);
data:
    enum preview_ScrollDirection scroll;
    int scrollLine;				/* line to use for scrolling. */
    int scrollDist;				/* distance to scroll */
Boolean debug;
Boolean DoScaling;
boolean DviFileComplete;
FILE * DviFileIn;
FILE * DviFileOut;
int DisplayResolution;
int DviFileLength;
preview_coordinate   InputResolution;
preview_coordinate   xoff, yoff;
preview_coordinate   PhysicalX, PhysicalY;
preview_coordinate   LogicalX, LogicalY;
preview_coordinate   xPixelsPerPage;
preview_coordinate   yPixelsPerPage;
preview_coordinate   Centre;
preview_coordinate   CentreY;
Boolean CharactersOnThisPage /* returned by DviToDisplay */;
Boolean RedrawRequested;
Boolean SizeChanged;
preview_coordinate   WindowWidth;
preview_coordinate   WindowHeight;
preview_coordinate   minWidth;
preview_coordinate   minHeight;
short PollCount;
int peekc;
int   DviFonts[preview_MaxDviFonts];
struct preview_fontname   WMFonts[preview_MaxWMFonts];
int   NWMFonts; 
int   curfont ;  
int   cursize;
int   slant;
boolean hasInputFocus;
struct keystate *keystate;
struct menulist *menulist;
struct proctable_Entry *ScaleProc;
struct proctable_Entry *FullSizeProc;
struct proctable_Entry *SetPageProc;
struct {
	  preview_pagenumber PageNumber;
	  long  FileOffset;
} PageTable[preview_MAXPageTable];
preview_pagetableindex NumberofPageTableEntries;
Boolean       FindFirstPage;
preview_pagetableindex    LowestNonBlankPageIndex ;
preview_pagetableindex    CurrentPageTableIndex ;
Preview_Line	      WindowTitle;
char          CurrentCursor;
int   lastc ;
boolean CreatedTemp;
char DviBaseName[512];
char DviFileName[512];
int nowreading;
Preview_Line menubuf;
int menupage;
int printpending;
int quitpending;
int CursorChanged;
struct cursor *cursor;
};
