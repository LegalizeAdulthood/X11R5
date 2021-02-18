#ifndef lint
static char *rcsid = "$Header: sj3conv.c,v 1.2 91/10/01 18:28:17 nao Locked $";
#endif
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
 */
/*
 * Author: Naoshi Suzuki, SONY Corporation.  (nao@sm.sony.co.jp)
 */

#include <X11/IntrinsicP.h>
#include "WStr.h"
#include "sj3def.h"
#ifdef NEWS_OS4
#include    <sj3lib.h>
#else
#include    "sj3lib.h"
#endif
#include <stdio.h>
#include <ctype.h>
#include "sj3ctype.h"

#define OPEN_FAILED     1
#define READ_FAILED     -1
#define ALLOC_FAILED    -2
#define OK              0

#define SKIP(p)         { while (*p == '\t' || *p == ' ') p++; }
#define CHECK_END(p)    { if (*p == '\n' || *p == '#') return (READ_FAILED); }

typedef struct _sj3Buf      *sj3Buf;
static unsigned char        *chnowp = NULL,     *chmaxp = NULL;

#ifndef USE_RKCONV
static SJ3_RKTB             *rknowtp = NULL,    *rkmaxtp = NULL;
#endif /* USE_RKCONV */

#ifndef USE_HANTOZEN
static SJ3_HKTB             *hknowtp = NULL,    *hkmaxtp = NULL;
static SJ3_ZHTB             *zhnowtp = NULL,    *zhmaxtp = NULL;
#endif /* USE_HANTOZEN */

unsigned char *
sj3_store_uchar(ch, len)
    unsigned char   *ch;
    int             len;
{
    register unsigned char  *chp;

    if (chnowp == NULL || chnowp + len > chmaxp) {
        chp = (unsigned char *)malloc(BUFSIZ);
        if (chp == NULL)
            return (NULL);
        chnowp = chp;
        chmaxp = chnowp + (BUFSIZ/sizeof(unsigned char));
        strcpy (chp, ch);
        chnowp += len;
        return (chp);
    } else {
        chp = chnowp;
        strcpy (chp, ch);
        chnowp += len;
        return (chp);
    }
}

static int
readsjis(src, dest)
    register unsigned char  *src,   *dest;
{
    register unsigned char  c;
    register int            i = 0;

    while (*src != '\t' && *src != ' ' && *src != '\n' && *src != '#') {
        c = *src++;
        if (c == '\\') {
            c = *src++;
            i++;
        }
        if (issjis1(c) && issjis2(*src)) {
            *dest++ = c;
            *dest++ = *src++;
            i++;
        } else {
            *dest++ = c;
        }
        i++;
    }
    *dest = '\0';
    return (i);
}

static int
readeuc(src, dest)
    register unsigned char  *src,   *dest;
{
    register unsigned char  c;
    register unsigned short s;
    register int            i = 0;

    while (*src != '\t' && *src != ' ' && *src != '\n' && *src != '#') {
        c = *src++;
        if (c == '\\') {
            c = *src++;
            i++;
        }
        if (iseuc(c) && iseuc(*src)) {
            s = (c << 8) + (*src++ & 0x7f);
            s = euc2sjis(s);
            *dest++ = (s >> 8) & 0xff;
            *dest++ = s & 0xff;
            i++;
        } else if (iseuckana(c) && iskana2(*src)) {
            *dest++ = *src++;
            i++;
        } else {
            *dest++ = c;
        }
        i++;
    }
    *dest = '\0';
    return (i);
}

static int
readascii(src, dest)
    register unsigned char  *src,   *dest;
{
    register int            i = 0;

    while (*src != '\t' && *src != ' ' && *src != '\n' && *src != '#') {
        if (*src == '\\') {
            src++;
            i++;
            *dest++ = *src++;
        } else
            *dest++ = *src++;
        i++;
    }
    *dest = '\0';
    return (i);
}

/*
 *  sj3_readsb()
 *   Read sjsb file that symbol table file.
 *  Make symbol table and return it.
 */
