/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/cmenu.h,v 2.5 1991/09/12 20:22:36 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/cmenu.h,v $ */

#include <cmerror.h>

/* Could have just as easily used TRUE and FALSE for these. */
#define cmenu_Inactive                  0
#define cmenu_Active                    1

#define cmenu_DisallowDuplicates        1
#define cmenu_CreatePane                2
#define cmenu_DeleteEmptyPanes          4

#define cmenu_NoBackground              0
#define cmenu_BackgroundPixel           1
#define cmenu_BackgroundPixmap          2
#define cmenu_NoSaveUnder               3

#ifdef _STDC_
extern struct cmenu *cmenu_Create(Display *display, Window parent,
                         char *defaultEnvironment, void (*freeFunction)());
extern cmenu_Destroy(struct cmenu *menu);
extern int cmenu_AddPane(struct cmenu *menu, char *paneTitle,
                         int panePriority, int flags);
extern int cmenu_DeletePane(struct cmenu *menu, char *paneTitle, int priority);
extern int cmenu_AddSelection(struct cmenu *menu, char *paneTitle,
               int panePriority, char *selectionLabel, int selectionPriority,
               long selectionData, int flags);
extern int cmenu_DeleteSelection(struct cmenu *menu, char *paneTitle,
               int panePriority, char *slectionLabel, int selectionPriority,
               int flags);
extern int cmenu_Activate(struct cmenu *menu, XButtonEvent *menuEvent,
               long *data, int backgroundType, long backgroundValue);
#else /* _STDC_ */
extern struct cmenu *cmenu_Create();
extern int cmenu_AddPane();
extern int cmenu_AddSelection();
extern int cmenu_DeletePane();
extern int cmenu_DeleteSelection();
#endif /* _STDC_ */
