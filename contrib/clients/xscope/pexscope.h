/* $XConsortium: pexscope.h,v 5.1 91/02/16 09:31:57 rws Exp $ */

/***********************************************************
Copyright 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

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

#ifdef __STDC__
#define PEX_DECODE_REQUEST(_request_name,ptr) \
	(void) fprintf(stderr,"\t........PEX_REQUEST: %s\n", \
	pex_req_name[PEX_##_request_name]); \
	(void) fprintf(stderr,"\t.............length: %d\n", \
	((pex##_request_name##Req *)ptr)->length)
#else
#define PEX_DECODE_REQUEST(_request_name,ptr) \
	(void) fprintf(stderr,"\t........PEX_REQUEST: %s\n", \
	pex_req_name[PEX_/**/_request_name]); \
	(void) fprintf(stderr,"\t.............length: %d\n", \
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

#endif /* PEXSCOPE_H_INCLUDED */