unsigned char **
sj3_readsb(sbnum, file, code)
    int         *sbnum;
    char        *file;
    int         code;
{
    FILE                    *fp;
    unsigned char           line[256];
    unsigned char           *p;
    unsigned char           **sbtable;
    unsigned char           data[RBUFSIZ];
    register int            i,  j;

    if ((fp = fopen(file, "r")) == NULL) {
        return (NULL);
    }

    i = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] != '#' && line[0] != '\n')
            i++;
    }
    
    if ((sbtable = (unsigned char **)
            calloc(i, sizeof(unsigned char *))) == NULL) {
        return (NULL);
    }
    *sbnum = i;

    rewind(fp);
    
    j = 0;
    while (fgets(line, sizeof(line), fp) != NULL && j < i) {
        p = line;
        while (*p != '\n' && *p != '#') {
            if (code == USR_SJIS ) {
                p += readsjis(p, data);
            } else {
                p += readeuc(p, data);
            }
            SKIP(p);
            if (data[0] == '\0')
                break;
            sbtable[j] = sj3_store_uchar(data, strlen(data) + 1);
            if (!sbtable[j])
                return (NULL);
            j++;
        }
    }
    fclose(fp);
    return (sbtable);
}

#ifndef USE_RKCONV
#define RKTBMAX (BUFSIZ/sizeof(SJ3_RKTB))

SJ3_RKTB *
sj3_alloc_rktable()
{
    register SJ3_RKTB   *rktp;

    if (rknowtp == NULL || rknowtp > rkmaxtp) {
        rktp = (SJ3_RKTB *)malloc(sizeof(SJ3_RKTB) * RKTBMAX);
        if (rktp == NULL)
            return (NULL);
        rknowtp = rktp;
        rkmaxtp = rknowtp + RKTBMAX - 1;
        rknowtp++;
        return (rktp);
    } else {
        rktp = rknowtp;
        rknowtp++;
        return (rktp);
    }
}

/*
 *  sj3_readrk()
 *   Read sjrk file like a format sj3's roma-kana conversion file,
 *  and make conversion table.
 *   But this routine distinguishes upper-case and lower-case.
 *  It's a big difference.
 */
int
sj3_readrk(rktable, file, code)
    SJ3_RKTB   *rktable;
    String      file;
    int         code;
{
    FILE                    *fp;
    unsigned char           line[256];
    unsigned char           *p;
    unsigned char           roma[RBUFSIZ],  yomi[YBUFSIZ],  str[RBUFSIZ];
    int                     begin = 0;
    SJ3_RKTB                *rktp,  *rktq,  *rktr;

    if ((fp = fopen(file, "r")) == NULL) {
        return (OPEN_FAILED);
    }
    
    rktp = rktable;
    while (fgets(line, sizeof(line), fp) != NULL) {
        p = line;
        while (*p != '\n' && *p != '#') {
            p += readascii(p, roma);
            CHECK_END(p);
            SKIP(p);
            if (code == USR_SJIS ) {
                p += readsjis(p, yomi);
            } else {
                p += readeuc(p, yomi);
            }
            SKIP(p);
            p += readascii(p, str);
            if (roma[0] == '\0' || yomi[0] == '\0')
                break;
            if (begin++) {
                rktr = sj3_alloc_rktable();
                if (!rktr) {
                    sj3_warning("can't allocate roma-kana conversion table 1");
                    return (ALLOC_FAILED);
                }
                rktp = rktq->next = rktr;
            }
            rktp->roma = sj3_store_uchar(roma, strlen(roma) + 1);
            rktp->yomi = sj3_store_uchar(yomi, strlen(yomi) + 1);
            rktp->str = sj3_store_uchar(str, strlen(str) + 1);
            if (!rktp->roma || !rktp->yomi || !rktp->str) {
                sj3_warning("can't allocate roma-kana conversion table 2");
                return(ALLOC_FAILED);
            }
            rktp->rlen = strlen(roma);
            rktq = rktp;
        }
    }
    rktp->next = NULL;
    fclose(fp);
    return (OK);
}

#define MATCH           0
#define NOT             1

