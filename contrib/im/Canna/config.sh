/*
 * このファイルでは、各ファイルをインストールするディレクトリなどを
 * 決定するためのマクロを設定します。
 * 
 * ※コンパイルを行う前に必ず config.sh の設定を行ってください。
 * 
 * 以下に示すマクロを設定して下さい。
 *
 * irohaBinDir;
 *		コマンドをインストールするディレクトリです。
 *		このディレクトリにコマンドサーチパスを張るように
 *		してください。
 * irohaLibDir;
 *		ライブラリ等をインストールするディレクトリです。
 * DicDir;
 *		辞書をインストールするディレクトリです。
 * ErrDir:
 *		ログファイルをインストールするディレクトリです。
 *		ログファイルは、かな漢字変換サーバがエラーメッセージを
 *		出力するファイルです。
 * LockDir;
 *		ロックファイルをインストールするディレクトリです。    
 *		ロックファイルは、かな漢字変換サーバが
 *		複数個起動しないようにするために、作成されるファイルです。
 * LockFile;
 *		ロックファイルの名前です。
 */

irohaBinDir = /usr/local/bin
irohaLibDir = /usr/local/lib/iroha
DicDir = $(irohaLibDir)/dic
ErrDir = /tmp
LockDir = /tmp
LockFile = .IROHALOCK

/*
 * ここから下は変更しないようにしてください。
 */

              BINDIR = $(irohaBinDir)
              LIBDIR = $(irohaLibDir)
 IROHASERVER_DEFINES = -DLOCKDIR=\"$(LockDir)\" \
			-DDICHOME=\"$(DicDir)\" \
			-DERRDIR=\"$(ErrDir)\" \
			-DLOCKFILE=\"$(LockFile)\"
         RKC_DEFINES = -DIROHAHOSTFILE=\"$(irohaLibDir)/irohahost\"
       UILIB_DEFINES = -DLIBDIR=\"$(LIBDIR)\"
     SCRIPTS_DEFINES = -DLOCKFILEPATH=\"$(LockDir)/$(LockFile)\"
