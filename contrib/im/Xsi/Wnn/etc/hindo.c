/*
 * $Id: hindo.c,v 1.2 1991/09/16 21:29:19 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
/*
		  				修正 9/1/89

	a＝実頻度, b＝仮頻度（共に非負整数）のとき

	b＝[a÷x]＋2(x−1) where x＝[( sqrt(1＋2a)＋1 )÷2]
	（計算結果が126を越えた場合は126とする。）

	(min)a＝(b−2y)(y＋1)	      where y＝[b÷4]
	(mid)a＝(b−2y)(y＋1)＋[y÷2] where y＝[b÷4]
	（b≦126とする。）

	ここで[ ]はガウス記号
	bを与えてのaの推測値には幅があり、(min)aとはその最小値、
	(mid)aはその中間値
	b≦4 なら、(min)a＝(mid)a＝b

	仮頻度がbのとき、頻度の更新の確率は 1 / ([b÷4]＋1)
	但し b＝0の時は、頻度更新確率 0

    
	 a == -1 <==> b == 0x7f = 127; 
	この時、このエントリーは、変換に決して用いられない
    	(コメントアウトされている)ことを表す。
		    	9/1/89 H.T.
*/

 /** 整数引数の平方根関数。但し引数<0の時のエラーチェックはなし（0を返す）。*/
static	int
isqrt(i)
int	i;
{
	register int	a, b;

	if(i <= 0) return(0);
	for(a = i, b = 1; b <<= 1, a >>= 2; );
	while((a = i / b) < b) b = (b + a) >> 1;
	return(b);
}

 /** 実頻度a→仮頻度b */
int
asshuku(hin)
int	hin;
{
	register int	c;

	if(hin == -1) return(127);
	if(hin <= 4) return(hin); 
	 /* 大半は頻度0と想定してのスピードアップ。motoni1,2でも同じ */

	c = (isqrt((hin <<= 1) + 1) + 1) & ~1;
	c += hin / c - 2;
	return( c < 126 ? c : 126);
}

 /** 仮頻度b→実頻度(min)a */
/*
int
motoni1(hin)
int	hin;
{
	register int	c;

	if(hin == 127) return(-1);
	if(hin <= 4) return(hin); 
	c = hin >> 2;
	return( (hin - (c << 1)) * (c + 1) );
}
*/

 /** 仮頻度b→実頻度(mid)a */
int
motoni2(hin)
int	hin;
{
	register int	c;

	if(hin == 127) return(-1);
	if(hin <= 4) return(hin); 
	c = hin >> 2;
	return( (hin - (c << 1)) * (c + 1) + (c >> 1) );
}