/*
 *  sj3_romatozkana()
 *   Get roman characters from the second argument and put back hiragana or
 *  katakana characters to the third argument in conversion table.
 *   If roman characters (third argument) mach with the "roma" entry in table, 
 *  put back the "str" entry to the third argument and the "yomi" entry
 *  to the third argument, then it returns number of converted roman
 *  characters.
 *   If not but any letters of roman characters is match with the "roma"
 *  entry in table and the number of roman characters is less than
 *  the number of the letters of "roma" entry, put back the same to
 *  the third argument and nothing to the third argument, then it returns
 *  zero.
 *   Then last (not converted case) it returns minus 1.
 *   Any arguments must be null('\0') terminated.
 *   First argument is pointer to conversion table;
 */ 
int
sj3_romatozkana(rktable, roma, yomi)
    SJ3_RKTB                    *rktable;
    register unsigned char      *roma;
    register unsigned char      *yomi;
{
    SJ3_RKTB                *rktp;
    register unsigned char  *p,     *q;
    unsigned char           tmp[RBUFSIZ];
    int                     match, result = CONV_FAILED;                    
    register int            len;

    if (!roma || (len = strlen(roma)) == 0)
        return (CONV_FAILED);
    for (rktp = rktable; rktp != NULL; rktp = rktp->next) {
        if (!rktp->roma) {
            continue;
        }
        p = roma;
        q = rktp->roma;
        if (len > rktp->rlen) {
            if (*q++ == *p++) {
                match = MATCH;
                while (*q != '\0') {
                    if (*q != *p) {
                        match = NOT;
                        break;
                    }
                    q++;
                    p++;
                }
                if (match == NOT) {
                    continue;
                }
                if (result < 0) {
                    result = rktp->rlen;
                    strcpy(yomi, rktp->yomi);
                    strcpy(tmp, p);
                } else {
                    continue;
                }
            } else {
                continue;
            }
        } else {
            if (*p++ == *q++) {
                match = MATCH;
                while (*p != '\0') {
                    if (*p++ != *q++) {
                        match = NOT;
                        break;
                    }
                }
                if (match == NOT) {
                    continue;
                }
                if (*q != '\0') {
                    result = CONV_UNFIXED;
                    continue;
                } else if (result == CONV_UNFIXED) {
                    continue;
                } else {
                    result = rktp->rlen;
                    strcpy(yomi, rktp->yomi);
                    strcpy(tmp, rktp->str);
                }
            } else {
                continue;
            }
        }
    }
    if (result > 0)
        strcpy(roma, tmp);
    return (result);
}

#ifndef USE_HANTOZEN
void
sj3_zkanatohalpha(rktable, hktable, zhtable, roma, kana)
    SJ3_RKTB                *rktable;
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *roma;
    register unsigned char  *kana;
{
    register unsigned char  c,  *p, *q;
    register unsigned short s;
    register SJ3_RKTB       *rktp;
    register SJ3_HKTB       *hktp;
    register SJ3_ZHTB       *zhtp;
    register int            ylen,   len,    plen;
    unsigned char           tyomi[KANABUFSIZ];
    unsigned char           tmp[RBUFSIZ];

    p = tyomi;
    while ((c = *kana++) != '\0') {
        if (issjis1(c) && issjis2(*kana)) {
            s = (c << 8) + *kana;
            if (iskata(s)) {
                for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                    if (c == *hktp->zkata)
                        if (*kana == *(hktp->zkata + 1))
                            break;
                }
                if (hktp != NULL) {
                    *p++ = *hktp->hira;
                    *p++ = *(hktp->hira + 1);
                } else {
                    *p++ = c;
                    *p++ = *kana;
                    sj3_warning("wrong sjhk table");
                }

            } else if (!ishira(s)) {
                for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                    if (c == *zhtp->zkana) 
                        if (*kana == *(zhtp->zkana + 1))
                            break;
                }
                if (zhtp != NULL) {
                    *p++ = *zhtp->halpha;
                } else {
                    for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                        if (c == *zhtp->zalpha) {
                            if (*kana == *(zhtp->zalpha + 1))
                                break;
                        }
                    }
                    if (zhtp != NULL) {
                        *p++ = *zhtp->halpha;
                    } else {
                        sj3_warning("can't convert to halpha %#x",s);
                        *p++ = c;
                        *p++ = *kana;
                    }
                }
            } else {
                *p++ = c;
                *p++ = *kana;
            }
            kana++;
        } else {
            *p++ = c;
        }
    }
    *p = '\0';
    p = tyomi;
    plen = strlen(tyomi);

    while ((c = *p) != '\0') {
        if (issjis1(c) && issjis2(*(p + 1))) {
            q = tmp;
            *q = '\0';
            ylen = 0;
            for (rktp = rktable; rktp->next != NULL; rktp = rktp->next) {
                if (ylen < (len = strlen(rktp->yomi))) {
                    if (plen >= len && !strncmp(rktp->yomi, p, len)) {
                        ylen = len;
                        strcpy(tmp, rktp->roma);
                    }
                }
            }
            if (ylen > 0) {
                plen -= ylen;
                p += ylen;
                while (*q != '\0') 
                    *roma++ = *q++;
            } else {
                *roma++ = *p++;
                *roma++ = *p++;
                plen -= 2;
            }
        } else {
            *roma++ = *p++;
            plen--;
        }
    }
    *roma = '\0';
}

