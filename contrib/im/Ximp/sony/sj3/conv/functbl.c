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

#include "sj_rename.h"
#include "sj_typedef.h"

Void	num_type00(), num_type01(), num_type02(), num_type03();
Void	num_type04(), num_type05(), num_type06(), num_type07();
Void	num_type08(), num_type09(), num_type10(), num_type11();
Void	num_type12(), num_type13(), num_type14();
VFunc	cvtnum_func[] = {
	num_type00,	num_type01,	num_type02,	num_type03,
	num_type04,	num_type05,	num_type06,	num_type07,
	num_type08,	num_type09,	num_type10,	num_type11,
	num_type12,	num_type13,	num_type14
};

Uchar	*getkan_hira(), *getkan_kata(), *getkan_knj(), *getkan_ofs();
Uchar	*getkan_norm(), *getkan_none();
UCPFunc	getkan_func[] = {
	getkan_norm,	getkan_norm,	getkan_hira,	getkan_kata,
	getkan_knj,	getkan_ofs,	getkan_norm,	getkan_none,
	getkan_norm,	getkan_norm,	getkan_hira,	getkan_kata,
	getkan_knj,	getkan_ofs,	getkan_norm,	getkan_none
};

Int	setj_norm1(), setj_norm2(), setj_knj(), setj_ofs(), setj_atrb();
IFunc	setjrec_func[] = {
	setj_norm2,	setj_norm2,	setj_norm1,	setj_norm1,
	setj_knj,	setj_ofs,	setj_norm2,	setj_atrb,
	setj_norm2,	setj_norm2,	setj_norm1,	setj_norm1,
	setj_knj,	setj_ofs,	setj_norm2,	setj_atrb
};

Int	hiraknj_atrb(), hiraknj_ofs(), hiraknj_knj(), hiraknj_hask();
Int	hiraknj_kask(), hiraknj_norm(), hiraknj_hira();
IFunc	hiraknj_func[] = {
	hiraknj_hira,	hiraknj_norm,	hiraknj_hask,	hiraknj_kask,
	hiraknj_knj,	hiraknj_ofs,	hiraknj_norm,	hiraknj_atrb,
	hiraknj_hira,	hiraknj_norm,	hiraknj_hask,	hiraknj_kask,
	hiraknj_knj,	hiraknj_ofs,	hiraknj_norm,	hiraknj_atrb
};

Uchar	*makekan_1byte(), *makekan_knj(), *makekan_ofs();
Uchar	*makekan_norm(), *makekan_none();
UCPFunc	makekan_func[] = {
	makekan_norm,	makekan_norm,	makekan_1byte,	makekan_1byte,
	makekan_knj,	makekan_ofs,	makekan_norm,	makekan_none,
	makekan_norm,	makekan_norm,	makekan_1byte,	makekan_1byte,
	makekan_knj,	makekan_ofs,	makekan_norm,	makekan_none
};
