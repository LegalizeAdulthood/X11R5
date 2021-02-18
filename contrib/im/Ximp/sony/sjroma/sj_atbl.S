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
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	sj_atbl.c
 *	hankaku/zenkaku - alphabet/katakana conversion table
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Sat May  9 01:44:52 JST 1987
 *
 *	This file is included Shift-JIS code.
 */
/*
 * $Header: sj_atbl.c,v 4.0 90/11/28 13:03:09 root Rel40 $ SONY;
 */

char *Atable[][4] = {
	" ",	"Å@",	"Å@",	" ",	/*	0	*/
	"!",	"ÅI",	"ÅI",	"!",	/*	1	*/
	"\"",	"Åh",	"Åh",	"\"",	/*	2	*/
	"#",	"Åî",	"Åî",	"#",	/*	3	*/
	"$",	"Åê",	"Åê",	"$",	/*	4	*/
	"%",	"Åì",	"Åì",	"%",	/*	5	*/
	"&",	"Åï",	"Åï",	"&",	/*	6	*/
	"'",	"Åf",	"Åf",	"'",	/*	7	*/
	"(",	"Åi",	"Åi",	"(",	/*	8	*/
	")",	"Åj",	"Åj",	")",	/*	9	*/
	"*",	"Åñ",	"Åñ",	"*",	/*	10	*/
	"+",	"Å{",	"Å{",	"+",	/*	11	*/
	",",	"ÅC",	"ÅA",	"§",	/*	12 sp	*/
	"-",	"Å|",	"Å[",	"∞",	/*	13 sp	*/
	".",	"ÅD",	"ÅB",	"°",	/*	14 sp	*/
	"/",	"Å^",	"Å^",	"/",	/*	15	*/
	"0",	"ÇO",	"ÇO",	"0",	/*	16	*/
	"1",	"ÇP",	"ÇP",	"1",	/*	17	*/
	"2",	"ÇQ",	"ÇQ",	"2",	/*	18	*/
	"3",	"ÇR",	"ÇR",	"3",	/*	19	*/
	"4",	"ÇS",	"ÇS",	"4",	/*	20	*/
	"5",	"ÇT",	"ÇT",	"5",	/*	21	*/
	"6",	"ÇU",	"ÇU",	"6",	/*	22	*/
	"7",	"ÇV",	"ÇV",	"7",	/*	23	*/
	"8",	"ÇW",	"ÇW",	"8",	/*	24	*/
	"9",	"ÇX",	"ÇX",	"9",	/*	25	*/
	":",	"ÅF",	"ÅF",	":",	/*	26	*/
	";",	"ÅG",	"ÅG",	";",	/*	27	*/
	"<",	"ÅÉ",	"ÅÉ",	"<",	/*	28	*/
	"=",	"ÅÅ",	"ÅÅ",	"=",	/*	29	*/
	">",	"ÅÑ",	"ÅÑ",	">",	/*	30	*/
	"?",	"ÅH",	"ÅH",	"?",	/*	31	*/
	"@",	"Åó",	"ÅE",	"•",	/*	32 sp	*/
	"A",	"Ç`",	"Ç`",	"A",	/*	33	*/
	"B",	"Ça",	"Ça",	"B",	/*	34	*/
	"C",	"Çb",	"Çb",	"C",	/*	35	*/
	"D",	"Çc",	"Çc",	"D",	/*	36	*/
	"E",	"Çd",	"Çd",	"E",	/*	37	*/
	"F",	"Çe",	"Çe",	"F",	/*	38	*/
	"G",	"Çf",	"Çf",	"G",	/*	39	*/
	"H",	"Çg",	"Çg",	"H",	/*	40	*/
	"I",	"Çh",	"Çh",	"I",	/*	41	*/
	"J",	"Çi",	"Çi",	"J",	/*	42	*/
	"K",	"Çj",	"Çj",	"K",	/*	43	*/
	"L",	"Çk",	"Çk",	"L",	/*	44	*/
	"M",	"Çl",	"Çl",	"M",	/*	45	*/
	"N",	"Çm",	"Çm",	"N",	/*	46	*/
	"O",	"Çn",	"Çn",	"O",	/*	47	*/
	"P",	"Ço",	"Ço",	"P",	/*	48	*/
	"Q",	"Çp",	"Çp",	"Q",	/*	49	*/
	"R",	"Çq",	"Çq",	"R",	/*	50	*/
	"S",	"Çr",	"Çr",	"S",	/*	51	*/
	"T",	"Çs",	"Çs",	"T",	/*	52	*/
	"U",	"Çt",	"Çt",	"U",	/*	53	*/
	"V",	"Çu",	"Çu",	"V",	/*	54	*/
	"W",	"Çv",	"Çv",	"W",	/*	55	*/
	"X",	"Çw",	"Çw",	"X",	/*	56	*/
	"Y",	"Çx",	"Çx",	"Y",	/*	57	*/
	"Z",	"Çy",	"Çy",	"Z",	/*	58	*/
	"[",	"Åm",	"Åu",	"¢",	/*	59 sp	*/
	"\\",	"Å_",	"Åè",	"\\",	/*	60 sp	*/
	"]",	"Ån",	"Åv",	"£",	/*	61 sp	*/
	"^",	"ÅO",	"ÅO",	"^",	/*	62	*/
	"_",	"ÅQ",	"ÅQ",	"_",	/*	63	*/
	"`",	"Åe",	"Åe",	"`",	/*	64	*/
	"a",	"ÇÅ",	"ÇÅ",	"a",	/*	65	*/
	"b",	"ÇÇ",	"ÇÇ",	"b",	/*	66	*/
	"c",	"ÇÉ",	"ÇÉ",	"c",	/*	67	*/
	"d",	"ÇÑ",	"ÇÑ",	"d",	/*	68	*/
	"e",	"ÇÖ",	"ÇÖ",	"e",	/*	69	*/
	"f",	"ÇÜ",	"ÇÜ",	"f",	/*	70	*/
	"g",	"Çá",	"Çá",	"g",	/*	71	*/
	"h",	"Çà",	"Çà",	"h",	/*	72	*/
	"i",	"Çâ",	"Çâ",	"i",	/*	73	*/
	"j",	"Çä",	"Çä",	"j",	/*	74	*/
	"k",	"Çã",	"Çã",	"k",	/*	75	*/
	"l",	"Çå",	"Çå",	"l",	/*	76	*/
	"m",	"Çç",	"Çç",	"m",	/*	77	*/
	"n",	"Çé",	"Çé",	"n",	/*	78	*/
	"o",	"Çè",	"Çè",	"o",	/*	79	*/
	"p",	"Çê",	"Çê",	"p",	/*	80	*/
	"q",	"Çë",	"Çë",	"q",	/*	81	*/
	"r",	"Çí",	"Çí",	"r",	/*	82	*/
	"s",	"Çì",	"Çì",	"s",	/*	83	*/
	"t",	"Çî",	"Çî",	"t",	/*	84	*/
	"u",	"Çï",	"Çï",	"u",	/*	85	*/
	"v",	"Çñ",	"Çñ",	"v",	/*	86	*/
	"w",	"Çó",	"Çó",	"w",	/*	87	*/
	"x",	"Çò",	"Çò",	"x",	/*	88	*/
	"y",	"Çô",	"Çô",	"y",	/*	89	*/
	"z",	"Çö",	"Çö",	"z",	/*	90	*/
	"{",	"Åo",	"Åo",	"{",	/*	91	*/
	"|",	"Åb",	"Åb",	"|",	/*	92	*/
	"}",	"Åp",	"Åp",	"}",	/*	93	*/
	"~",	"Å`",	"Å`",	"~",	/*	94	*/
	"ﬁ",	"ÅJ",	"ÅJ",	"ﬁ",	/*	95	*/
	"ﬂ",	"ÅK",	"ÅK",	"ﬂ",	/*	96	*/
	",",	"ÅC",	"ÅA",	"§",	/*	12 ref	*/
	"-",	"Å|",	"Å[",	"∞",	/*	13 ref	*/
	".",	"ÅD",	"ÅB",	"°",	/*	14 ref	*/
	"@",	"Åó",	"ÅE",	"•",	/*	32 ref	*/
	"[",	"Åm",	"Åu",	"¢",	/*	59 ref	*/
	"\\",	"Å_",	"Åè",	"\\",	/*	60 ref	*/
	"]",	"Ån",	"Åv",	"£",	/*	61 ref	*/
	"-",	"Å|",	"Å|",	"-",	/*	add	*/
	".",	"ÅD",	"ÅD",	".",	/*	add	*/
	"",	"",	"",	""
};

