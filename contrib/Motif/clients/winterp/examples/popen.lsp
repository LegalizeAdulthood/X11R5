; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         popen.lsp
; RCS:          $Header: popen.lsp,v 1.3 91/10/05 18:33:42 mayer Exp $
; Description:  Play around with  POPEN to collect unix data.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:51:55 1989
; Modified:     Sat Oct  5 18:33:26 1991 (Niels Mayer) mayer@hplnpm
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


(defun ls (dirname)
  (do* 
   ((fp (popen (strcat "/bin/ls -1 -r " dirname) :direction :input))
    (line (read-line fp) (read-line fp))
    (result (list line) (cons line result))
    )
   ((null line)
    (pclose fp)
    (cdr result)
    )
   )
  )

(defun vls (dirname)
  (do* 
   ((top_w (send TOP_LEVEL_SHELL_WIDGET_CLASS :new
		 :XMN_GEOMETRY "500x500+0+0"
		 ))
    (rc_w (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed top_w
		:XMN_ADJUST_LAST nil
		))
    (fp (popen (strcat "/bin/ls -1 -r " dirname) :direction :input))
    (line (read-line fp) (read-line fp))
    )
   ((null line)
    (pclose fp)
    (send top_w :realize)
    )
   (send (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed rc_w
	       :XMN_LABEL_STRING line)
	 :add_callback :XMN_ACTIVATE_CALLBACK '()
	 `((system (strcat "$EDITOR " ,dirname "/" ,line))))
   )
  )

(defun mh-scan (foldername msgs)
  (do* 
   ((fp (popen (strcat "scan -reverse +" foldername " " msgs) 
	       :direction :input))
    (line (read-line fp) (read-line fp))
    (result (list line) (cons line result))
    )
   ((null line)
    (pclose fp)
    (cdr result)
    )
   )
  )

(mh-scan "inbox" "all")
(vls "/users/mayer")
