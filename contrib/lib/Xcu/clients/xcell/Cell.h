/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

#ifndef _Xcu_Cell_h
#define _Xcu_Cell_h

#include <X11/Xcu/Simple.h>

#define XtNnColors		"nColors"
#define XtRNColors		"NColors"
#define XtCCellNColors		"CellNColors"

#define XtNcolors		"colors"
#define XtRColors		"Colors"
#define XtCCellColors		"CellColors"

#define XtNcolor0		"color0"
#define XtNcolor1		"color1"
#define XtNcolor2		"color2"
#define XtNcolor3		"color3"
#define XtNcolor4		"color4"
#define XtNcolor5		"color5"
#define XtNcolor6		"color6"
#define XtNcolor7		"color7"
#define XtCCellColor		"CellColor"

#define XtCCellDirectory	"CellDirectory"
#define XtNdirectory		"directory"

#define XtCCellRulesFile	"CellRulesFile"
#define XtNrulesFile		"rulesFile"

#define XtCCellPerturb		"CellPerturb"
#define XtNperturb		"perturb"

#define XtCCellAlternating	"CellAlternating"
#define XtNalternating		"alternating"

#define XtCCellPickFile		"CellPickFile"
#define XtNpickFile		"pickFile"

#define XtCCellSeed		"CellSeed"
#define XtNseed			"seed"

#define XtCCellType		"CellType"
#define XtNcellType		"cellType"

#define XtCCellPerRow		"CellPerRow"
#define XtNperRow		"perRow"

#define XtCCellPerCol		"CellPerCol"
#define XtNperCol		"perCol"

#define XtCCellVerticalMerge	"CellVerticalMerge"
#define XtNverticalMerge	"verticalMerge"

#define XtCCellHorizontalMerge	"CellHorizontalMerge"
#define XtNhorizontalMerge	"horizontalMerge"

#define XtCCellRuleIndex	"CellRuleIndex"
#define XtNruleIndex		"ruleIndex"

#define XtCCellIgnoreColors	"CellIgnoreColors"
#define XtNignoreColors		"ignoreColors"

#define XtCCellOrderedRules	"CellOrderedRules"
#define XtNorderedRules		"orderedRules"

#define XtRXcuCellSequence	"XcuCellSequence"
#define XtCCellSequence		"CellSequence"
#define XtNsequence		"sequence"

#define XtRXcuCellFrequency	"XcuCellFrequency"
#define XtCCellFrequency	"CellFrequency"
#define XtNfrequency		"frequency"

#define XtCBeep			"Beep"
#define XtNbeep			"beep"

#define XtCMutate		"Mutate"
#define XtNmutate		"mutate"

/* Class record constants */

extern WidgetClass xcuCellWidgetClass;

typedef struct _XcuCellClassRec *XcuCellWidgetClass;
typedef struct _XcuCellRec      *XcuCellWidget;

/* Public Functions */

typedef enum 
    {
     XcuORDERED_PERMUTE
    ,XcuRANDOM_PERMUTE
    ,XcuRANDOM_COLOR
    }
    XcuCellSequenceType ;

typedef enum 
    {
     XcuFIXED_FREQUENCY
    ,XcuPAGE_FREQUENCY
    ,XcuRULE_FREQUENCY
    ,XcuLINE_FREQUENCY
    }
    XcuCellFrequencyType ;

void XcuCellPickRule () ;
void XcuCellStep () ;
void XcuCellNextMap () ;
void XcuCellFirstMap () ;

#endif _Xcu_Cell_h
