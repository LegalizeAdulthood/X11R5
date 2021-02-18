/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txtstvec.h,v 2.7 1991/09/12 20:03:20 bobg Exp $ */
/* $ACIS:txtstvec.h 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txtstvec.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtextstatevector = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txtstvec.h,v 2.7 1991/09/12 20:03:20 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#ifndef TEXTSTATEVECTOR_DEFINED
#define TEXTSTATEVECTOR_DEFINED


#include <style.ih>

struct text_statevector {
    long CurLeftMargin;
    long CurRightMargin;
    long CurRightEdge;
    long CurLeftEdge;
    long CurTopMargin;
    long CurBottomMargin;
    long CurFontAttributes;
    long CurScriptMovement;
    long CurFontSize;
    long CurIndentation;
    enum style_Justification CurJustification;
    long CurSpacing;
    long CurSpread;
    long SpecialFlags;
    struct fontdesc *CurCachedFont;
    char *CurFontFamily;
/* 
    struct dfamily * CurFontFamily;
    struct dfont * CurCachedFont;
 */
    struct tabs *tabs;
    struct view * CurView;		/* Pointer to view wrapped in the environment */
    char *CurColor;			/* Color for embedded text */
};


#endif /* TEXTSTATEVECTOR_DEFINED */
