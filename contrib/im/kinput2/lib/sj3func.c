#ifndef lint
static char *rcsid = "$Header: sj3func.c,v 1.5 91/10/02 17:42:32 nao Locked $";
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
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#ifdef sony_news
#include <X11/Xlib.h>
#else
#include <stdlib.h>
#endif
#include "DebugPrint.h"
#include "ICtypes.h"
#include "WStr.h"
#include "sj3def.h"
#include "sj3conv.h"

#ifdef NEWS_OS4
#include    <sj3lib.h>
#else
#include    "sj3lib.h"
#endif

#include <pwd.h>
#include <stdio.h>
#include <ctype.h>
#include "sj3ctype.h"
#include <locale.h>

#define InputModeMask   (1<<16)
#define UnConvModeMask  (1<<17)
#define ConvModeMask    (1<<18)
#define AllModeMask     (InputModeMask|UnConvModeMask|ConvModeMask)
#define OutputModeMask  (UnConvModeMask|ConvModeMask)
#define FlushModeMask   (InputModeMask|UnConvModeMask)

typedef struct _sjrctable {
    unsigned char   *key[2];
    unsigned char   *value[10];
} SjrcRec,  *Sjrctable;

struct _sjrcdeftable {
    char    *key[2];
    char    *value[10];
};

static struct _sjrcdeftable def_guide_table[] = {
    "guide",    "hira",  /* かな      */ "\202\251\202\310",
    "", "", "", "", "", "", "", "", "",
    "guide",    "zkata", /* 全カナ    */ "\221\123\203\112\203\151",
    "", "", "", "", "", "", "", "", "",
    "guide",    "hkata", /* 半カナ    */ "\224\274\203\112\203\151",
    "", "", "", "", "", "", "", "", "",
    "guide",    "halpha",/* ABC       */ "\101\102\103",
    "", "", "", "", "", "", "", "", "",
    "guide",    "zalpha",/* ＡＢＣ    */ "\202\140\202\141\202\142",
    "", "", "", "", "", "", "", "", "",
    "guide",    "jis",   /* ＪＩＳ    */ "\202\151\202\150\202\162",
    "", "", "", "", "", "", "", "", "",
    "guide",    "sjis",  /* Shift-JIS */ "\123\150\151\146\164\55\112\111\123",
    "", "", "", "", "", "", "", "", "",
    "guide",    "kuten", /* 区点      */ "\213\346\223\137",
    "", "", "", "", "", "", "", "", "",
    "guide",    "euc",   /* ＥＵＣ    */ "\202\144\202\164\202\142",
    "", "", "", "", "", "", "", "", "",
} ;

static int  sj3_set_comkey(), sj3_set_keytab(), sj3_set_etckeys(),
    sj3_set_intr(), sj3_set_bskey(), sj3_set_delkey(), sj3_set_goto(),
    sj3_set_trap(), sj3_set_init_mode(), sj3_set_helpmenu(), 
    sj3_set_defcode(), sj3_set_muhenkan(), sj3_set_muedit(),
    sj3_set_m_toggle(), sj3_set_guide(), sj3_set_forkshell(),
    sj3_set_forkshell(), sj3_set_dict(), sj3_set_mdict(), sj3_set_study(),
    sj3_set_romaji(), sj3_set_bstudy(), sj3_set_silent(), sj3_set_shrinkall(),
    sj3_set_flush_after_conv(), sj3_set_rkebell(), sj3_set_server(),
    sj3_set_server2(),
    sj3_set_flush_in_conv(), sj3_set_del_change(), sj3_set_conversion_seg(),
    sj3_set_alphaconv(), sj3_set_backdisplay(), sj3_set_expandmconv(),
    sj3_set_shrinkmconv(), sj3_readrc();

static struct functable {
    char    *keyword;
    int     (*func)();
} funcs[] = {
    "key",                  sj3_set_keytab,
    "escape",               sj3_set_keytab,
    "sjxkey",               sj3_set_keytab,
    "etckey",               sj3_set_etckeys,
    "intr",                 sj3_set_intr,
    "bskey",                sj3_set_bskey,
    "delkey",               sj3_set_delkey,
    "setnormal",            sj3_set_goto,
    "throughnext",          sj3_set_trap,
    "initialmode",          sj3_set_init_mode,
    "helpmode",             sj3_set_helpmenu,
    "defaultcode",          sj3_set_defcode,
    "muhenkanmode",         sj3_set_muhenkan,
    "muhenkaninedit",       sj3_set_muedit,
    "muhenkantoggle",       sj3_set_m_toggle,
    "guide",                sj3_set_guide,
    "forkshell",            sj3_set_forkshell,
    "execshell",            sj3_set_forkshell,
    "dictionary",           sj3_set_dict,
    "userdic",              sj3_set_dict,
    "maindic",              sj3_set_mdict,
    "romaji",               sj3_set_romaji,
    "bstudy",               sj3_set_bstudy,
    "silent",               sj3_set_silent,
    "flushafterconversion", sj3_set_flush_after_conv,
    "rkerrbell",            sj3_set_rkebell,
    "server",               sj3_set_server,
    "server2",              sj3_set_server2,
    "alphabetconversion",   sj3_set_alphaconv,
    "backdisplay",          sj3_set_backdisplay,
    "beginconversionlast",  sj3_set_conversion_seg,
    "expandmodeconversion", sj3_set_expandmconv,
    "shrinkmodeconversion", sj3_set_shrinkmconv,
    "shrinkall",            sj3_set_shrinkall,
    "flushinconversion",    sj3_set_flush_in_conv,
    "deletechangemode",     sj3_set_del_change,
    "bstudy",               sj3_set_study,
    NULL,                   NULL
};

static SJ3_EVENT sj3_delete(), sj3_henkan(), sj3_saihenkan(),
    sj3_muhen(), sj3_kettei(), sj3_mode_hira(),
    sj3_mode_halpha(), sj3_mode_zalpha(), sj3_mode_hkata(), sj3_select(),
    sj3_mode_zkata(), sj3_mode_code(), sj3_mode_etc(), sj3_mode_rotate(),
    sj3_kouho_right(), sj3_kouho_left(), sj3_return(),
    sj3_expand(), sj3_backward(), sj3_forward(), sj3_kouho_nextp(), 
    sj3_kouho_prevp(), sj3_symbol_begin(), sj3_mode_clear(),
    sj3_shrink(), sj3_kouho_up(), sj3_kouho_down(), sj3_mode_toggle(),
    sj3_end(), sj3_start(), sj3_cancel(), sj3_connect();

static struct sj3keytable {
    char                *keyword;
    KeySym              ksym;
    unsigned long       modmask;
    SJ3_EVENT           (*func)();
    struct sj3keytable  *prev;
    struct sj3keytable  *next;
} defkeys[] = {
    "henkan",   XK_Select,  InputModeMask,  sj3_henkan,         NULL,   NULL,
    "muhen",    XK_Cancel,  UnConvModeMask, sj3_muhen,          NULL,   NULL,
    "kettei",   XK_Execute, FlushModeMask,  sj3_kettei,         NULL,   NULL,
    "kettei",   XK_KP_Enter,FlushModeMask,  sj3_kettei,         NULL,   NULL,
    "flush",    XK_Execute, FlushModeMask,  sj3_kettei,         NULL,   NULL,
    "flush",    XK_KP_Enter,FlushModeMask,  sj3_kettei,         NULL,   NULL,
    "flush",    XK_Escape,  FlushModeMask,  sj3_kettei,         NULL,   NULL,
    "return",   XK_Return,  FlushModeMask,  sj3_return,         NULL,   NULL,
    "select",   XK_Return,  ConvModeMask,   sj3_select,         NULL,   NULL,
    "select",   XK_Execute, ConvModeMask,   sj3_select,         NULL,   NULL,
    "select",   XK_Escape,  ConvModeMask,   sj3_select,         NULL,   NULL,
    "halpha",   XK_F6,      FlushModeMask,  sj3_mode_halpha,    NULL,   NULL,
    "zalpha",   XK_F7,      FlushModeMask,  sj3_mode_zalpha,    NULL,   NULL,
    "hkata",    XK_F8,      FlushModeMask,  sj3_mode_hkata,     NULL,   NULL,
    "zkata",    XK_F9,      FlushModeMask,  sj3_mode_zkata,     NULL,   NULL,
    "hira",     XK_F10,     FlushModeMask,  sj3_mode_hira,      NULL,   NULL,
    "symbol",   XK_F4,      InputModeMask,  sj3_symbol_begin,   NULL,   NULL,
    "code",     XK_F5,      InputModeMask,  sj3_mode_code,      NULL,   NULL,
    "mode",     XK_Tab,     FlushModeMask,  sj3_mode_rotate,    NULL,   NULL,
    "toggle",   XK_Cancel,  UnConvModeMask|ControlMask,   
                                            sj3_mode_toggle,    NULL,   NULL,
    "right",    XK_Right,   ConvModeMask,   sj3_kouho_right,    NULL,   NULL,
    "left",     XK_Left,    ConvModeMask,   sj3_kouho_left,     NULL,   NULL,
    "up",       XK_Up,      ConvModeMask,   sj3_kouho_up,       NULL,   NULL,
    "down",     XK_Down,    ConvModeMask,   sj3_kouho_down,     NULL,   NULL,
    "prevp",    XK_Cancel,  ConvModeMask,   sj3_kouho_prevp,    NULL,   NULL,
    "nextp",    XK_Select,  ConvModeMask,   sj3_kouho_nextp,    NULL,   NULL,
    "backward", XK_Left,    UnConvModeMask, sj3_backward,       NULL,   NULL,
    "forward",  XK_Right,   UnConvModeMask, sj3_forward,        NULL,   NULL,
    "kakuchou", XK_Up,      UnConvModeMask, sj3_expand,         NULL,   NULL,
    "syukusyou",XK_Down,    UnConvModeMask, sj3_shrink,         NULL,   NULL,
    "cancel",   XK_Cancel,  AllModeMask|ShiftMask,  sj3_cancel, NULL,   NULL,
    "saihenkan",XK_Select,  UnConvModeMask, sj3_saihenkan,      NULL,   NULL,
    "syoukyo",  XK_BackSpace,AllModeMask,   sj3_delete,         NULL,   NULL,
    "end",      XK_Kanji,   AllModeMask|ShiftMask,  sj3_end,    NULL,   NULL,
    "reconnect",XK_Kanji,   AllModeMask|Mod1Mask|ShiftMask, 
                                            sj3_connect,        NULL,   NULL,
    "start",    NoSymbol,   AllModeMask,    sj3_start,          NULL,   NULL,
    "toroku",   NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "toggle",   NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "wrapback", NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "wrap",     NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "sjrc",     NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "help",     NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "ha-kettei",NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "debug",    NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "ignore",   NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "top",      NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "kill",     NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    "edit",     NoSymbol,   NULL,           NULL, /* not yet */ NULL,   NULL,
    NULL,       NULL,       NULL,           NULL, /* not yet */ NULL,   NULL
};

/*
 * define for input mode
 */
#define MODE_HIRA       0
#define MODE_ZKATA      1
#define MODE_HKATA      2
#define MODE_HALPHA     3
#define MODE_ZALPHA     4
#define MODE_JIS        5
#define MODE_SJIS       6
#define MODE_KUTEN      7
#define MODE_EUC        8
#define MODE_NUM        9

typedef struct _sj3Buf {
    unsigned long       convmode;
    int                 len;
    char                str[RBUFSIZ];
    char                *strbuf;
    unsigned char       yomi[KANABUFSIZ];
    unsigned char       *ybuf;
    unsigned short      w_yomi[KANABUFSIZ];
    unsigned short      *w_ybuf;
    char                oldstr[RBUFSIZ];
    int                 oldlen;
    int                 n_roma;
    SJ3_BUNSETU         dispbuf[BUNBUFSIZ];
    unsigned long       cursegmode[BUNBUFSIZ];
    int                 changelen[BUNBUFSIZ];
    int                 segnum;
    int                 curseg;
    int                 pos;
    SJ3_DOUON           *items;
    int                 itemnum;
    unsigned long       muhenmode;
    unsigned long       curmode;
    unsigned short      *mode_str[MODE_NUM];
    unsigned long       defcode;
    struct sj3keytable  *key;
    char                *server;
    char                *server2;
    int                 alphaconv;
    int                 backdisplay;
    int                 beginlastseg;
    int                 expandmconv;
    int                 shrinkmconv;
    int                 shrinkall;
    int                 rkbell;
    int                 togglemode;
    int                 flushaconv;
    int                 flushiconv;
    int                 delchange;
    int                 gakusyuu;
    int                 dotoggle;
    unsigned char       **sbtable;
    int                 symbolnum;
    int                 cursymbol;
#ifndef USE_RKCONV
    SJ3_RKTB            *rktable;
#endif /* USE_RKCONV */
#ifndef USE_HANTOZEN    
    SJ3_HKTB            *hktable;
    SJ3_ZHTB            *zhtable;
#endif /* USE_HANTOZEN */
} sj3BufRec,    *sj3Buf;

void                        sj3_reconnect();
void                        sj3_set_locale();
void                        sj3_set_user();
void                        sj3_set_sjrk();
#ifndef USE_HANTOZEN
void                        sj3_set_sjhk();
void                        sj3_set_sjzh();
#endif /* USE_HANTOZEN */
int                         sj3_initialize();
void                        sj3_down();
sj3Buf                      sj3_set_sjrc();
wchar                       *sj3_get_mode_str();
wchar                       *sj3_get_seg();
wchar                       *sj3_get_converted();
int                         sj3_get_segnum();
int                         sj3_get_position();
void                        sj3_clear_buf();
void                        sj3_error();
void                        sj3_warning();
SJ3_EVENT                   sj3_conv_key();
void                        sj3_get_itemnum();
void                        sj3_init_items();
int                         sj3_set_item();
void                        sj3_get_symbol();
int                         sj3_set_symbol();
int                         sj3_get_symbolnum();
int                         sj3_get_cursymbol();
void                        sj3_item_end();
void                        sj3_symbol_end();

static void                 sj3_open_error();
static void                 sj3_close_error();
static void                 sj3_set_keysym();
static int                  sj3_store_dispbuf();
static SJ3_EVENT            sj3_conv_mode();
static int                  sj3_code_conv();
static void                 mkmodestr();
static wchar                *toeuc();
static wchar                *lwctoswc();
static int                  cmp();
static unsigned long        checkmode();
static struct sj3keytable   *sj3_alloc_keytable();
static void                 sj3_rkconvs();
#ifdef USE_HANTOZEN
static void                 sj3_hiratokata();
static void                 sj3_katatohira();
#endif /* USE_HANTOZEN */

static int                  connected = 0;
static struct sj3keytable   *keynowtp = NULL,   *keymaxtp = NULL;

/*
 *  sj3_set_locale() set locale and dicide file code for sjrc
 */
void
sj3_set_locale(locale, code)
    char    *locale;
    int     *code;
{
    char    *loc;
    if (loc = setlocale (LC_CTYPE, ""))
        strcpy(locale, loc);
    if (loc) {
        if (!strcmp(loc, "ja_JP.SJIS"))
            *code = USR_SJIS;
        else 
            *code = USR_EUC;
    } else
        *code = USR_OTHER;
#ifdef FORCE_SJIS
    *code = USR_SJIS;
#endif
}

/*
 *  sj3_set_user() set username and home directory
 */
void
sj3_set_user(user, home)
    char    *user;
    char    *home;
{
    extern char             *getenv(),  *getlogin();
    char                    *login;
    struct passwd           *pwd,   *getpwnam(),    *getpwuid();


    if (!user || *user == '\0') {
        if (login = getlogin())
            strcpy(user, login);
    }
    setpwent();
    if (!user || *user == '\0') {
        if (pwd = getpwuid(getuid())) {
            strcpy(user,pwd->pw_name);
        }
    } else {
        pwd = getpwnam(user);
    }
    if (pwd)
        strcpy(home, pwd->pw_dir);
    else
        strcpy(home, getenv("HOME"));
    endpwent();
}

