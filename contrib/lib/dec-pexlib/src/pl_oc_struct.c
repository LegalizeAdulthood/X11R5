/***********************************************************
Copyright 1987,1991 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


/*
**++
**  File Name:
**
**	pl_oc_struct.c
**
**  Subsystem:
**
**	PEXlib
**
**  Version:
**
**	V1.0
**
**  Abstract:
**
**	This file contains the routines associated with the PEX structure
**	output commands.
**
**  Keywords:
**
**  Environment:
**
**	VMS and Ultrix
**
**
**  Creation Date:
**
**  Modification History:
**
**	19-Aug-88  
**--
*/

/*
 *   Table of Contents (Routine names in order of appearance)
 */

/*
 * E   PEXExecuteStructure
 * E   PEXLabel
 * E   PEXApplicationData
 * E   PEXGse
 */

/*
 *   Include files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEX.h"
#include "PEXlib.h"
#include "PEXlibint.h"
#include "PEXocbuf.h"

#include "pl_ref_data.h"

/*+
 * NAME:
 * 	PEXExecuteStructure
 *
 * FORMAT:
 * 	`PEXExecuteStructure`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	structure	The resource identifier of the structure to be executed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command causes the
 *	specified structure to be executed.  Executing a structure
 *	consists of:
 *
 *	>
 *   1. Saving the current state of the rendering pipeline
 *   2. Setting the global transform to the current composite modeling transform
 *   3. Setting the local transform to the identity matrix
 *   4. Processing all structure elements in the called structure
 *   5. Restoring the state saved at step (1).
 *	<
 *
 *	If `structure' does not
 *	exist at the time when `PEXExecuteStructure' is processed
 *	by a renderer, the command has no effect.  The structure must first
 *	be created by using the `PEXCreateStructure' command.
 *
 *	C programmers can elect to use the equivalent C-language macro defined
 *	in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_ExecuteStructure (ocbuf, structure)
 *	!=
 *
 *	where `ocbuf' is a pointer to an output command buffer, and
 *	`structure' is a pointer to a value of type `pxlStructure' (defined in
 *	the `PEXprotost.h' include file) containing the resource identifier
 *	of the structure to be executed
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXCreateStructure", `PEXBeginStructure",
 *	`PEXEndStructure"
 *
 */

void
PEXExecuteStructure (ocbuf, structure)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlStructure	structure;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCExecuteStructure, sizeof( pxlStructure), &structure);
}

/*+
 * NAME:
 * 	PEXLabel
 *
 * FORMAT:
 * 	`PEXLabel`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	label		A value used as a label.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	When processed by a renderer, this output command is has no visible
 *	effect.  Its primary function comes when it is stored in a structure,
 *	as labels can be used in various element pointer positioning
 *	operations.
 *
 *	C programmers can elect to use the equivalent C-language macro
 *	defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_Label (ocbuf, label)
 *	!=
 *
 *	where `ocbuf' is a pointer to an output command buffer and `label'
 *	is a pointer to a long integer containing the label.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXDeleteToLabel", `PEXDeleteBetweenLabels",
 *	`PEXSetElementPtrAtLabel"
 *
 */

void
PEXLabel (ocbuf, label)

INPUT pxlOCBuf		*ocbuf;
INPUT long		label;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCLabel, sizeof( INT32), &label);
}

/*+
 * NAME:
 * 	PEXApplicationData
 *
 * FORMAT:
 * 	`PEXApplicationData`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	data		A pointer to the application data stored in the
 *			command.
 *
 *	numBytes	The size, in bytes, of the application data.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this command has no visible
 *	effect.  Its main
 *	usefulness is as a structure element to store the specified client
 *	application data.
 *
 *	No equivalent C-language macro is provided for this routine.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXApplicationData (ocbuf, data, numBytes)

INPUT pxlOCBuf		*ocbuf;
INPUT char		*data;
INPUT int		numBytes;

{
/* AUTHOR: */
    PEXAddListOC( ocbuf, PEXOCApplicationData, True/* count needed */,
		  numBytes, sizeof( char), data);
}

/*+
 * NAME:
 * 	PEXGse
 *
 * FORMAT:
 * 	`PEXGse`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	id		The identifier of the Generalized Structure Element.
 *
 *	data		The data used by the GSE.
 *
 *	numBytes	The size, in bytes, of the GSE data.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, the effect of this command is
 *	implementation dependent.  Because of floating point and colour format
 *	discrepancies across a network interface, it is expected that GSEs
 *	will be of limited usefulness:  they are provided for compatibility
 *	with PHIGS.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 * 	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 * 	`PEXAllocateOCBuffer"
 *
 */

void
PEXGse (ocbuf, id, data, numBytes)

INPUT pxlOCBuf		*ocbuf;
INPUT unsigned long	id;
INPUT char		*data;
INPUT int		numBytes;

{
/* AUTHOR:  Sally C. Barry */
    pexGse		*pReq;

    PEXInitOC (ocbuf, PEXOCGse, LENOF( pexGse), NUMWORDS( numBytes), 
		&pReq);
    if (pReq == NULL) return;

    pReq->id = id;
    pReq->numElements = numBytes;

    PEXCopyBytesToOC( ocbuf, numBytes, data);

    PEXFinishOC ( ocbuf);

    return;
}
