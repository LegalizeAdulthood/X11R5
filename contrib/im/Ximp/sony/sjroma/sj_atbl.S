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
	" ",	"�@",	"�@",	" ",	/*	0	*/
	"!",	"�I",	"�I",	"!",	/*	1	*/
	"\"",	"�h",	"�h",	"\"",	/*	2	*/
	"#",	"��",	"��",	"#",	/*	3	*/
	"$",	"��",	"��",	"$",	/*	4	*/
	"%",	"��",	"��",	"%",	/*	5	*/
	"&",	"��",	"��",	"&",	/*	6	*/
	"'",	"�f",	"�f",	"'",	/*	7	*/
	"(",	"�i",	"�i",	"(",	/*	8	*/
	")",	"�j",	"�j",	")",	/*	9	*/
	"*",	"��",	"��",	"*",	/*	10	*/
	"+",	"�{",	"�{",	"+",	/*	11	*/
	",",	"�C",	"�A",	"�",	/*	12 sp	*/
	"-",	"�|",	"�[",	"�",	/*	13 sp	*/
	".",	"�D",	"�B",	"�",	/*	14 sp	*/
	"/",	"�^",	"�^",	"/",	/*	15	*/
	"0",	"�O",	"�O",	"0",	/*	16	*/
	"1",	"�P",	"�P",	"1",	/*	17	*/
	"2",	"�Q",	"�Q",	"2",	/*	18	*/
	"3",	"�R",	"�R",	"3",	/*	19	*/
	"4",	"�S",	"�S",	"4",	/*	20	*/
	"5",	"�T",	"�T",	"5",	/*	21	*/
	"6",	"�U",	"�U",	"6",	/*	22	*/
	"7",	"�V",	"�V",	"7",	/*	23	*/
	"8",	"�W",	"�W",	"8",	/*	24	*/
	"9",	"�X",	"�X",	"9",	/*	25	*/
	":",	"�F",	"�F",	":",	/*	26	*/
	";",	"�G",	"�G",	";",	/*	27	*/
	"<",	"��",	"��",	"<",	/*	28	*/
	"=",	"��",	"��",	"=",	/*	29	*/
	">",	"��",	"��",	">",	/*	30	*/
	"?",	"�H",	"�H",	"?",	/*	31	*/
	"@",	"��",	"�E",	"�",	/*	32 sp	*/
	"A",	"�`",	"�`",	"A",	/*	33	*/
	"B",	"�a",	"�a",	"B",	/*	34	*/
	"C",	"�b",	"�b",	"C",	/*	35	*/
	"D",	"�c",	"�c",	"D",	/*	36	*/
	"E",	"�d",	"�d",	"E",	/*	37	*/
	"F",	"�e",	"�e",	"F",	/*	38	*/
	"G",	"�f",	"�f",	"G",	/*	39	*/
	"H",	"�g",	"�g",	"H",	/*	40	*/
	"I",	"�h",	"�h",	"I",	/*	41	*/
	"J",	"�i",	"�i",	"J",	/*	42	*/
	"K",	"�j",	"�j",	"K",	/*	43	*/
	"L",	"�k",	"�k",	"L",	/*	44	*/
	"M",	"�l",	"�l",	"M",	/*	45	*/
	"N",	"�m",	"�m",	"N",	/*	46	*/
	"O",	"�n",	"�n",	"O",	/*	47	*/
	"P",	"�o",	"�o",	"P",	/*	48	*/
	"Q",	"�p",	"�p",	"Q",	/*	49	*/
	"R",	"�q",	"�q",	"R",	/*	50	*/
	"S",	"�r",	"�r",	"S",	/*	51	*/
	"T",	"�s",	"�s",	"T",	/*	52	*/
	"U",	"�t",	"�t",	"U",	/*	53	*/
	"V",	"�u",	"�u",	"V",	/*	54	*/
	"W",	"�v",	"�v",	"W",	/*	55	*/
	"X",	"�w",	"�w",	"X",	/*	56	*/
	"Y",	"�x",	"�x",	"Y",	/*	57	*/
	"Z",	"�y",	"�y",	"Z",	/*	58	*/
	"[",	"�m",	"�u",	"�",	/*	59 sp	*/
	"\\",	"�_",	"��",	"\\",	/*	60 sp	*/
	"]",	"�n",	"�v",	"�",	/*	61 sp	*/
	"^",	"�O",	"�O",	"^",	/*	62	*/
	"_",	"�Q",	"�Q",	"_",	/*	63	*/
	"`",	"�e",	"�e",	"`",	/*	64	*/
	"a",	"��",	"��",	"a",	/*	65	*/
	"b",	"��",	"��",	"b",	/*	66	*/
	"c",	"��",	"��",	"c",	/*	67	*/
	"d",	"��",	"��",	"d",	/*	68	*/
	"e",	"��",	"��",	"e",	/*	69	*/
	"f",	"��",	"��",	"f",	/*	70	*/
	"g",	"��",	"��",	"g",	/*	71	*/
	"h",	"��",	"��",	"h",	/*	72	*/
	"i",	"��",	"��",	"i",	/*	73	*/
	"j",	"��",	"��",	"j",	/*	74	*/
	"k",	"��",	"��",	"k",	/*	75	*/
	"l",	"��",	"��",	"l",	/*	76	*/
	"m",	"��",	"��",	"m",	/*	77	*/
	"n",	"��",	"��",	"n",	/*	78	*/
	"o",	"��",	"��",	"o",	/*	79	*/
	"p",	"��",	"��",	"p",	/*	80	*/
	"q",	"��",	"��",	"q",	/*	81	*/
	"r",	"��",	"��",	"r",	/*	82	*/
	"s",	"��",	"��",	"s",	/*	83	*/
	"t",	"��",	"��",	"t",	/*	84	*/
	"u",	"��",	"��",	"u",	/*	85	*/
	"v",	"��",	"��",	"v",	/*	86	*/
	"w",	"��",	"��",	"w",	/*	87	*/
	"x",	"��",	"��",	"x",	/*	88	*/
	"y",	"��",	"��",	"y",	/*	89	*/
	"z",	"��",	"��",	"z",	/*	90	*/
	"{",	"�o",	"�o",	"{",	/*	91	*/
	"|",	"�b",	"�b",	"|",	/*	92	*/
	"}",	"�p",	"�p",	"}",	/*	93	*/
	"~",	"�`",	"�`",	"~",	/*	94	*/
	"�",	"�J",	"�J",	"�",	/*	95	*/
	"�",	"�K",	"�K",	"�",	/*	96	*/
	",",	"�C",	"�A",	"�",	/*	12 ref	*/
	"-",	"�|",	"�[",	"�",	/*	13 ref	*/
	".",	"�D",	"�B",	"�",	/*	14 ref	*/
	"@",	"��",	"�E",	"�",	/*	32 ref	*/
	"[",	"�m",	"�u",	"�",	/*	59 ref	*/
	"\\",	"�_",	"��",	"\\",	/*	60 ref	*/
	"]",	"�n",	"�v",	"�",	/*	61 ref	*/
	"-",	"�|",	"�|",	"-",	/*	add	*/
	".",	"�D",	"�D",	".",	/*	add	*/
	"",	"",	"",	""
};

sj_atbl_set1 ()
{
	set_atbl (12, "�A", "�");	/*	12 sp	*/
	set_atbl (13, "�[", "�");	/*	13 sp	*/
	set_atbl (14, "�B", "�");	/*	14 sp	*/
	set_atbl (32, "�E", "�");	/*	32 sp	*/
	set_atbl (59, "�u", "�");	/*	59 sp	*/
	set_atbl (60, "��", "\\");	/*	60 sp	*/
	set_atbl (61, "�v", "�");	/*	61 sp	*/
}

sj_atbl_set2 ()
{
	set_atbl (12, "�C", ",");	/*	12 sp	*/
	set_atbl (13, "�|", "-");	/*	13 sp	*/
	set_atbl (14, "�D", ".");	/*	14 sp	*/
	set_atbl (32, "��", "@");	/*	32 sp	*/
	set_atbl (59, "�m", "[");	/*	59 sp	*/
	set_atbl (60, "�_", "\\");	/*	60 sp	*/
	set_atbl (61, "�n", "]");	/*	61 sp	*/
}

set_atbl (n, s1, s2)
int	n;
char	*s1, *s2;
{
	strncpy (Atable[n][2], s1, 2);
	strncpy (Atable[n][3], s2, 1);
}
