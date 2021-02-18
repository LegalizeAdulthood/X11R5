; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         rc-shell.lsp
; RCS:          $Header: rc-shell.lsp,v 1.3 91/10/05 18:48:47 mayer Exp $
; Description:  Load this file to put up a default rowcolumn shell for
;               experimentation purposes. 
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:27:22 1989
; Modified:     Sat Oct  5 18:48:35 1991 (Niels Mayer) mayer@hplnpm
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

(defun make-rc-shell (s_geom)
  (let*
      ((toplevel_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
	      :XMN_GEOMETRY s_geom
	      :XMN_TITLE "Winterp:RC-SHELL"
	      :XMN_ICON_NAME "W:RC-SHELL"
	      ))
       (scrl_w
	(send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
	      "sc" toplevel_w
	      :XMN_SCROLLING_POLICY :AUTOMATIC
	      ))
       (rowcol_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	      "rc" scrl_w
	      :XMN_ORIENTATION :VERTICAL
	      :XMN_PACKING :PACK_TIGHT
	      :XMN_ENTRY_ALIGNMENT :ALIGNMENT_CENTER
	      )))

    (send toplevel_w :realize)
    (setq rc_w rowcol_w)
    (setq top_w toplevel_w)
    )
  )

(make-rc-shell "=300x300+0+0")
(format T "RC-SHELL Loaded:\n")
(format T "	TopLevelShell widget instance is in variable TOP_W\n")
(format T "	RowColumn widget instance is in variable RC_W\n")
