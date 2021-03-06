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

#include "sj_hinsi.h"

static	struct	hlist {
	int	code;
	char	*str;
} hinsi[] = {
	MEISI_1,	"����",
	MEISI_1,	"���P",
	MEISI_2,	"���Q",
	MEISI_3,	"���R",
	MEISI_4,	"���S",
	MEISI_5,	"���T",
	MEISI_6,	"���U",
	MEISI_7,	"���V",
	MEISI_8,	"���W",
	MEISI_9,	"���X",
	MEISI_10,	"���P�O",
	MEISI_11,	"���P�P",
	MEISI_20,	"���Q�O",
	D_MEISI_1,	"�㖼��",
	D_MEISI_1,	"��P",
	D_MEISI_2,	"��Q",
	D_MEISI_3,	"��R",
	D_MEISI_4,	"��S",
	D_MEISI_5,	"��T",
	D_MEISI_6,	"��U",
	MYOUJI,		"�c��",
	NAMAE,		"���O",
	KIGYOU,		"���",
	TIMEI,		"�n��",
	KEN_KU,		"���^�於",
	KEN_KU,		"����",
	RENTAISI,	"�A�̎�",
	RENTAISI,	"�A��",
	SETUZOKU,	"�ڑ���",
	SETUZOKU,	"�ڑ�",
	KANDOUSI,	"����",
	JOSUUSI,	"������",
	JOSUUSI,	"����",
	SUUSI,		"����",
	SETTOU_1,	"�ړ���",
	SETTOU_1,	"�ړ��P",
	SETTOU_2,	"�ړ��Q",
	SETTOU_3,	"�ړ��R",
	SETTOU_4,	"�ړ��S",
	SETTOU_5,	"�ړ��T",
	SETUBI_1,	"�ڔ���",
	SETUBI_1,	"�ڔ��P",
	SETUBI_2,	"�ڔ��Q",
	SETUBI_3,	"�ڔ��R",
	SETUBI_4,	"�ڔ��S",
	SETUBI_5,	"�ڔ��T",
	SETUBI_6,	"�ڔ��U",
	SETUBI_7,	"�ڔ��V",
	SETUBI_8,	"�ڔ��W",
	SETUBI_9,	"�ڔ��X",
	FUKUSI_1,	"����",
	FUKUSI_1,	"���P",
	FUKUSI_2,	"���Q",
	FUKUSI_3,	"���R",
	FUKUSI_4,	"���S",
	FUKUSI_5,	"���T",
	FUKUSI_6,	"���U",
	FUKUSI_7,	"���V",
	FUKUSI_8,	"���W",
	FUKUSI_9,	"���X",
	JOSUUSI2,	"�����Q",
	KEIYOUSI_1,	"�`�e��",
	KEIYOUSI_1,	"�`�P",
	KEIYOUSI_2,	"�`�Q",
	KEIYOUSI_3,	"�`�R",
	KEIYOUSI_4,	"�`�S",
	KEIYOUSI_5,	"�`�T",
	KEIYOUSI_6,	"�`�U",
	KEIYOUSI_7,	"�`�V",
	KEIYOUSI_8,	"�`�W",
	KEIYOUSI_9,	"�`�X",
	KEIYOUSI_10,	"�`�P�O",
	KEIYOUSI_11,	"�`�P�P",
	KE_DOUSI_1,	"�`�e����",
	KE_DOUSI_1,	"�`���P",
	KE_DOUSI_2,	"�`���Q",
	KE_DOUSI_3,	"�`���R",
	KE_DOUSI_4,	"�`���S",
	KE_DOUSI_5,	"�`���T",
	KE_DOUSI_6,	"�`���U",
	KE_DOUSI_7,	"�`���V",
	KE_DOUSI_8,	"�`���W",
	KE_DOUSI_9,	"�`���X",
	DO_SAHEN,	"���E�T��",
	DO_SAHEN,	"�T��",
	DO_ZAHEN,	"���E�U��",
	DO_ZAHEN,	"�U��",
	DO_1DAN_1,	"���E��i",
	DO_1DAN_1,	"��i�P",
	DO_KAGO_1,	"���E�J��",
	DO_KAGO_1,	"�J�܂P",
	DO_GAGO_1,	"���E�K��",
	DO_GAGO_1,	"�K�܂P",
	DO_SAGO_1,	"���E�T��",
	DO_SAGO_1,	"�T�܂P",
	DO_TAGO_1,	"���E�^��",
	DO_TAGO_1,	"�^�܂P",
	DO_NAGO_1,	"���E�i��",
	DO_NAGO_1,	"�i��",
	DO_BAGO_1,	"���E�o��",
	DO_BAGO_1,	"�o�܂P",
	DO_MAGO_1,	"���E�}��",
	DO_MAGO_1,	"�}�܂P",
	DO_RAGO_1,	"���E����",
	DO_RAGO_1,	"���܂P",
	DO_WAGO_1,	"���E����",
	DO_WAGO_1,	"���܂P",
	DO_1DAN_2,	"��i�Q",
	DO_KAGO_2,	"�J�܂Q",
	DO_GAGO_2,	"�K�܂Q",
	DO_SAGO_2,	"�T�܂Q",
	DO_TAGO_2,	"�^�܂Q",
	DO_BAGO_2,	"�o�܂Q",
	DO_MAGO_2,	"�}�܂Q",
	DO_RAGO_2,	"���܂Q",
	DO_WAGO_2,	"���܂Q",
	DO_KAGO_5,	"�J�܂T",
	DO_GAGO_5,	"�K�܂T",
	DO_SAGO_5,	"�T�܂T",
	DO_TAGO_5,	"�^�܂T",
	DO_BAGO_5,	"�o�܂T",
	DO_MAGO_5,	"�}�܂T",
	DO_RAGO_5,	"���܂T",
	DO_WAGO_5,	"���܂T",
	DO_KAGO_6,	"�J�܂U",
	DO_GAGO_6,	"�K�܂U",
	DO_SAGO_6,	"�T�܂U",
	DO_TAGO_6,	"�^�܂U",
	DO_BAGO_6,	"�o�܂U",
	DO_MAGO_6,	"�}�܂U",
	DO_RAGO_6,	"���܂U",
	DO_WAGO_6,	"���܂U",
	DO_1DAN_3,	"��i�R",
	DO_KAGO_3,	"�J�܂R",
	DO_GAGO_3,	"�K�܂R",
	DO_SAGO_3,	"�T�܂R",
	DO_TAGO_3,	"�^�܂R",
	DO_BAGO_3,	"�o�܂R",
	DO_MAGO_3,	"�}�܂R",
	DO_RAGO_3,	"���܂R",
	DO_WAGO_3,	"���܂R",
	DO_1DAN_4,	"��i�S",
	DO_KAGO_4,	"�J�܂S",
	DO_GAGO_4,	"�K�܂S",
	DO_SAGO_4,	"�T�܂S",
	DO_TAGO_4,	"�^�܂S",
	DO_BAGO_4,	"�o�܂S",
	DO_MAGO_4,	"�}�܂S",
	DO_RAGO_4,	"���܂S",
	DO_WAGO_4,	"���܂S",
	DO_KAGO_7,	"�J�܂V",
	DO_GAGO_7,	"�K�܂V",
	DO_SAGO_7,	"�T�܂V",
	DO_TAGO_7,	"�^�܂V",
	DO_BAGO_7,	"�o�܂V",
	DO_MAGO_7,	"�}�܂V",
	DO_RAGO_7,	"���܂V",
	DO_WAGO_7,	"���܂V",
	DO_KAGO_8,	"�J�܂W",
	DO_GAGO_8,	"�K�܂W",
	DO_SAGO_8,	"�T�܂W",
	DO_TAGO_8,	"�^�܂W",
	DO_BAGO_8,	"�o�܂W",
	DO_MAGO_8,	"�}�܂W",
	DO_RAGO_8,	"���܂W",
	DO_WAGO_8,	"���܂W",
	SP_SA_MI1,	"�T�ϖ��P",
	SP_SA_MI2,	"�T�ϖ��Q",
	SP_SA_YOU,	"�T�ϖ��p",
	SP_SA_SYU,	"�T�ϏI��",
	SP_SA_KAT,	"�T�ω�",
	SP_SA_ME1,	"�T�ϖ��P",
	SP_SA_ME2,	"�T�ϖ��Q",
	SP_KA_MIZ,	"�J�ϖ�",
	SP_KA_YOU,	"�J�ϗp",
	SP_KA_SYU,	"�J�ϏI��",
	SP_KA_KAT,	"�J�ω�",
	SP_KA_MEI,	"�J�ϖ�",
	TEINEI1,	"���J�P",
	TEINEI2,	"���J�Q",
	SP_KA_ONB,	"�J�܉���",
	SP_FUKUSI,	"���ꕛ",
	AISATU,		"���A",
	SP_KEIYOUSI,	"����`�e",
	TANKANJI,	"�P����",
	TANKANJI,	"�P��",
	IKKATU,		"�ꊇ",
	0,		0
};

char	*hns2str(code)
int	code;
{
	struct	hlist	*p;

	for (p = hinsi ; p -> code ; p++) {
		if (p -> code == code)
			return p -> str;
	}

	return "ERROR";
}

int	str2hns(str)
char	*str;
{
	struct	hlist	*p;

	for (p = hinsi ; p -> code ; p++) {
		if (strcmp(p -> str, str) == 0)
			return p -> code;
	}

	return 0;
}