#endif /* USE_HANTOZEN */

#endif /* USE_RKCONV */

#ifndef USE_HANTOZEN

#define HKTBMAX (BUFSIZ/sizeof(SJ3_HKTB))

SJ3_HKTB *
sj3_alloc_hktable()
{
    register SJ3_HKTB   *hktp;

    if (hknowtp == NULL || hknowtp > hkmaxtp) {
        hktp = (SJ3_HKTB *)malloc(sizeof(SJ3_HKTB) * HKTBMAX);
        if (hktp == NULL)
            return (NULL);
        hknowtp = hktp;
        hkmaxtp = hknowtp + HKTBMAX - 1;
        hknowtp++;
        return (hktp);
    } else {
        hktp = hknowtp;
        hknowtp++;
        return (hktp);
    }
}

#define ZHTBMAX (BUFSIZ/sizeof(SJ3_ZHTB))

SJ3_ZHTB *
sj3_alloc_zhtable()
{
    register SJ3_ZHTB   *zhtp;

    if (zhnowtp == NULL || zhnowtp > zhmaxtp) {
        zhtp = (SJ3_ZHTB *)malloc(sizeof(SJ3_ZHTB) * ZHTBMAX);
        if (zhtp == NULL)
            return (NULL);
        zhnowtp = zhtp;
        zhmaxtp = zhnowtp + ZHTBMAX - 1;
        zhnowtp++;
        return (zhtp);
    } else {
        zhtp = zhnowtp;
        zhnowtp++;
        return (zhtp);
    }
}

int
sj3_readhk(hktable, file, code)
    SJ3_HKTB   *hktable;
    String      file;
    int         code;
{
    FILE                    *fp;
    unsigned char           line[256];
    unsigned char           *p;
    unsigned char           hira[RBUFSIZ],  zkata[YBUFSIZ], hkata[RBUFSIZ];
    int                     begin = 0;
    SJ3_HKTB                *hktp,  *hktq,  *hktr;

    if ((fp = fopen(file, "r")) == NULL) {
        return (OPEN_FAILED);
    }
    
    hktp = hktable;
    while (fgets(line, sizeof(line), fp) != NULL) {
        p = line;
        while (*p != '\n' && *p != '#') {
            if (code == USR_SJIS ) {
                p += readsjis(p, hira);
                CHECK_END(p);
                SKIP(p);
                p += readsjis(p, zkata);
                CHECK_END(p);
                SKIP(p);
                p += readsjis(p, hkata);
            } else {
                p += readeuc(p, hira);
                CHECK_END(p);
                SKIP(p);
                p += readeuc(p, zkata);
                CHECK_END(p);
                SKIP(p);
                p += readeuc(p, hkata);
            }
            if (hira[0] == '\0' || zkata[0] == '\0' || hkata[0] == '\0')
                break;
            if (begin++) {
                hktr = sj3_alloc_hktable();
                if (!hktr) {
                    sj3_warning("can't allocate hira-kana conversion table 1");
                    return (ALLOC_FAILED);
                }
                hktp = hktq->next = hktr;
            }
            hktp->hira = sj3_store_uchar(hira, strlen(hira) + 1);
            hktp->zkata = sj3_store_uchar(zkata, strlen(zkata) + 1);
            hktp->hkata = sj3_store_uchar(hkata, strlen(hkata) + 1);
            if (!hktp->hira || !hktp->zkata || !hktp->hkata) {
                sj3_warning("can't allocate hira-kana conversion table 2");
                return(ALLOC_FAILED);
            }
            hktp->hlen = strlen(hkata);
            hktq = hktp;
        }
    }
    hktp->next = NULL;
    fclose(fp);
    return (OK);
}

