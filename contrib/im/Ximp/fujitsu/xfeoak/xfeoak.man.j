XFEOAK(1)

名称
    xfeoak - Ｘウィンドウ・インプットサーバ（かな漢字変換フロントエンド）

記述形式
    xfeoak [-display displayname] [-geometry geom] [-none] [-jis]
    [-oyayubi] [-fg color] [-bg color] [-bd color] [-bw number]
    [-bp bitmapfile] [-fn fontname] [-fk kanjifontname] [-verbose]

機能説明

    Xfeoakは，X Window System用の日本語インプットサーバです．
　　xfeoak と Input method library 間の通信は，XIMPプロトコルを使用します．
　　このプロトコルは，X Window System で規定されるクライアントメッセージと
　　プロパティを使用しています．

　　xfeoakは，以下の特徴を持っています．
　　−富士通のＸウィンドウのＸサーバと辞書と接続して，ＯＡＫと互換性のある
      操作性を提供します．
　　−各種マシンの辞書をサポートできるようにマルチ辞書対応をしています．

オプション

    -display ディスプレイ
	使用するディスプレイを指定する．

    -geometry ジオメトリ
	ウィンドウのサイズと位置を指定する．

    -none
	一般のＸサーバで使用するときに指定する．
	以下の -jis または -oyayubi オプションを省略した場合の省略値．

    -jis
	富士通のＸサーバでＪＩＳキーボードを使用するときに指定する．

    -oya[yubi]
	富士通のＸサーバで親指キーボードを使用するときに指定する．

    -fg 色
	ウィンドウの前面色を指定する．省略値は，白である．

    -bg 色
	ウィンドウの背景色を指定する．省略値は，黒である．

    -bd 色
	ウィンドウを囲む枠の色を指定する．省略値は，白である．

    -bw 数字
	ウィンドウを囲む枠の幅をピクセル単位で指定する．

    -bp ビットマップファイル名
	ウィンドウの背景ピックスマップを指定する．
	ファイル名に"."を指定すると"X11/bitmaps/1x1"同様な背景を表示する．

    -fn フォント
	英数カナ文字用フォントを指定する． 省略値は，
	"-*-fixed-medium-r-normal--14-*-*-*-*-*-jisx0201.1976-*"である．

    -fk フォント
	漢字用フォントを指定する．省略値は，
	"-*-fixed-medium-r-normal--14-*-*-*-*-*-jisx0208.1983-*"である．

    -nobeep
	ベルを鳴らさない時に指定する．省略値は鳴らす．

    -v[erbose]
	XLIBのエラーメッセージの表示をするか指定する．省略値は表示しない．

    その他の辞書に依存するオプションについては, READMEを参照すること.
    	
リソース

    xfeoak は、標準的なリソースのほかに，以下のものを受け付ける．

    keyBord (KeyBord クラス)
	使用するキーボードの種別を指定する．省略値はnoneである．
    	none    : 一般のＸサーバ で使用するときに指定する．
    	jis     : 富士通のＸサーバでＪＩＳキーボードを使用するときに指定する．
    	oyayubi : 富士通のＸサーバで親指キーボードを使用するときに指定する．
    
    backgroundPixmap (BackgroundPixmap クラス)
	ウィンドウの背景ピックスマップを指定する．
	ファイル名に"."を指定すると"X11/bitmaps/1x1"同様な背景を表示する．

    font (Font クラス)
	英数カナ文字用フォントを指定する．省略値は，
	"-*-fixed-medium-r-normal--14-*-*-*-*-*-jisx0201.1976-*"である．

    kanjiFont (KanjiFont クラス)
	漢字用フォントを指定する．省略値は，
	"-*-fixed-medium-r-normal--14-*-*-*-*-*-jisx0208.1983-*"である．

    maxClient (MaxClient クラス)
	接続可能なアプリケーション数を指定する．省略値は32である．

    workSize (WorkSize クラス)
	初期化時に確保する作業用領域の大きさを指定する．省略値は4096である．

    beep (Beep クラス)
	ベルを鳴らすかどうかを（on/off)で指定する．省略値はon(鳴らす)である．

環境

    DISPLAY  : 省略時のＸサーバのホスト名と番号を得る．

操作方法

  1) 一般的な操作方法

      一般のＸサーバ で使用する場合の操作方法は，ＪＩＳキーボードを使用する
    場合の操作に準じます．
    物理的なカナ/英字キーの切替えは論理的な入力モードより優先します．

    基本動作方法を以下に示します．
       1. 起動方法
             $ xfeoak &
             画面の下部にウィンドウが表示されます．
       2. 日本語入力開始
            Control + スペース キーを押下すると開始します．
            下部ウィンドウの表示が「日本語     Ｒかな」に変更されます．
       3. 入力例
            入力キーで 'a','i'を押下すると，Preeditウィンドウに"あい"と
            表示されます．
            Control + h を押下すると Preeditウィンドウ内の文字がかな漢字変換
            します．その結果"愛"と表示します．
            （再度 Control + H で次候補が表示されます）
            Control + l を押下すると確定しアプリケーションに文字列を通知します．
       4. 日本語入力終了
            Control + スペース　キーを押下すると終了します． 

    その他の詳細の操作方法を以下に示す．
       
     - 入力モード（ステータス表示）については，表１を参照してください．
     - 入力モードの遷移方法は，表２を参照してください．
     - 各種変換方法については，表３を参照してください．

  2) ＯＡＫの操作方法

      富士通製のＸサーバ でＪＩＳキーボードまたは親指キーボードを使用する場合
    の操作方法は，ＯＡＫ日本語入力と同様に操作できます．
    ただし，いくつかの機能はサポートされていません．

