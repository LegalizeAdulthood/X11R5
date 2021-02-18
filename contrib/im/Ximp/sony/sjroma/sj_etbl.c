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
 *	sj_etbl.c
 *	romaji tuduri table.
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Mon May 15 15:35:34 JST 1989
 *
 *	Caution: This table must be sorted.
 */
/*
 * $Header: sj_etbl.c,v 1.1 91/04/29 17:56:43 masaki Exp $ SONY;
 */

struct romaji {
	char	*kana;
	char	*tuduri;
};

struct romaji Etable[] = {
	{"  ",	"\240"},
	{"!!",	"\241"},
	{"!^",	"\246"},
	{"' ",	"'"},
	{"''",	"\264"},
	{"'A",	"\301"},
	{"'E",	"\311"},
	{"'I",	"\315"},
	{"'O",	"\323"},
	{"'U",	"\332"},
	{"'Y",	"\335"},
	{"'a",	"\341"},
	{"'e",	"\351"},
	{"'i",	"\355"},
	{"'o",	"\363"},
	{"'u",	"\372"},
	{"'y",	"\375"},
	{"((",	"["},
	{"(-",	"{"},
	{"))",	"]"},
	{")^",	"}"},
	{"*A",	"\305"},
	{"*a",	"\345"},
	{"++",	"#"},
	{"+-",	"\261"},
	{", ",	","},
	{",,",	"\270"},
	{"-,*",	"\254"},
	{"--",	"\255"},
	{"-:",	"\367"},
	{"-D",	"\320"},
	{"-d",	"\360"},
	{".^",	"\267"},
	{"//",	"\\"},
	{"/<",	"\\"},
	{"/U*",	"\265"},
	{"/^",	"|"},
	{"0^",	"\260"},
	{"12*",	"\275"},
	{"14*",	"\274"},
	{"1^",	"\271"},
	{"2^",	"\262"},
	{"34*",	"\276"},
	{"3^",	"\263"},
	{"<<",	"\253"},
	{">>",	"\273"},
	{"??",	"\277"},
	{"A'",	"\301"},
	{"A*",	"\305"},
	{"AA",	"@"},
	{"AE*",	"\306"},
	{"A\"",	"\304"},
	{"A^",	"\302"},
	{"A_",	"\252"},
	{"A`",	"\300"},
	{"A~",	"\303"},
	{"C,",	"\307"},
	{"C/",	"\242"},
	{"C0",	"\251"},
	{"CO",	"\251"},
	{"C|",	"\242"},
	{"E'",	"\311"},
	{"E\"",	"\313"},
	{"E^",	"\312"},
	{"E`",	"\310"},
	{"I'",	"\315"},
	{"I\"",	"\317"},
	{"I^",	"\316"},
	{"I`",	"\314"},
	{"L-",	"\243"},
	{"L=",	"\243"},
	{"N~",	"\321"},
	{"O'",	"\323"},
	{"O/",	"\330"},
	{"O\"",	"\326"},
	{"O^",	"\324"},
	{"O_",	"\272"},
	{"O`",	"\322"},
	{"O~",	"\325"},
	{"P!",	"\266"},
	{"RO",	"\256"},
	{"S!",	"\247"},
	{"S0",	"\247"},
	{"SO",	"\247"},
	{"TH",	"\336"},
	{"U'",	"\332"},
	{"U\"",	"\334"},
	{"U^",	"\333"},
	{"U`",	"\331"},
	{"X0",	"\244"},
	{"XO",	"\244"},
	{"Y'",	"\335"},
	{"Y-",	"\245"},
	{"Y=",	"\245"},
	{"\" ",	"\""},
	{"\" ",	"\""},
	{"\"A",	"\304"},
	{"\"E",	"\313"},
	{"\"O",	"\326"},
	{"\"U",	"\334"},
	{"\"\"","\250"},
	{"\"a",	"\344"},
	{"\"e",	"\353"},
	{"\"i",	"\357"},
	{"\"o",	"\366"},
	{"\"u",	"\374"},
	{"\"y",	"\377"},
	{"^ ",	"^"},
	{"^A",	"\302"},
	{"^E",	"\312"},
	{"^I",	"\316"},
	{"^O",	"\324"},
	{"^U",	"\333"},
	{"^a",	"\342"},
	{"^e",	"\352"},
	{"^i",	"\356"},
	{"^o",	"\364"},
	{"^u",	"\373"},
	{"_^",	"\257"},
	{"` ",	"`"},
	{"`A",	"\300"},
	{"`E",	"\310"},
	{"`I",	"\314"},
	{"`O",	"\322"},
	{"`U",	"\331"},
	{"`a",	"\340"},
	{"`e",	"\350"},
	{"`i",	"\354"},
	{"`o",	"\362"},
	{"`u",	"\371"},
	{"a'",	"\341"},
	{"a*",	"\345"},
	{"a\"",	"\344"},
	{"a^",	"\342"},
	{"a`",	"\340"},
	{"ae*",	"\346"},
	{"a~",	"\343"},
	{"c,",	"\347"},
	{"e'",	"\351"},
	{"e\"",	"\353"},
	{"e^",	"\352"},
	{"e`",	"\350"},
	{"i'",	"\355"},
	{"i\"",	"\357"},
	{"i^",	"\356"},
	{"i`",	"\354"},
	{"n~",	"\361"},
	{"o'",	"\363"},
	{"o/",	"\370"},
	{"o\"",	"\366"},
	{"o^",	"\364"},
	{"o`",	"\362"},
	{"o~",	"\365"},
	{"ss",	"\337"},
	{"th",	"\376"},
	{"u'",	"\372"},
	{"u\"",	"\374"},
	{"u^",	"\373"},
	{"u`",	"\371"},
	{"xx",	"\327"},
	{"y'",	"\375"},
	{"y\"",	"\377"},
	{"||",	"\246"},
	{"~ ",	"~"},
	{"~A",	"\303"},
	{"~N",	"\321"},
	{"~O",	"\325"},
	{"~a",	"\343"},
	{"~n",	"\361"},
	{"~o",	"\365"},
	{"",	""}
};
