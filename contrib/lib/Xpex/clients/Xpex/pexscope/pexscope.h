/* $Header: pexscope.h,v 2.3 91/09/11 15:52:50 sinyaw Exp $ */
/* @(#)pexscope.h 1.4 90/04/05  SMI/MIT */

/***********************************************************
Copyright 1990 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef PEXSCOPE_H_INCLUDED
#define PEXSCOPE_H_INCLUDED

static char *pex_req_name[] = {
	"",
	"GetExtensionInfo",
	"GetEnumeratedTypeInfo",
	"GetImpDepConstants",
	"CreateLookupTable",
	"CopyLookupTable",
	"FreeLookupTable",
	"GetTableInfo",
	"GetPredefinedEntries",
	"GetDefinedIndices",
	"GetTableEntry",
	"GetTableEntries",
	"SetTableEntries",
	"DeleteTableEntries",
	"CreatePipelineContext",
	"CopyPipelineContext",
	"FreePipelineContext",
	"GetPipelineContext",
	"ChangePipelineContext",
	"CreateRenderer",
	"FreeRenderer",
	"ChangeRenderer",
	"GetRendererAttributes",
	"GetRendererDynamics",
	"BeginRendering",
	"EndRendering",
	"BeginStructure",
	"EndStructure",
	"RenderOutputCommands",
	"RenderNetwork",
	"CreateStructure",
	"CopyStructure",
	"DestroyStructures",
	"GetStructureInfo",
	"GetElementInfo",
	"GetStructuresInNetwork",
	"GetAncestors",
	"GetDescendants",
	"FetchElements",
	"SetEditingMode",
	"SetElementPointer",
	"SetElementPointerAtLabel",
	"ElementSearch",
	"StoreElements",
	"DeleteElements",
	"DeleteElementsToLabel",
	"DeleteBetweenLabels ",
	"CopyElements",
	"ChangeStructureRefs",
	"CreateNameSet",
	"CopyNameSet",
	"FreeNameSet",    
	"GetNameSet",    
	"ChangeNameSet",
	"CreateSearchContext",
	"CopySearchContext",
	"FreeSearchContext",
	"GetSearchContext",
	"ChangeSearchContext",
	"SearchNetwork",
	"CreatePhigsWks",
	"FreePhigsWks",
	"GetWksInfo",
	"GetDynamics",
	"GetViewRep",
	"RedrawAllStructures",
	"UpdateWorkstation",
	"RedrawClipRegion",
	"ExecuteDeferredActions",
	"SetViewPriority",
	"SetDisplayUpdateMode",
	"MapDCtoWC",
	"MapWCtoDC",
	"SetViewRep",
	"SetWksWindow",
	"SetWksViewport",
	"SetHlhsrMode ",
    "SetWksBufferMode",
	"PostStructure",
	"UnpostStructure",     
	"UnpostAllStructures",
	"GetWksPostings",
	"GetPickDevice",
	"ChangePickDevice",
	"CreatePickMeasure",
	"FreePickMeasure",
	"GetPickMeasure",
	"UpdatePickMeasure", 
	"OpenFont",
	"CloseFont",
	"QueryFont",
	"ListFonts",
	"ListFontsWithInfo",
	"QueryTextExtents"
};

static char *pex_comp_type[] = {
    "PreConcatenate",
    "PostConcatenate",
    "Replace"
};

static char *pex_editing_mode[] = {
    "StructureInsert",
    "StructureReplace",
};

static char *pex_whence[] = {
    "Beginning",
    "Current",
    "End"
};

static char *pex_display_update_mode[] = {
    "",
    "VisualizeEach",
    "VisualizeEasy",
    "VisualizeNone",
    "SimulateSome",
    "VisualizeWhenever"
};

#ifdef __STDC__
#define PEX_DECODE_REQUEST(_request_name,ptr) \
	(void) fprintf( stderr,".... PEX_REQUEST -- %s\n", \
	pex_req_name[PEX_##_request_name]); \
	(void) fprintf(stderr,"request length: %d\n", \
	((pex##_request_name##Req *)ptr)->length)
#else
#define PEX_DECODE_REQUEST(_request_name,ptr) \
	(void) fprintf(stderr,".... PEX_REQUEST -- %s\n", \
	pex_req_name[PEX_/**/_request_name]); \
	(void) fprintf(stderr,"request length: %d\n", \
	((pex/**/_request_name/**/Req *)ptr)->length)
#endif

#ifdef __STDC__
#define PEX_DECODE_REPLY(_reply_name,ptr) \
	(void) fprintf(stderr,"\t..........PEX_REPLY: %s\n", \
	pex_req_name[PEX_##_reply_name]); \
	(void) fprintf(stderr,"\t.............length: %d\n", \
	((pex##_reply_name##Reply *)ptr)->length)
#else
#define PEX_DECODE_REPLY(_reply_name,ptr) \
	(void) fprintf(stderr,"\t..........PEX_REPLY: %s\n", \
	pex_req_name[PEX_/**/_reply_name]); \
	(void) fprintf(stderr,"\t.............length: %d\n", \
	((pex/**/_reply_name/**/Reply *)ptr)->length)
#endif

#define PEX_DECODE_ELEM(_opcode) \
	(void) fprint(stderr,"\t..................... %s\n", \
	pex_oc_name[_opcode])

#define PRINT(format,data) \
(void) fprintf(stderr,format,data)

#define PRINT_STRING_VALUE(FORMAT,STRING,VALUE) \
(void) fprintf(stderr,FORMAT,STRING,VALUE)

#if defined(__STDC__) && !defined(UNIXCPP)
#define CAPTURE_OUTPUT_CMD(name,oc,buf) \
pex/##/name/##/ *oc = (pex/##/name/##/ *) buf
#else
#define CAPTURE_OUTPUT_CMD(name,oc,buf) \
pex/**/name/**/ *oc = (pex/**/name/**/ *) buf
#endif

#define PRINT_MATRIX(_m) \
{ \
	register long i, j; \
	for (i = 0; i <= 3; i++) { \
		(void) fprintf( stderr, "[ %.4f %.4f %.4f %.4f ]\n", \
		_m[0][i],_m[1][i],_m[2][i],_m[3][i]); \
	} \
	(void) fprintf( stderr, "\n"); \
}

#define LocalTransform(_buf) \
{ \
	CAPTURE_OUTPUT_CMD(LocalTransform,oc,buf); \
\
	PRINT("composition type : %s\n", NameOfComposition(oc->compType)); \
	PRINT_MATRIX(oc->matrix); \
}

#endif /* PEXSCOPE_H_INCLUDED */