#define KEYTBMAX    (BUFSIZ/sizeof(struct sj3keytable) - 1)

static struct sj3keytable *
sj3_alloc_keytable()
{
    register struct sj3keytable *keytp;

    if (keynowtp == NULL || keynowtp > keymaxtp) {
        keytp = (struct sj3keytable *)malloc(BUFSIZ);
        if (keytp == NULL)
            return (NULL);
        keynowtp = keytp;
        keymaxtp = keynowtp + KEYTBMAX;
        keynowtp++;
        return (keytp);
    } else {
        keytp = keynowtp;
        keynowtp++;
        return (keytp);
    }
}

#define END     -1
#define COMMENT 0
#define NORMAL  1
#define BAD     2
#define OTHERS  3
#define SERIAL  "nwp511"
#define SERIAL2  "nwp517"
#define SERIAL3  "nwp-511"

/*
 *  sj3_set_sjrc() allocate buffer and set resources
 */
sj3Buf
sj3_set_sjrc (sjrc, home, code)
    char    *sjrc;
    char    *home;
    int     code;
{
    extern char         *getenv();
    register char       *p;
    char                rcfile[BUFSIZ];
    SjrcRec             rctable;
    struct functable    *functp;
    FILE                *fp;
    register int        line,   i;
    int                 status;
    sj3Buf              buf;
    struct sj3keytable  *keytp, *keytq, *keytr, *keyts;

    /*  Get sjrc file and open */
    if (sjrc) {
        if ((fp = fopen(sjrc, "r")) == NULL)
                sj3_error("can't open sjrc file %s", sjrc);
        DPRINT(("sjrc file is %s\n",sjrc));
    } else {
        rcfile[0] = '\0';
        if ((p = getenv("SJRC")) && *p != '\0') {
            if (*p != '/') {
                if (home)
                    strcpy(rcfile, home);
                strcat(rcfile, "/");
            }
            strcat(rcfile, p);
        } else if (home) {
            strcpy(rcfile, home);
            strcat(rcfile, "/.sjrc");
        } else {
            strcpy(rcfile, SJ3DEFPATH);
            strcat(rcfile, DEF_SJRC_FILE);
        }
        if ((fp = fopen(rcfile, "r")) == NULL) {
                sj3_warning("can't open sjrc file %s", rcfile);
                strcpy(rcfile, SJ3DEFPATH);
                strcat(rcfile, DEF_SJRC_FILE);
                if ((fp = fopen(rcfile, "r")) == NULL) {
                    sj3_error("can't open sjrc file %s", rcfile);
                }
        }
        DPRINT(("sjrc file is %s\n",rcfile));
    }

    /*  Allocate buffer fields  */
    if ((buf = (sj3Buf)malloc(sizeof(sj3BufRec))) == NULL) {
        return (sj3Buf)NULL;
    }

    /*  Initialize buffer fields    */
    *buf->str = '\0';
    buf->strbuf = buf->str;
    *buf->oldstr = '\0';
    *buf->yomi = '\0';
    buf->ybuf = buf->yomi;
    *buf->w_yomi = '\0';
    buf->w_ybuf = buf->w_yomi;
    buf->n_roma = 0;
    buf->len = 0;
    buf->oldlen = 0;
    for ( i = 0; i < MODE_NUM; i++)
        buf->mode_str[i] = NULL;
    buf->muhenmode = MODE_HIRA;
    buf->curmode = MODE_HIRA;
    for ( i = 0; i < BUNBUFSIZ; i++)
        buf->cursegmode[i] = MODE_HIRA;
    buf->defcode = MODE_SJIS;
    buf->togglemode = MODE_ZKATA;
    buf->server = NULL;
    buf->server2 = NULL;
    buf->alphaconv = False;
    buf->backdisplay = False;
    buf->beginlastseg = False;
    buf->expandmconv = False;
    buf->shrinkmconv = False;
    buf->shrinkall = False;
    buf->rkbell = False;
    buf->flushaconv = False;
    buf->flushiconv = True;
    buf->delchange = False;
    buf->gakusyuu = False;
    buf->dotoggle = True;
    buf->convmode = InputModeMask;
    buf->segnum = 0;
    buf->pos = 0;
    buf->curseg = 0;

    buf->cursymbol = 0;

    for ( i = 0; i < BUNBUFSIZ; i++) {
        if ((buf->dispbuf[i].deststr
                = (unsigned char *)malloc(KANJIBUFSIZ)) == NULL) {
            return (sj3Buf)NULL;
        }
        if ((buf->dispbuf[i].srcstr
                = (unsigned char *)malloc(KANABUFSIZ)) == NULL) {
            return (sj3Buf)NULL;
        }
    }

#ifndef USE_RKCONV
    buf->rktable = sj3_alloc_rktable();
    if (!buf->rktable) {
        sj3_warning("can't allocate roma-kana conversion table 0");
        return (sj3Buf)NULL;
    }
#endif
#ifndef USE_HANTOZEN
    buf->hktable = sj3_alloc_hktable();
    if (!buf->hktable) {
        sj3_warning("can't allocate hira-kana conversion table 0");
        return (sj3Buf)NULL;
    }
    buf->zhtable = sj3_alloc_zhtable();
    if (!buf->zhtable) {
        sj3_warning("can't allocate zenkaku-hankaku conversion table 0");
        return (sj3Buf)NULL;
    }
#endif

    /*  Make keysym to function hash table  */
    for (keytp = defkeys, i = 0; keytp->keyword != NULL; keytp++) {
        if (keytp->ksym != NoSymbol) {
            keytq = sj3_alloc_keytable();
            if (!keytq) {
                sj3_warning("can't allocate keysym to func table");
                return (sj3Buf)NULL;
            }
            if (!i++) {
                buf->key = keytq;
                keytq->prev = NULL;
            } else {
                keytr->next = keytq;
                keytq->prev = keytr;
            }
            keytq->keyword = keytp->keyword;
            keytq->ksym = keytp->ksym;
            keytq->modmask = keytp->modmask;
            keytq->func = keytp->func;
            keytr = keytq;
        }
    }
    keytq->next = NULL;

    /*  Read sjrc file and set buffer fields    */
    for (line = 0; (status = 
        sj3_readrc(fp, &rctable)) != END; line++)  {
        functp = funcs;
        if (status == NORMAL) {
            while (functp->keyword != NULL) {
                if (cmp(rctable.key[0], functp->keyword)) {
                    (*(functp->func))(buf, &rctable, code);
                    break;
                }
                functp++;
                if (functp->keyword == NULL)
                    sj3_warning("keyword %s is not supported",rctable.key[0]);
            }
        } else if (status == COMMENT || status == OTHERS)
            continue;
        else {
            sj3_warning("bad line in sjrc file. line = %d\n",line);
            continue;
        }
    }
    fclose(fp);

    /* Default definition for uninitialized field */
    for ( i = 0; i < MODE_NUM; i++) {
        if (!buf->mode_str[i]) {
            sj3_set_guide(buf, &def_guide_table[i], USR_SJIS);
        }
    }
    return (buf);
}

/*
 *  sj3_readrc() non memory copy version
 */
static int
sj3_readrc(fp, rctable)
    FILE                *fp;
    Sjrctable           rctable;
{
    static unsigned char        buf[256];
    register int                i,  j,  k,  end;

    if ((fgets (buf, sizeof(buf), fp)) == NULL) 
        return END;
    
    if (buf[0] == '\n' || buf[0] == '#' || buf[0] == '\0' )
        return COMMENT;

    for (i = 0, k = 0, end = 0; buf[i] !=  ' ' && buf[i] != '\t' && 
        buf[i] != '#' && buf[i] != '\n';) {
        for (j = i; buf[i] != ' ' && buf[i] != '\t'
                && buf[i] != '#' && buf[i] != '\n' && buf[i] != '.'; i++);
        if (j == i && buf[i] == '.' ) {
            i++;
            continue;
        }
        if (buf[i] == '#' || buf[i] == '\n' )
            return BAD;
        if (buf[i] == '\t' || buf[i] == ' ' )
            end++;
        buf[i++] = '\0';
        if (!strcmp(&buf[j],SERIAL) || !strcmp(&buf[j],SERIAL2)
            || !strcmp(&buf[j],SERIAL3))
            return OTHERS;
        rctable->key[k++] =  &buf[j];
        DPRINT(("rckey[%d]=%s:",k-1,rctable->key[k-1]));
        if (end)
            break;
    }
    if ( k < 1) 
        return BAD;
    DPRINT(("\n"));
    while(buf[i] == ' ' || buf[i] == '\t') i++;
    for (k = 0, end = 0; buf[i] != '\n';) {
        for (j = i; buf[i] != '\n' ; i+=2) {
            if (issjis1(buf[i]) || iseuc(buf[i])) {
                if (issjis2(buf[i+1]) || iseuc(buf[i+1]))
                    continue;
                else if (buf[i+1] == '#' || buf[i+1] == ' '
                        || buf[i+1] == '\t'|| buf[i+1] == '\n') {
                    i++;
                    break;
                } else
                    continue;
            } else if (buf[i] == '#' || buf[i] == ' ' || buf[i] == '\t') {
                break;
            } else if (buf[i] == '.') {
                if (i > 0 && buf[i - 1] == '\\')
                    continue;
                else
                    break;
            } else if (buf[i+1] == '#' || buf[i+1] == ' '
                    || buf[i+1] == '\t' || buf[i+1] == '\n') {
                i++;
                break;
            } else
                continue;
        }
        if (buf[i] == '#' || buf[i] == '\n' )
            end++;
        buf[i++] = '\0';
        rctable->value[k++] = &buf[j];
        DPRINT(("rcvalue[%d]=%s:",k-1,rctable->value[k-1]));
        if (end)
            break;
        while(buf[i] == ' ' || buf[i] == '\t') i++;
    }
    DPRINT(("\n"));
    if ( k < 1) 
        return BAD;
    return NORMAL;
}

/*
 *  sj3_set_sjrk()
 */
