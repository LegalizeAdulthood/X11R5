/*
 * $Id: READ.ME.j,v 1.3 1991/09/18 05:36:28 proj Exp $
 */

●	Ｗｎｎ仮名漢字変換システム バージョン4.1

Ｗｎｎ仮名漢字変換システムは、京都大学数理解析研究所、オムロン株式会社、
株式会社アステックの３者によって開発されたネットワーク仮名漢字変換シス
テムです。

これは、Ｗｎｎ仮名漢字変換システムのバージョン4.1です。

今回のリリースでは、以下の機械で動作が確認されています。

SUN の OS4.0.3/4.1.1

OMRON LUNA の UniOS-B (BSD4.3)、UniOS-U (SystemV R2.1)、UniOS-Mach (MACH)




●	インストール

 ソースファイルには、リテラル文字列、および、コメントが、ECUコードで書かれて
 います。コンパイルの際には、8ビットが通るコンパイラで、make して下さい。

 ・Imakeでコンパイル

	cd ..
	make World -f Makefile.ini

 ・Imakeでインストール

    Wnnの環境すべてをインストールする場合
	make install

    Wnnのサーバ側だけインストールする場合
	make instserver
    (jserver, jserverrc, hinsi.data, pubdic)

    Wnnのクライアント側だけインストールする場合
	make instclient
    (uum, jutil, configuration files)

    Wnnのライブラリをインストールする場合
	make instlib
    (libwnn.a, include files)

 
 Project.tmpl ($(TOP)/contrib/im/Xsi/configにある) に JWNNBINDIR、WNNWNNDIR、
 JWNNPUBDICDIR があります。 jserver, uum, 辞書ユーティリティなどのコマンド
 (JWNNBINDIR)、設定ファイル(WNNWNNDIR)、Ｐｕｂｄｉｃ(JWNNPUBDICDIR)が、
 インストールされるディレクトリーです。
 だだし、LIBDIR は、include/config.h に、パス名が書かれていますので、
 これだけ変えても動きません。
 デフォルトでは、それぞれ、/usr/local/bin/Wnn4, /usr/local/lib/wnn/ja_JP,
 /usr/local/lib/wnn/ja_JP/dic/pubdic になっています。

 Project.tmpl ($(TOP)/contrib/im/Xsi/configにある) に WNNOWNER があります。
 これは、Wnn のプログラム(uum を除く) をどのユーザの権限で動かすか指定
 します。バージョン3まででは、jserver は、root の権限で動かしていたので
 すが、root で動かすのは危険性が高いため、wnn 用のユーザを作って、その
 権限で jserver だけは動かすこ とを推奨します。
 デフォルトでは、wnn になっています。

 uum は、オーナが root で、 sビットが立っている必要があります。

 それでは、サーチパスに、/usr/local/bin/Wnn4 を追加して、jserver および、uum 
 を使ってみて下さい。



●	manual 以外のディレクトリは、以下の通りです。

conv
	コンバート・キー(ファンクションキーの吐くコード列を、適当なコー
	ドに変換する)のソースが存在します。
etc
	複数のディレクトリで共有されるソースファイルが存在します。
fuzokugo
	付属語ファイル作成プログラム(atof)、および、pubdic の付属語ファイルが
	存在します。
include 
	ヘッダーファイルが存在します。
jd
	uum, jserver, ローマ字仮名変換などの初期化ファイル、
	品詞ファイルが存在します。このディレクトリの内容は、
	/usr/local/lib/wnn/ja_JP にインストールされます。

	    jserverrc - jserver の立上りの設定ファイルです。
	    uumrc	uum の立上りの設定ファイルです。
	    uumkey	uum のキーバインド設定ファイルです。
	    wnnenvrc	uum 使用時の辞書等の設定ファイルです。
	jd の下には、いろいろな設定のものが用意されています。
	好みに合わせて御利用下さい。

jlib
	ライブラリのソースが存在します。
jlib.V3
	バージョン3に対する互換ライブラリのソースが存在します。
jserver
	jserver のソースが存在します。
jutil
	辞書ユーティリティのソースが存在します。
		atod	 辞書の作成
		dtoa	 辞書のテキストへの変更
		oldatonewa バージョン３までの辞書のテキスト形式を、
			バージョン４のテキスト形式に変更
		wnnstat	 jserver の状態(ユーザ、環境、ファイル、辞書)を調べる。
		wnnkill	 jserver を終了させる。
		wnntouch 辞書ファイルなどのファイルのFID が、ファイルのヘッダに
			持っているものと異なる時に一致させる。
		wddel	単語削除をバッチ的に行なう
		wdreg	単語登録をバッチ的に行なう
pubdic 			
	pubdic の辞書が存在します。
	辞書は、 逆引き形式で作ってあります。
romkan
	ローマ字仮名変換のソースが存在します。
uum
	フロントエンド・プロセッサ uum のソースが存在します。

-----------------------------------------------------------------------
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
