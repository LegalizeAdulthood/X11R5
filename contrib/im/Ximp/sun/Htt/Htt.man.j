Htt(1)                  ユーザ・コマンド                   Htt(1)

【名前】
    Htt - X Window version 11 Release 5 用 多国語入力サーバー

【形式】
    Htt [-display displayname] [-geometry geom] 
    [-fg color] [-bg color] [-bd color] [-bw number]
    [-bp bitmapfile] [-fn fontname] [-fk kanjifontname] [-verbose]

【使用条件】
    今回のバージョンは、SunOS4.x/JLE1.x の国際化支援ライブラリへの
    依存性および言語エンジンへの依存性をクリーナップしていないため、
    作成／実行に際して SunOS4.x/JLE1.x が必要です。
    特に、言語エンジンは、JLE の提供する libmle 経由でアクセスする
    インターフェイスしか、十分にテストされていません。
    また、入力インターフェースとしては Callback 系しか十分に
    テストされていません。
    libmle の致命的なバグのため、使用可能な libmle は、
    libmle.so.1.4 以降に限られます。
【国際機能】
【機能説明】
    Htt は，X Window System用の多国語入力サーバーです．
　　Htt と Input method library 間の通信は，XIMP version 3.5 プロトコル
　　を使用します．このプロトコルは，X Window System で規定される
　　クライアントメッセージとプロパティのみを使用しているため、
    X プロトコル内で閉じており、他の手段（たとえば socket による独自の
    通信路）を必要としない、きわめて X Window 的にクリーンなものです。

　　Httは，以下の特徴を持っています．
     ◎ 業界標準の XIMP プロトコルを使用しているため、
        他の入力サーバーと高い interoperability をもちます。
     ◎ 複数の言語エンジンをサポートします。
     ◎ 複数の入力スタイルをサポートします。
	入力スタイルとして、
		Preedit Nothing （Htt による root window）
		Preedit Position（Htt による over the spot）
		Preedit Callback（client によって描画）
	ステータス・スタイルとして、
		Status Nothing  （Htt の status window に表示）
		Status Area     （Htt よって client の領域に表示）
		Status Callback （client によって描画）
	候補選択スタイルとして、
		Htt の status window に表示
		Htt の popup window に表示
		Lookup Choice Callback （client によって描画）


【オプション】

    -display ディスプレイ
	使用するディスプレイを指定する．
	これによって、ネットワークを介してリモートから
	入力機能を提供できる。

    -geometry ジオメトリ
	ウィンドウのサイズと位置を指定する．

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
    	
リソース

    backgroundPixmap (BackgroundPixmap クラス)
	ウィンドウの背景ピックスマップを指定する．

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

【環境】

    DISPLAY  : 省略時のＸサーバのホスト名と番号

【操作方法】

    JLE1.x のかな漢字変換エンジンを使った場合の標準的操作方法を以下に示します．
       1. 起動方法
             % Htt &
             サーバーウィンドウが立ち上がります。
       2. 日本語入力開始
            Control + スペース キーを押下すると開始します．
       3. 入力例
	    ローマ字入力が可能です。
            Control + n を押下すると Preeditウィンドウ内の文字がかな漢字変換
            します．
	    Control + w を押下すると、候補選択モードになります。
            Control + k を押下すると確定しアプリケーションに文字列を
	    通知します．
       4. 日本語入力終了
            Control + スペース　キーを押下すると終了します． 

関連項目

  JLE 環境化での、デフォールト・キーバインディングについては
  マニュアルを参照してください。
