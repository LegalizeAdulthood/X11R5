/*LIBS: -lbasics -lm
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/table/RCS/spread.c,v 1.6 1991/09/12 16:33:55 bobg Exp $ */
/* $ACIS:spread.c 1.6$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/table/RCS/spread.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/table/RCS/spread.c,v 1.6 1991/09/12 16:33:55 bobg Exp $";
#endif /* lint */


extern char * malloc();

#include <class.h>

#include <view.ih>
#include <graphic.ih>
#include <cursor.ih>
#include <environ.ih>
#include <fontdesc.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <scroll.ih>
#include <rect.h>
#include <dataobj.ih>
#include <table.ih>

#include <spread.eh>

/* initialize entire class */

static struct keymap *mainmap = (struct keymap *) NULL;
static struct menulist *mainmenus = (struct menulist *) NULL;

boolean spread__InitializeClass(classID)
struct classheader *classID;
{
    if (debug)
	printf("spread__InitializeClass(%x)\n", classID);


    mainmap = keymap_New ();
    k_DefineKeys (mainmap, &spread_classinfo);
    
    mainmenus = menulist_New ();
    DefineMenus (mainmenus, mainmap, &spread_classinfo);
    return TRUE;
}

/* initialize table view */

boolean spread__InitializeObject(classID, V)
struct classheader *classID;
register struct spread * V;
{
    if (debug)
	printf("spread__InitializeObject(%x, %x)\n", classID, V);

    V->finalizing = FALSE;
    V->hasInputFocus = FALSE;
    V->updateRequests = 0;
    V->borderDrawn = FALSE;
    V->rowInfoCount = 0;
    V->rowInfo = NULL;
    V->movemode = 0;
    V->currentslice = 0;
    V->currentoffset = 0;
    V->icx = V->icy = 0;
    V->lastTime = -1;
    V->vOffset = 0;
    V->hOffset = 0;
    V->selectionvisible = FALSE;
    V->bufferstatus = BUFFEREMPTY;
    V->keystate = keystate_Create(V, mainmap);
    V->menulist = menulist_DuplicateML (mainmenus, V);
    V->tableCursor = NULL;
    V->writingFont = NULL;
    V->standardHeight = 0;
    V->zeroWidth = 0;
    V->dotWidth = 0;
    V->grayPix = NULL;
    V->blackPix = NULL;
    ResetCurrentCell(V);
    return TRUE;
}

/* initialize graphic-dependent data */

void InitializeGraphic(V)
struct spread *V;
{
    struct fontdesc *tempFont;
    char *wfontname = NULL;
    struct FontSummary *fs;

    if (debug)
	printf("InitializeGraphic(%x)\n", V);

    if (!getDrawable(V)) {
	printf("InitializeGraphic called without drawable.\n");
	return;
    }
    V->grayPix = spread_GrayPattern(V, 8, 16);
    V->blackPix = spread_BlackPattern(V);
    V->tableCursor = cursor_Create(V);
    tempFont = fontdesc_Create("table", fontdesc_Plain, 12);
    cursor_SetGlyph(V->tableCursor, tempFont, 't');
    wfontname = environ_GetProfile("bodyfontfamily");
    if (!wfontname || !*wfontname) wfontname = "andysans";
    V->writingFont = fontdesc_Create (wfontname, fontdesc_Plain, 12);
    fs = fontdesc_FontSummary(V->writingFont, getDrawable(V));
    V->standardHeight = fs->maxHeight;
}

/* recompute thickness of rows */

