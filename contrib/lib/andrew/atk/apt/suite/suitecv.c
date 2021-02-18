/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *suitecv_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/suitecv.c,v 1.15 1991/09/12 15:56:43 bobg Exp $";


/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidsuitecv_c = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/suitecv.c,v 1.15 1991/09/12 15:56:43 bobg Exp $";
#endif

/*
    $Log: suitecv.c,v $
 * Revision 1.15  1991/09/12  15:56:43  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.14  1990/06/13  19:02:18  gk5g
 * Added use of fontdesc_StringBoundingBox method contributed by Bill Janssen.
 * Changed the way ItemFullUpdate handles a suite_item that simply has a handle on a child-view. Now just insert it and FullUpdate it.
 *
 * Revision 1.13  89/12/14  15:14:28  cfe
 * Sync with MIT tape (add additional log stuff).
 * 
 * Revision 1.12  89/12/12  14:58:15  ghoti
 * sync with MIT tape
 * 
 * Revision 1.2  89/11/28  15:50:12  xguest
 * Added Gary Keim's diffs.
 * 
 * Revision 1.1  89/11/28  15:42:32  xguest
 * Initial revision
 * 
 * Revision 1.11  89/09/08  09:20:00  ghoti
 * removed unused variable declaration
 * 
 * Revision 1.10  89/08/24  19:47:02  gk5g
 * Changes in support of V1.0 of the SuiteProgGuide.doc.
 * 
 * Revision 1.9  89/08/04  16:19:14  gk5g
 * Fixed a stupid typo.
 * 
 * Revision 1.8  89/08/04  15:41:17  gk5g
 * Fixed the synthesis of a carriage-return when a ReadWrite item has the focus and then it looses it due to a hit outside of its space.
 * 
 * Revision 1.7  89/07/13  16:09:43  gk5g
 * Simply changed all occurances of #include "foo.h" to #include <foo.h>.
 * 
 * Revision 1.6  89/06/09  17:25:23  gk5g
 * Removed suite_Reset() from suitecv_ReceiveInputFocus().
 * Added text_Clear() and text_InsertCharacters() to suitecv_ReceiveInputFocus().
 * 
 * Revision 1.5  89/05/30  18:43:02  gk5g
 * Finally fixed getting the correct caption from a RW item!
 * 
 * Revision 1.4  89/05/26  20:02:42  gk5g
 * Worked on improving the ReadWrite items behavior with
 * 		respect to hits in other items when it has the inputFocus
 * 
 * Revision 1.3  89/05/23  20:28:42  gk5g
 * Removed clearing of the suitecv upon ReceiveInputFocus().
 * 
 * Revision 1.2  89/05/08  16:42:47  gk5g
 * changed references from suiteEV to suiteev
 * 
 * Revision 1.1  89/05/04  12:36:57  gk5g
 * Initial revision
 * 
 * Revision 1.1  89/04/28  20:26:37  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Suite-object

MODULE	suitecv.c

VERSION: 0.0

AUTHOR	TC Peters & GW Keim
 	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Suite-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  11/01/88	Created (GW Keim)
  05/04/89	Changed to lower-case naming convention (GW Keim)
  05/23/89	Removed the clearing of the suitecv textview on 
		ReceiveInputFocus (GW Keim)
  05/26/89	Worked on getting the correct caption from the text object
		after a caption string has already been entered once; (GW Keim)
   05/30/89	Finally fixed getting the correct caption from a RW item! (GW Keim)
END-SPECIFICATION  ************************************************************/

#include <class.h>
#include <rect.h>
#include <keystate.ih>
#include <keymap.ih>
#include <bind.ih>
#include <proctbl.ih>
#include <text.ih>
#include <suite.ih>
#include <suiteev.ih>
#include <suitecv.eh>

#define EV			    ((self)->parent_EV)
#define ParentItem		    ((self)->parent_item)
#define KeyState		    ((self)->kstate)
#define Debug			    ((self)->debug)
#define	Suite			    ((EV)->parent)
#define	ClientAnchor		    ((Suite)->anchor)

void suitecv_InsertNLCmd();
static struct keymap *KeyMap;
struct bind_Description Bindings[] = {
    {"suitecv-insert-newline","\015",0,NULL,0,0,(void(*)())suitecv_InsertNLCmd,
	"Insert a newline character","suitecv"},NULL};

boolean 
suitecv__InitializeClass( ClassID )
    struct classheader *ClassID;
{
    if(!(KeyMap =  keymap_New())) {
	printf("suitecv:Could not create a keymap\n");
	exit(1);
    }
    bind_BindList(Bindings,KeyMap,NULL,&suitecv_classinfo);
    return(TRUE);
}

boolean
suitecv__InitializeObject( ClassID, self )
    struct classheader *ClassID;
    struct suitecv *self;
{
    self->kstate = keystate_Create(self,KeyMap);
    self->parent_item = NULL;
    self->parent_EV = NULL;
    return(TRUE);
}

void
suitecv__FinalizeObject( self )
    struct suitecv *self;
{}

void
suitecv_InsertNLCmd( self, key )
    struct suitecv *self;
    long key;
{
    if(ClientAnchor)
	suitecv_WantInputFocus(self,ClientAnchor);
    else 
	suitecv_WantInputFocus(self,Suite);
}

void 
suitecv__PostKeyState(self,kstate)
    struct suitecv *self;
    struct keystate *kstate;
{
    if (kstate == self->header.textview.keystate) {
	keystate_AddBefore(KeyState,kstate); 
	super_PostKeyState(self,KeyState);
    }
    else super_PostKeyState(self,kstate);
}

void
suitecv__ReceiveInputFocus( self )
    struct suitecv	    *self;
{
    register struct text    *txt = NULL;

    super_ReceiveInputFocus(self);
    if(!ParentItem->hithandler) suiteev_ItemHighlight(EV,ParentItem);
    text_Clear(txt = (struct text*)ParentItem->dataobject);
    if (ParentItem->caption)
        text_InsertCharacters(txt,0,ParentItem->caption,
			   strlen(ParentItem->caption));
    suitecv_CollapseDot(self);
}

void
suitecv__LoseInputFocus( self )
struct suitecv *self;
{
    register struct text *RWtext = (struct text*) suitecv_GetDataObject(self);
    long len = 0;
    register struct suite_item *item = ParentItem;
    register struct suite *suite = EV->parent;
    register long i = 0;

    super_LoseInputFocus(self);
    suiteev_ItemNormalize(EV,item);
    if(item->caption) free(item->caption);
    len = text_GetLength(RWtext);
    item->caption = (char*)malloc(len+1);
    while(i < len) {
	item->caption[i] = text_GetChar(RWtext,i);
	i++;
    }
    item->caption[len] = '\0';
    if(item->hithandler) 
	item->hithandler(suite->anchor,suite,item,suite_ItemObject,0,0,0);
    else if(suite->hithandler) 
	suite->hithandler(suite->anchor,suite,item,suite_ItemObject,0,0,0);
}

