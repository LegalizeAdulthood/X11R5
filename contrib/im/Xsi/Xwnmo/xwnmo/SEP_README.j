/*
 * $Id: SEP_README.j,v 1.1 1991/09/13 07:45:49 proj Exp $
 */
	SEP - SpotLocation拡張プロトコル

SEPは、スポットロケーションの高速移動を実現するプロトコルです。

通常、アプリケーションにおけるカーソル移動の処理は以下ようになります。

	nested_list = XVaCreateNestedList(dummy, XNSpotLocation, spot NULL);
	XSetICValues(ic, XNPreeditAttributes, nested_list, NULL);
	XFree(dpy, nested_list);

ターミナルエミュレータのようなアプリケーションの場合、頻繁にカーソルを移動する
必要があります。それゆえ、このような処理は、アプリケーションの性能を著しく落して
しまいます。
SEPは、この問題を解決します。アプリケーションは、SEPを使うか、以下の関数を呼ぶ
だけです。

	_XipChangeSpot(ic, spot_x, spot_y);

_XipChangeSpot()の詳細は、xwnmoのマニュアルのSEP部分を参照下さい。

-------------------------------------------------------------------------
XIMライブラリとXWNMO間のソケットを使った通信データ

    要求
	バイト数	値		内容
    XIM -> XWNMO (ximChangeSpotReq)
	1	    XIM_ChangeSpot(20)	リクエストタイプ
	1				未使用
	2		12		データの長さ
	4		CARD32		xic
	2		INT16		spot location x
	2		INT16		spot location y

    応答
	バイト数	値		内容
    XIM <- XWNMO (sz_ximEventReply)
	2				応答
			0		 正常
			-1		 異常
	6				未使用

