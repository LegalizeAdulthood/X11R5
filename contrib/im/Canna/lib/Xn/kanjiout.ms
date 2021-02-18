.\"
.\" 環境設定
.\"
.bd K 3 3
.nr PI 2n
.nr PO 2c
.\"
.nr LL 16.5c
.nr PS 10
.nr VS 14p
.\"
.nr PD 10p
.ds CH
.ds RH
.ds LH
.ds CF
.ds RF
.ds LF
.BG
.\"
.\" 本文
.\"
.B
.ce
.ps 16p
Ｘ１１での漢字まじり文字列の出力について
.vs 14p
.ps 8p
.ce 3
１９８８年７月
日本電気（株）基本ソフトウェア開発本部
今　　昭
.PP
Ｘ１１では２バイトコードの文字が表示できるようになっており、漢字を出力す
ることが可能である。しかし英数コードと漢字コードが混在している場合には、表
示のときにそれぞれ、XDrawString、XDrawString16といったように別個の関数（お
よび、フォント、ＧＣ）を用いなければならない。
.PP
しかし、通常のプログラミングにおいてはエスケープ・シーケンスを用いたり、
あるいはＥＵＣのようにMSBを用いたりして、１バイトコードと２バイトコードを
混在させるのが普通である。このとき、与えられた文字列から１バイトコードと２
バイトコードを切り分けてからXDrawStringあるいはXDrawString16に与えるといっ
た処理をしなければならず、非常に面倒である。
.PP
以下の関数群はこの処理を行いながらストリング処理を行うライブラリ関数群で
ある。これらの関数は引数として、１バイトコードと２バイトコードの混在した文
字列を受け付ける。
.PP
ここで対象となる関数は以下の６つである。
.RS
(1) XKanjiTextWidth
(2) XKanjiTextExtents
(3) XQueryKanjiTextExtents
(4) XDrawKanjiText
(5) XDrawKanjiString
(6) XDrawImageKanjiString
.RE
.br
これらのライブラリ関数を用いて日本語に対応するには
.RS
.IP (1) 4
関数名がＸ××のところをＸ×Kanji×に変える。
.IP (2) 4
ｇｃを与えるところをｇｃの配列へのポインタを与える。
.IP (3) 4
フォントＩＤを与えるところをフォントＩＤの配列へのポインタを与える。
.IP (4) 4
フォント構造体のポインタの配列へのポインタを与える。
（もともとフォント構造体のポインタを与えていた場合にはその部分を取り除いて
フォント構造体のポインタの配列へのポインタを与えるようにする。）
.IP (5) 4
最終引数として漢字のモード（フォント切り替えのモード）を与える。
.RE
.br
といった形になる。ｇｃやフォントの配列には、８ビットコードに対応するものお
よび、１６ビットコードに対応するものを格納する。
.PP
最終引数として与える漢字モードは１バイトコードと２バイトコードの混在がどのよ
うにされているかを示すもので、現在以下のモードが準備されている。
.RS
.IP fs_euc_mode 20
ＥＵＣコードでの混在のみを解釈する。
.IP fs_simple_jis_mode 20
エスケープ・シーケンスでの混在のみを解釈する
.IP fs_jis_mode 20
エスケープ・シーケンスでの混在のみを解釈する
.IP fs_jis_euc_mode 20
ＥＵＣ,エスケープ・シーケンス、の両者を解釈する。
.RE
.PP
それぞれのモード時にフォントやＧＣの配列は次のように格納する。
.RS
.IP fs_euc_mode（ＥＵＣ） 4
.nf
配列の０番目に　　ａｎｋ用のフォントに対応したものを入れる。
配列の１番目に　　漢字用のフォントに対応したものを入れる。
.fi
.IP fs_simple_jis_mode（ＪＩＳ） 4
.nf
配列の０番目に　　ａｎｋ用のフォントに対応したものを入れる。
配列の１番目に　　漢字用のフォントに対応したものを入れる。
.fi
.IP fs_jis_mode（ＪＩＳ） 4
.nf
配列の０番目に　　アスキー文字フォントに対応したものを入れる。
配列の１番目に　　ＵＫ文字フォントに対応したものを入れる。
配列の２番目に　　ＪＩＳローマ文字フォントに対応したものを入れる。
配列の３番目に　　ＪＩＳ８ビットカナ文字フォントに対応したものを入れる。
配列の４番目に　　旧ＪＩＳローマ文字フォントに対応したものを入れる。
配列の５番目に　　ＪＩＳ漢字文字フォントに対応したものを入れる。
配列の６番目に　　旧ＪＩＳ漢字文字フォントに対応したものを入れる。
.fi
.IP fs_jis_euc_mode（ＪＩＳおよびＥＵＣ） 4
.nf
配列の０番目に　　ａｎｋ用のフォントに対応したものを入れる。
配列の１番目に　　漢字用のフォントに対応したものを入れる。
.fi
.RE
.PP
＜注意＞
.PP
デフォルトの漢字フォントとしては
.RS
.IP kanji 8
（対応するankフォントはromankana）　（ＥＷＳでは１６×１６ドット）　
.br
あるいは
.IP k14 8
（対応するankフォントはa14）　　　　（１４×１４ドット）　　　　　
.RE
.br
を用いるのが望ましい。
.PP
それぞれの関数において文字長を与えるが、これは純粋なバイト長を与えればよ
い。例えば漢字コードは２バイトとなり、またエスケープ・シーケンスに要する文
字列も数に入れる。
.PP
例）
.nf
”漢字”は
                              EUC なら  ４バイト
                              JIS なら　１０バイト
