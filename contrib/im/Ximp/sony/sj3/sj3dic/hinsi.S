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
	MEISI_1,	"–¼Œ",
	MEISI_1,	"–¼‚P",
	MEISI_2,	"–¼‚Q",
	MEISI_3,	"–¼‚R",
	MEISI_4,	"–¼‚S",
	MEISI_5,	"–¼‚T",
	MEISI_6,	"–¼‚U",
	MEISI_7,	"–¼‚V",
	MEISI_8,	"–¼‚W",
	MEISI_9,	"–¼‚X",
	MEISI_10,	"–¼‚P‚O",
	MEISI_11,	"–¼‚P‚P",
	MEISI_20,	"–¼‚Q‚O",
	D_MEISI_1,	"‘ã–¼Œ",
	D_MEISI_1,	"‘ã‚P",
	D_MEISI_2,	"‘ã‚Q",
	D_MEISI_3,	"‘ã‚R",
	D_MEISI_4,	"‘ã‚S",
	D_MEISI_5,	"‘ã‚T",
	D_MEISI_6,	"‘ã‚U",
	MYOUJI,		"•cš",
	NAMAE,		"–¼‘O",
	KIGYOU,		"Šé‹Æ",
	TIMEI,		"’n–¼",
	KEN_KU,		"Œ§^‹æ–¼",
	KEN_KU,		"Œ§‹æ",
	RENTAISI,	"˜A‘ÌŒ",
	RENTAISI,	"˜A‘Ì",
	SETUZOKU,	"Ú‘±Œ",
	SETUZOKU,	"Ú‘±",
	KANDOUSI,	"Š´“®",
	JOSUUSI,	"•”Œ",
	JOSUUSI,	"•”",
	SUUSI,		"”Œ",
	SETTOU_1,	"Ú“ªŒê",
	SETTOU_1,	"Ú“ª‚P",
	SETTOU_2,	"Ú“ª‚Q",
	SETTOU_3,	"Ú“ª‚R",
	SETTOU_4,	"Ú“ª‚S",
	SETTOU_5,	"Ú“ª‚T",
	SETUBI_1,	"Ú”öŒê",
	SETUBI_1,	"Ú”ö‚P",
	SETUBI_2,	"Ú”ö‚Q",
	SETUBI_3,	"Ú”ö‚R",
	SETUBI_4,	"Ú”ö‚S",
	SETUBI_5,	"Ú”ö‚T",
	SETUBI_6,	"Ú”ö‚U",
	SETUBI_7,	"Ú”ö‚V",
	SETUBI_8,	"Ú”ö‚W",
	SETUBI_9,	"Ú”ö‚X",
	FUKUSI_1,	"•›Œ",
	FUKUSI_1,	"•›‚P",
	FUKUSI_2,	"•›‚Q",
	FUKUSI_3,	"•›‚R",
	FUKUSI_4,	"•›‚S",
	FUKUSI_5,	"•›‚T",
	FUKUSI_6,	"•›‚U",
	FUKUSI_7,	"•›‚V",
	FUKUSI_8,	"•›‚W",
	FUKUSI_9,	"•›‚X",
	JOSUUSI2,	"•”‚Q",
	KEIYOUSI_1,	"Œ`—eŒ",
	KEIYOUSI_1,	"Œ`‚P",
	KEIYOUSI_2,	"Œ`‚Q",
	KEIYOUSI_3,	"Œ`‚R",
	KEIYOUSI_4,	"Œ`‚S",
	KEIYOUSI_5,	"Œ`‚T",
	KEIYOUSI_6,	"Œ`‚U",
	KEIYOUSI_7,	"Œ`‚V",
	KEIYOUSI_8,	"Œ`‚W",
	KEIYOUSI_9,	"Œ`‚X",
	KEIYOUSI_10,	"Œ`‚P‚O",
	KEIYOUSI_11,	"Œ`‚P‚P",
	KE_DOUSI_1,	"Œ`—e“®Œ",
	KE_DOUSI_1,	"Œ`“®‚P",
	KE_DOUSI_2,	"Œ`“®‚Q",
	KE_DOUSI_3,	"Œ`“®‚R",
	KE_DOUSI_4,	"Œ`“®‚S",
	KE_DOUSI_5,	"Œ`“®‚T",
	KE_DOUSI_6,	"Œ`“®‚U",
	KE_DOUSI_7,	"Œ`“®‚V",
	KE_DOUSI_8,	"Œ`“®‚W",
	KE_DOUSI_9,	"Œ`“®‚X",
	DO_SAHEN,	"“®EƒT•Ï",
	DO_SAHEN,	"ƒT•Ï",
	DO_ZAHEN,	"“®EƒU•Ï",
	DO_ZAHEN,	"ƒU•Ï",
	DO_1DAN_1,	"“®Eˆê’i",
	DO_1DAN_1,	"ˆê’i‚P",
	DO_KAGO_1,	"“®EƒJŒÜ",
	DO_KAGO_1,	"ƒJŒÜ‚P",
	DO_GAGO_1,	"“®EƒKŒÜ",
	DO_GAGO_1,	"ƒKŒÜ‚P",
	DO_SAGO_1,	"“®EƒTŒÜ",
	DO_SAGO_1,	"ƒTŒÜ‚P",
	DO_TAGO_1,	"“®Eƒ^ŒÜ",
	DO_TAGO_1,	"ƒ^ŒÜ‚P",
	DO_NAGO_1,	"“®EƒiŒÜ",
	DO_NAGO_1,	"ƒiŒÜ",
	DO_BAGO_1,	"“®EƒoŒÜ",
	DO_BAGO_1,	"ƒoŒÜ‚P",
	DO_MAGO_1,	"“®Eƒ}ŒÜ",
	DO_MAGO_1,	"ƒ}ŒÜ‚P",
	DO_RAGO_1,	"“®Eƒ‰ŒÜ",
	DO_RAGO_1,	"ƒ‰ŒÜ‚P",
	DO_WAGO_1,	"“®EƒŒÜ",
	DO_WAGO_1,	"ƒŒÜ‚P",
	DO_1DAN_2,	"ˆê’i‚Q",
	DO_KAGO_2,	"ƒJŒÜ‚Q",
	DO_GAGO_2,	"ƒKŒÜ‚Q",
	DO_SAGO_2,	"ƒTŒÜ‚Q",
	DO_TAGO_2,	"ƒ^ŒÜ‚Q",
	DO_BAGO_2,	"ƒoŒÜ‚Q",
	DO_MAGO_2,	"ƒ}ŒÜ‚Q",
	DO_RAGO_2,	"ƒ‰ŒÜ‚Q",
	DO_WAGO_2,	"ƒŒÜ‚Q",
	DO_KAGO_5,	"ƒJŒÜ‚T",
	DO_GAGO_5,	"ƒKŒÜ‚T",
	DO_SAGO_5,	"ƒTŒÜ‚T",
	DO_TAGO_5,	"ƒ^ŒÜ‚T",
	DO_BAGO_5,	"ƒoŒÜ‚T",
	DO_MAGO_5,	"ƒ}ŒÜ‚T",
	DO_RAGO_5,	"ƒ‰ŒÜ‚T",
	DO_WAGO_5,	"ƒŒÜ‚T",
	DO_KAGO_6,	"ƒJŒÜ‚U",
	DO_GAGO_6,	"ƒKŒÜ‚U",
	DO_SAGO_6,	"ƒTŒÜ‚U",
	DO_TAGO_6,	"ƒ^ŒÜ‚U",
	DO_BAGO_6,	"ƒoŒÜ‚U",
	DO_MAGO_6,	"ƒ}ŒÜ‚U",
	DO_RAGO_6,	"ƒ‰ŒÜ‚U",
	DO_WAGO_6,	"ƒŒÜ‚U",
	DO_1DAN_3,	"ˆê’i‚R",
	DO_KAGO_3,	"ƒJŒÜ‚R",
	DO_GAGO_3,	"ƒKŒÜ‚R",
	DO_SAGO_3,	"ƒTŒÜ‚R",
	DO_TAGO_3,	"ƒ^ŒÜ‚R",
	DO_BAGO_3,	"ƒoŒÜ‚R",
	DO_MAGO_3,	"ƒ}ŒÜ‚R",
	DO_RAGO_3,	"ƒ‰ŒÜ‚R",
	DO_WAGO_3,	"ƒŒÜ‚R",
	DO_1DAN_4,	"ˆê’i‚S",
	DO_KAGO_4,	"ƒJŒÜ‚S",
	DO_GAGO_4,	"ƒKŒÜ‚S",
	DO_SAGO_4,	"ƒTŒÜ‚S",
	DO_TAGO_4,	"ƒ^ŒÜ‚S",
	DO_BAGO_4,	"ƒoŒÜ‚S",
	DO_MAGO_4,	"ƒ}ŒÜ‚S",
	DO_RAGO_4,	"ƒ‰ŒÜ‚S",
	DO_WAGO_4,	"ƒŒÜ‚S",
	DO_KAGO_7,	"ƒJŒÜ‚V",
	DO_GAGO_7,	"ƒKŒÜ‚V",
	DO_SAGO_7,	"ƒTŒÜ‚V",
	DO_TAGO_7,	"ƒ^ŒÜ‚V",
	DO_BAGO_7,	"ƒoŒÜ‚V",
	DO_MAGO_7,	"ƒ}ŒÜ‚V",
	DO_RAGO_7,	"ƒ‰ŒÜ‚V",
	DO_WAGO_7,	"ƒŒÜ‚V",
	DO_KAGO_8,	"ƒJŒÜ‚W",
	DO_GAGO_8,	"ƒKŒÜ‚W",
	DO_SAGO_8,	"ƒTŒÜ‚W",
	DO_TAGO_8,	"ƒ^ŒÜ‚W",
	DO_BAGO_8,	"ƒoŒÜ‚W",
	DO_MAGO_8,	"ƒ}ŒÜ‚W",
	DO_RAGO_8,	"ƒ‰ŒÜ‚W",
	DO_WAGO_8,	"ƒŒÜ‚W",
	SP_SA_MI1,	"ƒT•Ï–¢‚P",
	SP_SA_MI2,	"ƒT•Ï–¢‚Q",
	SP_SA_YOU,	"ƒT•Ï–¢—p",
	SP_SA_SYU,	"ƒT•ÏI‘Ì",
	SP_SA_KAT,	"ƒT•Ï‰¼",
	SP_SA_ME1,	"ƒT•Ï–½‚P",
	SP_SA_ME2,	"ƒT•Ï–½‚Q",
	SP_KA_MIZ,	"ƒJ•Ï–¢",
	SP_KA_YOU,	"ƒJ•Ï—p",
	SP_KA_SYU,	"ƒJ•ÏI‘Ì",
	SP_KA_KAT,	"ƒJ•Ï‰¼",
	SP_KA_MEI,	"ƒJ•Ï–½",
	TEINEI1,	"’š”J‚P",
	TEINEI2,	"’š”J‚Q",
	SP_KA_ONB,	"ƒJŒÜ‰¹•Ö",
	SP_FUKUSI,	"“Áê•›",
	AISATU,		"ˆ¥A",
	SP_KEIYOUSI,	"“ÁêŒ`—e",
	TANKANJI,	"’PŠ¿š",
	TANKANJI,	"’PŠ¿",
	IKKATU,		"ˆêŠ‡",
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