ComputeRowSizes(V)
register struct spread *V;
{
    struct table *T = MyTable(V);
    int r, c;
    struct cell *cell;
    long dWidth, dHeight;
    struct view *child;

    if (debug)
	printf("ComputeRowSizes(%x) with standardHeight = %d\n", V, V->standardHeight);

    if (V->rowInfoCount != table_NumberOfRows(T) || V->rowInfo == NULL) {
	if (V->rowInfo != NULL)
	    free(V->rowInfo);
	V->rowInfo = (struct rowInformation *) malloc(table_NumberOfRows(T) * sizeof(struct rowInformation));
	if (V->rowInfo == NULL) {
	    fprintf(stderr, "Out of memory for row sizes\n");
	    exit(4);
	}
	V->rowInfoCount = table_NumberOfRows(T);
    }

    for (r = table_NumberOfRows(T) - 1; r >= 0; r--) {
	if (table_RowHeight(MyTable(V), r) > 0)
	    V->rowInfo[r].computedHeight = table_RowHeight(MyTable(V), r);
	else {
	    V->rowInfo[r].computedHeight = V->standardHeight + 2 * spread_CELLMARGIN + spread_SPACING;
	    V->rowInfo[r].biggestCol = 1;
	    for (c = 0; c < table_NumberOfColumns(T); c++) {
		if (!table_IsJoinedToAnother(T, r, c)) {
		    cell = table_GetCell(T, r, c);
		    if (cell->celltype == table_ImbeddedObject) {
			register int t;
			register int width;

			dHeight = 0;
			width = table_ColumnWidth(T, c) - 2 * spread_CELLMARGIN;
			for (t = c + 1; t < table_NumberOfColumns(T) && table_IsJoinedToLeft(T, r, t); t++)  {
			    width += table_ColumnWidth(T, t);
			}
			child = FindSubview(V, cell);
			if (child)
			    view_DesiredSize(child, width , V->rowInfo[r].computedHeight, view_WidthSet, &dWidth, &dHeight);
			else
			    dWidth = dHeight = 0;
			if (debug)
			    printf(" [%d,%d] wants %ld\n", r+1, c+1, dHeight);
			dHeight += 2 * spread_CELLMARGIN + spread_SPACING;
			for (t = r + 1; r < table_NumberOfRows(T) && table_IsJoinedAbove(T, t, c); t++) {
			    dHeight -= V->rowInfo[t].computedHeight;
			}
			if (V->rowInfo[r].computedHeight < dHeight) {
			    V->rowInfo[r].computedHeight = dHeight;
			    V->rowInfo[r].biggestCol = c;
			}
		    }
		}
	    }
	}
    }
}

/* filter update requests */

void spread__WantUpdate(V, requestor)
register struct spread * V;
struct view *requestor;
{

    if (debug)
	printf("spread__WantUpdate(%x,%x) requests = %d\n", V, requestor, V->updateRequests);

    if ((&getView(V) != requestor || !(V->updateRequests++)) && getView(V).parent != NULL)
	view_WantUpdate(getView(V).parent, requestor);

    if (debug)
	printf("spread__WantUpdate exited\n");
}

/* negotiate size of view */

enum view_DSattributes spread__DesiredSize(V, width, height, pass, dWidth, dHeight)
register struct spread * V;
long width, height;
enum view_DSpass pass;
long *dWidth, *dHeight;
{
    long GrossWidth;
    long GrossHeight;

    if (debug)
	printf("spread_DesiredSize(%x, %d, %d, %d, .. )\n", V, width, height, (int)pass);

    if (V->grayPix == NULL)
	InitializeGraphic(V);
    ComputeRowSizes(V);

    GrossWidth = Width(V, 0, table_NumberOfColumns(MyTable(V))) + spread_BORDER + spread_SPACING;
    GrossHeight = Height(V, 0, table_NumberOfRows(MyTable(V))) + spread_BORDER + spread_SPACING;

    *dWidth = (pass == view_WidthSet) ? width : GrossWidth;
    *dHeight = (pass == view_HeightSet) ? height : GrossHeight;

    return (enum view_DSattributes)
      ((int)((*dWidth > GrossWidth) ? view_WidthFlexible : view_WidthLarger)
      | (int)((*dHeight > GrossHeight) ? view_HeightFlexible : view_HeightLarger));
}

/* handle child's request for a new size */

void spread__WantNewSize(V, requestor)
struct spread *V;
struct view *requestor;
{
    struct table *T = MyTable(V);

    if (debug)
	printf("spread_WantNewSize(%x, %x)\n", V, requestor);

    table_StampEverything(T);
    table_NotifyObservers(T, 0);
    table_SetModified(T);
}

/* print as part of larger document */

void spread__Print(V, f, proc, format, toplevel)
register struct spread * V;
FILE * f;
char *proc;		/* processor */
char *format;		/* final format */
boolean toplevel;	/* am I the top level view? */
{
    if (debug)
	printf("spread_Print(%x, %x, %s, %s, %d)\n", V, f, proc, format, toplevel);
    putc('\n', f);
    WriteTroff(V, f, proc, format, toplevel);
}

