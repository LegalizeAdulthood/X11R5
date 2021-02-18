/*
 * $Id: rext.h,v 1.2 1991/09/16 21:34:37 ohm Exp $
 */
/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				

/*
 * rk_main.c
 */
extern char *chrcat();
extern letter *ltrgrow();
extern int ltrlen();
extern char *strend();
extern int ltrcmp();
extern letter *ltr1cut();
extern int ltrstrcmp();
extern void BUGreport();

/*
 * rk_alloc.c
 */
extern void malloc_for_modetable_struct();
extern void free_for_modetable_struct();
extern void check_and_realloc_for_modetable_struct();
extern void malloc_for_modesw();
extern void free_for_modesw();
extern void check_and_realloc_for_modesw();
extern void malloc_for_modebuf();
extern void free_for_modebuf();
extern void malloc_for_modenaibu();
extern void realloc_for_modenaibu();
extern void free_for_modenaibu();
extern void malloc_for_hyo();
extern void free_for_hyo();
extern void malloc_for_usehyo();
extern void free_for_usehyo();
extern void malloc_for_hyo_area();
extern void free_for_hyo_area();
extern void malloc_for_hensuudef();
extern void free_for_hensuudef();
extern void check_and_realloc_for_hensuudef();
extern void malloc_for_hensuu();
extern void free_for_hensuu();
extern void check_and_realloc_for_hensuu();
extern void malloc_for_henmatch();
extern void free_for_henmatch();
extern void malloc_for_hyobuf();
extern void free_for_hyobuf();
extern void check_and_realloc_for_hyobuf();
extern void malloc_for_heniki();
extern void free_for_heniki();
extern void check_and_realloc_for_heniki();

/*
 * rk_bltinfn.c
 */
extern letter to_zenalpha();
extern void to_hankata();
extern letter to_zenhira();
extern letter to_zenkata();
void handakuadd();
void dakuadd();
void to_digit();

/*
 * rk_modread.c
 */
int readmode();
int readfnm();
void choosehyo();
int filnamchk();

/*
 * rk_read.c
 */
extern void readdata();
extern void ltr1tostr();
extern letter  get1ltr();
extern letter  unget1ltr();
extern int int_get1ltr();
extern int int_unget1ltr();
extern letter getfrom_dblq();
extern int getfrom_lptr();
extern void de_bcksla();
extern int ltov();
extern letter vtol();
