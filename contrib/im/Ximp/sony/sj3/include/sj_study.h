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

#ifndef	_SJ_STUDY

#define	_SJ_STUDY

#define	StudyExist()	(stdy_base)

#define	StudyCount	(stdy_base -> stdycnt)
#define	StudyMax	(stdy_base -> stdymax)
#define	StudyDict	(stdy_base -> stdydic)

#define	ClStudyIdx	(stdy_base -> clstdyidx)
#define	ClStudyStep	(stdy_base -> clstdystep)
#define	ClStudyLen 	(stdy_base -> clstdylen)
#define	ClStudyDict	(stdy_base -> clstdydic)

#define	StudyTail	(StudyDict + StudyMax)
#define	StudyRecSize	(sizeof(STDYIN))

#define	ClStudyTail	(ClStudyDict + ClStudyLen)

#define	StudyNormEnd	0
#define	StudyNoDict	1
#define	StudyFullArea	2
#define	StudyNotStudy	3
#define	StudyError	4


#define	CLSTDY_TMPLEN	256
#define	ClstudyNormEnd	0
#define	ClstudyNoDict	1
#define	ClstudyParaErr	2
#define	ClstudyYomiErr	3
#define	ClstudyNoArea	4

#define	CL_YLEN		0
#define	CL_GRAM2	1
#define	CL_Y1LEN	2
#define	CL_NUMH		3
#define	CL_NUML		4
#define	CL_YDATA	5
#define	CL_HDLEN	5

#define	iseocl(p)	( !ClYomiLen(p) || (Uchar *)(p) >= ClStudyTail )

#define	ClGramCode(p)	*((p) + CL_GRAM2)

#define	ClYomiLen(p)	*((p) + CL_YLEN)

#define	ClYomi1Len(p)	*((p) + CL_Y1LEN)

#define	ClHighNum(p)	*(p + CL_NUMH)
#define	ClLowNum(p)	*(p + CL_NUML)
#define	ClGetNum(p)	(ClHighNum(p) * 0x100 + ClLowNum(p))
#define	ClSetNum(p, n)	(ClHighNum(p)=(Uchar)((n) >> 8), ClLowNum(p)=(Uchar)(n))

#define	ClYomiPos(p)	((p) + CL_YDATA)

#define	ClBlkLen(p)	(ClYomiLen(p) + CL_HDLEN)

#define	ClNextTag(p)	((p) + ClBlkLen(p))

#endif

