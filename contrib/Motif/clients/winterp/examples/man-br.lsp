; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         man-br.lsp
; RCS:          $Header: man-br.lsp,v 1.5 91/10/05 18:30:54 mayer Exp $
; Description:  Load this file to create a simple manual page browser for
;		looking at formatted manual pages installed in
;		/usr/local/man/cat3/*.3X. I use this application to browse my Motif
;		manual pages, which are kept in that directory. To start WINTERP up
;		running man-br as a standalone application, do:
;		"winterp -init_file <path>/man-br.lsp -no_unix_server -no_init_msgs &"
;		where "<path>/man-br.lsp" is the full path to this file.
;		Assumptions:
;			(1) You have formatted manual pages for Motif
;			    in /usr/local/man/cat3/*.3X.
;			(2) $PAGER environment variable is set to a paging
;			    program such as "less" or "more". I've only tested
;			    this with "less"... it is possible that
;                           "xterm -e more" will exit automatically if
;			    displaying short pages...
;			(3) xterm(1) program is on your $PATH.
;		Hints:
;			The names of the Motif manual pages displayed in
;			the file-selection-box browser will be more
;			intuitive if you convert the manual page names to
;			long-filenames -- for details, see the
;			doc/build.doc script supplied with Motif 1.1.
;			(Within HP, you may pick up the longfilename
;			formatted manpages via anonymous ftp from
;			hplnpm.hpl.hp.com, directory pub,
;			file OSFMotif1.1.1-man-output.tar.Z)
; Author:       Niels Mayer, HPLabs
; Created:      Tue Jul  2 19:00:39 1991
; Modified:     Sat Oct  5 18:30:37 1991 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r5 contrib tape release
;
; WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
; XLISP version 2.1, Copyright (c) 1989, by David Betz.
;
; Permission to use, copy, modify, distribute, and sell this software and its
; documentation for any purpose is hereby granted without fee, provided that
; the above copyright notice appear in all copies and that both that
; copyright notice and this permission notice appear in supporting
; documentation, and that the name of Hewlett-Packard and Niels Mayer not be
; used in advertising or publicity pertaining to distribution of the software
; without specific, written prior permission.  Hewlett-Packard and Niels Mayer
; makes no representations about the suitability of this software for any
; purpose.  It is provided "as is" without express or implied warranty.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;
;;; resources for WinterpManBrowser and
;;; xterm displaying manual page ("xterm -name man-term")
;;;
;;; Mwm*WinterpManBrowser*iconImage: /usr/local/include/X11/bitmaps/xman-i.h
;;; Mwm*man-term*iconImage:          /usr/local/include/X11/bitmaps/xman-i.h
;;;
;;; !
;;; ! man-term == xterm -name man-term
;;; !
;;; ! man-term*name: xterm
;;; ! man-term*IconGeometry:
;;; ! man-term*title:
;;; ! man-term*iconName:
;;; 		       
;;; man-term*VT100*font:     -*-*-medium-r-*--10-*-*-*-m-*-iso8859-1
;;; man-term*VT100*boldFont: -*-*-bold-r-*--10-*-*-*-m-*-iso8859-1
;;; man-term*c132: false
;;; man-term*curses: true
;;; man-term*VT100*background: LightGrey
;;; man-term*VT100*foreground: Black
;;; ! man-term*background: White
;;; ! man-term*foreground: Black
;;; man-term*VT100*cursorColor: FireBrick
;;; man-term*jumpScroll: false
;;; man-term*logging: false
;;; man-term*logInhibit: true
;;; man-term*loginShell: false
;;; man-term*marginBell: false
;;; man-term*multiScroll: false
;;; man-term*reverseVideo: false
;;; man-term*reverseWrap: false
;;; man-term*saveLines: 0
;;; man-term*scrollBar: false
;;; man-term*scrollInput: false
;;; man-term*scrollKey: false
;;; man-term*signalInhibit: false
;;; man-term*tekInhibit: true
;;; man-term*tekStartup: false
;;; man-term*visualBell: false
;;;

(setq top_w
      (send APPLICATION_SHELL_WIDGET_CLASS :new "winterpManBrowser" "WinterpManBrowser"
	    :XMN_TITLE "Motif Manual Page Browser"
	    :XMN_ICON_NAME "ManBrowser"
	    ))

(setq fsb_w 
      (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new :managed "files" top_w
    ))
(send (send fsb_w :get_child :DIALOG_HELP_BUTTON) :unmanage)
(send (send fsb_w :get_child :DIALOG_CANCEL_BUTTON) :unmanage)

(send fsb_w :set_callback :XMN_OK_CALLBACK
      '(CALLBACK_VALUE)
      '(
	(let (
	      (filename (xm_string_get_l_to_r CALLBACK_VALUE))
	      )
	  (system (format NIL
			  "xterm -geometry 120x24 -name man-term -T ~A -n ~A -e $PAGER ~A &"
			  filename
			  (remove-path filename)
			  filename
			  ))
	  )
	))

(send fsb_w :do_search "/usr/local/man/cat3/*.3X")

(send top_w :realize)

(defun remove-path (string)
  (do
   ((i (1- (length string)) (1- i))
    )
   ((or (< i 0) (char= (char string i) #\/))
    (if (< i 0)
	string
      (subseq string (1+ i)))
    )
   )
  )