void
sj3_set_sjrk (buf, sjrk, home, code)
    sj3Buf  buf;
    String  sjrk;
    String  home;
    int     code;
{
    extern char         *getenv();
    register char       *p;
    char                rkfile[BUFSIZ];
    int                 value;

#ifdef USE_RKCONV
    if (code >= 0)
            sj3_rkcode(code);
    else
        sj3_warning("null locale");
#endif /* USE_RKCONV */

    if (sjrk) {
#ifdef USE_RKCONV
        if ((value = sj3_rkinit(sjrk)) > 0 )
#else /* USE_RKCONV */
        if ((value = sj3_readrk(buf->rktable, sjrk, code)) > 0 )
#endif /* USE_RKCONV */
            sj3_error("can't open sjrk file %s", sjrk);
        else if ( value < 0 )
            sj3_error("read failed sjrk file %s", sjrk);
        DPRINT(("sjrk file is %s\n",sjrk));
    } else {
        rkfile[0] = '\0';
        if ((p = getenv("SJRK")) && *p != '\0') {
            if (*p != '/') {
                if (home)
                    strcpy(rkfile, home);
                strcat(rkfile, "/");
            }
            strcat(rkfile, p);
        } else if (home) {
            strcpy(rkfile, home);
            strcat(rkfile, "/.sjrk");
        } else {
            strcpy(rkfile, SJ3DEFPATH);
            strcat(rkfile, DEF_SJRK_FILE);
        }
#ifdef USE_RKCONV
        if ((value = sj3_rkinit(rkfile)) > 0 ) {
#else /* USE_RKCONV */
        if ((value = sj3_readrk(buf->rktable, rkfile, code)) > 0 ) {
#endif /* USE_RKCONV */
            strcpy(rkfile, SJ3DEFPATH);
            strcat(rkfile, DEF_SJRK_FILE);
#ifdef USE_RKCONV
            if ((value = sj3_rkinit(rkfile)) > 0 ) {
#else /* USE_RKCONV */
            if ((value = sj3_readrk(buf->rktable, rkfile, code)) > 0 ) {
#endif /* USE_RKCONV */
                sj3_error("can't open sjrk file %s", rkfile);
            }
        } else if ( value < 0 ) {
            sj3_error("read failed sjrk file %s", rkfile);
        }
        DPRINT(("sjrk file is %s\n",rkfile));
    }
}

void
sj3_set_sjsb(buf, sjsb, home, code)
    sj3Buf  buf;
    String  sjsb;
    String  home;
    int     code;
{
    extern char         *getenv();
    register char       *p;
    char                sbfile[BUFSIZ];

    if (sjsb) {
        if ((buf->sbtable = sj3_readsb(&buf->symbolnum, sjsb, code)) == NULL)
            sj3_error("read failed sjsb file %s", sjsb);
        DPRINT(("sjsb file is %s\n",sjsb));
    } else {
        sbfile[0] = '\0';
        if ((p = getenv("SJSB")) && *p != '\0') {
            if (*p != '/') {
                if (home)
                    strcpy(sbfile, home);
                strcat(sbfile, "/");
            }
            strcat(sbfile, p);
        } else if (home) {
            strcpy(sbfile, home);
            strcat(sbfile, "/.sjsb");
        } else  {
            strcpy(sbfile, SJ3DEFPATH);
            strcat(sbfile, DEF_SJSB_FILE);
        }
        if ((buf->sbtable = sj3_readsb(&buf->symbolnum, sbfile, code))
                == NULL) {
            strcpy(sbfile, SJ3DEFPATH);
            strcat(sbfile, DEF_SJSB_FILE);
            if ((buf->sbtable = sj3_readsb(&buf->symbolnum, sbfile, code))
                        == NULL) {
                sj3_error("read failed sjsb file %s", sbfile);
            }
        }
        DPRINT(("sjsb file is %s\n",sbfile));
    }
}

#ifndef USE_HANTOZEN
void
sj3_set_sjhk(buf, sjhk, home, code)
    sj3Buf  buf;
    String  sjhk;
    String  home;
    int     code;
{
    extern char         *getenv();
    register char       *p;
    char                hkfile[BUFSIZ];
    int                 value;

    if (sjhk) {
        if ((value = sj3_readhk(buf->hktable, sjhk, code)) > 0 )
            sj3_error("can't open sjhk file %s", sjhk);
        else if ( value < 0 )
            sj3_error("read failed sjhk file %s", sjhk);
        DPRINT(("sjhk file is %s\n",sjhk));
    } else {
        hkfile[0] = '\0';
        if ((p = getenv("SJHK")) && *p != '\0') {
            if (*p != '/') {
                if (home)
                    strcpy(hkfile, home);
                strcat(hkfile, "/");
            }
            strcat(hkfile, p);
        } else if (home) {
            strcpy(hkfile, home);
            strcat(hkfile, "/.sjhk");
        } else  {
            strcpy(hkfile, SJ3DEFPATH);
            strcat(hkfile, DEF_SJHK_FILE);
        }
        if ((value = sj3_readhk(buf->hktable, hkfile, code)) > 0 ) {
            strcpy(hkfile, SJ3DEFPATH);
            strcat(hkfile, DEF_SJHK_FILE);
            if ((value = sj3_readhk(buf->hktable, hkfile, code)) > 0 ) {
                sj3_error("can't open sjhk file %s", hkfile);
            }
        } else if ( value < 0 ) {
            sj3_error("read failed sjhk file %s", hkfile);
        }
        DPRINT(("sjhk file is %s\n",hkfile));
    }
}

void
sj3_set_sjzh(buf, sjzh, home, code)
    sj3Buf  buf;
    String  sjzh;
    String  home;
    int     code;
{
    extern char         *getenv();
    register char       *p;
    char                zhfile[BUFSIZ];
    int                 value;

    if (sjzh) {
        if ((value = sj3_readzh(buf->zhtable, sjzh, code)) > 0 )
            sj3_error("can't open sjzh file %s", sjzh);
        else if ( value < 0 )
            sj3_error("read failed sjzh file %s", sjzh);
        DPRINT(("sjzh file is %s\n",sjzh));
    } else {
        zhfile[0] = '\0';
        if ((p = getenv("SJZH")) && *p != '\0') {
            if (*p != '/') {
                if (home)
                    strcpy(zhfile, home);
                strcat(zhfile, "/");
            }
            strcat(zhfile, p);
        } else if (home) {
            strcpy(zhfile, home);
            strcat(zhfile, "/.sjzh");
        } else {
            strcpy(zhfile, SJ3DEFPATH);
            strcat(zhfile, DEF_SJZH_FILE);
        }
        if ((value = sj3_readzh(buf->zhtable, zhfile, code)) > 0 ) {
            strcpy(zhfile, SJ3DEFPATH);
            strcat(zhfile, DEF_SJZH_FILE);
            if ((value = sj3_readzh(buf->zhtable, zhfile, code)) > 0 ) {
                sj3_error("can't open sjzh file %s", zhfile);
            }
        } else if ( value < 0 ) {
            sj3_error("read failed sjzh file %s", zhfile);
        }
        DPRINT(("sjzh file is %s\n",zhfile));
    }
}
#endif /* USE_HANTOZEN */

/*
 *  sj3_initialize()
 */
int
sj3_initialize(buf, host, user, force, second)
    sj3Buf  buf;
    String  host;
    String  user;
    int     force;
    int     second;
{
    extern char         *getenv();
    register char       *p;
    char                hostname[32];
    int                 value;
    
    if (connected && !force) {
        DPRINT(("Already connected to server\n"));
        return CONNECT_OK;
    }
    if (host) {
        if (user) {
            if ((value = sj3_open(host, user)) != SJ3_NORMAL_END ) {
                sj3_open_error(value);
                if (value == SJ3_SERVER_DEAD || value == SJ3_CONNECT_ERROR)
                    return CONNECT_FAIL;
            }
        } else {
            sj3_error("can't connect sj3serv by null user");
            return CONNECT_FAIL;
        }
        DPRINT(("sj3serv: %s@%s\n",user, host));
    } else {
        if (user) {
            hostname[0] = '\0';
            if (!second) {
                if ((p = getenv("SJ3SERV")) && *p != '\0') {
                    strcpy(hostname, p);
                } else if (buf->server) {
                    strcpy(hostname, buf->server);
                }
            } else {
                if ((p = getenv("SJ3SERV2")) && *p != '\0') {
                    strcpy(hostname, p);
                } else if (buf->server2) {
                    strcpy(hostname, buf->server2);
                }
            }
            if (hostname) {
                DPRINT(("%s@%s\n",user, hostname));
            } else {
                DPRINT(("%s@localhost\n",user));
            }
            if ((value = sj3_open(hostname, user)) != SJ3_NORMAL_END) {
                sj3_open_error(value);
                if (value == SJ3_SERVER_DEAD || value == SJ3_CONNECT_ERROR)
                    return CONNECT_FAIL;
            }
        } else {
            sj3_error("can't connect sj3serv by null user");
            return CONNECT_FAIL;
        }
    }
    connected = 1;
    return CONNECT_OK;
}

void
sj3_reconnect(buf, host, host2, user)
    sj3Buf  buf;
    String  host;
    String  host2;
    String  user;
{
    char        uname[256];
    char        home[256];

    sj3_unlockserv();
    sj3_down(True);
    if (!user || *user == '\0') {
        uname[0] = '\0';
        sj3_set_user(uname, home);
        user = uname;
    }
    if ((sj3_initialize(buf, host, user, False, False)) != CONNECT_OK) {
        sj3_warning("Failed to connect first server. try to second server");
        if ((sj3_initialize(buf, host2, user, False, True)) != CONNECT_OK) {
            sj3_error("Failed to connect second server.");
        }
    }
}

void
sj3_down(force)
    int force;
{
    int     value;

    if (connected) {
        if ((value = sj3_close ()) != SJ3_NORMAL_END && !force) {
            sj3_close_error(value);
        }
        connected = 0;
    }
}

static int
sj3_set_comkey(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{

    return True;
}

static int
sj3_set_keytab(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    struct sj3keytable      *keytp, *keytq, *keytr;
    KeySym              ksym;
    int                 modmask;

    sj3_set_keysym(table->value, &ksym, &modmask); 
    for (keytp = buf->key; keytp->next != NULL; keytp = keytp->next) {
        if (keytp->ksym == ksym &&
            (keytp->modmask & ~AllModeMask) == (modmask & ~AllModeMask)
            && ((keytp->modmask & AllModeMask)|modmask)) {
            keytq = keytp->next;    
            keytp = keytp->prev;
            keytp->next = keytq;
        } 
    }

    keytr = keytp;
    keytq = defkeys;
    while (keytq->keyword != NULL) {
        if (cmp(table->key[1], keytq->keyword)) {
            keytp = sj3_alloc_keytable();
            if (!keytp) {
                sj3_warning("can't allocate keysym to func table");
                return False;
            }
            keytr->next = keytp;
            keytp->prev = keytr;
            keytp->keyword = keytq->keyword;
            keytp->func = keytq->func;
            keytp->ksym = ksym;
            keytp->modmask = (unsigned long)modmask;
            keytr = keytp;
            keytp->next = NULL;
            if (keytp->keyword == NULL) {
                sj3_warning("keyword %s is not supported",table->key[1]);
                return False;
            }
        } 
        keytq++;
    }
    return True;
}

static void
sj3_set_keysym(string, ksym, modmask)
    char            **string;
    KeySym          *ksym;
    int             *modmask;
{
    register char   *p;
    int             sub;

    *modmask = 0;
    *ksym = NoSymbol;
    p = *string;
    if (p) {
        if(*p == '^') { /* sj2/sj3 backward compatibility   */
            if (++p) {
                if (*p == '[') {
                    *ksym = XK_Escape;
                    return;
                } else if (*p == '?') {
                    *ksym = XK_Delete;
                    return;
                } else if (isupper(*p)) {
                    sub = *p - 'A';
                    *ksym = XK_a + sub;
                } else if (islower(*p)) {
                    sub = *p - 'a';
                    *ksym = XK_a + sub;
                } else if (*p >= '\\' && *p <= '_') {
                    sub = *p - '\\';
                    *ksym = XK_backslash + sub;
                } else if (*p >= '!' && *p <= '>') {
                    sub = *p - '!';
                    *ksym = XK_exclam + sub;
                } else {
                    sj3_warning("wrong keybind in sjrc file");
                    return;
                }
                *modmask = ControlMask;
                return;
            }
        } else if ((*modmask = getmodmask(p)) >= 0) {
            if (++string)
                *ksym = XStringToKeysym(*string);
            else
                sj3_warning("wrong keybind in sjrc file");
        } else {
            sj3_warning("wrong keybind in sjrc file");
        }
    }
}

static int
getmodmask(p)
    register char   *p;
{
    char            mode[256];
    register char   *q;
    int             ret = AllModeMask,  mask = AllModeMask;
    unsigned long   conversion = 0;

    while (*p != '\0') {
        q = mode;
        while (*p != '\0' && *p != '|') {
            *q++ = *p++;
        }
        if (*p != '\0')
            p++;
        *q = '\0';
        q = mode; 
        if (*q == 'n') {
            continue;
        } else if (*q == 'N') {
            continue;
        } else if (*q == 's') {
            ret |= ShiftMask;
            continue;
        } else if (*q == 'S') {
            ret |= ShiftMask;
            continue;
        } else if (*q == 'c') {
            ret |= ControlMask;
            continue;
        } else if (*q == 'C') {
            ret |= ControlMask;
            continue;
        } else if (*q == 'm') {
            ret |= Mod1Mask;
            continue;
        } else if (*q == 'M') {
            ret |= Mod1Mask;
            continue;
        } else if (*q == 'l') {
            ret |= LockMask;
            continue;
        } else if (*q == 'L') {
            ret |= LockMask;
            continue;
        } else if (*q == 'i') {
            conversion |= InputModeMask;
            continue;
        } else if (*q == 'i') {
            conversion |= InputModeMask;
            continue;
        } else if (*q == 'u') {
            conversion |= UnConvModeMask;
            continue;
        } else if (*q == 'U') {
            conversion |= UnConvModeMask;
            continue;
        } else if (*q == 'v') {
            conversion |= ConvModeMask;
            continue;
        } else if (*q == 'V') {
            conversion |= ConvModeMask;
            continue;
        } else if (*q == 'o') {
            conversion |= OutputModeMask;
            continue;
        } else if (*q == 'O') {
            conversion |= OutputModeMask;
            continue;
        } else if (*q == 'f') {
            conversion |= FlushModeMask;
            continue;
        } else if (*q == 'F') {
            conversion |= FlushModeMask;
            continue;
        } else if (*q == 'a') {
            conversion |= AllModeMask;
            continue;
        } else if (*q == 'A') {
            conversion |= AllModeMask;
            continue;
        } else {
            sj3_warning("Ileegal keybind modmask %s in sjrc file",mode);
            ret = -1;
        }
    }
    if (conversion) {
        ret &= ~mask;
        ret |= conversion;
    }
    return ret;
}

static int
sj3_set_etckeys(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* Not yet Supported */
    return True;
}

static int
sj3_set_intr(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported */
    return True;
}

static int
sj3_set_bskey(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* Set backspace key. it now can set by sjrc file, .key.syoukyo */
    /* No Supported */
    return True;
}

static int
sj3_set_delkey(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* Set delete key. it now can set by sjrc file, .key.syoukyo    */
    /* No Supported */
    return True;
}

static int
sj3_set_goto(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported */
    return True;
}

static int
sj3_set_trap(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported */
    return True;
}

static int
sj3_set_init_mode(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{

    if (table->value[1]) {
        if (cmp(table->value[1], "hira")) {
            buf->curmode = MODE_HIRA;
        } else if (cmp(table->value[1], "zkata")) {
            buf->curmode = MODE_ZKATA;
        } else if (cmp(table->value[1], "hkata")) {
            buf->curmode = MODE_HKATA;
        } else if (cmp(table->value[1], "halpha")) {
            buf->curmode = MODE_HALPHA;
        } else if (cmp(table->value[1], "zalpha")) {
            buf->curmode = MODE_ZALPHA;
        } else if (cmp(table->value[1], "jis")) {
            buf->curmode = MODE_JIS;
        } else if (cmp(table->value[1], "sjis")) {
            buf->curmode = MODE_SJIS;
        } else if (cmp(table->value[1], "kuten")) {
            buf->curmode = MODE_KUTEN;
        } else if (cmp(table->value[1], "euc")) {
            buf->curmode = MODE_EUC;
        } else {
            sj3_warning("Unknown init mode");
        }
    }
    return True;
}

static int
sj3_set_helpmenu(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported */
    return True;
}

static int
sj3_set_defcode(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    if (table->value[1]) {
        if (cmp(table->value[1], "sjis")) {
            buf->defcode = MODE_SJIS;
        } else if (cmp(table->value[0], "euc")) {
            buf->defcode = MODE_EUC;
        } else if (cmp(table->value[0], "jis")) {
            buf->defcode = MODE_JIS;
        } else if (cmp(table->value[0], "kuten")) {
            buf->defcode = MODE_KUTEN;
        } else {
            sj3_warning("Unknown default code");
        }
    }
    return True;
}

static int
sj3_set_muhenkan(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    if (table->value[1]) {
        if (cmp(table->value[1], "hira")) {
            buf->muhenmode = MODE_HIRA;
        } else if (cmp(table->value[0], "zkata")) {
            buf->muhenmode = MODE_ZKATA;
        } else if (cmp(table->value[0], "hkata")) {
            buf->muhenmode = MODE_HKATA;
        } else if (cmp(table->value[0], "halpha")) {
            buf->muhenmode = MODE_HALPHA;
        } else if (cmp(table->value[0], "zalpha")) {
            buf->muhenmode = MODE_ZALPHA;
        } else if (cmp(table->value[0], "jis")) {
            buf->curmode = MODE_JIS;
        } else if (cmp(table->value[0], "sjis")) {
            buf->curmode = MODE_SJIS;
        } else if (cmp(table->value[0], "kuten")) {
            buf->curmode = MODE_KUTEN;
        } else if (cmp(table->value[0], "euc")) {
            buf->curmode = MODE_EUC;
        } else {
            sj3_warning("Invalid muhenkan mode");
        }
    }
    return True;
}

static int
sj3_set_muedit(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* Set mode to use in ConvMode toggle by sj3_mode_toggle() */
    if (cmp(table->value[0], "hira")) {
        buf->togglemode = MODE_HIRA;
    } else if (cmp(table->value[0], "zkata")) {
        buf->togglemode = MODE_ZKATA;
    } else if (cmp(table->value[0], "hkata")) {
        buf->togglemode = MODE_HKATA;
    } else if (cmp(table->value[0], "halpha")) {
        buf->togglemode = MODE_HALPHA;
    } else if (cmp(table->value[0], "zalpha")) {
        buf->togglemode = MODE_ZALPHA;
    } else if (cmp(table->value[0], "jis")) {
        buf->togglemode = MODE_JIS;
    } else if (cmp(table->value[0], "sjis")) {
        buf->togglemode = MODE_SJIS;
    } else if (cmp(table->value[0], "kuten")) {
        buf->togglemode = MODE_KUTEN;
    } else if (cmp(table->value[0], "euc")) {
        buf->togglemode = MODE_EUC;
    } else {
        sj3_warning("Unknown toggle mode");
    }

    return True;
}

static int
sj3_set_m_toggle(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{

    if (cmp(table->value[0],"on"))
        buf->dotoggle = True;
    else
        buf->dotoggle = False;
    return True;
}

static int
sj3_set_guide(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    wchar_t                 mode[32];
    int                     len;
    
    if (!table->value[0])
        return False;
    if (table->key[1]) {
        if (cmp(table->key[1], "hira")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_HIRA, code);
        } else if (cmp(table->key[1], "zkata")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_ZKATA, code);
        } else if (cmp(table->key[1], "hkata")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_HKATA, code);
        } else if (cmp(table->key[1], "halpha")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_HALPHA, code);
        } else if (cmp(table->key[1], "zalpha")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_ZALPHA, code);
        } else if (cmp(table->key[1], "jis")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_JIS, code);
        } else if (cmp(table->key[1], "sjis")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_SJIS, code);
        } else if (cmp(table->key[1], "kuten")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_KUTEN, code);
        } else if (cmp(table->key[1], "euc")) {
            len = mbstowcs(mode, table->value[0], strlen(table->value[0]));
            mkmodestr(buf, len, mode, MODE_EUC, code);
        }
    }
}

static void
mkmodestr(buf, len, mode, mode_num, code)
    sj3Buf              buf;
    int                 len;
    wchar_t             mode;
    int                 mode_num;
    int                 code;
{
    if (!buf->mode_str[mode_num])
        buf->mode_str[mode_num]
            = (wchar *)malloc((len + 1) * sizeof(wchar));
    if (code == USR_SJIS ) {
#ifdef LWCHAR
        wchar                   s_mode[32];
        lwctoswc(s_mode, mode, len);
        toeuc(s_mode, buf->mode_str[mode_num]);
#else /* LWCHAR */
        toeuc(mode, buf->mode_str[mode_num]);
#endif /* LWCHAR */
    } else {
#ifdef LWCHAR
        lwctoswc(buf->mode_str[mode_num], mode, len);
#else /* LWCHAR */
        strcpy(buf->mode_str[mode_num], mode);
#endif /* LWCHAR */
    }
}

static int
sj3_set_forkshell(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* Set shell name to fork in sj2/sj3: No Support */

    return True;
}

static int
sj3_set_dict(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported Yet */

    return True;
}

static int
sj3_set_mdict(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported Yet */

    return True;
}

static int
sj3_set_romaji(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported Yet */

    return True;
}

static int
sj3_set_bstudy(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* No Supported Yet */

    return True;
}

