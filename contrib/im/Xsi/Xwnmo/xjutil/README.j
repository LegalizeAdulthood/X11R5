/*
 * $Id: README.j,v 1.1 1991/09/13 09:02:52 proj Exp $
 */
		XJUTIL - 辞書ユーティリティ・マネージャ


このディレクトリには、XJUTILのソースがあります。

XJUTILは、Wnnの辞書を操作する辞書ユーティリティ・マネージャです。
XJUTILは、XWNMOより自動的に起動されます。

XJUTILの処理
	辞書追加:
	辞書一覧:
	登録:
	検索:
	パラメータ変更:
	頻度セーブ:
	辞書情報:
	附属語変更:

マニュアルをインストールする場合、デフォルトは英語のマニュアルです。
もし、日本語のマニュアル(EUC)が表示できるシステムなら、Imakefile の
LOCALMAN を "man.en" から "man.ja" に変更して下さい。

[XJUTIL の作り方]

    xwnmo/README の [XWNMO の作り方] を参照下さい。

[XJUTIL の使い方]

    XWNMO をコンパイル時に、USING_XJUTILをデファインすれば、XWNMOは自動的に
    XJUTILを起動します。何もする必要はありません。
