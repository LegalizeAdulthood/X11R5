; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         getvalues.lsp
; RCS:          $Header: getvalues.lsp,v 1.3 91/10/05 15:57:54 mayer Exp $
; Description:  This file should is a random test to see whether the code
;               in winterp/src-server/w_resources.c has any machine dependencies.
;               Load this file, and if your stdout beeps and you see messages
;               about "failed: ..." then please send the output to mayer@hplabs.hp.com
;
;		NOTE: the actual graphical result of loading this file is not
;		pretty. In fact, it's not supposed to be pretty....
; Author:       Niels Mayer, HPLabs
; Created:      Tue Jul 31 19:50:20 1990
; Modified:     Sat Oct  5 15:56:26 1991 (Niels Mayer) mayer@hplnpm
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

(progn
  (setq top_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "Winterp: Set/GetValues Test"
	      ))
  (setq scr_w
	(send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed top_w
	      :XMN_SCROLLING_POLICY :AUTOMATIC
	      ))
  (setq rc_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	      "rc" scr_w
	      :XMN_ORIENTATION :VERTICAL
	      :XMN_PACKING :PACK_COLUMN
	      :XMN_NUM_COLUMNS 2
	      :XMN_ENTRY_ALIGNMENT :ALIGNMENT_CENTER
	      ))

  (setq te_w (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled rc_w
		   :XMN_EDIT_MODE :MULTI_LINE_EDIT
		   ))
  (setq pb_w (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb" rc_w))
  (setq to_w (send XM_TOGGLE_BUTTON_WIDGET_CLASS :new :managed "tog" rc_w))
  (setq pa_w (send XM_PANED_WINDOW_WIDGET_CLASS :new :managed "paned" rc_w))
  (setq te1_w (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled pa_w
		    :XMN_EDIT_MODE :MULTI_LINE_EDIT
		    ))
  (setq te2_w (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled pa_w
		    :XMN_EDIT_MODE :MULTI_LINE_EDIT
		    ))
  (setq te3_w (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled pa_w
		    :XMN_EDIT_MODE :MULTI_LINE_EDIT
		    ))
  (setq te4_w (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled pa_w
		    :XMN_EDIT_MODE :MULTI_LINE_EDIT
		    ))
  (setq la_w (send XM_LABEL_WIDGET_CLASS :new :managed "label" rc_w))

  (setq li_w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled rc_w
	      :xmn_selection_policy :browse_select
	      :xmn_items #("foo" "bar" "baz" "bof" "boof" "frob" "snob" "blog")
	      :xmn_item_count 8
	      :xmn_visible_item_count 20
	      ))

  (send top_w :realize)
  )

(defun test (w r v)
  (let (result)
    (send w :set_values r v)
    (send w :get_values r 'result)
    (cond
     ((eq v result)
      (format T "success: widget=~A resource=~A value=~A\n" w r v))
     (t
      (format T "\007\007\007failed: widget=~A resource=~A value=~A result=~A\n" w r v result))
     )))


(test pb_w :XMN_BORDER_WIDTH 6)		;XmRDimension
(test pb_w :XMN_HIGHLIGHT_THICKNESS 10)	;XmRShort
(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
    (test pb_w :XMN_SHOW_AS_DEFAULT 5)	;Motif 1.0 -- XmRShort
  (test pb_w :XMN_SHOW_AS_DEFAULT T))	;Motif 1.1 -- XmRBooleanDimension
(test pb_w :XMN_SHADOW_THICKNESS 6)	;XmRShort
(test pa_w :XMN_SASH_INDENT -20)	;XmRPosition
(test rc_w :XMN_NUM_COLUMNS 1)		;XmRShort
(if (not (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))) ;don't do this test for Motif 1.0
    (test to_w :XMN_SPACING 100) ;buggy in 1.0 due to misdeclared resource  names in Motif 1.0 (see comments in w_resources.c)
  )
(test rc_w :XMN_SPACING 10)
(test rc_w :XMN_SPACING 10)
(test pa_w :XMN_SPACING 50)

(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
    (test rc_w :xmn_rcmargin_width 21)	;Motif 1.0 workaround
  (test rc_w :xmn_margin_width 10)	;works in 1.1, buggy in 1.0
  )

(test la_w :xmn_margin_width 22)

(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
    (test rc_w :xmn_rcmargin_height 21)  ;Motif 1.0 workaround
  (test rc_w :xmn_margin_height 20)	;works in 1.1, buggy in 1.0
  )
(test la_w :xmn_margin_height 22)

(test rc_w :XMN_ORIENTATION :vertical)
(test rc_w :XMN_PACKING :pack_none)
(test rc_w :XMN_ENTRY_ALIGNMENT :alignment_beginning)
(test rc_w :XMN_PACKING :no_packing)

(test li_w :XMN_SELECTION_POLICY :multiple_select)
(test li_w :XMN_SELECTION_POLICY :single_select)
(test li_w :XMN_SELECTION_POLICY :browse_select)
(test li_w :XMN_SELECTION_POLICY :extended_select)

(test top_w :xmn_keyboard_focus_policy :explicit)
(test top_w :xmn_keyboard_focus_policy :pointer)

;; XmRBoolean
(test top_w :XMN_OVERRIDE_REDIRECT t)
(test top_w :XMN_OVERRIDE_REDIRECT nil)
(test top_w :XMN_WAIT_FOR_WM nil)
(test top_w :XMN_WAIT_FOR_WM t)
(test top_w :XMN_TRANSIENT t)
(test top_w :XMN_TRANSIENT nil)

;; XmRChar
(test la_w :XMN_MNEMONIC #\a)
(test la_w :XMN_MNEMONIC #\Z)
(test la_w :XMN_MNEMONIC #\ )
(test la_w :XMN_MNEMONIC #\z)
(test la_w :XMN_MNEMONIC #\\)
(test la_w :XMN_MNEMONIC #\l)
