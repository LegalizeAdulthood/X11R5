/*
 * $Id: README.j,v 1.1 1991/09/13 07:42:32 proj Exp $
 */
	XWNMO - Ｘ・インプット・マネージャ

このディレクトリには、XWNMOのソースがあります。

XWNMOは、XIMライブラリに対して、多国語入力環境を提供するインプット・マネージャ
です。

XWNMOは、以下のインプットスタイルをサポートしています。

	(XIMPreeditArea | XIMStatusArea)		: Off-The-Spot
	(XIMPreeditPosition | XIMStatusArea)		: Over-The-Spot
	(XIMPreeditNothing | XIMStatusNothing )		: Root
	(XIMPreeditCallbacks | XIMStatusArea )		: On-The-Spot
	(XIMPreeditArea | XIMStatusCallbacks )		: On-The-Spot
	(XIMPreeditPosition | XIMStatusCallbacks )	: On-The-Spot
	(XIMPreeditCallbacks | XIMStatusCallbacks )	: On-The-Spot

なお、On-The-Spotのインプットスタイルは、コンパイル時のデファインで
選択できます。[XWNMO の作り方]を参照下さい。

XWNMOは、国際化対応したＷｎｎを変換に使用します。
XWNMOは、クライアント(ライブラリ)の言語に従って、変換サーバを選択します。
また、ダイナミックに変換サーバを変更することができます。
XWNMOは、複数のクライアント、複数のサーバをサポートできます。

マニュアルをインストールする場合、デフォルトは英語のマニュアルです。
もし、日本語のマニュアル(EUC)が表示できるシステムなら、Imakefile の
LOCALMAN を "man.en" から "man.ja" に変更して下さい。

[XWNMO の作り方]

    注意: Xsi は独自の Project.tmpl を持っているので、xmkmf は使用できません。

    まず、X11R5 のコア部分をメイクして下さい。
    次に、XWNMO、XJUTIL、および、国際化対応Wnnをメイクするために、以下の
    コマンドを実行して下さい。

	% cd contrib/im/Xsi
	% make World

    XWNMO、XJUTIL、および、国際化対応Wnnをインストールするには、

	% make install

    XWNMOは、コンパイル時にいくつかのデファインを持っています。それらは、
    Imakefile に書かれおり、デフォルトでは、すべてがデファインされています。
    もし、どれかを外す場合は、コンパイル前に、Imakefile を適当に変更して
    ください。
    以下のデファインが選択できます。

	USING_XJUTIL	: XJUTIL(辞書ユーティリティマネージャ)を使用
	CALLBACKS	: On-The-Spot インプットスタイルをサポート
	SPOT		: SpotLocation Extended Protocol をサポート
	XJPLIB		: XJpのプロトコル(コミュニケーションキット部分)を
			  サポート
	XJPLIB_DIRECT	: XJpのプロトコル(ダイレクトインタフェース部分)を
			  サポート
    
    それぞれの詳細:
      USING_XJUTIL:
	  これをデファインすると、XWNMOは、XJUTIL 辞書ユーティリティマネージャ
	  を起動時に走らせます。これにより、辞書に対するさまざまなオペレーション
	  ができます。(登録、削除、辞書一覧など)
	  XJUTILの詳細は、xjutil/README を参照下さい。
    
      CALLBACKS:
	  これをデファインすると、XWNMOは、 On-The-Spot インプットスタイルを
	  サポートします。インプットスタイルは:
		(XIMPreeditCallbacks | XIMStatusArea )
		(XIMPreeditArea | XIMStatusCallbacks )
		(XIMPreeditPosition | XIMStatusCallbacks )
		(XIMPreeditCallbacks | XIMStatusCallbacks )
      
      SPOT:
	  これをデファインすると、XWNMOは、SpotLocation 拡張プロトコルを
	  サポートします。このプロトコルを使い、スポットの移動ができます。
	  SpotLocation 拡張プロトコルの詳細は、SEP_README あるいは、xwnmo
	  のマニュアルの SEP 部分を参照下さい。

      XJPLIB:
	  これをデファインすると、XWNMOは、XJp プロトコルのコミュニケーション
	  キット部分をサポートします。XJp のコミュニケーションキットライブラリ
	  が使用できます。(XJp_open()、XJp_begin() など)

      XJPLIB_DIRECT:
	  これをデファインすると、XWNMOは、XJp プロトコルのダイレクト
	  インタフェース部分をサポートします。XJp の ダイレクトインタフェース
	  が使用できます。(XLookupKanjiString())
	  これをデファインする場合は、XJPLIB を必ずデファインして下さい。

[XWNMO の使い方]

    XWNMO を起動する前に、変換サーバである、jserver と cserver を起動して
    下さい。Jserver は、日本語変換サーバで、cserver は、中国語変換サーバです。

    XWNMO を起動するには

	% xwnmo

    XML をデファインし、コア部分をコンパイルすると、アプリケーションは、
    複数言語環境が使用できます。xwnmo のマニュアルの XML 部分を参照下さい。

