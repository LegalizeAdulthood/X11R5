/* textobject.c - Implements the TextType for draw
 *
 * Copyright (C) 1990-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include "objects.h"
#include "textmgr.h"
#include "drawsupprt.h"
#include "draw.h"
#include <DPS/dpsXclient.h>


/*	===== BEGIN	CONSTANTS/INLINES =====	*/
#define	DfltFontName	"Times-Roman"
/*	===== END	CONSTANTS/INLINES =====	*/


/*	===== BEGIN	TYPE DECLS =====	*/
typedef struct {
  GenericObject base;	/* Must be first!	*/
  char *font;
  int size;
  char *text;
  } TextObject;
/*	===== END	TYPE DECLS =====	*/


/*	===== BEGIN	PRIVATE ROUTINES =====	*/

static void ComputeTextBBox(ctxt, obj, mappingData, bbox)
  DPSContext ctxt;
  GenericObject *obj;
  MappingData mappingData;
  XRectangle *bbox;
  {
  float llx, lly, urx, ury;

  PSWTextBBox(
    ctxt, obj->x1, obj->y1,
    (obj->y2 == obj->y1 && obj->x2 == obj->x1) ? obj->x2+1: obj->x2, obj->y2,
    &llx, &lly, &urx, &ury);

  *bbox = MakeXBBox(&llx, &lly, &urx, &ury, mappingData);
  }
  
GenericObject *CreateTextObject(type, ctxt)
  ObjectType type;
  DPSContext ctxt;
  {
  TextObject *obj = (TextObject *)malloc(sizeof(TextObject));

  GetTextParams(
    (Widget)objDescriptors[TextType].objectData,
    &obj->font, &obj->text, &obj->size);
  PSWSetFontAndText(ctxt, obj->font, DfltFontName, obj->text, obj->size);

  obj->base.objectType = type;
  return((GenericObject *)obj);
  }

void DrawTextObject(ctxt, obj, mode)
  DPSContext ctxt;
  TextObject *obj;
  DrawMode mode;
  {
  register GenericObject *base = &(obj->base);

  if (base->y2 == base->y1 && base->x2 == base->x1)
    base->x2 = base->x1 + 1;

  switch (mode) {
    case ReDraw:
      PSWSetAllTextParams(
	ctxt, obj->font, DfltFontName, obj->text, obj->size,
	base->x1, base->y1, base->x2, base->y2);
      /* Intentionally no 'break' here		*/
    case InitialDraw:
    case IntermediateDraw:
    case FinalDraw:
      PSWDrawText(ctxt);
      break;
    default: /* CantHappen(); */ break;
    }
  }
static void Nothing()
  {
  }

/*	===== END	PRIVATE ROUTINES =====	*/

/*	===== BEGIN	PUBLIC ROUTINES =====	*/
void InitTextObject(parent, ctxt, data)
  Widget parent;
  DPSContext ctxt;
  DrawData *data;
  {
  objDescriptors[TextType].type = TextType;
  objDescriptors[TextType].label = "text";
  objDescriptors[TextType].computeBBox = ComputeTextBBox;
  objDescriptors[TextType].createObject = CreateTextObject;
  objDescriptors[TextType].drawObject = DrawTextObject;
  objDescriptors[TextType].objectSelected = Nothing;
  objDescriptors[TextType].objectDeselected = Nothing;
  objDescriptors[TextType].objectData = (char *) data;
  
  PSWLoadTextProcs(ctxt);
  }
/*	===== END	PUBLIC PROCEDURES =====	*/