static int
sj3_set_silent(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    /* switch verbose off reource of sj2/sj3: No Support */

    return True;
}

static int
sj3_set_flush_after_conv(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->flushaconv = True;
    else
        buf->flushaconv = False;
    return True;
}

static int
sj3_set_flush_in_conv(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->flushiconv = True;
    else
        buf->flushiconv = False;
    return True;
}

static int
sj3_set_rkebell(buf, table, code)
    sj3Buf              buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->rkbell = True;
    else
        buf->rkbell = False;
    return True;
}

static int
sj3_set_server(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (table->value[0]) {
        static char hostname[32];
        strcpy (hostname, table->value[0]);
        buf->server = hostname;
        return True;
    }
    return False;
}

static int
sj3_set_server2(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (table->value[0]) {
        static char hostname[32];
        strcpy (hostname, table->value[0]);
        buf->server2 = hostname;
        return True;
    }
    return False;
}

static int
sj3_set_alphaconv(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->alphaconv = True;
    else
        buf->alphaconv = False;
    return True;
}

static int
sj3_set_backdisplay(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->backdisplay = True;
    else
        buf->backdisplay = False;
    return True;
}

static int
sj3_set_conversion_seg(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->beginlastseg = True;
    else
        buf->beginlastseg = False;
    return True;
}

static int
sj3_set_shrinkall(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->shrinkall = True;
    else
        buf->shrinkall = False;
    return True;
}

static int
sj3_set_del_change(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->delchange = True;
    else
        buf->delchange = False;
    return True;
}

static int
sj3_set_expandmconv(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->expandmconv = True;
    else
        buf->expandmconv = False;
    return True;
}

static int
sj3_set_shrinkmconv(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->shrinkmconv = True;
    else
        buf->shrinkmconv = False;
    return True;
}

static int
sj3_set_study(buf, table, code)
    sj3Buf  buf;
    Sjrctable           table;
    int                 code;
{
    if (cmp(table->value[0],"on"))
        buf->gakusyuu = True;
    else
        buf->gakusyuu = False;
    return True;
}

static void
sj3_open_error(error)
    int error;
{
    switch (error) {
        case SJ3_SERVER_DEAD:
            sj3_warning("sj3_open: server died in connecting");
            break;
        case SJ3_CONNECT_ERROR:
            sj3_warning("sj3_open: connected to server error");
            break;
        case SJ3_ALREADY_CONNECTED:
            sj3_warning("sj3_open: already connected to server");
            break;
        case SJ3_CANNOT_OPEN_MDICT:
            sj3_warning("sj3_open: can't open main dictionaries");
            break;
        case SJ3_CANNOT_OPEN_UDICT:
            sj3_warning("sj3_open: can't open user dictionaries");
            break;
        case SJ3_CANNOT_OPEN_STUDY:
            sj3_warning("sj3_open: can't open files for study");
            break;
        case SJ3_CANNOT_MAKE_UDIR:
            sj3_warning("sj3_open: can't make directries for user");
            break;
        case SJ3_CANNOT_MAKE_UDICT:
            sj3_warning("sj3_open: can't make user dictionaries");
            break;
        case SJ3_CANNOT_MAKE_STUDY:
            sj3_warning("sj3_open: can't make files for study");
            break;
        default:
            sj3_warning("sj3_open: unknown error");
            break;
    }
}

static void
sj3_close_error(error)
    int error;
{
    switch (error) {
        case SJ3_SERVER_DEAD:
            sj3_warning("sj3_close: server died in disconnecting");
            break;
        case SJ3_DISCONNECT_ERROR:
            sj3_warning("sj3_close: server internal error");
            break;
        case SJ3_NOT_CONNECTED:
            sj3_warning("sj3_close: already disconnected to server");
            break;
        case SJ3_NOT_OPENED_MDICT:
            sj3_warning("sj3_close: main dictionaries are not opend");
            break;
        case SJ3_NOT_OPENED_UDICT:
            sj3_warning("sj3_close: user dictionaries are not opend");
            break;
        case SJ3_NOT_OPENED_STUDY:
            sj3_warning("sj3_close: files for study are not opend");
            break;
        case SJ3_CLOSE_MDICT_ERROR:
            sj3_warning("sj3_close: can't close main dictionaries");
            break;
        case SJ3_CLOSE_UDICT_ERROR:
            sj3_warning("sj3_close: can't close user dictionaries");
            break;
        case SJ3_CLOSE_STUDY_ERROR:
            sj3_warning("sj3_close: can't close files for study");
            break;
        default:
            sj3_warning("sj3_close: unknown error");
            break;
    }
}

#define Iskana(ks) (((unsigned)(ks) & 0x0f00) == 0x0400)

/*
 *  sj3_conv_key() main dispatch routine of sj3func
 */
SJ3_EVENT
sj3_conv_key(buf, ev)
    sj3Buf              buf;
    XKeyPressedEvent    *ev;
{
    register char           *trbuf;
#ifdef LWCHAR
    wchar                   swc[YBUFSIZ];
#endif /* LWCHAR */
    wchar_t                 lwc[YBUFSIZ];
    wchar                   tyomi[YBUFSIZ];
    unsigned char           *tybuf;
    unsigned char           ttybuf[YBUFSIZ];
    unsigned char           kanabuf[YBUFSIZ];
    KeySym                  ks;
    struct sj3keytable      *keytp;
    unsigned long           modemask;
    register int            nbytes, len,    i;
    SJ3_EVENT               ret = KEY_TEXT_CHANGE;


    /* 文字列及び KeySym に変換する */

    nbytes = XmuLookupKana (ev, buf->strbuf, RBUFSIZ, &ks, NULL);

    /* KeySym とファンクションのテーブルをサーチして合致するものが  */
    /* あればかな->漢字変換、変換終了等のファンクションを実行する   */

    modemask = ev->state & 0xff;
    for (keytp = buf->key; keytp != NULL; keytp = keytp->next) {
                                    /* KeySym */
        if (ks == keytp->ksym &&
                                    /* Modifier Key */
                (keytp->modmask & ~AllModeMask) == modemask &&
                                    /* Conversion mode */
                (keytp->modmask & buf->convmode) == buf->convmode) {
                                    /* Do function  */
            return (*(keytp->func))(buf, ks);
        }
    }
    /* XmuLookupKana でテキストに変換できなかった場合は NULL 
       を返してなにもしない。 */
    if (!nbytes || buf->convmode == ConvModeMask)
        return KEY_NULL;

    if (buf->convmode  != InputModeMask) {
        if (buf->flushiconv) {
			if (buf->n_roma) {
				i = nbytes;
				trbuf = buf->strbuf;
				while (i--) {
					*(trbuf - buf->n_roma) = *trbuf;
					trbuf++;
				}
				*(trbuf - buf->n_roma) = '\0';
				buf->strbuf = buf->strbuf - buf->n_roma;
				buf->n_roma = 0;
			}
			*buf->oldstr = '\0';
            *buf->yomi = '\0';
            buf->ybuf = buf->yomi;
            *buf->w_yomi = '\0';
            buf->w_ybuf = buf->w_yomi;
            ret |= KEY_TEXT_FLUSH;
        } else {
            buf->convmode = InputModeMask;
        }
    } else {
        buf->segnum = 1;
    }

    /* LookupString の結果は NULL terminate していないので NULL */
    /* terminate させる */

    trbuf = buf->strbuf;
    i = nbytes;
    while(i--) {
        if iscntrl(*trbuf++)
            return KEY_NULL;
    }
    *trbuf = '\0';

    switch (buf->curmode) {
    case MODE_HIRA:
    case MODE_ZKATA:
    case MODE_HKATA:
        if (Iskana(ks)) {
        /* 半角カナの直接入力の場合 */
            if (buf->curmode == MODE_HIRA) {
#ifdef USE_HANTOZEN
                sj3_hantozen(ttybuf, buf->strbuf);
#else
                sj3_hkatatohira(buf->hktable, buf->zhtable,
                            ttybuf, buf->strbuf);
#endif
            } else if (buf->curmode == MODE_ZKATA) {
#ifdef USE_HANTOZEN
                sj3_hantozen(kanabuf, buf->strbuf);
                sj3_hiratokata(ttybuf, kanabuf);
#else
                sj3_hkatatozkata(buf->hktable, buf->zhtable,
                            ttybuf, buf->strbuf);
#endif
            } else {
                strcpy(ttybuf, buf->strbuf);
            }
            buf->strbuf= buf->str;
            *buf->oldstr= '\0';
            buf->n_roma = 0;
            strcpy(buf->ybuf, ttybuf);
            while (*buf->ybuf != '\0')
                buf->ybuf++;
            len = mbstowcs(lwc, ttybuf, strlen(ttybuf)); 
            goto kana;
        }

        /* ROMA->かな 変換を行う    */

#ifdef USE_RKCONV
        if ((buf->len = sj3_rkconv((unsigned char *)buf->str, kanabuf)) > 0) {
#else /* USE_RKCONV */
        if ((buf->len = sj3_romatozkana(buf->rktable, 
                    (unsigned char *)buf->str, kanabuf)) > 0) {
#endif /* USE_RKCONV */

        /* ROMA->かな 変換が成功した場合    */

            while (buf->n_roma--) {
                buf->ybuf--;
                buf->w_ybuf--;
            }
            strcpy(buf->oldstr, buf->ybuf);
            if (buf->curmode == MODE_HIRA) {
                strcpy(buf->ybuf, kanabuf);
                buf->oldlen = strlen(kanabuf);
            } else if (buf->curmode == MODE_ZKATA) {
#ifdef USE_HANTOZEN
                sj3_hiratokata(buf->ybuf, kanabuf);
#else
                sj3_hiratozkata(buf->hktable, buf->zhtable, buf->ybuf, kanabuf);
#endif
                buf->oldlen = strlen(buf->ybuf);
            } else {
#ifdef USE_HANTOZEN
                sj3_zentohan(buf->ybuf, kanabuf);
#else
                sj3_zkanatohkata(buf->hktable, buf->zhtable,
                                buf->ybuf, kanabuf);
#endif
                buf->oldlen = strlen(buf->ybuf);
            }
            tybuf = buf->ybuf;
            while (*buf->ybuf != '\0') {
                buf->ybuf++;
            }
            buf->n_roma = 0;

            /* 変換できない文字が残っていたら(例「っ」に変換される  */
            /* 場合等)かなバッファの最後にその文字を付与する    */

            if (*buf->str != '\0') {
                strcpy(buf->ybuf, buf->str);
                while (*buf->ybuf != '\0') 
                    buf->ybuf++;
                buf->strbuf = buf->str;
                while (*buf->strbuf != '\0') {
                    buf->strbuf++;
                    buf->n_roma++;
                }
            } else {
                buf->strbuf = buf->str;
            }

        } else if (buf->len == 0) {

        /* ROMA->かな 変換の結果が不定の場合 */

            i = nbytes;
            tybuf = buf->ybuf;
            while (i--) {
                *buf->ybuf++ = *buf->strbuf++;
                buf->n_roma++;
            }
            *buf->ybuf = '\0';

        } else {

        /* ROMA->かな 変換の結果が不成功の場合 */
            /* rkbell リソースが on に設定されている場合ベルを鳴らす */
            if (buf->rkbell)
                ret |= KEY_BELL;

            i = nbytes;
            tybuf = buf->ybuf;
            strcpy(buf->oldstr,buf->str);
            while (i--)
                *buf->ybuf++ = *buf->strbuf++;
            *buf->ybuf = '\0';

            /* 最後の１文字を ROMA->かな 変換用の   */
            /* バッファ(rbuf)に残す */

            buf->strbuf = buf->str;
            *buf->strbuf = *(--buf->ybuf);
            buf->ybuf++;
            *(++buf->strbuf) = '\0';
            buf->n_roma = 1;
        }

        /* AlphabetConversion リソースが on だったら半角アルファベットを */
        /* 全角アルファベットに変換する     */

        if (buf->alphaconv && buf->curmode != MODE_HKATA) {
#ifdef USE_HANTOZEN
            sj3_hantozen(ttybuf, tybuf);
#else
            sj3_halphatozkana(buf->hktable, buf->zhtable, ttybuf, tybuf);
#endif
            len = mbstowcs(lwc, ttybuf, strlen(ttybuf)); 
        } else {
            len = mbstowcs(lwc, tybuf, strlen(tybuf)); 
        }
        break;
    case MODE_HALPHA:
    case MODE_ZALPHA:
        if (Iskana(ks)) 
            return (KEY_BELL);
        i = nbytes;
        tybuf = buf->ybuf;
        while (i--)
            *buf->ybuf++ = *buf->strbuf++;
        *buf->ybuf = '\0';
        buf->strbuf = buf->str;
        if (buf->curmode == MODE_ZALPHA) {
#ifdef USE_HANTOZEN
            sj3_hantozen(ttybuf, tybuf);
#else
            sj3_halphatozalpha(buf->hktable, buf->zhtable, ttybuf, tybuf);
#endif
            len = mbstowcs(lwc, ttybuf, strlen(ttybuf)); 
        } else {
            len = mbstowcs(lwc, tybuf, strlen(tybuf)); 
        }
        break;
    case MODE_SJIS:
    case MODE_EUC:
    case MODE_JIS:
    case MODE_KUTEN:
        for (i = 0; i < nbytes; i++) {
            if (!isxdigit(buf->str[i])
                || (buf->curmode == MODE_KUTEN && !isdigit(buf->str[i])))
                return (KEY_BELL);
        }
        if ((buf->len = sj3_code_conv((unsigned char *)buf->str,
                    kanabuf, buf->curmode)) > 0) {
            while (buf->n_roma--) {
                buf->ybuf--;
                buf->w_ybuf--;
            }
            strcpy(buf->oldstr, buf->ybuf);
            strcpy(buf->ybuf, kanabuf);
            buf->oldlen = strlen(kanabuf);
            tybuf = buf->ybuf;
            while (*buf->ybuf != '\0') {
                buf->ybuf++;
            }
            buf->n_roma = 0;
            buf->strbuf = buf->str;
        } else if (buf->len == 0) {
            i = nbytes;
            tybuf = buf->ybuf;
            while (i--) {
                *buf->ybuf++ = *buf->strbuf++;
                buf->n_roma++;
            }
            *buf->ybuf = '\0';
        } else {
            while (buf->n_roma--) {
                buf->ybuf--;
                buf->w_ybuf--;
            }
            *buf->ybuf = '\0';
            *buf->w_ybuf = '\0';
            buf->strbuf = buf->str;
            buf->n_roma = 0;
            buf->pos = buf->w_ybuf - buf->w_yomi;
            if (buf->w_ybuf == buf->w_yomi)
                buf->segnum = 0;
            ret |= KEY_BELL;
            return ret;
        }
        if (buf->alphaconv) {
#ifdef USE_HANTOZEN
            sj3_hantozen(ttybuf, tybuf);
#else
            sj3_halphatozkana(buf->hktable, buf->zhtable, ttybuf, tybuf);
#endif
            len = mbstowcs(lwc, ttybuf, strlen(ttybuf)); 
        } else {
            len = mbstowcs(lwc, tybuf, strlen(tybuf)); 
        }
        break;
    default:
        sj3_warning("Illegal current mode");
        return (KEY_BELL);
    }
kana:
#ifdef LWCHAR
    lwctoswc(swc, lwc, len);
    toeuc(swc, tyomi);
#else /* LWCHAR */
    toeuc(lwc, tyomi);
#endif /* LWCHAR */
    wstrcpy(buf->w_ybuf, tyomi);
    while (*buf->w_ybuf != '\0')
        buf->w_ybuf++;
    buf->pos = buf->w_ybuf - buf->w_yomi;

    return ret;
}

static int
sj3_code_conv(code, kanji, mode)
    register unsigned char      *code;
    register unsigned char      *kanji;
    unsigned long               mode;
{
    int                     len;
    register int            i,  j;
    unsigned register short k;
    unsigned register char  c1, c2, kbuf[4];

    if ((len = strlen(code)) < 4)
        return CONV_UNFIXED;

    k = 0;  
    for (j = 0; j < 4  && code ; code++, j++) {
        if (isdigit(*code))
            i = *code - '0';
        else if (islower(*code))
            i = 10 + *code - 'a';
        else if (isupper(*code))
            i = 10 + *code - 'A';
        else
            return CONV_FAILED;
        kbuf[j] = i;
        k += i << ((3 - j) * 4);
    }
    switch (mode) {
    case MODE_SJIS:
        if (issjis1(c1 = (k >> 8)) && issjis2(c2 = (k & 0xff))) {
            *kanji++ = c1;
            *kanji++ = c2;
            *kanji = '\0';
        } else {
            return CONV_FAILED;
        }
        break;
    case MODE_EUC:
        if (iseuc(k >> 8) && iseuc(k & 0xff)) {
            k = euc2sjis(k);
            *kanji++ = k >> 8;
            *kanji++ = k & 0xff;
            *kanji = '\0';
        } else {
            return CONV_FAILED;
        }
        break;
    case MODE_JIS:
        k = jis2sjis(k);
        if (issjis1(c1 = (k >> 8)) && issjis2(c2 = (k & 0xff))) {
            *kanji++ = c1;
            *kanji++ = c2;
            *kanji = '\0';
        } else {
            return CONV_FAILED;
        }
        break;
    case MODE_KUTEN:
        c1 = kbuf[0] * 10 + kbuf[1];
        c2 = kbuf[2] * 10 + kbuf[3];
        k = (c1 << 8) + c2;
        k = jis2sjis(k + 0x2020);
        if (issjis1(c1 = (k >> 8)) && issjis2(c2 = (k & 0xff))) {
            *kanji++ = c1;
            *kanji++ = c2;
            *kanji = '\0';
        } else {
            return CONV_FAILED;
        }
        break;
    default:
        sj3_warning("Illegal mode");
        return CONV_FAILED;
    }   
        
    return CONV_FIXED;
}

/*
 *  sj3_warning() message warning (limit of 10 args)
 */
/*VARARGS1*/
void
sj3_warning(message, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9)
    String  message;
    String  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9;
{
    (void)fprintf(stderr, "sj3_warning: ");
    if (message && *message) {
       (void)fprintf(stderr, message, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9);
       (void)fprintf(stderr, "\n");
    }
    return;
}

/*
 *  sj3_error() message error and exit (limit of 10 args)
 */
/*VARARGS1*/
void
sj3_error(message, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9)
    String  message;
    String  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9;
{
    (void)fprintf(stderr, "sj3_error: ");
    if (message && *message) {
       (void)fprintf(stderr, message, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9);
       (void)fprintf(stderr, "\n");
    }
    exit(1);
}

static SJ3_EVENT
sj3_henkan(buf)
    sj3Buf  buf;
{
    unsigned char               knjbuf[KANJIBUFSIZ];
    unsigned char               tybuf[KANABUFSIZ];
    SJ3_BUNSETU                 bun[BUNBUFSIZ];

    if (buf->convmode = InputModeMask) {
        if (buf->alphaconv) {
#ifdef USE_HANTOZEN
            sj3_hantozen(tybuf, buf->yomi);
#else
            sj3_halphatozkana(buf->hktable, buf->zhtable, tybuf, buf->yomi);
#endif
            buf->segnum = sj3_getkan(tybuf, bun, knjbuf, KANJIBUFSIZ);
        } else {
            buf->segnum = sj3_getkan(buf->yomi, bun, knjbuf, KANJIBUFSIZ);
        }
        if (buf->segnum  <= 0) {
            if (buf->segnum  < 0) 
                sj3_warning("sj3serv is down. reconnect please");
            else 
                sj3_warning("too long segment");
            buf->segnum = 1;
            return KEY_BELL;
        }
        buf->convmode = UnConvModeMask;
        buf->pos = 0;
        if (buf->beginlastseg)
            buf->curseg = buf->segnum - 1;
        else
            buf->curseg = 1;
        sj3_store_dispbuf(buf, bun);
        return KEY_TEXT_CHANGE;
    } else {
        return KEY_BELL;
    }
}

static int
sj3_store_dispbuf(buf, bun)
    register sj3Buf         buf;
    register SJ3_BUNSETU    *bun;
{
    register int                i,  j;
    register unsigned char      *p, *q;

    for (i = 0; i < buf->segnum; i++,bun++) {
        if (bun->deststr && bun->destlen) {
            j = buf->dispbuf[i].destlen = bun->destlen;
            p = buf->dispbuf[i].deststr;
            q = bun->deststr;
            while (j--)
                *p++ = *q++;
            *p = '\0';
            j = buf->dispbuf[i].srclen = bun->srclen;
            p = buf->dispbuf[i].srcstr;
            q = bun->srcstr;
            if (buf->gakusyuu) {
                buf->changelen[i]= 0;
                buf->dispbuf[i].dcid = bun->dcid;
            }
            buf->cursegmode[i] = checkmode(q);
            while (j--)
                *p++ = *q++;
            *p = '\0';
        } else {
            j = buf->dispbuf[i].srclen = bun->srclen;
            p = buf->dispbuf[i].deststr;
            q = bun->srcstr;
            while (j--)
                *p++ = *q++;
            *p = '\0';
            j = buf->dispbuf[i].srclen = bun->srclen;
            p = buf->dispbuf[i].srcstr;
            q = bun->srcstr;
            if (buf->gakusyuu) {
                buf->changelen[i]= 0;
                buf->dispbuf[i].dcid = bun->dcid;
            }
            buf->cursegmode[i] = checkmode(q);
            while (j--)
                *p++ = *q++;
            *p = '\0';
        }
    }
}

static unsigned long 
checkmode(p)
    register unsigned char  *p;
{
    register unsigned char  c;
    register unsigned short s;

    if ((c = *p++) != '\0') {
        if (issjis1(c) && issjis2(*p)) {
            s = (c << 8) + *p;
            if (iskata(s))
                return (MODE_ZKATA);
            else if (iszalpha(s)) 
                return (MODE_ZALPHA);
            else
                return (MODE_HIRA);
        } else {
            if (iskana(c))
                return (MODE_HKATA);
            else
                return (MODE_HALPHA);
        }
    } else {
        return (MODE_HIRA);
    }
}

void
sj3_free_dispbuf(buf)
    sj3Buf  buf;
{
    register int                i;

    for (i = 0; i < buf->segnum; i++) {
        free (buf->dispbuf[i].deststr);
        free (buf->dispbuf[i].srcstr);
    }
}

static SJ3_EVENT
sj3_saihenkan(buf)
    sj3Buf  buf;
{
    if (buf->convmode == UnConvModeMask) {
        if (buf->cursegmode[buf->curseg] != MODE_HIRA) {
            sj3_conv_mode(buf, buf->dispbuf[buf->curseg].srcstr,
                    buf->dispbuf[buf->curseg].deststr, MODE_HIRA);
            strcpy(buf->dispbuf[buf->curseg].srcstr,
                    buf->dispbuf[buf->curseg].deststr);
            buf->dispbuf[buf->curseg].srclen 
                    = strlen (buf->dispbuf[buf->curseg].srcstr);
            buf->dispbuf[buf->curseg].destlen
                    = buf->dispbuf[buf->curseg].srclen;
            buf->cursegmode[buf->curseg] = MODE_HIRA;
            return KEY_TEXT_CHANGE;
        } else {
            buf->convmode = ConvModeMask;
            return KEY_SELECT_START;
        }
    } else {
            return KEY_BELL;
    }
}

static SJ3_EVENT
sj3_cancel(buf)
    sj3Buf  buf;
{
    buf->segnum = 1;
    sj3_clear_buf(buf);
    buf->pos = 0;
    if (buf->convmode == UnConvModeMask || buf->convmode == InputModeMask) {
        buf->convmode = InputModeMask;
        return (KEY_TEXT_CLEAR);
    } else if (buf->convmode == ConvModeMask) {
        buf->convmode = InputModeMask;
        return (KEY_SELECT_END|KEY_TEXT_CLEAR);
    }
}

static SJ3_EVENT
sj3_muhen(buf)
    sj3Buf  buf;
{
    buf->segnum = 1;
    buf->pos = 0;
    if (buf->convmode == UnConvModeMask) {
        buf->convmode = InputModeMask;
        buf->curmode = buf->muhenmode;
        return (KEY_TEXT_CHANGE);
    } else if (buf->convmode == ConvModeMask) {
        buf->convmode = InputModeMask;
        buf->curmode = buf->muhenmode;
        return (KEY_SELECT_END|KEY_TEXT_CHANGE);
    }
}

static SJ3_EVENT
sj3_kouho_nextp(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask)
        return (KEY_SELECT_NEXTP);
    else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_kouho_prevp(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask)
        return (KEY_SELECT_PREVP);
    else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_kettei(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask) {
        return (KEY_TEXT_FIXED|KEY_TEXT_CHANGE|KEY_SELECT_END);
    } else {
        return (KEY_TEXT_FIXED|KEY_TEXT_CHANGE);
    }
}