int
sj3_readzh(zhtable, file, code)
    SJ3_ZHTB   *zhtable;
    String      file;
    int         code;
{
    FILE            *fp;
    unsigned char   line[256];
    unsigned char   *p;
    unsigned char   halpha[RBUFSIZ],    zalpha[YBUFSIZ];
    unsigned char   zkana[RBUFSIZ],     hkata[RBUFSIZ];
    int             begin = 0;
    SJ3_ZHTB        *zhtp,  *zhtq,  *zhtr;

    if ((fp = fopen(file, "r")) == NULL) {
        return (OPEN_FAILED);
    }
    
    zhtp = zhtable;
    while (fgets(line, sizeof(line), fp) != NULL) {
        p = line;
        while (*p != '\n' && *p != '#') {
            p += readascii(p, halpha);
            CHECK_END(p);
            SKIP(p);
            if (code == USR_SJIS ) {
                p += readsjis(p, zalpha);
                CHECK_END(p);
                SKIP(p);
                p += readsjis(p, zkana);
                CHECK_END(p);
                SKIP(p);
                p += readsjis(p, hkata);
            } else {
                p += readeuc(p, zalpha);
                CHECK_END(p);
                SKIP(p);
                p += readeuc(p, zkana);
                CHECK_END(p);
                SKIP(p);
                p += readeuc(p, hkata);
            }
            if (halpha[0] == '\0' || zalpha[0] == '\0'
                    || zkana[0] == '\0' || hkata[0] == '\0')
                break;
            if (begin++) {
                zhtr = sj3_alloc_zhtable();
                if (!zhtr) {
                    sj3_warning("can't allocate hira-kana conversion table 1");
                    return (ALLOC_FAILED);
                }
                zhtp = zhtq->next = zhtr;
            }
            zhtp->halpha = sj3_store_uchar(halpha, strlen(halpha) + 1);
            zhtp->zalpha = sj3_store_uchar(zalpha, strlen(zalpha) + 1);
            zhtp->zkana = sj3_store_uchar(zkana, strlen(zkana) + 1);
            zhtp->hkata = sj3_store_uchar(hkata, strlen(hkata) + 1);
            if (!zhtp->halpha || !zhtp->zalpha
                    || !zhtp->zkana || !zhtp->hkata) {
                sj3_warning("can't allocate zen/han-kaku conversion table 2");
                return(ALLOC_FAILED);
            }
            zhtq = zhtp;
        }
    }
    zhtp->next = NULL;
    fclose(fp);
    return (OK);
}

void
sj3_hiratozkata(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned short s;
    register unsigned char  c;
    register SJ3_HKTB       *hktp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            s = (c << 8) + *src;
            if (ishira(s)) {
                for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                    if (c == *hktp->hira)
                        if (*src == *(hktp->hira + 1))
                            break;
                }
                if (hktp != NULL) {
                    *dest++ = *hktp->zkata;
                    *dest++ = *(hktp->zkata + 1);
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                    sj3_warning("wrong sjhk table");
                }
            } else {
                *dest++ = c;
                *dest++ = *src;
            }
            src++;
        } else {
            *dest++ = c;
        }
    }
    *dest = '\0';
}

