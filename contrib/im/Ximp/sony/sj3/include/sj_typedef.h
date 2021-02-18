/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#ifndef	_SJ_TYPEDEF

#define	_SJ_TYPEDEF

#define	RECURS

#define	TFar
#define	Far
#define	Reg1	register
#define	Reg2	register
#define	Reg3	register
#define	Reg4	register
#define	Reg5	register
#define	Reg6	register
#define	Reg7	register
#define	Reg8	register

#define	Void	void
#define	Char	char
#define	Uchar	unsigned char
#define	Short	short
#define	Ushort	unsigned short
#define	Int	int
#define	Uint	unsigned int
#define	Long	long
#define	Ulong	unsigned long

typedef	Void	(*VFunc)();
typedef	Uchar	*(*UCPFunc)();
typedef	Int	(*IFunc)();


#include <sys/types.h>

typedef	Uchar	TypeGroup;
typedef	Uchar	TypeGram;
typedef	Ushort	TypeIdxOfs;
typedef	Short	TypeDicSeg;
typedef	Ushort	TypeDicOfs;
typedef	ino_t	TypeDicID;
typedef	Uchar	TypeCnct;
typedef	Ushort	TypeStyNum;
typedef	Uchar	TypeClass;
typedef	Uchar	TypeBunNum;

#ifndef	NULL
#	define	NULL	0
#endif

#endif

