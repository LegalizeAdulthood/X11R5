; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         bitmap-br.lsp
; RCS:          $Header: bitmap-br.lsp,v 1.3 91/10/05 05:14:17 mayer Exp $
; Description:  Given a directory of X11 bitmaps at location
;               <bitmap_directory_path>, the function 
;                      (BROWSE-BITMAP-DIRECTORY <bitmap_directory_path>)
;               will put up a browser that will allow you to change your root
;               pixmap pattern by clicking on a bitmap image in the browser.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 00:53:06 1989
; Modified:     Sat Oct  5 05:13:37 1991 (Niels Mayer) mayer@hplnpm
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

(defun browse-bitmap-directory (dir)
  (let* (
	 (top_w (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "bmbrshl"
		      :XMN_GEOMETRY	"=360x720+0+0"
		      :XMN_TITLE	(strcat "Bitmap browser: " dir)
		      :XMN_ICON_NAME	(strcat "Bitmaps[" dir "]")
		      ))
	 (sc_w (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
		     "sc" top_w
		     :XMN_SCROLLING_POLICY :AUTOMATIC
		     ))
	 (rc_w (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
		     "rc" sc_w
		     :XMN_ORIENTATION		:vertical
		     :XMN_PACKING		:pack_tight
		     :XMN_ENTRY_ALIGNMENT	:alignment_center
;;;		     :XMN_FOREGROUND		"Black"
;;;		     :XMN_BACKGROUND		"LightGray"
		     ))
	 )
    (do* 
     ((fp (popen (strcat "/bin/ls " dir) :direction :input))
      (name (read-line fp) (read-line fp))
      bitmap-file
      )
     ((null name)
      (pclose fp)
      (send top_w :realize)
      )
     (setq bitmap-file (strcat dir "/" name))
     (format T "name=~A\n" name)
     (send XM_LABEL_GADGET_CLASS :new :managed
	   "filename" rc_w
	   :XMN_LABEL_TYPE	:string
	   :XMN_LABEL_STRING	name
	   )
     (send
      (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed
	    "image" rc_w
	    :XMN_LABEL_TYPE	:pixmap
	    :XMN_LABEL_PIXMAP	bitmap-file
	    )
      :add_callback :XMN_ACTIVATE_CALLBACK '()
      `((xsetroot ,bitmap-file))
      )
     (send XM_SEPARATOR_GADGET_CLASS :new :managed
	   "sep" rc_w
	   :XMN_SEPARATOR_TYPE :DOUBLE_LINE
	   )
     )
    )
  )

(defun xsetroot (filename)
  (system (format nil "xsetroot -bitmap ~A -fg Black -bg DimGrey" filename)))
