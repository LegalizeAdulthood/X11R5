/* $Header: sj3ctype.h,v 1.1 91/09/28 10:24:06 nao Locked $ */

#ifndef _sj3ctype_h
#define _sj3ctype_h

#ifdef sony_news
#ifdef NEWS_OS4
#include <stdlib.h>
#else
#include <X11/Xlib.h>
#endif
#else
#include <stdlib.h>
#endif
#if (!defined(sony_news) || BSD < 43)
#ifndef issjis1
#define issjis1(x)  ((0x81<=(x) && (x)<=0x9f) || (0xe0<=(x) && (x)<=0xfc))
#endif
#ifndef issjis2
#define issjis2(x)  ((0x40<=(x) && (x)<=0x7e) || (0x80<=(x) && (x)<=0xfc))
#endif
#ifndef iskanji
#define iskanji(x)  ((0x81<=(x) && (x)<=0x9f) || (0xe0<=(x) && (x)<=0xfc))
#endif
#ifndef iskana
#define iskana(x)   (0xa1<=(x) && (x)<=0xdf)
#endif
#ifndef iskana2
#define iskana2(x)  (0xa1<=(x) && (x)<=0xdf)
#endif
#ifndef iseuc
#define iseuc(x)    (0xa1<=(x) && (x)<=0xfe)
#endif
#ifndef iseuckana
#define iseuckana(x)    ((x)==0x8e)
#endif
#endif  /* !sony_news   */

#endif
