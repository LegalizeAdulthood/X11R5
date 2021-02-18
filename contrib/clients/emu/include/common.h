#ifndef _XP_COMMON_H
#define _XP_COMMON_H
/* $Header: /usr3/emu/include/RCS/common.h,v 1.6 90/07/26 02:40:53 jkh Exp Locker: jkh $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Common definitions for emu.
 *
 * Author: Jordan K. Hubbard, Michael Elbel and Terry Jones
 * Date: March 20th, 1990.
 * Description: This file is included by all widgets in the emu system.
 *		OS or site specific changes should not be made here.
 *		Those go in os.h and config.h
 *
 * Revision History:
 *
 * $Log:	common.h,v $
 * Revision 1.6  90/07/26  02:40:53  jkh
 * Added new copyright.
 *
 */

/**************************************************************
 * Various helper macros/typedefs to make the code more clear *
 **************************************************************/

#define Local	static
#define Import	extern
#define Export

#include "os.h"
#include "xt_ops.h"
#include "config.h"

/* Used for various internal buffers */
#define BUF_SZ                1024


/*********************************************************
 * Types and macros for various internal data structures *
 *********************************************************/

/* The number of registers in a common block. (Should be at least 26) */
#define CB_NREGS		128

typedef void (*VoidFuncPtr)();		/* Function returning void */
typedef String (*StrFuncPtr)();		/* Function returning String */
typedef int (*IntFuncPtr)();		/* Function returning int */
typedef XtGeometryResult (*GFuncPtr)();	/* Function returning geometry res */

/* General linked list data structure */
typedef struct _link {
     struct _link *next;
     Generic data;
} *Link;

/* A register */
typedef struct _reg {
     unsigned char type;	/* data type */
     Generic data;		/* data ptr */
} Register;

#define reg_size		(sizeof(struct _reg))
#define reg_type(reg)		((reg).type)
#define reg_data(reg)		((reg).data)

/* A communications block */
typedef struct _comblock {
     int opcode;			/* operation code */
     unsigned char buffer[BUF_SZ];	/* data buffer */
     int nbytes;			/* data buffer size */
     Register regs[CB_NREGS];		/* register array */
} ComBlock, *ComBlockPtr;

/* Macros to access a comblock */
#define cb_size			(sizeof(struct _comblock))
#define cb_opcode(x)		(((ComBlockPtr)(x))->opcode)
#define cb_buffer(x)		(((ComBlockPtr)(x))->buffer)
#define cb_nbytes(x)		(((ComBlockPtr)(x))->nbytes)
#define cb_regs(x)		(((ComBlockPtr)(x))->regs)
#define cb_reg(x, r)		(cb_regs(x)[(r)])

/* shorthand */
#define cb_reg_type(x, r)	(reg_type(cb_reg((x), (r))))
#define cb_reg_data(x, r)	(reg_data(cb_reg((x), (r))))

/* Global resources */
typedef struct _res {
     String command;		/* command to execute */
     String termtype;		/* terminal type we're emulating */
} Resources;

/* A list of parsers */
typedef struct _parsertab {
     String name;		/* terminal name */
     VoidFuncPtr in_parser;	/* hard-coded parser routine */
     VoidFuncPtr out_parser;	/* optional output sink */
} ParserTable;

/**********************************************
 * External function and variable definitions *
 **********************************************/

Import int Debug;
Import Resources GlobalRes;

/* Misc routines */
Import void fatal(), warn(), pmessage(), debug();
Import String basename(), backslash_convert();
Import Generic malloc_and_clear();
Import String get_sub_resource_string(), reg_type_name();

#endif /* _XP_COMMON_H */