void
sj3_hiratohkata(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register unsigned short s;
    register SJ3_HKTB       *hktp;
    register SJ3_ZHTB       *zhtp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            s = (c << 8) + *src;
            if (ishira(s)) {
                for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                    if (c == *hktp->hira)
                        if (*src == *(hktp->hira + 1))
                            break;
                }
                if (hktp != NULL) {
                    *dest++ = *hktp->hkata;
                    if (hktp->hlen > 1)
                        *dest++ = *(hktp->hkata + 1);
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                    sj3_warning("wrong sjhk table");
                }
            } else {
                for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                    if (c == *zhtp->zkana)
                        if (*src == *(zhtp->zkana + 1))
                            break;
                }
                if (zhtp != NULL) {
                    *dest++ = *zhtp->hkata;
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                }
            }
            src++;
        } else {
            *dest++ = c;
        }
    }
    *dest = '\0';
}

void
sj3_hkatatohira(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register SJ3_HKTB       *hktp;
    register SJ3_ZHTB       *zhtp;
    register int            len;
    unsigned char           tmp[RBUFSIZ];

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            *dest++ = c;
            *dest++ = *src++;
            continue;
        }
        if (iskana(c)) {
            len = 0;
            for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                if (len < hktp->hlen && c == *hktp->hkata) {
                    if (hktp->hlen > 1) {
                        if (iskana(*src) && *src == *(hktp->hkata + 1)) {
                            src++;
                            len = hktp->hlen;
                            strcpy(tmp, hktp->hira);
                        }
                    } else {
                        len = 1;
                        strcpy(tmp, hktp->hira);
                    }
                }
            }
            if (len) {
                strcpy(dest, tmp);
                dest += strlen(tmp);
            } else {
                for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                    if (c == *zhtp->hkata)
                        break;
                }
                if (zhtp != NULL) {
                    *dest++ = *zhtp->zkana;
                    *dest++ = *(zhtp->zkana + 1);
                } else {
                    *dest++ = c;
                }
            }
        } else {
            for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                if (c == *zhtp->hkata)
                    break;
            }
            if (zhtp != NULL) {
                *dest++ = *zhtp->zkana;
                *dest++ = *(zhtp->zkana + 1);
            } else {
                *dest++ = c;
            }
        }
    }
    *dest = '\0';
}

void
sj3_hkatatozkata(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register SJ3_HKTB       *hktp;
    register SJ3_ZHTB       *zhtp;
    register int            len;
    unsigned char           tmp[RBUFSIZ];

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            *dest++ = c;
            *dest++ = *src++;
            continue;
        }
        if (iskana(c)) {
            len = 0;
            for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                if (len < hktp->hlen && c == *hktp->hkata) {
                    if (hktp->hlen > 1) {
                        if (iskana(*src) && *src == *(hktp->hkata + 1)) {
                            src++;
                            strcpy(tmp, hktp->zkata);
                            len = hktp->hlen;
                        }
                    } else {
                        strcpy(tmp, hktp->zkata);
                        len = hktp->hlen;
                    }
                }
            }
            if (len) {
                strcpy(dest, tmp);
                dest += strlen(tmp);
            } else {
                for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                    if (c == *zhtp->hkata)
                        break;
                }
                if (zhtp != NULL) {
                    *dest++ = *zhtp->zkana;
                    *dest++ = *(zhtp->zkana + 1);
                } else {
                    *dest++ = c;
                }
            }
        } else {
            for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                if (c == *zhtp->hkata)
                    break;
            }
            if (zhtp != NULL) {
                *dest++ = *zhtp->zkana;
                *dest++ = *(zhtp->zkana + 1);
            } else {
                *dest++ = c;
            }
        }
    }
    *dest = '\0';
}