sj_atbl_set1 ()
{
	set_atbl (12, "ÅA", "§");	/*	12 sp	*/
	set_atbl (13, "Å[", "∞");	/*	13 sp	*/
	set_atbl (14, "ÅB", "°");	/*	14 sp	*/
	set_atbl (32, "ÅE", "•");	/*	32 sp	*/
	set_atbl (59, "Åu", "¢");	/*	59 sp	*/
	set_atbl (60, "Åè", "\\");	/*	60 sp	*/
	set_atbl (61, "Åv", "£");	/*	61 sp	*/
}

sj_atbl_set2 ()
{
	set_atbl (12, "ÅC", ",");	/*	12 sp	*/
	set_atbl (13, "Å|", "-");	/*	13 sp	*/
	set_atbl (14, "ÅD", ".");	/*	14 sp	*/
	set_atbl (32, "Åó", "@");	/*	32 sp	*/
	set_atbl (59, "Åm", "[");	/*	59 sp	*/
	set_atbl (60, "Å_", "\\");	/*	60 sp	*/
	set_atbl (61, "Ån", "]");	/*	61 sp	*/
}

set_atbl (n, s1, s2)
int	n;
char	*s1, *s2;
{
	strncpy (Atable[n][2], s1, 2);
	strncpy (Atable[n][3], s2, 1);
}