static SJ3_EVENT
sj3_return(buf)
    sj3Buf  buf;
{
    if (buf->convmode == InputModeMask) {
        *buf->w_ybuf++ = '\r';
        *buf->w_ybuf = '\0';
        return (KEY_TEXT_FIXED|KEY_TEXT_CHANGE);
    } else if (buf->convmode == UnConvModeMask) {
        strcat(buf->dispbuf[buf->segnum -1].deststr, "\n");
        return (KEY_TEXT_FIXED|KEY_TEXT_CHANGE);
    } else {
        return (KEY_BELL);
    }
}

static SJ3_EVENT
sj3_mode_hira(buf)
    sj3Buf  buf;
{
    SJ3_EVENT       result;

    if (buf->convmode == InputModeMask) {
        if (buf->curmode != MODE_HIRA) {
            buf->curmode = MODE_HIRA;
            return (sj3_mode_clear(buf));
        } else {
            return (KEY_TEXT_NO_CHANGE);
        }
    } else if (buf->convmode == UnConvModeMask) {
        if ((result = sj3_conv_mode(buf, buf->dispbuf[buf->curseg].srcstr, 
                buf->dispbuf[buf->curseg].deststr, MODE_HIRA))
                & KEY_TEXT_CHANGE) {
            buf->cursegmode[buf->curseg] = MODE_HIRA;
            strcpy(buf->dispbuf[buf->curseg].srcstr,
                    buf->dispbuf[buf->curseg].deststr);
            buf->dispbuf[buf->curseg].srclen 
                    = strlen (buf->dispbuf[buf->curseg].srcstr);
            buf->dispbuf[buf->curseg].destlen
                    = buf->dispbuf[buf->curseg].srclen;
        }
        return result;
    } else {
        return (KEY_BELL);
    }
}

static SJ3_EVENT
sj3_mode_halpha(buf)
    sj3Buf  buf;
{
    SJ3_EVENT       result;

    if (buf->convmode == InputModeMask) {
        if (buf->curmode != MODE_HALPHA) {
            buf->curmode = MODE_HALPHA;
            return (sj3_mode_clear(buf));
        } else {
            return (KEY_TEXT_NO_CHANGE);
        }
    } else if (buf->convmode == UnConvModeMask) {
        if ((result = sj3_conv_mode(buf, buf->dispbuf[buf->curseg].srcstr, 
                buf->dispbuf[buf->curseg].deststr, MODE_HALPHA))
                & KEY_TEXT_CHANGE) {
            buf->cursegmode[buf->curseg] = MODE_HALPHA;
            strcpy(buf->dispbuf[buf->curseg].srcstr,
                    buf->dispbuf[buf->curseg].deststr);
            buf->dispbuf[buf->curseg].srclen 
                    = strlen (buf->dispbuf[buf->curseg].srcstr);
            buf->dispbuf[buf->curseg].destlen
                    = buf->dispbuf[buf->curseg].srclen;
        }
        return result;
    } else {
        return (KEY_BELL);
    }
}

static SJ3_EVENT
sj3_mode_zalpha(buf)
    sj3Buf  buf;
{
    SJ3_EVENT       result;

    if (buf->convmode == InputModeMask) {
        if (buf->curmode != MODE_ZALPHA) {
            buf->curmode = MODE_ZALPHA;
            return (sj3_mode_clear(buf));
        } else {
            return (KEY_TEXT_NO_CHANGE);
        }
    } else if (buf->convmode == UnConvModeMask) {
        if ((result = sj3_conv_mode(buf, buf->dispbuf[buf->curseg].srcstr, 
                buf->dispbuf[buf->curseg].deststr, MODE_ZALPHA))
                & KEY_TEXT_CHANGE) {
            buf->cursegmode[buf->curseg] = MODE_ZALPHA;
            strcpy(buf->dispbuf[buf->curseg].srcstr,
                    buf->dispbuf[buf->curseg].deststr);
            buf->dispbuf[buf->curseg].srclen 
                    = strlen (buf->dispbuf[buf->curseg].srcstr);
            buf->dispbuf[buf->curseg].destlen
                    = buf->dispbuf[buf->curseg].srclen;
        }
        return result;
    } else {
        return (KEY_BELL);
    }
}

static SJ3_EVENT
sj3_mode_hkata(buf)
    sj3Buf  buf;
{
    SJ3_EVENT       result;

    if (buf->convmode == InputModeMask) {
        if (buf->curmode != MODE_HKATA) {
            buf->curmode = MODE_HKATA;
            return (sj3_mode_clear(buf));
        } else {
            return (KEY_TEXT_NO_CHANGE);
        }
    } else if (buf->convmode == UnConvModeMask) {
        if ((result = sj3_conv_mode(buf, buf->dispbuf[buf->curseg].srcstr, 
                buf->dispbuf[buf->curseg].deststr, MODE_HKATA))
                & KEY_TEXT_CHANGE) {
            buf->cursegmode[buf->curseg] = MODE_HKATA;
            strcpy(buf->dispbuf[buf->curseg].srcstr,
                    buf->dispbuf[buf->curseg].deststr);
            buf->dispbuf[buf->curseg].srclen 
                    = strlen (buf->dispbuf[buf->curseg].srcstr);
            buf->dispbuf[buf->curseg].destlen
                    = buf->dispbuf[buf->curseg].srclen;
        }
        return result;
    } else {
        return (KEY_BELL);
    }
}

static SJ3_EVENT
sj3_mode_zkata(buf)
    sj3Buf  buf;
{
    SJ3_EVENT       result;

    if (buf->convmode == InputModeMask) {
        if (buf->curmode != MODE_ZKATA) {
            buf->curmode = MODE_ZKATA;
            return (sj3_mode_clear(buf));
        } else {
            return (KEY_TEXT_NO_CHANGE);
        }
    } else if (buf->convmode == UnConvModeMask) {
        if ((result = sj3_conv_mode(buf, buf->dispbuf[buf->curseg].srcstr, 
                buf->dispbuf[buf->curseg].deststr, MODE_ZKATA))
                & KEY_TEXT_CHANGE) {
            buf->cursegmode[buf->curseg] = MODE_ZKATA;
            strcpy(buf->dispbuf[buf->curseg].srcstr,
                    buf->dispbuf[buf->curseg].deststr);
            buf->dispbuf[buf->curseg].srclen 
                    = strlen (buf->dispbuf[buf->curseg].srcstr);
            buf->dispbuf[buf->curseg].destlen
                    = buf->dispbuf[buf->curseg].srclen;
        }
        return result;
    } else {
        return (KEY_BELL);
    }
}