.fi
.PP
いずれにせよstringのバイト長はstrlen(string)にて得られる。
.PP
漢字混在のモードはユーザが独自のものを作ることが可能である。漢字の混在モ
ードを表すfs_mode型は次のように定義されている。
.br
.nf
typedef enum {single_byte_code, double_byte_code} code_len;
typedef struct {
        code_len nbyte;
        int (*func)();
} fs_mode;
.fi
.PP
例えば、fs_euc_modeは８ビットコードだけを拾いあげる関数pickupEUC8へのポ
インタと１６ビットコードだけを拾いあげる関数pickupEUC16 へのポインタを含ん
でいる。pickup関数は次のような引数とともに呼び出され、戻り値として拾いあげ
た文字数を返す。
.br
.nf
pickup(from, to, len, next_return, remain_return, 
       nextIndex_return)
    char *from, *to,  **next_return;
    int len, *remain_return, *nextIndex_return;
.fi
.PP
fromは８ビットコードと１６ビットコードの混在した文字列へのポインタである
。len はその文字列のバイト長を表す。８ビット文字あるいは１６ビット文字のみ
が拾いあげられtoで示された領域へ格納される。pickupの戻り値はtoへ格納された
文字の文字数を表す。next_returnにはこの関数が読み残したfrom内の文字列への
ポインタが返され、remain_returnには残った文字列のバイト数が返される。next
Index_returnには読み残した文字列のコード系が何であるかを示す数値が返される
。
.PP
以上のような規約に従えば新しいモードを作成することができる。詳しくはソー
スコードを参照すること。
.bp
.SH
各ライブラリ関数の説明
.SH
1. XKanjiTextWidth
.br
文字列の幅を返す関数。
.nf
int
XKanjiTextWidth(font_structs, string, count, mode) 

　　XFontStruct **font_structs; 
　　char        *string; 
　　int         count;  /*　バイト単位　*/
　  fs_mode     *mode; 
.fi
.SH
2. XKanjiTextExtents
.nf
int 
XKanjiTextExtents(font_structs, string, nchars,
              direction_return, ascent_return, descent_return, 
　　　　　　  overall_return, mode)

    XFontStruct **font_structs; 
    char        *string; 
    int         nchars; /*　バイト単位　*/
　　int         *direction_return, *ascent_return, *descent_return; 
    XCharStruct *overall_return;
    fs_mode     *mode;
