/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */


#ifndef Xcu_CELL_I_H
#define Xcu_CELL_I_H

#define mix		lw->cell.mix_
#define spread		lw->cell.spread_
#define reduct		lw->cell.reduct_
#define windo		lw->cell.window_

#define irule		lw->cell.irule_
#define arule		lw->cell.arule_
#define save_rule	lw->cell.save_rule_
#define asave_rule	lw->cell.asave_rule_
#define many_per_page	lw->cell.per_col_
#define hmany_per_page	lw->cell.per_row_

#define order_number	lw->cell.order_number_
#define rrule		lw->cell.rule_index_

#define fpick		lw->cell.fpick_

#define masks		lw->cell.masks_
#define mask		lw->cell.mask_

#define rule		lw->cell.rule_
#define nrule		lw->cell.nrule_

#define wrap_left	lw->cell.wrap_left_
#define wrap_right	lw->cell.wrap_right_

#define hmerge		lw->cell.horizontal_merge_
#define vmerge		lw->cell.vertical_merge_

#define many_count	lw->cell.many_count_
#define hmany_count	lw->cell.hmany_count_

#define row		lw->cell.row_
#define row_count	lw->cell.row_count_

#define mask_iter	lw->cell.mask_iter_
#define allrandom	lw->cell.all_random_
#define order		lw->cell.order_
#define rule_number	lw->cell.rule_number_

#define row0		lw->cell.row0_
#define row1		lw->cell.row1_

#endif Xcu_CELL_I_H