static SJ3_EVENT
sj3_conv_mode(buf, preseg, postseg, postmode)
    sj3Buf          buf;
    unsigned char   *preseg,    *postseg;
    unsigned long   postmode;
{
    SJ3_EVENT       result = KEY_NULL;
    unsigned char   tmp1[KANABUFSIZ],   tmp2[KANABUFSIZ];

    switch (postmode) {
    case MODE_HIRA:
#ifdef USE_HANTOZEN
        sj3_zentohan(tmp1, preseg);
        sj3_rkconvs(buf, tmp2, tmp1);
        sj3_hantozen(tmp1, tmp2);
        sj3_katatohira(postseg, tmp1);
#else /* USE_HANTOZEN */
        sj3_zalphatohalpha(buf->hktable, buf->zhtable, tmp1, preseg);
        sj3_rkconvs(buf, tmp2, tmp1);
        sj3_hkatatohira(buf->hktable, buf->zhtable, tmp1, tmp2);
        sj3_zkatatohira(buf->hktable, buf->zhtable, postseg, tmp1);
#endif /* USE_HANTOZEN */
        result = KEY_TEXT_CHANGE;
        break;

    case MODE_ZKATA:
#ifdef USE_HANTOZEN
        sj3_zentohan(tmp1, preseg);
        sj3_rkconvs(buf, tmp2, tmp1);
        sj3_hantozen(tmp1, tmp2);
        sj3_hiratokata(postseg, tmp1);
#else /* USE_HANTOZEN */
        sj3_zalphatohalpha(buf->hktable, buf->zhtable, tmp1, preseg);
        sj3_rkconvs(buf, tmp2, tmp1);
        sj3_hkatatozkata(buf->hktable, buf->zhtable, tmp1, tmp2);
        sj3_hiratozkata(buf->hktable, buf->zhtable, postseg, tmp1);
#endif /* USE_HANTOZEN */
        result = KEY_TEXT_CHANGE;
        break;

    case MODE_HKATA:
#ifdef USE_HANTOZEN
        sj3_zentohan(tmp1, preseg);
        sj3_rkconvs(buf, tmp2, tmp1);
        sj3_zentohan(postseg, tmp2);
#else /* USE_HANTOZEN */
        sj3_zalphatohalpha(buf->hktable, buf->zhtable, tmp1, preseg);
        sj3_rkconvs(buf, tmp2, tmp1);
        sj3_zkanatohkata(buf->hktable, buf->zhtable, postseg, tmp2);
#endif /* USE_HANTOZEN */
        result = KEY_TEXT_CHANGE;
        break;

    case MODE_HALPHA:
#ifdef USE_HANTOZEN
        result = KEY_BELL;
#else /* USE_HANTOZEN */
        sj3_hkatatohira(buf->hktable, buf->zhtable, tmp1, preseg);
        sj3_zkanatohalpha(buf->rktable, buf->hktable, buf->zhtable,
                                postseg, tmp1);
        result = KEY_TEXT_CHANGE;
#endif /* USE_HANTOZEN */
        break;

    case MODE_ZALPHA:
#ifdef USE_HANTOZEN
        result = KEY_BELL;
#else /* USE_HANTOZEN */
        sj3_hkatatohira(buf->hktable, buf->zhtable, tmp1, preseg);
        sj3_zkanatohalpha(buf->rktable, buf->hktable, buf->zhtable, tmp2, tmp1);
        sj3_halphatozalpha(buf->hktable, buf->zhtable, postseg, tmp2);
        result = KEY_TEXT_CHANGE;
#endif /* USE_HANTOZEN */
        break;

    case MODE_SJIS:
    case MODE_EUC:
    case MODE_JIS:
    case MODE_KUTEN:
    default:
        result = KEY_BELL;
        break;
    }
    return (result);
}

#ifdef USE_HANTOZEN
static void
sj3_hiratokata(kata, hira)
    register unsigned char  *kata,  *hira;
{
    register unsigned short s;
    register unsigned char  c;

    while ((c = *hira++) != '\0') {
        if (issjis1(c) && issjis2(*hira)) {
            s = (c << 8) + *hira;
            if (ishira(s)) {
                if (s > 0x82dd)
                    s += 0xa2;
                else
                    s += 0xa1;
            }
            *kata++ = s >> 8;
            *kata++ = s & 0xff;
            hira++;
        } else
            *kata++ = c;
    }
    *kata = '\0';
}

static void
sj3_katatohira(hira, kata)
    register unsigned char  *hira,  *kata;
{
    register unsigned short s;
    register unsigned char  c;

    while ((c = *kata++) != '\0') {
        if (issjis1(c) && issjis2(*kata)) {
            s = (c << 8) + *kata;
            if (iskata(s)) {
                if (s > 0x837f) {
                    if ( s < 0x8394)
                        s -= 0xa2;
                    else if ( s < 0x8395)
                        s = 0x82d4;
                    else if ( s < 0x8396)
                        s = 0x82a9;
                    else 
                        s = 0x82af;
                } else
                    s -= 0xa1;
            }
            *hira++ = (s >> 8) & 0xff;
            *hira++ = s & 0xff;
            kata++;
        } else
            *hira++ = c;
    }
    *hira = '\0';
}
#endif /* USE_HANTOZEN */

