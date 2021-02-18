/* $Header: sj3def.h,v 1.1 91/09/28 10:24:07 nao Locked $ */

/*
 * Author: Naoshi Suzuki, SONY Corporation.  (nao@sm.sony.co.jp)
 */

#ifndef _sj3def_h
#define _sj3def_h

/* #define NEWS_OS4 */
#define X11R5
#undef FORCE_SJIS
        /* If setlocale() returns null and use Shift-JIS code define this */

/*
 * Kana-Kanji conversion control key event define
 */
#define KEY_NULL                (0L)
#define KEY_HENKAN_START        (1L<<0)
#define KEY_HENKAN_END          (1L<<1)
#define KEY_MODE_CHANGE         (1L<<2)
#define KEY_TEXT_CHANGE         (1L<<4)
#define KEY_TEXT_NO_CHANGE      (1L<<5)
#define KEY_TEXT_FIXED          (1L<<6)
#define KEY_TEXT_FLUSH          (1L<<7)
#define KEY_SELECT_START        (1L<<8)
#define KEY_SELECT_END          (1L<<9)
#define KEY_SELECT_ABORT        (1L<<10)
#define KEY_SELECT_SYMBOL       (1L<<11)
#define KEY_SELECT_RIGHT        (1L<<12)
#define KEY_SELECT_LEFT         (1L<<13)
#define KEY_SELECT_UP           (1L<<14)
#define KEY_SELECT_DOWN         (1L<<15)
#define KEY_SELECT_FIRST        (1L<<16)
#define KEY_SELECT_LAST         (1L<<17)
#define KEY_SELECT_NEXTP        (1L<<18)
#define KEY_SELECT_PREVP        (1L<<19)
#define KEY_SELECT_RIGHTMOST    (1L<<20)
#define KEY_SELECT_LEFTMOST     (1L<<21)
#define KEY_RECONNECT           (1L<<22)
#define KEY_BELL                (1L<<23)
#define KEY_SYMBOL_START        (1L<<24)

#define KEY_TEXT_CLEAR  (KEY_TEXT_CHANGE|KEY_TEXT_FIXED)
#define KEY_CHANGE      (KEY_HENKAN_END|KEY_MODE_CHANGE \
                |KEY_TEXT_CHANGE|KEY_TEXT_FIXED)
#define KEY_SELECT      (KEY_SELECT_START|KEY_SELECT_END|KEY_SELECT_ABORT \
                |KEY_SELECT_RIGHT|KEY_SELECT_LEFT|KEY_SELECT_UP \
                |KEY_SELECT_DOWN|KEY_SELECT_FIRST|KEY_SELECT_LAST \
                |KEY_SELECT_NEXTP|KEY_SELECT_PREVP|KEY_SELECT_LEFTMOST \
                |KEY_SELECT_RIGHTMOST|KEY_SYMBOL_START)

/*
 * define for server status
 */
#define CONNECT_OK          0
#define CONNECT_FAIL        -1

/*
 * define for user's internal code
 */
#define USR_SJIS            0
#define USR_EUC             1
#define USR_OTHER           -1

/*
 * define for buffer size
 */
#define YBUFSIZ             32
#define RBUFSIZ             16
#define ROMABUFSIZ          512
#define KANABUFSIZ          512
#define KANJIBUFSIZ         512
#define BUNBUFSIZ           128

#if defined(NEWS_OS4) && defined(sony_news)
#define USE_RKCONV  /* Use rkconv() function in libsj3lib.a in NEWS-OS 4.X */
#define USE_HANTOZEN /* There are zentohan()/hantozen() function
                in libsj3lib.a in NEWS-OS 4.X   */
#endif

#ifdef sony_news
#define LWCHAR      /* NEWS-OS 4.1 wchar_t is unsigned long */
#endif

#ifdef X11R5
#undef USE_RKCONV   /* There is no rkconv() function in libsj3lib.a
                        in X11R5 contrib    */
#undef USE_HANTOZEN
        /* There is no hankaku-zenkaku conversion function in libsj3lib.a
            in X11R5 contrib    */
#endif

#ifndef USE_HANTOZEN
#undef USE_RKCONV
#endif

/*
 * define for default conversion definition file
 *          and resource file
 */
#if defined(NEWS_OS4) && defined(USE_RKCONV) && defined(sony_news)
#define SJ3DEFPATH  "/usr/sony/lib/sj3/"
#else
#ifndef SJ3DEFPATH
#define SJ3DEFPATH  "/usr/lib/X11/sj3def/"
#endif
#endif

#define DEF_SJRC_FILE       "sjrc"
#define DEF_SJRK_FILE       "sjrk"
#define DEF_SJHK_FILE       "sjhk"
#define DEF_SJZH_FILE       "sjzh"
#define DEF_SJSB_FILE       "sjsb"

typedef unsigned long   SJ3_EVENT;

#define CONV_FAILED     -1
#define CONV_UNFIXED    0
#define CONV_FIXED      1


#define iskata(c) (c >= 0x8340 && c <= 0x8396 && c != 0x837f)
#define ishira(c) (c >= 0x829f && c <= 0x82f1)
#define iszalpha(c) (c >= 0x824f && c <= 0x829a)

#ifndef USE_RKCONV
typedef struct sj3rktable {
    unsigned char           *roma;
    unsigned char           *yomi;
    unsigned char           *str;
    int                     rlen;
    struct sj3rktable       *next;
} SJ3_RKTB;
#endif /* USE_RKCONV */

#ifndef USE_HANTOZEN
typedef struct sj3hktable {
    unsigned char           *hira;
    unsigned char           *zkata;
    unsigned char           *hkata;
    int                     hlen;
    struct sj3hktable       *next;
} SJ3_HKTB;

typedef struct sj3zhtable {
    unsigned char           *halpha;
    unsigned char           *zalpha;
    unsigned char           *zkana;
    unsigned char           *hkata;
    struct sj3zhtable   *next;
} SJ3_ZHTB;

#endif /* USE_HANTOZEN */

#endif