void
sj3_zkanatohkata(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register unsigned short s;
    register SJ3_HKTB       *hktp;
    register SJ3_ZHTB       *zhtp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            s = (c << 8) + *src;
            if (ishira(s)) {
                for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                    if (c == *hktp->hira)
                        if (*src == *(hktp->hira + 1))
                            break;
                }
                if (hktp != NULL) {
                    *dest++ = *hktp->hkata;
                    if (hktp->hlen > 1)
                        *dest++ = *(hktp->hkata + 1);
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                    sj3_warning("wrong sjhk table");
                }
            } else if (iskata(s)) {
                for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                    if (c == *hktp->zkata)
                        if (*src == *(hktp->zkata + 1))
                            break;
                }
                if (hktp != NULL) {
                    *dest++ = *hktp->hkata;
                    if (hktp->hlen > 1)
                        *dest++ = *(hktp->hkata + 1);
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                    sj3_warning("wrong sjhk table");
                }
            } else {
                for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                    if (c == *zhtp->zkana)
                        if (*src == *(zhtp->zkana + 1))
                            break;
                }
                if (zhtp != NULL) {
                    *dest++ = *zhtp->hkata;
                } else {
                    for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                        if (c == *zhtp->zalpha)
                            if (*src == *(zhtp->zalpha + 1))
                                break;
                    }
                    if (zhtp != NULL) {
                        *dest++ = *zhtp->hkata;
                    } else {
                        *dest++ = c;
                        *dest++ = *src;
                    }
                }
            }
            src++;
        } else {
            *dest++ = c;
        }
    }
    *dest = '\0';
}

void
sj3_zkatatohira(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned short s;
    register unsigned char  c;
    register SJ3_HKTB       *hktp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            s = (c << 8) + *src;
            if (iskata(s)) {
                for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                    if (c == *hktp->zkata)
                        if (*src == *(hktp->zkata + 1))
                            break;
                }
                if (hktp != NULL) {
                    *dest++ = *hktp->hira;
                    *dest++ = *(hktp->hira + 1);
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                    sj3_warning("wrong sjhk table");
                }
            } else {
                *dest++ = c;
                *dest++ = *src;
            }
            src++;
        } else {
            *dest++ = c;
        }
    }
    *dest = '\0';
}

void
sj3_zkatatohkata(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register unsigned short s;
    register SJ3_HKTB       *hktp;
    register SJ3_ZHTB       *zhtp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            s = (c << 8) + *src;
            if (iskata(s)) {
                for (hktp = hktable; hktp != NULL; hktp = hktp->next) {
                    if (c == *hktp->zkata)
                        if (*src == *(hktp->zkata + 1))
                            break;
                }
                if (hktp != NULL) {
                    *dest++ = *hktp->hkata;
                    if (hktp->hlen > 1)
                        *dest++ = *(hktp->hkata + 1);
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                    sj3_warning("wrong sjhk table");
                }
            } else {
                for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                    if (c == *zhtp->zkana)
                        if (*src == *(zhtp->zkana + 1))
                            break;
                }
                if (zhtp != NULL) {
                    *dest++ = *zhtp->hkata;
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                }
            }
            src++;
        } else {
            *dest++ = c;
        }
    }
    *dest = '\0';
}

void
sj3_halphatozalpha(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register unsigned short s;
    register SJ3_ZHTB       *zhtp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            *dest++ = c;
            *dest++ = *src++;
            continue;
        }
        for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
            if (c == *zhtp->halpha)
                break;
        }
        if (zhtp != NULL) {
            *dest++ = *zhtp->zalpha;
            *dest++ = *(zhtp->zalpha + 1);
        } else {
            sj3_warning("sj3_halphatozalpha: could not convert to zalpha");
            *dest++ = c;
        }
    }
    *dest = '\0';
}

void
sj3_halphatozkana(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register SJ3_ZHTB       *zhtp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            *dest++ = c;
            *dest++ = *src++;
            continue;
        }
        for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
            if (c == *zhtp->halpha)
                break;
        }
        if (zhtp != NULL) {
            *dest++ = *zhtp->zkana;
            *dest++ = *(zhtp->zkana + 1);
        } else {
            *dest++ = c;
        }
    }
    *dest = '\0';
}

