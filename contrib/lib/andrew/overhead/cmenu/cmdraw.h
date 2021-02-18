/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/cmdraw.h,v 2.7 1991/09/12 20:22:32 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/cmdraw.h,v $ */

struct drawingState {
    int stackLeft;
    int stackTop;
    int stackHeight;
    int stackWidth;
    int paneWidth;
    int paneHeight;
    int maxSelections;
    int ySelectionOffset;
    int wormLeft;
    int wormRight;
    int wormTop;
    int wormBottom;
    int paneNum;                /* Index of current pane. */
    struct pane *panePtr;          /* Pointer to current pane. */
    int selectionNum;           /* Index of current selection. */
    struct selection *selectionPtr;/* Pointer to current Selection. */
#ifdef ATTEMPTSAVEUNDERS
    int doSaveUnder;            /* Boolean indicating whether or not to do save under. */
    Pixmap saveUnder;           /* Actual saved bits. */
    int saveUnderX;             /* rectangle of saveunder in parent window. */
    int saveUnderY;
    int saveUnderWidth;
    int saveUnderHeight;
#endif /* ATTEMPTSAVEUNDERS */
};

#define cmenu_Behind     1 /* Draw a card behind another card. */
#define cmenu_BeFront    2 /* Draw a card in fron of th top card. */
#define cmenu_OnTop      3 /* Draw this card as the one the user is selecting on. */
#define cmenu_Expose     4 /* Go from Behind to OnTop efficiently. */
#define cmenu_Hide       5 /* Go from OnTop to Behind efficiently. */

#define BOUNDINGBOXSLOPX 40
#define BOUNDINGBOXSLOPY 40

extern struct pane *SetPaneNum();
extern int SetPanePtr();
extern struct selection *SetSelectionNum();
extern int SetSelectionPtr();
extern void InitializeBoundingBoxWindow();
extern void ShrinkBoundingBoxWindow();
extern void CalculatePaneAndSelection();
extern void ShowAPane();
extern void CreateMenuStack();

#define GetPaneNum(state) ((state)->paneNum)
#define GetPanePtr(state) ((state)->panePtr)
#define GetSelectionNum(state) ((state)->selectionNum)
#define GetSelectionPtr(state) ((state)->selectionPtr)
#define SetPanePtrAndNum(state, ptr, num) ((state)->panePtr = (ptr), (state)->paneNum = (num))
#define SetSelectionPtrAndNum(state, ptr, num) ((state)->selectionPtr = (ptr), (state)->selectionNum = (num))