/* full update when window changes */

void spread__FullUpdate(V, how, left, top, width, height)
register struct spread * V;
enum view_UpdateType how;
long left, top, width, height;
{
    struct rectangle cliprect;
    if (debug)
	printf("spread_FullUpdate(%x, %d, %d, %d, %d, %d)\n", V, (int)how, left, top, width, height);
    if (how == view_PartialRedraw || how == view_LastPartialRedraw)
	rectangle_SetRectSize(&cliprect, left, top, width, height);
    else
	rectangle_SetRectSize(&cliprect, view_GetVisualLeft(&getView(V)), view_GetVisualTop(&getView(V)), view_GetVisualWidth(&getView(V)), view_GetVisualHeight(&getView(V)));
    FullUpdate(V, how, &cliprect);
}

/* partial update */

void spread__Update(V)
register struct spread * V;
{
    struct rectangle cliprect;
    if (debug)
	printf("spread_Update(%x)\n", V);
    rectangle_SetRectSize(&cliprect, view_GetVisualLeft(&getView(V)), view_GetVisualTop(&getView(V)), view_GetVisualWidth(&getView(V)), view_GetVisualHeight(&getView(V)));
    PartialUpdate(V, view_FullRedraw, &cliprect);
}

/* process mouse hit */

extern struct view * MouseHit();

struct view * spread__Hit(V, action, x, y, numberOfClicks)
register struct spread * V;
enum view_MouseAction action;
long x, y;
long numberOfClicks;
{
    if (debug)
	printf("spread_Hit(%x, %d, %ld, %ld, %ld)\n", V, (int) action, x, y, numberOfClicks);
    return MouseHit(V, action, x, y, numberOfClicks);
}

/* input focus lost; remove highlighting */

void spread__LoseInputFocus(V)
register struct spread * V;
{
    if (debug)
	printf("spread_LoseInputFocus(%x)\n", V);

    if(V->hasInputFocus) {
	V->hasInputFocus = 0;
	if (!WantHighlight(V))
	    spread_WantUpdate(V, &getView(V));
    }
}

/* input focus obtained; highlight something */

void spread__ReceiveInputFocus(V)
register struct spread * V;
{
    if (debug)
	printf("spread_ReceiveInputFocus(%x)\n", V);

    if (!(V->hasInputFocus)) {
	if (!WantHighlight(V))
	    spread_WantUpdate(V, &getView(V));
	V->hasInputFocus = 1;
	V->keystate->next = NULL;
	spread_PostKeyState(V, V->keystate);
	spread_PostMenus(V, V->menulist);
    }
}

/* application layer for main program */

struct view *spread__GetApplicationLayer(V)
register struct spread *V;
{
    if (debug)
	printf("spread_GetApplicationLayer(%x)\n", V);

    return (struct view *)scroll_Create(V, scroll_LEFT | scroll_TOP);
}

/* scroll vertically */

void ySetFrame(V, pos, coord, denom)
register struct spread * V;
long pos;		/* pel within view to move */
long coord;		/* where to move it to (numerator) */
long denom;		/* where to move it to (denominator) */
{
    long k = Height(V, 0, table_NumberOfRows(MyTable(V))) + spread_BORDER + spread_SPACING - localHeight(V);
    V->vOffset = pos - (coord * localHeight(V)) / denom;
    if (V->vOffset > k)
	V->vOffset = k;
    if (V->vOffset < 0)
	V->vOffset = 0;
    if (debug)
	printf ("ySetFrame(%x, %ld, %ld, %ld) = %ld\n", V, pos, coord, denom, V->vOffset);
    V->lastTime = -1;		/* for full update */
    spread_WantUpdate(V, &getView(V));
}


/* scroll horizontally */

void xSetFrame(V, pos, coord, denom)
register struct spread * V;
long pos;		/* pel within view to move */
long coord;		/* where to move it to (numerator) */
long denom;		/* where to move it to (denominator) */
{
    long k = Width(V, 0, table_NumberOfColumns(MyTable(V))) + spread_BORDER + spread_SPACING - localWidth(V);
    V->hOffset = pos - (coord * localWidth(V)) / denom;
    if (V->hOffset > k)
	V->hOffset = k;
    if (V->hOffset < 0)
	V->hOffset = 0;
    if (debug)
	printf ("xSetFrame(%x, %ld, %ld, %ld) = %ld\n", V, pos, coord, denom, V->hOffset);
    V->lastTime = -1;
    spread_WantUpdate(V, &getView(V));
}