void
sj3_zalphatohalpha(hktable, zhtable, dest, src)
    SJ3_HKTB                *hktable;
    SJ3_ZHTB                *zhtable;
    register unsigned char  *dest,  *src;
{
    register unsigned char  c;
    register unsigned short s;
    register SJ3_ZHTB       *zhtp;

    while ((c = *src++) != '\0') {
        if (issjis1(c) && issjis2(*src)) {
            s = (c << 8) + *src;
            if (iszalpha(s)) {
                for (zhtp = zhtable; zhtp != NULL; zhtp = zhtp->next) {
                    if (c == *zhtp->zalpha)
                        if (*src == *(zhtp->zalpha + 1))
                            break;
                }
                if (zhtp != NULL) {
                    *dest++ = *zhtp->halpha;
                } else {
                    *dest++ = c;
                    *dest++ = *src;
                }
            } else {
                *dest++ = c;
                *dest++ = *src;
            }
            src++;
        } else {
            *dest++ = c;
        }
    }
    *dest = '\0';
}
#endif /* USE_HANTOZEN */

#ifndef sony_news

#define MASK    0x7f
#define MSB 0x80

sj_euc2sjis (s)
u_char *s;
{
    s[0] &= MASK;
    s[1] &= MASK;
    sj_jis2sjis (s);
}

sj_jis2sjis (s)
u_char  *s;
{
    register int    high, low;
    register int    nh, nl;

    high = s[0];
    low = s[1];
    nh = ((high - 0x21) >> 1) + 0x81;
    if (nh > 0x9f)
        nh += 0x40;
    if (high & 1) {
        nl = low + 0x1f;
        if (low > 0x5f)
            nl ++;
    }
    else
        nl = low + 0x7e;
    s[0] = nh;
    s[1] = nl;
}

#define SS2 0x8e

sjistoeuc (in, out)
register u_char *in, *out;
{
    register int    c;
    u_char      tmp[3];

    while ((c = *(in ++)) != '\0') {
        if (iskana (c)) {
            *(out ++) = SS2;
            *(out ++) = c;
        }
        else if (iskanji (c)) {
            tmp[0] = c;
            if ((c = *(in ++)) == '\0') {
                *(out ++) = tmp[0];
                break;
            }
            tmp[1] = c;
            tmp[2] = '\0';
            sj_sjis2euc (tmp);
            *(out ++) = tmp[0];
            *(out ++) = tmp[1];
        }
        else {
            *(out ++) = c;
        }
    }
    *out = '\0';
}

sj_sjis2euc (s)
u_char  *s;
{
    sj_sjis2jis (s);
    s[0] |= MSB;
    s[1] |= MSB;
}

sj_sjis2jis (s)
u_char  *s;
{
    register int    byte1, byte2;
    register u_char *sp;

    sp = s;
    byte1 = *sp++;
    byte2 = *sp--;

    if (byte1 <= 0x80 || byte1 >= 0xf0
     || ( byte1>=0xa0 && byte1<=0xdf )
     || byte2 <= 0x3f || byte2 >= 0xfe || byte2==0x7f ) {
            byte1 = 0x81;
            byte2 = 0x40;
    }
    byte1 -= (byte1 >= 0xa0) ? 0xc1 : 0x81;
    if (byte2 >= 0x9f) {
        *sp++ = (byte1 << 1) + 0x22;
        *sp = byte2 - 0x7e;
    }
    else {
        *sp++ = (byte1 << 1) + 0x21;
        *sp = byte2 - ((byte2 <= 0x7e) ? 0x1f : 0x20 );
    }
}

sjis2euc (c)
int c;
{
    u_char  s[4];

    s[0] = (c >> 8) & 0xff;
    s[1] = c & 0xff;
    sj_sjis2euc (s);
    return ((s[0] << 8) | s[1]);
}

euc2sjis (c)
int c;
{
    u_char  s[4];

    s[0] = (c >> 8) & 0xff;
    s[1] = c & 0xff;
    sj_euc2sjis (s);
    return ((s[0] << 8) | s[1]);
}

sjis2jis (c)
int c;
{
    u_char  s[4];

    s[0] = (c >> 8) & 0xff;
    s[1] = c & 0xff;
    sj_sjis2jis (s);
    return ((s[0] << 8) | s[1]);
}

jis2sjis (c)
int c;
{
    u_char  s[4];

    s[0] = (c >> 8) & 0xff;
    s[1] = c & 0xff;
    sj_jis2sjis (s);
    return ((s[0] << 8) | s[1]);
}
#endif /* sony_news */