.fi
.SH
3. XQueryKanjiTextExtents
.nf
int 
XQueryKanjiTextExtents(display, font_IDs, font_structs, string, nchars,
                   direction_return, ascent_return, descent_return,
                   overall_return, mode) 

    Display     *display; 
    XID         font_IDs; 
    XFontStruct **font_structs; 
    char        *string; 
    int         nchars; /*　バイト単位　*/
　　int         *direction_return, *ascent_return, *descent_return; 
    XCharStruct *overall_return;
    fs_mode     *mode;
.fi
.SH
4. XDrawKanjiText
.nf
int 
XDrawKanjiText(display, d, gcs, font_structs,
           x, y, items, nitems, mode)

    Display     *display; 
    Drawable    d; 
    GC          *gc; 
    XFontStruct **font_structs; 
    int         x, y; 
    XTextItem   *items; 
    int         nitems; /*　バイト単位　*/
    fs_mode     *mode;
.fi
.SH
5. XDrawKanjiString
.nf
int 
XDrawKanjiString(display, d, gcs, font_structs,
             x, y, string, length, mode);

    Display     *display; 
    Drawable    d; 
    GC          *gcs;
    XFontStruct **font_structs; 
    int         x, y; 
    char        *string, 
    int         length; /*　バイト単位　*/
　  fs_mode     *mode; 
.fi
.SH
6. XDrawImageKanjiString
.nf
int 
XDrawImageKanjiString(display, d, gcs, font_structs, 
                  x, y, string, length, mode);

    Display     *display; 
    Drawable    d; 
    GC          *gcs;
    XFontStruct **font_structs; 
    int         x, y; 
    char        *string, 
    int         length; /*　バイト単位　*/
　  fs_mode     mode; 
.fi
.bp
.SH
この関数群の将来的なコンパチビリティ
.SH
１．　変更される可能性のある部分
.PP
この関数群は仕様がはっきりと決まったわけではないので将来的に次の点が変更
になる場合があります。
.IP (1)関数名 2
.PP
以下の関数の関数名が変更を受けるかもしれません。
例えばXKanjiTextWidthではなく、XnTextWidthのような名前になるかもしれません。
.br
.nf
XKanjiTextWidth
XKanjiTextExtents
XQueryKanjiTextExtents
XDrawKanjiText
XDrawKanjiString
XDrawImageKanjiString
.fi
.IP (2)モード名 2
.PP
以下のモード名が変更を受けるかもしれません。fs_euc_modeではなくて、
Xfs_euc_modeのような名前になるかもしれません。
.br
.nf
fs_euc_mode
fs_simple_jis_mode
fs_jis_mode
fs_jis_euc_mode
.fi
.IP (3)モードの型／型名 2
.PP
モードの型が変更を受けるかもしれません。
それに伴ってモードの型名も変更を受けるかもしれません。
例えば、モードの型は、ピックアップ関数のポインタの配列だけで表されるよう
になるかもしれません。
またモードの型名はfs_modeではなく、Xfs_modeのようになるかもしれません。
.br
.nf
typedef enum {single_byte_code, double_byte_code} code_len;
typedef struct {
        code_len nbyte;
        int (*func)();
} fs_mode;
.fi
.IP (4)ピックアップ関数 2
.PP
モードの定義に用いられるピックアップ関数の仕様が変更を受けるかもしれませ
ん。引数の仕様の変更が考えられます。
.br
.nf
pickup(from, to, len, next_return, remain_return, 
       nextIndex_return)
    char *from, *to,  **next_return;
    int len, *remain_return, *nextIndex_return;
.fi
.SH
２．　変更を受けない部分
.PP
以下の部分は将来にわたって変更が無いと思われます。
.IP (1)引数 2
.PP
以下の関数の引数の仕様は変更を受けないと思われます。
.br
.nf
XKanjiTextWidth
XKanjiTextExtents
XQueryKanjiTextExtents
XDrawKanjiText
XDrawKanjiString
XDrawImageKanjiString
.fi
.IP (2)モードの仕様 2
.PP
fs_euc_modeでは配列の０番目に英字用の情報を入れ、１番目に漢字用の情報を
入れるといった仕様は変更を受けないと思われます。
