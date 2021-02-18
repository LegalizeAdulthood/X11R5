/* $Header: sj3func.h,v 1.1 91/09/28 10:24:08 nao Locked $ */

#ifndef _sj3func_h
#define _sj3func_h

#ifdef sony_news
#ifdef NEWS_OS4
#include <stdlib.h>
#else
#include <X11/Xlib.h>
#endif
#else
#include <stdlib.h>
#endif
#include "sj3def.h"

typedef struct _sj3Buf   *sj3Buf;

extern unsigned long    sj3_conv_key();
extern void             sj3_set_locale();
extern void             sj3_set_user();
extern sj3Buf           sj3_set_sjrc();
extern void             sj3_set_sjrk();
extern void             sj3_set_sjsb();
#ifndef USE_HANTOZEN
extern void             sj3_set_sjhk();
extern void             sj3_set_sjzh();
#endif /* USE_HANTOZEN */
extern int              sj3_initialize();
extern void             sj3_down();
extern void             sj3_reconnect();
extern int              sj3_get_segnum();
extern int              sj3_get_position();
extern wchar *          sj3_get_seg();
extern void             sj3_clear_buf();
extern void             sj3_flush_buf();
extern wchar *          sj3_get_mode_str();
extern wchar *          sj3_get_converted();
extern int              sj3_get_position();
extern int              sj3_get_item();
extern void             sj3_get_itemnum();
extern int              sj3_set_item();
extern int              sj3_get_symbol();
extern int              sj3_get_symbolnum();
extern int              sj3_get_cursymbol();
extern int              sj3_set_symbol();
extern void             sj3_item_end();
extern void             sj3_symbol_end();
extern void             sj3_free_dispbuf();
extern void             sj3_init_items();

#endif