/* get vertical scrolling information */

void yGetInfo(V, total, seen, dot)
register struct spread * V;
struct range *total;	/* overall inset bounds */
struct range *seen;	/* visible region */
struct range *dot;	/* selected region */
{
    total->beg = 0;
    total->end = Height(V, 0, table_NumberOfRows(MyTable(V))) + spread_BORDER;
    seen->beg = V->vOffset;
    seen->end = seen->beg + localHeight(V);
    if (seen->end > total->end)
	seen->end = total->end;
    if(V->selection.BotRow >= 0) {
	dot->beg = Height(V, 0, V->selection.TopRow) + spread_BORDER;
	dot->end = dot->beg + Height(V, V->selection.TopRow, V->selection.BotRow + 1);
	if (V->selection.TopRow < 0)
	    dot->beg = 0;
    }
    else
	dot->beg = dot->end = Height(V, 0, V->selection.TopRow);
    if (debug) {
	printf ("yGetInfo l=%ld visible=%ld..%ld dot=%ld..%ld\n",
		total->end, seen->beg, seen->end, dot->beg, dot->end);
	fflush (stdout);
    }
}


/* get horizontal scrolling information */

void xGetInfo(V, total, seen, dot)
register struct spread * V;
struct range *total;	/* overall inset bounds */
struct range *seen;	/* visible region */
struct range *dot;	/* selected region */
{
    total->beg = 0;
    total->end = Width(V, 0, table_NumberOfColumns(MyTable(V))) + spread_BORDER;
    seen->beg = V->hOffset;
    seen->end = seen->beg + localWidth(V);
    if (seen->end > total->end)
	seen->end = total->end;
    if(V->selection.RightCol >= 0) {
	dot->beg = Width(V, 0, V->selection.LeftCol) + spread_BORDER;
	dot->end = dot->beg + Width(V, V->selection.LeftCol, V->selection.RightCol + 1);
	if (V->selection.LeftCol < 0)
	    dot->beg = 0;
    }
    else
	dot->beg = dot->end = Width(V, 0, V->selection.LeftCol);
    if (debug) {
	printf ("xGetInfo l=%ld visible=%ld..%ld dot=%ld..%ld\n",
		total->end, seen->beg, seen->end, dot->beg, dot->end);
	fflush (stdout);
    }
}

/* convert vertical window position to view position */

long yWhatIsAt(V, coord, denom)
register struct spread * V;
long coord;
long denom;
{
    long pos = (coord * localHeight(V)) / denom + V->vOffset;

    if (debug)
	printf ("yWhatIsAt(%x, %ld, %ld) = %ld\n", V, coord, denom, pos);
    return pos;
}

/* convert horizontal window position to view position */

long xWhatIsAt(V, coord, denom)
register struct spread * V;
long coord;
long denom;
{
    long pos = (coord * localWidth(V)) / denom + V->hOffset;

    if (debug)
	printf ("xWhatIsAt(%x, %ld, %ld) = %ld\n", V, coord, denom, pos);
    return pos;
}

/* Define interface for use by scrollbar */

static struct scrollfns verticalInterface = {
    yGetInfo,
    ySetFrame,
    NULL,
    yWhatIsAt,
};

static struct scrollfns horizontalInterface = {
    xGetInfo,
    xSetFrame,
    NULL,
    xWhatIsAt,
};

struct scrollfns *spread__GetInterface(V, type)
register struct spread * V;
char * type;
{
    if (debug)
	printf("spread_GetInterface(%x, %s)\n", V, type);

    if (strcmp(type, "scroll,vertical") == 0) {
	return &verticalInterface;
    } else if (strcmp(type, "scroll,horizontal") == 0) {
	return &horizontalInterface;
    } else
	return NULL;
}

