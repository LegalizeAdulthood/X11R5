/* $Header: sj3conv.h,v 1.1 91/09/28 10:24:04 nao Locked $ */
/*
 * mode convert routines
 * 
 * hira     zenkaku-hiragana
 * zkata    zenkaku-katakana
 * hkata    hankaku-katakana
 * halpha   hankaku-alphabet
 * zalpha   zenkaku-alphabet
 * zkana    zenkaku-hiragana & zenkaku-katakana & zenkaku-alphabet
 * roma     leters of hankaku-alphabet
 */

#ifndef _sj3conv_h
#define _sj3conv_h

extern unsigned char        **sj3_readsb();

#ifndef USE_RKCONV
extern int                  sj3_readrk();
extern int                  sj3_romatozkana();
extern SJ3_RKTB             *sj3_alloc_rktable();
#endif /* USE_RKCONV */

#ifndef USE_HANTOZEN
extern void                 sj3_zkanatohalpha();
#ifndef USE_RKCONV
#endif /* USE_RKCONV */
extern int                  sj3_readhk();
extern int                  sj3_readzh();
extern SJ3_HKTB             *sj3_alloc_hktable();
extern SJ3_ZHTB             *sj3_alloc_zhtable();
extern void                 sj3_hiratozkata();
extern void                 sj3_hiratohkata();
extern void                 sj3_hkatatohira();
extern void                 sj3_hkatatozkata();
extern void                 sj3_zkanatohkata();
extern void                 sj3_zkatatohira();
extern void                 sj3_zkatatohkata();
extern void                 sj3_halphatozalpha();
extern void                 sj3_halphatozkana();
extern void                 sj3_zalphatohalpha();

#endif /* USE_HANTOZEN */

#endif
