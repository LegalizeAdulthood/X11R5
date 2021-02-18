/*
 * $Id: wchar.h,v 1.9 1991/09/11 08:41:10 proj Exp $
 */

/*
 * Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
 *                      and Nippon Telegraph and Telephone Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON, NTT Software, NTT, and M.I.T.
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. OMRON, NTT Software,
 * NTT, and M.I.T. make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * OMRON, NTT SOFTWARE, NTT, AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OMRON, NTT SOFTWARE, NTT, OR M.I.T. BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 *	Author: Li Yuhong	OMRON Corporation
 *   
 */

#ifndef	_WCHAR_H_
#define _WCHAR_H_

#include <ctype.h>

#ifndef WNULL
typedef	unsigned long wchar;	/* must be unsigned 4-byte type, ISO10646 */
#ifndef wchar_t
#define wchar_t wchar
#endif
#ifndef wint_t
#define wint_t wchar
#endif

#define WNULL	0
#define WEOF	0xffffffffL
#endif

/*
 * DIS 10646 definition, but not reasonable in program I think.
#define WNULL	0x00808080
*/

#ifndef MB_LEN_MAX
#define MB_LEN_MAX 6
#endif

/***********************/
/* Copy from "mit/lib/X/Xsi/wcharint.h" */
#ifndef	SCRIPT1
#define SCRIPT1		0xffffff00
#define CONTROLSCRIPT	0x00ffffff
#define LATINSCRIPT	0x20202000
#define HIRAGANASCRIPT	0x20202f00
#define KATAKANASCRIPT	0x20202f80
#define HANZISCRIPT	0x20308080
#define KANJISCRIPT	0x20408080
#define HANJASCRIPT	0x20508080
#define CLPADPADPAD	0x00808080

#define PrivateZone	0x20203400	/* Private Use Zone */
#define PrivateZoneLast 0x20203b00	/* Last row of Private Use Zone */
#define PrivatePlane	0x20e00000	/* Private Use Plane */
#define PrivatePlaneLast 0x20ff0000	/* Last row of Private Use Plane */
#define PrivatePlaneLastI11 (PrivatePlaneLast | 0x8080)
#define PrivatePlaneLastI10 (PrivatePlaneLast | 0x8000)

#define	wgetgroup(wc)	((unsigned char)(((wc) >> 24) & 0x000000ff))
#define	wgetcell(wc)	((unsigned char)((wc) & 0x000000ff))
#endif
/***********************/
/*
 * macro version of wchar functions
 *	Macros here evaluate their argument more than once
 *	Please take care to use these macros.
 *
 * COMMENT:  DEL is 0x7f
 *
 *	List of macros:	_iswalnum(), _iswalpha(), _iswdigit(),
 *			_iswlower(), _iswpunct(), _iswupper(),
 *			_iswxdigit(),
 *			_iswcntrl(), _iswspace(),
 *			_towupper(), _towupper(),
 *			_atowc(), _wctoa()
 *			_iswlatin1()
 *	Function only:	iswascii(), iswblank(), iswgraph(), iswprint()
 */
#define	_iswalnum(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT	\
			 && isascii(wgetcell(wc)) && isalnum(wgetcell(wc)) )
#define	_iswalpha(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT	\
			 && isascii(wgetcell(wc)) && isalpha(wgetcell(wc)) )
#define	_iswdigit(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT	\
			 && isascii(wgetcell(wc)) && isdigit(wgetcell(wc)) )
#define	_iswlower(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT	\
			 && isascii(wgetcell(wc)) && islower(wgetcell(wc)) )
#define	_iswpunct(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT	\
			 && isascii(wgetcell(wc)) && ispunct(wgetcell(wc)) )
#define	_iswupper(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT	\
			 && isascii(wgetcell(wc)) && isupper(wgetcell(wc)) )
#define	_iswxdigit(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT	\
			 && isascii(wgetcell(wc)) && isxdigit(wgetcell(wc)) )
#define	_iswlatin1(wc)	( ((wc)&SCRIPT1)==LATINSCRIPT || _iswcntrl(wc) )

#define	_iswcntrl(wc)	( ((wc)==WNULL) || (((wc)&CONTROLSCRIPT)==CLPADPADPAD \
					  && iscntrl(wgetgroup(wc)&0x7f)) )
#define	_iswspace(wc)	( (((wc)&CONTROLSCRIPT)==CLPADPADPAD) ?	\
			 (isspace(wgetgroup(wc))) :		\
			 (_iswblank(wc)) )
