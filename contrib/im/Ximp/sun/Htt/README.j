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

ただし、今回は、i18nXView を Client とすることを第一ターゲットと
していたので、i18nXView のサポートする、Callback 系のみしか
実質的なテストをしていません。また、i18nXView 自身が、SunOS4.x/JLE1.x
がなければコンパイルできないので、Htt も、SunOS4.x/JLE1.x のテスト
環境でしか、テストされていません。さらに言語エンジンとしては、
JLE の libmle を経由して接続される、kkcv でしかテストされていません。
くわえて、version 1.3 までの libmle は致命的なバグがあり、Htt との
組合せでは即死するため、libmle.so.1.4 以降のものをつかう必要があります。
そのため、SPARC 用の libmle.so.1.4 バイナリが本ディレクトリに含まれて
います。

Htt の開発にあたって、富士通株式会社殿より多大な援助を頂きました。
その援助なくしてはこの時期に Htt のリリースを行なうことはできなかったでしょう。
この場をお借りして御礼申し上げます。