struct view * FindSubview (V, cell)
struct spread * V;
register struct cell * cell;
{
    register struct viewlist *vl;
    char *viewname;

    if (cell->celltype != table_ImbeddedObject) {
	printf("FindSubview called with non-subview cell! (internal bug)\n");
	return NULL;
    }
    for (vl = cell->interior.ImbeddedObject.views; vl && vl->parent != &getView(V); vl = vl->next) /* no body */ ;
    if (!vl) {
	vl = (struct viewlist *) malloc (sizeof *vl);
	if (!vl) {
	    printf("out of space for subview list\n");
	    return NULL;
	}
	else {
	    viewname = dataobject_ViewName(cell->interior.ImbeddedObject.data);
	    vl->parent = &getView(V);
	    vl->child = (struct view *) class_NewObject(viewname);
	    if (!vl->child) {
		printf("unable to create child view\n");
		return NULL;
	    }
	    view_SetDataObject(vl->child, cell->interior.ImbeddedObject.data);
	    view_LinkTree(vl->child, vl->parent);
	    if (debug)
		printf("FindSubview created %s at %x for %x\n", viewname, vl->child, vl->parent); 
	    vl->next = cell->interior.ImbeddedObject.views;
	    cell->interior.ImbeddedObject.views = vl;
	}
    }
    else if (debug)
	printf("FindSubview< found %x for %x\n", vl->child, vl->parent);
    return vl->child;
}


static void DestroySubviews(V,T)
register struct spread * V;
struct table *T;
{
    register struct cell * cell;
    register struct viewlist *vl;
    struct viewlist *prevvl, *nextvl;
    register int r, c;

    for (r = 0; r < table_NumberOfRows(T); r++) {
	for (c = 0; c < table_NumberOfColumns(T); c++) {
	    cell = table_GetCell(T, r, c);
	    if (cell->celltype == table_ImbeddedObject) {
		for (vl = cell->interior.ImbeddedObject.views, prevvl = NULL; vl != NULL; vl = nextvl) {
		    if (vl->parent == &getView(V)) {
			if (debug)
			    printf("destroying subview %x for %x\n", vl->child, cell->interior.ImbeddedObject.data);
			nextvl = vl->next;
			view_UnlinkTree(vl->child);
			view_Destroy(vl->child);
			if (prevvl)
			    prevvl->next = nextvl;
			else
			    cell->interior.ImbeddedObject.views = nextvl;
			free((char *) vl);
		    }
		    else {
			prevvl = vl;
			nextvl = vl->next;
		    }
		}
	    }
	}
    }
}

/* tear down a spreadsheet */

void spread__FinalizeObject (classID, V)
struct classheader *classID;
register struct spread *V;
{
    if (debug)
	printf("spread_FinalizeObject(%x, %x)\n", classID, V);
    V->finalizing=TRUE;
    if (V->rowInfo)
	free(V->rowInfo);
    DestroySubviews(V,MyTable(V));
}

void spread__LinkTree(V, parent)
register struct spread * V;
struct view * parent;
{
    register struct viewlist *vl;
    int r, c;
    struct cell *cell;

    if (debug)
	printf("spread_LinkTree(%x, %x)\n", V, parent);

    super_LinkTree(&getView(V), parent);
    if (MyTable(V) == NULL)
	return;
    for (r = 0; r < table_NumberOfRows(MyTable(V)); r++) {
	for (c = 0; c < table_NumberOfColumns(MyTable(V)); c++) {
	    cell = table_GetCell(MyTable(V), r, c);
	    if (cell->celltype == table_ImbeddedObject) {
		for (vl = cell->interior.ImbeddedObject.views; vl; vl = vl->next) {
		    if (vl->parent == &getView(V)) {
			view_LinkTree(vl->child, vl->parent);
		    }
		}
	    }
	}
    }
}


void spread__UnlinkNotification(V,tree)
struct spread *V;
struct view *tree;
{
    if(!V->finalizing) table_RemoveViewFromTable(MyTable(V),tree);
    super_UnlinkNotification((struct view *)V,tree);
    V->lastTime=(-1);
    spread_WantUpdate(V, &getView(V));
}

void spread__ObservedChanged(V, changed, status)
register struct spread *V;
struct observable *changed;
long status;
{
    if (debug)
	printf("spread_ObservedChanged(%x, %x, %ld)\n", V, changed, status);

    if(status == observable_OBJECTDESTROYED && !strcmp(class_GetTypeName(changed),"table")) {
	DestroySubviews(V,(struct table *)changed);
    }
    super_ObservedChanged(&getView(V), changed, status);
}
