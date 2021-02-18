/* $Header: Xpexlibint.h,v 2.5 91/09/11 16:06:26 sinyaw Exp $ */
/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#ifndef XPEXLIBINT_H
#define XPEXLIBINT_H

#define NEED_REPLIES

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include "PEX.h"
#include "PEXprotost.h"
#include "PEXproto.h"

#define PADDING(n) ( ((n)%4 == 0) ? 0 : 4 - ((n)%4) )

#define Xpex_sizeof_color(_t) \
(_t == (PEXIndexedColour) || (PEXRgb8Colour) ? 4 : \
((_t == PEXRgb16Colour) ? 4 : 3 * sizeof(XpexFloat)))

#define Xpex_sizeof_coord(_t) \
(_t == Xpex_Rational ? sizeof(pexCoord4D) : sizeof(pexCoord3D))

#ifndef MAX
#define MAX( a, b)  (((a) > (b)) ? (a) : (b))
#endif

#ifdef defined(SYSV) || defined(SVR4)
extern char *malloc();
#endif

#if defined(__STDC__) && !defined(UNIXCPP)
#define XpexGetReq(name,dpy,_i,req) \
	if (((dpy)->bufptr + sizeof(pex##name##Req)) > (dpy)->bufmax)\
		_XFlush(dpy);\
	(req) = (pex##name##Req *)((dpy)->last_req = (dpy)->bufptr);\
	(req)->reqType = (_i)->codes->major_opcode;\
	(req)->opcode = PEX_##name;\
	(req)->length = (sizeof(pex##name##Req))>>2;\
	(dpy)->bufptr += sizeof(pex##name##Req);\
	(dpy)->request++
#else
#define XpexGetReq(name,dpy,_i,req) \
	if (((dpy)->bufptr + sizeof(pex/**/name/**/Req)) > (dpy)->bufmax)\
		_XFlush(dpy);\
	(req) = (pex/**/name/**/Req *)((dpy)->last_req = (dpy)->bufptr);\
	(req)->reqType = (_i)->codes->major_opcode;\
	(req)->opcode = PEX_/**/name;\
	(req)->length = (sizeof(pex/**/name/**/Req))>>2;\
	(dpy)->bufptr += sizeof(pex/**/name/**/Req);\
	(dpy)->request++
#endif

#if defined(__STDC__) && !defined(UNIXCPP)
#define XpexGetReqExtra(name,dpy,_i,n,req) \
	if ((dpy->bufptr + sizeof(pex/**/name/**/Req) + n) > dpy->bufmax)\
		_XFlush(dpy); \
	req = (pex##name##Req *)(dpy->last_req = dpy->bufptr);\
	req->reqType = (_i)->codes->major_opcode;\
	req->opcode = PEX_##name;\
	req->length = (sizeof(pex##name##Req) + n)>>2;\
	dpy->bufptr += sizeof(pex##name##Req) + n;\
	dpy->request++
#else
#define XpexGetReqExtra(name,dpy,_i,n,req) \
	if ((dpy->bufptr + sizeof(pex/**/name/**/Req) + n) > dpy->bufmax)\
		_XFlush(dpy); \
	req = (pex/**/name/**/Req *)(dpy->last_req = dpy->bufptr);\
	req->reqType = (_i)->codes->major_opcode;\
	req->opcode = PEX_/**/name;\
	req->length = (sizeof(pex/**/name/**/Req) + n)>>2;\
	dpy->bufptr += sizeof(pex/**/name/**/Req) + n;\
	dpy->request++
#endif

#if defined(__STDC__) && !defined(UNIXCPP)
#define XpexGetResReq(name, dpy, _i, rid, req) \
	if ((dpy->bufptr + sizeof(pexResourceReq)) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (pexResourceReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = (_i)->codes->major_opcode;\
	req->opcode = PEX_##name;\
	req->length = 2;\
	req->id = (rid);\
	dpy->bufptr += sizeof(pexResourceReq);\
	dpy->request++
#else
#define XpexGetResReq(name, dpy, _i, rid, req) \
	if ((dpy->bufptr + sizeof(pexResourceReq)) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (pexResourceReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = (_i)->codes->major_opcode;\
	req->opcode = PEX_/**/name;\
	req->length = 2;\
	req->id = (rid);\
	dpy->bufptr += sizeof(pexResourceReq);\
	dpy->request++
#endif

/*
 * GetEmptyPexReq is for those requests that have no arguments
 * at all.
 */

#if defined(__STDC__) && !defined(UNIXCPP)
#define XpexGetEmptyReq(name, dpy, _i, req) \
	if ((dpy->bufptr + sizeof(pexReq)) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (pexReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = (_i)->codes->major_opcode;\
	req->opcode = PEX_##name;\
	req->length = 1;\
	dpy->bufptr += sizeof(pexReq);\
	dpy->request++
#else
#define XpexGetEmptyReq(name, dpy, _i, req) \
	if ((dpy->bufptr + sizeof(pexReq)) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (pexReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = (_i)->codes->major_opcode;\
	req->opcode = PEX_/**/name;\
	req->length = 1;\
	dpy->bufptr += sizeof(pexReq);\
	dpy->request++
#endif

#define JustFlushed(dpy) \
((((xReq *)(dpy->last_req))->reqType == 0) && \
(((xReq *)(dpy->last_req))->data == 0) && \
(((xReq *)(dpy->last_req))->length == 0))

/*
 * write a fixed length output command to the transport buffer. 
 */
#if defined(__STDC__) && !defined(UNIXCPP)
#define XpexGetOutputCmd(name,_dpy,_i,_oc) \
{ \
pexOutputCommandsReq *req; \
if (((_dpy)->bufptr + sizeof(pex##name)) > (_dpy)->bufmax) { \
_XFlush((_dpy)); \
} \
if (JustFlushed(dpy)) { \
req = (pexOutputCommandsReq *)(((_dpy)->last_req) = (_dpy)->bufptr); \
req->reqType = (_i)->codes->major_opcode; \
req->opcode = ((XpexDisplayInfo *)((_i)->data))->oc_target_type; \
req->length = (sizeof(pexOutputCommandsReq))>>2; \
req->fpFormat = XpexGetFloatingPointFormat((_dpy)); \
req->id = ((XpexDisplayInfo *)((_i)->data))->oc_target_res_id; \
req->numCommands = 0; \
(_dpy)->bufptr += sizeof(pexOutputCommandsReq); \
(_dpy)->request++; \
} /* end-if */ \
(_oc) = (pex##name *)((_dpy)->bufptr); \
(_oc)->head.elementType = (CARD16) PEXOC##name; \
(_oc)->head.length = (CARD16)(sizeof(pex##name)>>2); \
(_dpy)->bufptr += sizeof(pex##name); \
req = (pexOutputCommandsReq *)((_dpy)->last_req); \
req->length += oc->head.length; \
req->numCommands++; \
} 
#else
#define XpexGetOutputCmd(name,_dpy,_i,_oc) \
{ \
pexOutputCommandsReq *req; \
if (((_dpy)->bufptr + sizeof(pex/**/name)) > (_dpy)->bufmax) { \
_XFlush((_dpy)); \
} \
if (JustFlushed(dpy)) { \
req = (pexOutputCommandsReq *)(((_dpy)->last_req) = (_dpy)->bufptr); \
req->reqType = (_i)->codes->major_opcode; \
req->opcode = ((XpexDisplayInfo *)((_i)->data))->oc_target_type; \
req->length = (sizeof(pexOutputCommandsReq))>>2; \
req->fpFormat = XpexGetFloatingPointFormat((_dpy)); \
req->id = ((XpexDisplayInfo *)((_i)->data))->oc_target_res_id; \
req->numCommands = 0; \
(_dpy)->bufptr += sizeof(pexOutputCommandsReq); \
(_dpy)->request++; \
} /* end-if */ \
(_oc) = (pex/**/name *)((_dpy)->bufptr); \
(_oc)->head.elementType = (CARD16) PEXOC/**/name; \
(_oc)->head.length = (CARD16)(sizeof(pex/**/name)>>2); \
(_dpy)->bufptr += sizeof(pex/**/name); \
req = (pexOutputCommandsReq *)((_dpy)->last_req); \
req->length += oc->head.length; \
req->numCommands++; \
} 
#endif

/*
 * write a variable length output command to the transport buffer. 
 */

#if defined(__STDC__) && !defined(UNIXCPP)
#define XpexGetOutputCmdExtra(name,_dpy,_i,_n,_oc) \
{ \
pexOutputCommandsReq *req; \
if ((_dpy->bufptr + sizeof(pex##name) + _n) > _dpy->bufmax) { \
_XFlush(_dpy); \
} \
if (JustFlushed(dpy)) { \
req = (pexOutputCommandsReq *)((_dpy->last_req) = _dpy->bufptr); \
req->reqType = _i->codes->major_opcode; \
req->opcode = ((XpexDisplayInfo *)(_i)->data)->oc_target_type; \
req->length = (sizeof(pexOutputCommandsReq))>>2; \
req->fpFormat = XpexGetFloatingPointFormat(_dpy); \
req->id = ((XpexDisplayInfo *)(_i->data))->oc_target_res_id; \
req->numCommands = 0; \
_dpy->bufptr += sizeof(pexOutputCommandsReq); \
_dpy->request++; \
} /* end-if */ \
_oc = (pex##name *)(_dpy->bufptr); \
_oc->head.elementType = (CARD16) PEXOC##name; \
_oc->head.length = (CARD16)((sizeof(pex##name) + _n)>>2); \
_dpy->bufptr += sizeof(pex##name) + _n; \
req = (pexOutputCommandsReq *)(_dpy->last_req); \
req->length += oc->head.length; \
req->numCommands++; \
} 
#else
#define XpexGetOutputCmdExtra(name,_dpy,_i,_n,_oc) \
{ \
pexOutputCommandsReq *req; \
if ((_dpy->bufptr + sizeof(pex/**/name) + _n) > _dpy->bufmax) { \
_XFlush(_dpy); \
} \
if (JustFlushed(dpy)) { \
req = (pexOutputCommandsReq *)((_dpy->last_req) = _dpy->bufptr); \
req->reqType = _i->codes->major_opcode; \
req->opcode = ((XpexDisplayInfo *)(_i)->data)->oc_target_type; \
req->length = (sizeof(pexOutputCommandsReq))>>2; \
req->fpFormat = XpexGetFloatingPointFormat(_dpy); \
req->id = ((XpexDisplayInfo *)(_i->data))->oc_target_res_id; \
req->numCommands = 0; \
_dpy->bufptr += sizeof(pexOutputCommandsReq); \
_dpy->request++; \
} /* end-if */ \
_oc = (pex/**/name *)(_dpy->bufptr); \
_oc->head.elementType = (CARD16) PEXOC/**/name; \
_oc->head.length = (CARD16)((sizeof(pex/**/name) + _n)>>2); \
_dpy->bufptr += sizeof(pex/**/name) + _n; \
req = (pexOutputCommandsReq *)(_dpy->last_req); \
req->length += oc->head.length; \
req->numCommands++; \
} 
#endif

#define XpexGetOutputCmdsReq(_d,_i,_r) \
if (((_d)->bufptr + sizeof(pexOutputCommandsReq)) > (_d)->bufmax) { \
_XFlush(_d); \
} \
(_r) = (pexOutputCommandsReq *)((_d)->last_req = (_d)->bufptr); \
(_r)->reqType = (_i)->codes->major_opcode; \
(_r)->opcode = ((XpexDisplayInfo *)((_i)->data))->oc_target_type; \
(_r)->length = (sizeof(pexOutputCommandsReq))>>2; \
(_r)->fpFormat = XpexGetFloatingPointFormat((_d)); \
(_r)->id = ((XpexDisplayInfo *)((_i)->data))->oc_target_res_id; \
(_r)->numCommands = 0; \
(_d)->bufptr += sizeof(pexOutputCommandsReq); \
(_d)->request++
#endif /* XPEXLIBINT_H */