その他

    かな漢字変換（辞書）がなくても動作します．この時には，ローマ字変換，
    ひらがな，カタカナ変換のみになります．


関連項目

    ximpterm(1)

付録

  表１　入力モード

    --------------------+---------------------------------
    ステータス表示文字	| 入力状態
    --------------------+---------------------------------
    	       英数文字 | フロントエンドと接続していない状態
    --------------------+---------------------------------
    日本語     Ｒかな 	| 全角ローマ字かな変換入力状態
    日本語     Ｒカナ	| 全角ローマ字カナ変換入力状態
    日本語     ひらがな	| 全角ひらがな入力状態
    日本語     カタカナ	| 全角カタカナ入力状態
    日本語     英小文字	| 全角英小文字入力状態
    日本語     英大文字	| 全角英大文字入力状態
    日本語 半  Ｒかな 	| 半角ローマ字かな変換入力状態
    日本語 半  Ｒカナ	| 半角ローマ字カナ変換入力状態
    日本語 半  ひらがな	| 半角ひらがな入力状態
    日本語 半  カタカナ	| 半角カタカナ入力状態
    日本語 半  英小文字	| 半角英小文字入力状態
    日本語 半  英大文字	| 半角英大文字入力状態
    --------------------+---------------------------------

  表２　入力モード遷移

     ANK入力(フロントエンドと接続しない）
        ↑
      Control + Space
        ↓
    +--------------------- 日本語入力モード ------------------------+
    | +---------------------- 全角入力 --------------------------+  |
    | | +------------------------------------------------------+ |  |
    | | | +-ローマ字入力状態-+          +- ひらがな入力状態 -+ | |  |
    | | | |                  |          |                    | | |  |
    | | | | ひらがな変換入力 |          |  ひらがな入力      | | |  |
    | | | |     ↑           |          |      ↑            | | |  |
    | | | |     F1           | <- F3 -> |      F1            | | |  |
    | | | |     ↓           |          |      ↓            | | |  |
    | | | | カタカナ変換入力 |          |  カタカナ入力      | | |  |
    | | | +------------------+          +--------------------+ | |  |
    | | +------------------------------------------------------+ |  |
    | |                        F4   ↑                           |  |
    | |                        ↓   F1                           |  |
    | | +------------------------------------------------------+ |  |
    | | | +------------------+          +------------------+   | |  |
    | | | | 英小文字入力状態 | <- F4 -> | 英大文字入力状態 |   | |  |
    | | | +------------------+          +------------------+   | |  |
    | | +------------------------------------------------------+ |  |
    | +----------------------------------------------------------+  |
    |                            ↑                                 |
    |                            F2                                 |
    |                            ↓                                 |
    | +----------------------------------------------------------+  |
    | |                       半角入力                           |  |
    | +----------------------------------------------------------+  |
    +---------------------------------------------------------------+

  表３　かな漢字変換中（Preeditウィンドウ表示中）のキーと機能の対応表

    --------------------+---------------------------------------
       キーシム		|		機能
    --------------------+---------------------------------------
     Control + "h"	| 変換／次候補
     Control + "p"	| 前候補
     Control + "m"	| 無変換
     Control + "l"	| 確定
     Control + "w"	| 漢字選択
     Control + "u"	| 取消
     Control + "d"	| カーソル位置の一字削除
     Control + "i"	| カーソル位置に一字挿入
     Control + "b"	| カーソルを左に移動へ
     Control + "f"	| カーソルを右に移動へ
     Control + "a"	| カーソルを文の先頭
     Control + "e"	| カーソルを文の最後
    --------------------+---------------------------------------
     Kanji		| 変換／次候補
     Muhenkan		| 無変換
     Control + Kanji	| 漢字選択
     Control + Muhenkan	| 前候補
     Cancel		| 取消
     Escape		| 取消
     Delete		| カーソル位置の一文字削除
     F28		| カーソル位置の一文字削除
     Insert		| カーソル位置に一文字挿入
     Execute		| 確定
     Linefeed		| 確定し，改行(Linefeed)
     Return		| 確定し，改行(Return)
     BackSpace		| カーソルを左に移動へ
     Left		| カーソルを左に移動へ
     Right		| カーソルを右に移動へ
     Up			| カーソルを文の先頭
     Down		| カーソルを文の最後
     Tab		| カーソルを文の最後
    --------------------+----------------------------------------