static void
sj3_rkconvs (buf, yomi, alpha)
    sj3Buf          buf;
    unsigned char   *yomi;
    unsigned char   *alpha;
{
    register unsigned char  *p,     *q,     *yp;
    int                     i;
    int                     rlen,   len;            
    register unsigned short s;
    unsigned char           rbuf[RBUFSIZ];      
    unsigned char           ybuf[YBUFSIZ];  
    
    yp = yomi;
    p = rbuf;
    rlen = 0;
    i = 0;
    while (*alpha != '\0') {
        if (issjis1(*alpha) && issjis2(*(alpha + 1))) {
            *yp++ = *alpha++;
            *yp++ = *alpha++;
            continue;
        } else if (iskana(*alpha)) {
            *yp++ = *alpha++;
            continue;
        }
        *p++ = *alpha++;
        *p = '\0';
        rlen++;
        while(1) {
#ifdef USE_RKCONV
            if ((len = sj3_rkconv(rbuf, ybuf)) > 0) {
#else /* USE_RKCONV */
            if ((len = sj3_romatozkana(buf->rktable, rbuf, ybuf)) > 0) {
#endif /* USE_RKCONV */
                q = ybuf;
                while (*q != '\0') {
                    *yp++ = *q++;
                }
                i = rlen;
                rlen = strlen(rbuf);
                p = rbuf;
                p += rlen;
                if (len >= i)
                    break;
            } else if (len < 0) {
                *yp++ = rbuf[0];
                p = rbuf;
                q = (p + 1);
        if (*q == '\0') {
            *p = *q;
                    rlen--;
                    break;
                } 
                while(*q != '\0')
                    *p++ = *q++;
                *p = '\0';
                rlen--;
            } else {
                break;
            }
        }
    }
    if (rlen > 0) {
        *p++ = '\n';
        *p = '\0';
        rlen++;
        while(rbuf[0] != '\0') {
#ifdef USE_RKCONV
            if ((len = sj3_rkconv(rbuf, ybuf)) > 0) {
#else /* USE_RKCONV */
            if ((len = sj3_romatozkana(buf->rktable, rbuf, ybuf)) > 0) {
#endif /* USE_RKCONV */
                q = ybuf;
                while (*q != '\0') {
                    *yp++ = *q++;
                }
            
                rlen = strlen(rbuf);
                p = rbuf;
                p += rlen;
            } else {
                *yp++ = rbuf[0];
                p = rbuf;
                q = (p + 1);
                while(*q != '\0')
                    *p++ = *q++;
                *p = '\0';
            }
        }
        yp--;
    }
    *yp = '\0';
}

static SJ3_EVENT
sj3_mode_toggle(buf)
    sj3Buf      buf;
{
    unsigned long   backmode;
    SJ3_EVENT       ret;

    if (buf->curmode == UnConvModeMask && buf->dotoggle) {
        backmode = buf->cursegmode[buf->curseg];
        if (buf->togglemode == MODE_HIRA)
            ret = sj3_mode_zkata(buf);
        else if (buf->togglemode == MODE_ZKATA)
            ret = sj3_mode_hkata(buf);
        else if (buf->togglemode == MODE_HKATA)
            ret = sj3_mode_zalpha(buf);
        else if (buf->togglemode == MODE_ZALPHA)
            ret = sj3_mode_halpha(buf);
        else if (buf->togglemode == MODE_HALPHA)
            ret = sj3_mode_hira(buf);
        else
            ret = sj3_mode_hira(buf);
        buf->togglemode = backmode;
        return(ret);
    } else 
        return(KEY_BELL);
}

static SJ3_EVENT
sj3_mode_code(buf)
    sj3Buf  buf;
{
    if (buf->convmode == InputModeMask) {
        if (buf->curmode == MODE_SJIS) {
            buf->curmode = MODE_EUC;
            return (sj3_mode_clear(buf));
        } else if (buf->curmode == MODE_EUC) {
            buf->curmode = MODE_JIS;
            return (sj3_mode_clear(buf));
        } else if (buf->curmode == MODE_JIS) {
            buf->curmode = MODE_KUTEN;
            return (sj3_mode_clear(buf));
        } else if (buf->curmode == MODE_KUTEN) {
            buf->curmode = MODE_SJIS;
            return (sj3_mode_clear(buf));
        } else {
            buf->curmode = buf->defcode;
            return (sj3_mode_clear(buf));
        }
    } else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_mode_clear(buf)
    sj3Buf  buf;
{
    *buf->str = '\0';
    buf->strbuf = buf->str;
    buf->n_roma = 0;
    *buf->oldstr = '\0';
    buf->oldlen = 0;
    buf->len = 0;
    if (buf->flushaconv)
        return (KEY_MODE_CHANGE|KEY_TEXT_CLEAR);
    else
        return (KEY_MODE_CHANGE);
}

static SJ3_EVENT
sj3_mode_rotate(buf)
    sj3Buf  buf;
{
    if (buf->convmode == InputModeMask) {
        if (buf->curmode == MODE_HIRA)
            return(sj3_mode_zkata(buf));
        else if (buf->curmode == MODE_ZKATA)
            return(sj3_mode_hkata(buf));
        else if (buf->curmode == MODE_HKATA)
            return(sj3_mode_zalpha(buf));
        else if (buf->curmode == MODE_ZALPHA)
            return(sj3_mode_halpha(buf));
        else if (buf->curmode == MODE_HALPHA)
            return(sj3_mode_hira(buf));
        else
            return(sj3_mode_hira(buf));
    } else if (buf->convmode == UnConvModeMask) {
        if (buf->cursegmode[buf->curseg] == MODE_HIRA)
            return(sj3_mode_zkata(buf));
        else if (buf->cursegmode[buf->curseg] == MODE_ZKATA)
            return(sj3_mode_hkata(buf));
        else if (buf->cursegmode[buf->curseg] == MODE_HKATA)
            return(sj3_mode_zalpha(buf));
        else if (buf->cursegmode[buf->curseg] == MODE_ZALPHA)
            return(sj3_mode_halpha(buf));
        else if (buf->cursegmode[buf->curseg] == MODE_HALPHA)
            return(sj3_mode_hira(buf));
        else
            return(sj3_mode_hira(buf));
    } else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_select(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask)
        return (KEY_SELECT_END);
    else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_kouho_right(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask)
        return (KEY_SELECT_RIGHT);
    else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_kouho_left(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask)
        return (KEY_SELECT_LEFT);
    else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_kouho_up(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask)
        return (KEY_SELECT_UP);
    else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_kouho_down(buf)
    sj3Buf  buf;
{
    if (buf->convmode == ConvModeMask)
        return (KEY_SELECT_DOWN);
    else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_forward(buf)
    sj3Buf  buf;
{
    if (buf->convmode == UnConvModeMask || buf->convmode == InputModeMask) {
        if (buf->curseg < buf->segnum - 1)
            buf->curseg++;
        else 
            buf->curseg = buf->segnum - 1;
        return KEY_TEXT_CHANGE;
    } else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_backward(buf)
    sj3Buf  buf;
{
    if (buf->convmode == UnConvModeMask || buf->convmode == InputModeMask) {
        if (buf->curseg > 0)
            buf->curseg--;
        else 
            buf->curseg = 0;
        return KEY_TEXT_CHANGE;
    } else
        return (KEY_BELL);
}

static SJ3_EVENT
sj3_expand(buf)
    sj3Buf  buf;
{
    int                     nextseg = buf->curseg + 1;
    register int            i;
    int                     value;
    register unsigned char  *p, *q, *r;
    unsigned char           tmpbuf[KANJIBUFSIZ];
    unsigned char           backbuf[KANJIBUFSIZ];
    unsigned char           backbuf2[KANJIBUFSIZ];
    SJ3_BUNSETU             bun[BUNBUFSIZ];

    if (buf->convmode != UnConvModeMask)
        return (KEY_BELL);
    /*
     *  現文節が最後の文節の時は拡張できない。
     */
    if (nextseg >= buf->segnum)
        return (KEY_TEXT_NO_CHANGE|KEY_BELL);

    p = buf->dispbuf[nextseg].srcstr;
    q = backbuf;
    /*
     *  次文節の頭の１文字を一時バッファにコピーする。
     */
    if (issjis1(*p) && issjis2(*(p + 1))) {
        *q++ = *p++;
        *q++ = *p++;
        *q = '\0';
        buf->dispbuf[nextseg].srclen -= 2;
        if (buf->expandmconv &&
                buf->cursegmode[buf->curseg] != buf->cursegmode[nextseg]) {
            sj3_conv_mode(buf, backbuf, backbuf2, buf->cursegmode[buf->curseg]);
            strcpy(backbuf, backbuf2);
            buf->dispbuf[buf->curseg].srclen += strlen(backbuf);
        } else {
            buf->dispbuf[buf->curseg].srclen += 2;
        }
    } else {
        *q++ = *p++;
        *q = '\0';
        buf->dispbuf[nextseg].srclen--;
        if (buf->expandmconv && 
                buf->cursegmode[buf->curseg] != buf->cursegmode[nextseg]) {
            sj3_conv_mode(buf, backbuf, backbuf2, buf->cursegmode[buf->curseg]);
            strcpy(backbuf, backbuf2);
            buf->dispbuf[buf->curseg].srclen += strlen(backbuf);
        } else {
            buf->dispbuf[buf->curseg].srclen++;
        }
    }
    if (*p == '\0' || buf->dispbuf[nextseg].srclen == 0) {
        /*
         *  次文節が１文字しかなかった場合、文節数を１つ減じ
         *  更に、次文節が最後の文節でないときはその領域を最後に移す。
         */
        buf->segnum--;
        if (nextseg < buf->segnum) {
            q = buf->dispbuf[nextseg].srcstr;
            r = buf->dispbuf[nextseg].deststr;
            for ( i = nextseg; i < buf->segnum; i++) {
                buf->dispbuf[i].srcstr = buf->dispbuf[i + 1].srcstr;
                buf->dispbuf[i].srclen = buf->dispbuf[i + 1].srclen;
                buf->dispbuf[i].deststr = buf->dispbuf[i + 1].deststr;
                buf->dispbuf[i].destlen = buf->dispbuf[i + 1].destlen;
                if (buf->gakusyuu)
                    buf->dispbuf[i].dcid = buf->dispbuf[i + 1].dcid;
                buf->cursegmode[i] = buf->cursegmode[i + 1];
            }
            buf->dispbuf[buf->segnum].srcstr = q;
            buf->dispbuf[buf->segnum].deststr = r;
        }
    } else {
        /*
         *  次文節が２文字以上あった場合は次文節の頭の１文字を減じ
         *  再変換する。
         */
        strcpy(buf->dispbuf[nextseg].srcstr, p);
        value = sj3_getkan(buf->dispbuf[nextseg].srcstr,
            bun, tmpbuf, KANJIBUFSIZ);
        if (value > 0) {
            strcpy(buf->dispbuf[nextseg].deststr, tmpbuf);
        } else {
            if (value < 0) 
                sj3_warning("sj3serv is down. reconnect please");
            else 
                sj3_warning("too long segment");
            strcpy(buf->dispbuf[nextseg].deststr, buf->dispbuf[nextseg].srcstr);
        }
        buf->dispbuf[nextseg].destlen = strlen(buf->dispbuf[nextseg].deststr);
    }
    /*
     *  現文節の最後に次文節の１文字を足し、現文節を再変換する。
     */
    strcat(buf->dispbuf[buf->curseg].srcstr, backbuf);
    value = sj3_getkan(buf->dispbuf[buf->curseg].srcstr,
        bun, tmpbuf, KANJIBUFSIZ);
    if (value > 0) {
        strcpy(buf->dispbuf[buf->curseg].deststr, tmpbuf);
    } else  {
        if (value < 0) 
            sj3_warning("sj3serv is down. reconnect please");
        else 
            sj3_warning("too long segment");
        strcpy(buf->dispbuf[buf->curseg].deststr,
                        buf->dispbuf[buf->curseg].srcstr);
    }
    buf->dispbuf[buf->curseg].destlen
            = strlen(buf->dispbuf[buf->curseg].deststr);

    if (buf->gakusyuu)
        buf->changelen[buf->curseg] = 1;

    if (value > 0)
        return KEY_TEXT_CHANGE;
    else 
        return (KEY_TEXT_CHANGE|KEY_BELL);
}

static SJ3_EVENT
sj3_shrink(buf)
    sj3Buf  buf;
{
    register int            i;
    register unsigned char  *p, *q, *r;
    unsigned char           backbuf[KANJIBUFSIZ];
    unsigned char           backbuf2[KANJIBUFSIZ];
    unsigned char           tmpbuf[KANJIBUFSIZ];
    int                     nextseg = buf->curseg + 1;
    int                     len,    value;

    int                     erase_seg_flg = 0;
    /*
     *  ShrinkAll リソースの設定が on の時前文節に組み入れるか判断する flg 兼 
     *  文字数記憶変数
     */

    SJ3_BUNSETU             bun[BUNBUFSIZ];

    if (buf->convmode != UnConvModeMask)
        return (KEY_BELL);

    if (!buf->dispbuf[buf->curseg].srclen) 
        return (KEY_TEXT_NO_CHANGE|KEY_BELL);
    
    /*
     *  最後の文節のときは次文節長を初期化し、そうでないときは
     *  次文節の内容を退避する。
     */
    if (nextseg < buf->segnum)
        strcpy(backbuf, buf->dispbuf[nextseg].srcstr);
    else
        buf->dispbuf[nextseg].srclen = 0;

    p = buf->dispbuf[buf->curseg].srcstr;
    q = buf->dispbuf[nextseg].srcstr;

    r = p + buf->dispbuf[buf->curseg].srclen - 1;
    
    if (buf->dispbuf[buf->curseg].srclen > 1) {
        int             zenflg;
        unsigned char   *s;
        /*
         * SJIS 
         */
        for (i = 0, s = p; i < buf->dispbuf[buf->curseg].srclen; i++, s++) {
            if (issjis1(*s) && issjis2(*(s + 1))) {
                s++;
                i++;
                zenflg = 1;
            } else {
                zenflg = 0;
            }
        }
        if (zenflg) {
            /*
             *  現在の文節が１文字しかないときは ShrinkAll リソース
             *  の設定が on ならば最初の文節でないかぎり
             *  現在の文節を前文節の後ろに足す。２文字以上の時は
             *  最後の１文字を次文節に組み入れる。
             */
            if (buf->dispbuf[buf->curseg].srclen != 2
                    || (buf->shrinkall && buf->curseg)) {
                *q++ = *(r - 1);
                *q++ = *r;
                *(--r) = '\0';
                *q = '\0';
                buf->dispbuf[buf->curseg].srclen -= 2;
                if (buf->shrinkmconv &&
                    buf->cursegmode[buf->curseg] != buf->cursegmode[nextseg]) {
                    sj3_conv_mode(buf, buf->dispbuf[nextseg].srcstr,
                        backbuf2, buf->cursegmode[nextseg]);
                    strcpy(buf->dispbuf[nextseg].srcstr, backbuf2);
                    buf->dispbuf[nextseg].srclen +=
                        (len = strlen(buf->dispbuf[nextseg].srcstr));
                    if (!buf->dispbuf[buf->curseg].srclen)
                        erase_seg_flg += len;   
                } else {
                    buf->dispbuf[nextseg].srclen += 2;
                    if (!buf->dispbuf[buf->curseg].srclen)
                        erase_seg_flg += 2; 
                }
            } else {
                return (KEY_TEXT_NO_CHANGE|KEY_BELL);
            }
        } else {
            if (buf->dispbuf[buf->curseg].srclen != 1
                    || (buf->shrinkall && buf->curseg)) {
                *q++ = *r;
                *r = '\0';
                *q = '\0';
                buf->dispbuf[buf->curseg].srclen--;
                if (buf->shrinkmconv &&
                    buf->cursegmode[buf->curseg] != buf->cursegmode[nextseg]) {
                    sj3_conv_mode(buf, buf->dispbuf[nextseg].srcstr,
                        backbuf2, buf->cursegmode[nextseg]);
                    strcpy(buf->dispbuf[nextseg].srcstr, backbuf2);
                    buf->dispbuf[nextseg].srclen +=
                        (len = strlen(buf->dispbuf[nextseg].srcstr));
                    if (!buf->dispbuf[buf->curseg].srclen)
                        erase_seg_flg += len;   
                } else {
                    buf->dispbuf[nextseg].srclen++;
                    if (!buf->dispbuf[buf->curseg].srclen)
                        erase_seg_flg++;    
                }
            } else {
                return (KEY_TEXT_NO_CHANGE|KEY_BELL);
            }
        }
    } else if (buf->shrinkall && buf->curseg) {
        *q++ = *r;
        *r = '\0';
        *q = '\0';
        buf->dispbuf[buf->curseg].srclen--;
        if (buf->shrinkmconv &&
            buf->cursegmode[buf->curseg] != buf->cursegmode[nextseg]) {
            sj3_conv_mode(buf, buf->dispbuf[nextseg].srcstr,
                backbuf2, buf->cursegmode[nextseg]);
            strcpy(buf->dispbuf[nextseg].srcstr, backbuf2);
            buf->dispbuf[nextseg].srclen +=
                (len = strlen(buf->dispbuf[nextseg].srcstr));
            if (!buf->dispbuf[buf->curseg].srclen)
                erase_seg_flg += len;   
        } else {
            buf->dispbuf[nextseg].srclen++;
            if (!buf->dispbuf[buf->curseg].srclen)
                erase_seg_flg++;    
        }
    } else {
        return (KEY_TEXT_NO_CHANGE|KEY_BELL);
    }

    if (buf->shrinkall && buf->curseg && erase_seg_flg) {
        /*
         *  ShrinkAll リソースの設定が on で現文節が最初の文節でなく
         *  １文字しかない場合、次文節の頭にコピーされている現文節を
         *  前文節の後ろに足し、更に文節数を１つ減ずる。
         *  現文節が最後の文節でないときは次文節の退避していた文字列を
         *  元に戻し、現文節の領域を最後に移動する。
         *  そして更に注目する文節を前文節に移し、その文節を再変換する。
         */
        strcat(buf->dispbuf[buf->curseg - 1].srcstr,
                buf->dispbuf[nextseg].srcstr);
        buf->dispbuf[buf->curseg - 1].srclen += erase_seg_flg;
        buf->segnum--;
        if (nextseg <= buf->segnum) {
            strcpy(buf->dispbuf[nextseg].srcstr, backbuf);
            buf->dispbuf[nextseg].srclen -= erase_seg_flg;
            q = buf->dispbuf[buf->curseg].deststr;
            for ( i = buf->curseg; i < buf->segnum; i++) {
                buf->dispbuf[i].srcstr = buf->dispbuf[i + 1].srcstr;
                buf->dispbuf[i].srclen = buf->dispbuf[i + 1].srclen;
                buf->dispbuf[i].deststr = buf->dispbuf[i + 1].deststr;
                buf->dispbuf[i].destlen = buf->dispbuf[i + 1].destlen;
                if (buf->gakusyuu)
                    buf->dispbuf[i].dcid = buf->dispbuf[i + 1].dcid;
                buf->cursegmode[i] = buf->cursegmode[i + 1];
            }
            buf->dispbuf[buf->segnum].srcstr = p;
            buf->dispbuf[buf->segnum].deststr = q;
        }
        buf->curseg--;
    } else {
        /*
         *  最後の文節だったときは文節を１つ増やし、そうでないときは
         *  退避していた文字列を次文節の最後に足す。
         *  更に次文節と現文節を再変換する。
         */
        if (nextseg < buf->segnum) {
            strcat(buf->dispbuf[nextseg].srcstr, backbuf);
        } else {
            buf->segnum++;
            buf->cursegmode[nextseg] = buf->cursegmode[buf->curseg];
            if (buf->gakusyuu)
                buf->dispbuf[nextseg].dcid = buf->dispbuf[buf->curseg].dcid;
        }

        value = sj3_getkan(buf->dispbuf[nextseg].srcstr, bun,
                        tmpbuf, KANJIBUFSIZ);
        if (value > 0) {
            strcpy(buf->dispbuf[nextseg].deststr, tmpbuf);
        } else {
            if (value < 0) 
                sj3_warning("sj3serv is down. reconnect please");
            else 
                sj3_warning("too long segment");
            strcpy(buf->dispbuf[nextseg].deststr, buf->dispbuf[nextseg].srcstr);
        }
        buf->dispbuf[nextseg].destlen = strlen(buf->dispbuf[nextseg].deststr);
    }

    value = sj3_getkan(buf->dispbuf[buf->curseg].srcstr,
            bun, tmpbuf, KANJIBUFSIZ);
    if (value > 0) {
        strcpy(buf->dispbuf[buf->curseg].deststr, tmpbuf);
    } else {
        if (value < 0)
            sj3_warning("sj3serv is down. reconnect please");
        else 
            sj3_warning("too long segment");
        strcpy(buf->dispbuf[buf->curseg].deststr,
                    buf->dispbuf[buf->curseg].srcstr);
    }

    buf->dispbuf[buf->curseg].destlen
            = strlen(buf->dispbuf[buf->curseg].deststr);

    if (buf->gakusyuu)
        buf->changelen[buf->curseg] = 1;

    if (value > 0)
        return KEY_TEXT_CHANGE;
    else 
        return (KEY_TEXT_CHANGE|KEY_BELL);
}

#define ZEN_ALPHA(s)    (s > 0xa1a1 && s < 0xa3fb)

static SJ3_EVENT
sj3_delete(buf)
    sj3Buf  buf;
{
    unsigned char           tybuf[YBUFSIZ];
    wchar_t                 lwc[YBUFSIZ];
#ifdef LWCHAR
    wchar                   swc[YBUFSIZ];
#endif /* LWCHAR */
    int                     len;
    SJ3_EVENT               result = KEY_NULL;
    unsigned char           *p, *q;
    int                     nextseg = buf->curseg + 1;
    register int            i;

    if (buf->convmode == ConvModeMask)
        result |= KEY_SELECT_END;

    if (buf->convmode == InputModeMask ||
            (buf->convmode == UnConvModeMask && buf->delchange)) {
        if (buf->convmode != InputModeMask) {
            buf->convmode = InputModeMask;
            buf->curseg = 0;
            buf->segnum = 1;
        }
        if (buf->w_ybuf != buf->w_yomi) {
            if (buf->backdisplay) {
                if (buf->n_roma) {
                    buf->w_ybuf--;
                    buf->pos--;
                    buf->ybuf--;
                    *buf->ybuf = '\0';
                    buf->strbuf--;
                    *buf->strbuf = '\0';
                    buf->n_roma--;
                    *buf->w_ybuf = '\0';
                    if (*buf->oldstr != '\0' && buf->len < 0) {
                        strcpy(buf->str, buf->oldstr);
                        buf->n_roma = 1;
                        buf->strbuf = buf->str;
                        buf->strbuf++;
                        *buf->strbuf = '\0';
                        *buf->oldstr = '\0';
                    }
                } else  {
                    if (*buf->oldstr != '\0'
                            && iseuc((*(buf->w_ybuf - 1) >> 8) & 0xff)
                            && iseuc(*(buf->w_ybuf - 1) & 0xff)) {
                        buf->ybuf -= buf->oldlen;
                        buf->w_ybuf -= (buf->oldlen / 2);
                        buf->pos -= (buf->oldlen / 2);
                        strcpy(buf->ybuf, buf->oldstr);
                        strcpy(buf->str, buf->oldstr);
                        buf->n_roma = strlen(buf->oldstr);
                        buf->ybuf += buf->n_roma;
                        buf->strbuf += buf->n_roma;
                        if (buf->alphaconv) {
#ifdef USE_HANTOZEN
                            sj3_hantozen(tybuf, buf->oldstr);
#else
                            sj3_halphatozkana(buf->hktable, buf->zhtable,
                                        tybuf, buf->oldstr);
#endif
                            len = mbstowcs(lwc, tybuf, strlen(tybuf)); 
                        } else {
                            len = mbstowcs(lwc, buf->oldstr, strlen(buf->oldstr)); 
                        }
                        *buf->oldstr = '\0';
#ifdef LWCHAR
                        lwctoswc(swc, lwc, len);
                        toeuc(swc, buf->w_ybuf);
#else /* LWCHAR */
                        toeuc(lwc, buf->w_ybuf);
#endif /* LWCHAR */
                        while (*buf->w_ybuf != '\0')
                            buf->w_ybuf++;
                        buf->pos = buf->w_ybuf - buf->w_yomi;
                    } else {
                        buf->w_ybuf--;
                        *buf->str = '\0';
                        buf->strbuf = buf->str;
                        if (buf->alphaconv) {
                            if (ZEN_ALPHA(*buf->w_ybuf)
                                || buf->curmode == MODE_HALPHA
                                || buf->curmode == MODE_HKATA)
                                buf->ybuf--;
                            else
                                buf->ybuf -= 2;
                        } else {
                            if (iseuc(*buf->w_ybuf >> 8 & 0xff) &&
                                    iseuc(*buf->w_ybuf & 0xff))
                                buf->ybuf -= 2;
                            else
                                buf->ybuf--;
                        }
                        *buf->ybuf = '\0';
                        *buf->w_ybuf = '\0';
                        buf->pos--;
                    }
                }
            } else {
                buf->w_ybuf--;
                buf->pos--;
                *buf->str = '\0';
                buf->strbuf = buf->str;
                if (buf->n_roma) {
                    buf->ybuf -= buf->n_roma;
                    buf->w_ybuf -= (buf->n_roma - 1);
                    buf->n_roma = 0;
                } else {
                    if (buf->alphaconv) {
                        if (ZEN_ALPHA(*buf->w_ybuf)
                            || buf->curmode == MODE_HALPHA
                            || buf->curmode == MODE_HKATA)
                            buf->ybuf--;
                        else
                            buf->ybuf -= 2;
                    } else {
                        if (iseuc(*buf->w_ybuf >> 8 & 0xff) &&
                                iseuc(*buf->w_ybuf & 0xff))
                            buf->ybuf -= 2;
                        else
                            buf->ybuf--;
                    }
                }
                *buf->ybuf = '\0';
                *buf->w_ybuf = '\0';
            }
            if (buf->w_ybuf == buf->w_yomi) 
                buf->segnum = 0;
            result |= KEY_TEXT_CHANGE;
        } else {
            result |= KEY_TEXT_NO_CHANGE;
        }
    } else if (buf->convmode == UnConvModeMask && buf->segnum) {
        buf->segnum--;
        if (buf->curseg == buf->segnum) {
            /*
             *  最後の文節の時
             */
            buf->curseg--;
        } else {
            p = buf->dispbuf[buf->curseg].srcstr;
            q = buf->dispbuf[buf->curseg].deststr;
            for ( i = buf->curseg; i < buf->segnum; i++) {
                buf->dispbuf[i].srcstr = buf->dispbuf[i + 1].srcstr;
                buf->dispbuf[i].srclen = buf->dispbuf[i + 1].srclen;
                buf->dispbuf[i].deststr = buf->dispbuf[i + 1].deststr;
                buf->dispbuf[i].destlen = buf->dispbuf[i + 1].destlen;
                if (buf->gakusyuu)
                    buf->dispbuf[i].dcid = buf->dispbuf[i + 1].dcid;
                buf->cursegmode[i] = buf->cursegmode[i + 1];
            }
            buf->dispbuf[buf->segnum].srcstr = p;
            buf->dispbuf[buf->segnum].deststr = q;
        }
        if (!buf->segnum) {
            buf->convmode = InputModeMask;
            sj3_clear_buf(buf);
        }
        result |= KEY_TEXT_CHANGE;
    } else {
        result |= KEY_TEXT_NO_CHANGE|KEY_BELL;
    }

    return result;
}

static SJ3_EVENT
sj3_end(buf)
    sj3Buf  buf;
{
/******* must not free this version
    register int        i;
    struct sj3keytable  *keytp, *keytq;
    struct sj3rktable   *rktp,  *rktq;

    keytp = buf->key;
    for (keytp = keytp->next; keytp != NULL; keytp = keytp->next) {
        keytq = keytp;
        free (keytp->prev);
    }
    free (keytq);
#ifndef USE_RKCONV
    rktp = buf->rktable;
    while (rktp) {
        rktq = rktp;
        rktp = rktp->next;
        free (rktq->roma);
        free (rktq->yomi);
        free (rktq->str);
        free (rktq);
    }
#endif 
    for (i = 0; i < MODE_NUM; i++) 
        free (buf->mode_str[i]);
*******/
    if (buf->convmode == ConvModeMask)
        return (KEY_TEXT_FIXED|KEY_TEXT_CHANGE|KEY_HENKAN_END);
    else
        return (KEY_TEXT_CHANGE|KEY_HENKAN_END);
}

static SJ3_EVENT
sj3_start(buf)
    sj3Buf  buf;
{
    return (KEY_HENKAN_START);
}

static SJ3_EVENT
sj3_connect(buf)
    sj3Buf  buf;
{
    sj3_clear_buf(buf);
    buf->convmode = InputModeMask;
    return (KEY_SELECT_END|KEY_RECONNECT);
}

static SJ3_EVENT
sj3_symbol_begin(buf)
    sj3Buf  buf;
{
    if (buf->convmode == InputModeMask) {
        buf->convmode = ConvModeMask;
        return (KEY_SYMBOL_START);
    } else if (buf->convmode == UnConvModeMask){
        buf->convmode = ConvModeMask;
        return (KEY_SYMBOL_START|KEY_TEXT_FLUSH);
    } else {
        return (KEY_BELL);
    }
}

wchar *
sj3_get_mode_str(buf)
    sj3Buf  buf;
{
    return(buf->mode_str[buf->curmode]);
}

#define CURSOR_VISIBLE      1
#define CURSOR_UNVISIBLE    0

int
sj3_get_position(buf, curseg, pos)
    sj3Buf      buf;
    Cardinal    *curseg;
    Cardinal    *pos;
{
    if (buf->convmode == InputModeMask) {
        *curseg = 0;
        *pos = buf->pos;
    } else {
        *curseg = buf->curseg;
        *pos = buf->pos;
    }
    return CURSOR_VISIBLE;
}

int
sj3_get_segnum(buf)
    sj3Buf  buf;
{
    return(buf->segnum);
}

wchar *
sj3_get_seg(buf, n, attr)
    sj3Buf      buf;
    Cardinal    n;
    int         *attr;
{
    int                         len;
    wchar_t                     lwc[KANJIBUFSIZ];
#ifdef LWCHAR
    wchar                       swc[KANJIBUFSIZ];
#endif /* LWCHAR */
    wchar                       data[KANJIBUFSIZ];

    if (buf->convmode == InputModeMask) {
        *attr = ICAttrNotConverted;
        if (buf->w_yomi) {
            return (buf->w_yomi);
        } else {
            return (NULL);
        }
    } else {
        len = mbstowcs(lwc, buf->dispbuf[n].deststr, buf->dispbuf[n].destlen);
#ifdef LWCHAR
        lwctoswc(swc, lwc, len);
        toeuc(swc, data);
#else /* LWCHAR */
        toeuc(lwc, data);
#endif /* LWCHAR */
        if (n == buf->curseg)
            *attr = ICAttrConverted|ICAttrCurrentSegment;
        else
            *attr = ICAttrConverted;
        return (data);
    }
}

wchar *
sj3_get_converted(buf)
    sj3Buf  buf;
{
    int                         mlen = 0,   len;
    register int                i;
    unsigned char               knjbuf[KANJIBUFSIZ];
    wchar_t                     lwc[KANJIBUFSIZ];
#ifdef LWCHAR
    wchar                       swc[KANJIBUFSIZ];
#endif /* LWCHAR */
    wchar                       data[KANJIBUFSIZ];

    mlen = 0;
    if (buf->convmode == InputModeMask) {
            return (buf->w_yomi);
    } else {
        knjbuf[0] = '\0';
        if (buf->gakusyuu) {
            for (i = 0; i < buf->segnum - 1; i++) {
                if (buf->changelen[i]) {
                    if ((sj3_gakusyuu2(buf->dispbuf[i].srcstr,
                                buf->dispbuf[i + 1].srcstr,
                                &buf->dispbuf[i + 1].dcid )) < 0) {
                        sj3_warning("sj3serv is down. reconnect please");
                    }
                }
            }
        }
        for (i = 0; i < buf->segnum ; i++) {
            strcat(knjbuf, buf->dispbuf[i].deststr);
            mlen += buf->dispbuf[i].destlen;
        }
        len = mbstowcs(lwc, knjbuf, mlen);
#ifdef LWCHAR
        lwctoswc(swc, lwc, len);
        toeuc(swc, data);
#else /* LWCHAR */
        toeuc(lwc, data);
#endif /* LWCHAR */
        return (data);
    }
}

void
sj3_clear_buf(buf)
    sj3Buf      buf;
{
    register int    i;

    buf->pos = 0;
    buf->curseg = 0;
    buf->segnum = 0;

    *buf->str = '\0';
    buf->strbuf = buf->str;
    *buf->oldstr = '\0';
    *buf->yomi = '\0';
    buf->ybuf = buf->yomi;
    *buf->w_yomi = '\0';
    buf->w_ybuf = buf->w_yomi;
    buf->n_roma = 0;
    buf->oldlen = 0;
    buf->convmode = InputModeMask;
}

void
sj3_flush_buf(buf)
    sj3Buf      buf;
{
    register int    i;

    buf->pos = 0;
    buf->curseg = 0;
    buf->segnum = 1;
    buf->convmode = InputModeMask;
}

void
sj3_init_items(buf)
    sj3Buf      buf;
{
    register int    value;

    if (buf->itemnum > 0) {
        buf->items = (SJ3_DOUON *)calloc(buf->itemnum, sizeof(SJ3_DOUON));
        value = sj3_getdouon(buf->dispbuf[buf->curseg].srcstr, buf->items);
        if (value <= 0) {
            if (value < 0)
                sj3_warning("sj3serv is down. reconnect please");
            strcpy(buf->items[0], buf->dispbuf[buf->curseg].deststr);
        }
    }
}

void
sj3_get_item(buf, n, item)
    sj3Buf              buf;
    int                 n;
    wchar               *item;
{
    int                 len;
    wchar_t             lwc[KANJIBUFSIZ];
#ifdef LWCHAR
    wchar               swc[KANJIBUFSIZ];
#endif /* LWCHAR */

    if (buf->itemnum > 0) {
        len = mbstowcs(lwc, buf->items[n].ddata, buf->items[n].dlen);
    } else {
        len = mbstowcs(lwc, buf->dispbuf[buf->curseg].deststr, 
                buf->dispbuf[buf->curseg].destlen);
    }
#ifdef LWCHAR
    lwctoswc(swc, lwc, len);
    toeuc(swc, item);
#else /* LWCHAR */
    toeuc(lwc, item);
#endif /* LWCHAR */
}

void
sj3_get_symbol(buf, n, symbol)
    sj3Buf              buf;
    int                 n;
    wchar               *symbol;
{
    int                 len;
    wchar_t             lwc[KANJIBUFSIZ];
#ifdef LWCHAR
    wchar               swc[KANJIBUFSIZ];
#endif /* LWCHAR */

    len = mbstowcs(lwc, buf->sbtable[n], strlen(buf->sbtable[n]));
#ifdef LWCHAR
    lwctoswc(swc, lwc, len);
    toeuc(swc, symbol);
#else /* LWCHAR */
    toeuc(lwc, symbol);
#endif /* LWCHAR */
}

void
sj3_get_itemnum(buf, item_num, cur_item)
    sj3Buf      buf;
    int         *item_num;
    int         *cur_item;
{
    *item_num = buf->itemnum = sj3_douoncnt(buf->dispbuf[buf->curseg].srcstr);
    *cur_item = 0;
    if (buf->itemnum < 0)
        sj3_warning("sj3serv is down. reconnect please");
    else if (buf->itemnum == 0)
        *item_num = 1;
}

int
sj3_get_symbolnum(buf)
    sj3Buf      buf;
{
    return(buf->symbolnum);
}

int
sj3_get_cursymbol(buf)
    sj3Buf      buf;
{
    return(buf->cursymbol);
}

int
sj3_set_item(buf, sel_item, changed)
    sj3Buf      buf;
    int         sel_item;
    int         *changed;
{
    if (buf->itemnum > 0) {
        if ((sj3_gakusyuu(&buf->items[sel_item].dcid)) < 0) {
            sj3_warning("sj3serv is down. reconnect please");
        }
        if (strcmp(buf->items[sel_item].ddata, 
                buf->dispbuf[buf->curseg].deststr)) {
            strcpy(buf->dispbuf[buf->curseg].deststr,
                    buf->items[sel_item].ddata);
            buf->dispbuf[buf->curseg].destlen = buf->items[sel_item].dlen;
            *changed = True;
        } else {
            *changed = False;
        }
    } else {
        *changed = False;
    }
    return 0;
}

int
sj3_set_symbol(buf, sel_item, changed)
    sj3Buf      buf;
    int         sel_item;
    int         *changed;
{
#ifdef LWCHAR
    wchar                   swc[YBUFSIZ];
#endif /* LWCHAR */
    wchar_t                 lwc[YBUFSIZ];
    wchar                   tyomi[YBUFSIZ];
    unsigned char           *tybuf;
    int                     len;

    buf->strbuf = buf->str;
    *buf->oldstr = '\0';
    buf->oldlen = 0;
    buf->n_roma = 0;
    if (buf->segnum == 0)
        buf->segnum = 1;
    buf->curseg = 0;

    tybuf = buf->ybuf;
    strcpy(tybuf, buf->sbtable[sel_item]);
    while (*buf->ybuf != '\0') 
        buf->ybuf++;
    len = mbstowcs(lwc, tybuf, strlen(tybuf)); 
#ifdef LWCHAR
    lwctoswc(swc, lwc, len);
    toeuc(swc, tyomi);
#else /* LWCHAR */
    toeuc(lwc, tyomi);
#endif /* LWCHAR */
    wstrcpy(buf->w_ybuf, tyomi);
    while (*buf->w_ybuf != '\0')
        buf->w_ybuf++;
    buf->pos = buf->w_ybuf - buf->w_yomi;

    *changed = True;
    buf->cursymbol = sel_item;
    return 0;
}

void
sj3_item_end(buf)
    sj3Buf      buf;
{
    buf->convmode = UnConvModeMask;
    free (buf->items);
}

void
sj3_symbol_end(buf)
    sj3Buf      buf;
{
    buf->convmode = InputModeMask;
}

static wchar *
toeuc(p, q)
    register wchar  *p, *q;
{
    register wchar  *r;

    if (q == NULL)
        return (wchar *)NULL;
    r = q;
    while (p) {
        if (*p == '\0')
            break;
        if (!(issjis1(*p >> 8) && issjis2(0xff & *p))) {
            *r++ = *p++;
             continue;
        }
        *r++ = sjis2euc(*p++);
    }
    *r = '\0';
    return q;
}

#ifdef LWCHAR

static wchar *
lwctoswc(swc, lwc, len)
    register wchar      *swc;
    register wchar_t    *lwc;
    register int        len;
{
    register i;
    register wchar *s = swc;

    for (i = 0; i < len; i++) {
        if ((*swc++ = *lwc++) == '\0') {
            while (++i < len)
                *swc++ = '\0';
            return(s);
        }
    }
    *swc = '\0';
    return(s);
}

#endif /* LWCHAR */

static int
cmp(s1, s2)
    register char   *s1, *s2;
{
    register char   c1, c2;

    while (*s1 != '\0') {
        c1 = *s1++;
        c2 = *s2++;
        if (tolower(c1) != tolower(c2))
            return(False);
    } 
    if (s2 && *s2 != '\0')
        return(False);
    return(True);
}

/************ unused for this version
sjis2compound (store, n)
    unsigned char   *store;
    int             n;
{
    unsigned char           buff[KANJIBUFSIZ];
    register unsigned char  *s;
    register unsigned char  *t;
    register int            c;
    register int            stat;
    register int            len;
    register int            count;

#define ASCII_STAT  0
#define KANA_STAT   1
#define KANJI_STAT  2

    s = store;
    t = buff;
    stat = ASCII_STAT;
    count = 0;
    for (; n > 0 ; n--, count++) {
        c = *s++;
        if (issjis1 (c)) {
            if (stat != KANJI_STAT) {
                if (stat == KANA_STAT) {
                    len = endkana (t);
                    t += len;
                    count += len;
                }
                len = gotokanji (t);
                t += len;
                count += len;
                stat = KANJI_STAT;
            }
            c <<= 8;
            c |= *s++;
            c = sjis2jis (c);
            *t++ = (c >> 8) & 0x7f;
            *t++ = c & 0x7f;
            count ++;
            n --;
        }
        else if (iskana (c)) {
            if (stat != KANA_STAT) {
                if (stat != ASCII_STAT) {
                    len = gotoascii (t);
                    t += len;
                    count += len;
                    stat = ASCII_STAT;
                }
                len = gotokana (t);
                t += len;
                count += len;
                stat = KANA_STAT;
            }
            *t++ = c;
        }
        else {
            if (stat == KANA_STAT) {
                len = endkana (t);
                t += len;
                count += len;
                stat = ASCII_STAT;
            } else if (stat == KANJI_STAT) {
                len = gotoascii (t);
                t += len;
                count += len;
                stat = ASCII_STAT;
            }
            *t++ = c & 0x7f;
        }
    }
    if (stat == KANA_STAT) {
        len = endkana (t);
        t += len;
        count += len;
        stat = ASCII_STAT;
    } else if (stat == KANJI_STAT) {
        len = gotoascii (t);
        t += len;
        count += len;
        stat = ASCII_STAT;
    }
    strncpy (store, buff, count);
    return (count);
}

#define SEQUENCE_KANJI  "\033$(B"
#define SEQUENCE_KANA   "\033)I"
#define SEQUENCE_KEND   "\033-A"
#define SEQUENCE_ASCII  "\033(B"

static int
gotokanji (s)
    unsigned char   *s;
{
    strcpy (s, SEQUENCE_KANJI);
    return (strlen (SEQUENCE_KANJI));
}

static int
gotokana (s)
    unsigned char   *s;
{
    strcpy (s, SEQUENCE_KANA);
    return (strlen (SEQUENCE_KANA));
}

static int
endkana (s)
    unsigned char   *s;
{
    strcpy (s, SEQUENCE_KEND);
    return (strlen (SEQUENCE_KEND));
}

static int
gotoascii (s)
    unsigned char   *s;
{
    strcpy (s, SEQUENCE_ASCII);
    return (strlen (SEQUENCE_ASCII));
}
***************/