#define	_iswascii(wc)	iswascii(wc)
#define	_iswblank(wc)	iswblank(wc)
#define	_iswgraph(wc)	iswgraph(wc)
#define	_iswprint(wc)	iswprint(wc)

#define	_towlower(wc)	( (_iswupper(wc))?			\
			 (LATINSCRIPT|(tolower(wgetcell(wc)))):	\
			 (wc) )
#define	_towupper(wc)	( (_iswlower(wc))?			\
			 (LATINSCRIPT|(toupper(wgetcell(wc)))):	\
			 (wc) )

#define	_atowc(c)	( ((c)=='\0') ? WNULL:				\
			 (iscntrl((c)&0x7f)) ? (((c)<<24)|CLPADPADPAD):	\
			 (LATINSCRIPT|(c)) )
#define	_wctoa(wc)	( _iswcntrl(wc) ? wgetgroup(wc):		\
			 (((wc)&SCRIPT1)==LATINSCRIPT) ? wgetcell(wc):	\
			 (-1) )


/*
 * Function declarations
 */

#if NeedFunctionPrototypes
extern int	iswalnum(wint_t);
extern int	iswalpha(wint_t);
extern int	iswascii(wint_t);	/* not in MSE */
extern int	iswblank(wint_t);	/* not in MSE */
extern int	iswcntrl(wint_t);
extern int	iswdigit(wint_t);
extern int	iswgraph(wint_t);
extern int	iswlatin1(wint_t);	/* not in MSE */
extern int	iswlower(wint_t);
extern int	iswprint(wint_t);
extern int	iswspace(wint_t);
extern int	iswupper(wint_t);
extern int	iswxdigit(wint_t);
extern wint_t	towlower(wint_t);
extern wint_t	towupper(wint_t);
extern int	wcswidth(const wchar_t, size_t);
extern int	wcwidth(const wchar_t);
#else /* NeedFunctionPrototypes */
extern int	iswalnum();
extern int	iswalpha();
extern int	iswascii();	/* not in MSE */
extern int	iswblank();	/* not in MSE */
extern int	iswcntrl();
extern int	iswdigit();
extern int	iswgraph();
extern int	iswlatin1();	/* not in MSE */
extern int	iswlower();
extern int	iswprint();
extern int	iswspace();
extern int	iswupper();
extern int	iswxdigit();
extern wint_t	towlower();
extern wint_t	towupper();
extern int	wcswidth();
extern int	wcwidth();
#endif /* NeedFunctionPrototypes */

/* 
 * functions similar to char string manupulation.
*/
#if NeedFunctionPrototypes
extern wchar_t	atowc(const unsigned char);	/* not in MSE */
extern char	wctoa(const wchar_t);		/* not in MSE */
extern char	*wstos(char*, const wchar_t*);	/* not in MSE */
extern wchar_t 	*wcscpy(wchar_t*, const wchar_t*);
extern wchar_t 	*wcsncpy(wchar_t*, const wchar_t*, size_t);
extern wchar_t 	*wcscat(wchar_t*, const wchar_t*);
extern wchar_t 	*wcsncat(wchar_t*, const wchar_t*, size_t);
extern int	wcscmp(const wchar_t*, const wchar_t*);
extern int	wcsncmp(const wchar_t*, const wchar_t*, size_t);
extern wchar_t 	*wcschr(const wchar_t*, const wchar_t*);
extern wchar_t 	*wcsrchr(const wchar_t*, const wchar_t*);
extern size_t	wcslen(const wchar_t*);
extern void	wcsreverse(wchar_t*);		/* not in MSE */
#else /* NeedFunctionPrototypes */
extern wchar_t	atowc();		/* convert ASCII to DP 10646 */
extern char	wctoa();		/* convert DP 10464 to ASCII(char) */
extern char	*wstos();		/* convert DP 10646 ASCII to ASCII */
extern wchar_t 	*wcscpy();
extern wchar_t 	*wcsncpy();
extern wchar_t 	*wcscat();
extern wchar_t 	*wcsncat();
extern int	wcscmp();
extern int	wcsncmp();
extern wchar_t 	*wcschr();
extern wchar_t 	*wcsrchr();
extern unsigned long	wcslen();
extern void	wcsreverse();
#endif /* NeedFunctionPrototypes */

#endif /* _WCHAR_H_ */
