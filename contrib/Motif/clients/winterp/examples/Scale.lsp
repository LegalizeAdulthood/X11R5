; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         scale-widg.lsp
; RCS:          $Header: Scale.lsp,v 1.2 91/10/05 04:03:06 mayer Exp $
; Description:  XmScale widget tests.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Jul  7 21:43:32 1990
; Modified:     Sat Oct  5 04:02:32 1991 (Niels Mayer) mayer@hplnpm
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

(setq toplevel_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "sclshl"
	    :XMN_TITLE "Winterp:SCALE TEST"
	    :XMN_ICON_NAME "W:SCALE TEST"
	    ))

(setq rc_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	    "rc" toplevel_w
	    :XMN_ORIENTATION :HORIZONTAL
	    :XMN_PACKING :PACK_COLUMN
	    :XMN_ENTRY_ALIGNMENT :ALIGNMENT_CENTER
	    ))

(setq scale0_w 
      (send XM_SCALE_WIDGET_CLASS :new :managed "scale" rc_w
	    :XMN_TITLE_STRING "Utterness\nCoefficient"
	    :XMN_SENSITIVE t
	    ))
(send XM_LABEL_WIDGET_CLASS :new :managed "100" scale0_w
      )
(send XM_LABEL_WIDGET_CLASS :new :managed "50" scale0_w
      )
(send XM_LABEL_WIDGET_CLASS :new :managed "0" scale0_w
      )

(setq scale1_w 
      (send XM_SCALE_WIDGET_CLASS :new :managed "scale" rc_w
	    :XMN_TITLE_STRING "lameness\nper\nsecond^2"
	    :XMN_SENSITIVE t
	    :XMN_SHOW_VALUE t
	    ))

(send toplevel_w :realize)

(send scale0_w :set_value 50)
(send scale1_w :get_value)





