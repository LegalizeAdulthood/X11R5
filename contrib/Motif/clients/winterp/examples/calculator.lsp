; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         calculator.lsp
; RCS:          $Header: calculator.lsp,v 1.2 91/10/05 05:25:18 mayer Exp $
; Description:  A simple calculator -- note: the layout is pretty ugly.
;               Just load this file to get the application.
; Author:       Niels Mayer, HPLabs
; Created:      Wed Jun 27 23:39:09 1990
; Modified:     Sat Oct  5 05:24:44 1991 (Niels Mayer) mayer@hplnpm
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

(setq top_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "calc_shell"
	    :XMN_TITLE "WinterCalc"
	    :XMN_ICON_NAME "Calc"
	    ))

(setq paned_w
	(send XM_PANED_WINDOW_WIDGET_CLASS :new :managed
	      "pane" top_w
	      ))


;==============================================================================
;============================== The display object=============================
;==============================================================================

;; make a subclass of XM_TEXT_WIDGET_CLASS
(setq *calc_display_class*
      (send Class :new
	    '(cursor_pos
	      positive_p
	      ins_mode_p
	      begin_numentry_p
	      accumulator
	      prev_operator_symbol
	      )
	    '()				;no class variables for subclass
	    XM_TEXT_WIDGET_CLASS)) 

;; override XM_TEXT_WIDGET_CLASS's instance initializer
(send *calc_display_class* :answer :isnew '(init-value &rest args)
      '(
	(setq cursor_pos 0)
	(setq positive_p t)
	(setq ins_mode_p t)
	(setq begin_numentry_p nil)
	(setq accumulator 0.0)
	(setq prev_operator_symbol nil)
	(apply 'send-super
	       `(:isnew ,@args
			:XMN_STRING		""
			:XMN_EDIT_MODE		:single_line_edit
			:XMN_AUTO_SHOW_CURSOR_POSITION t
			:XMN_CURSOR_POSITION	,cursor_pos
			:XMN_EDITABLE		nil
;;;			:XMN_FOREGROUND		"Red"
;;;			:XMN_BACKGROUND		"Black"
			))
	))

(send *calc_display_class* :answer :enter_keystroke '(key_str)
      '(
	(cond
	 (begin_numentry_p
	  (send self :clear)
	  (setq begin_numentry_p nil)
	  )
	 )
	(cond
	 (ins_mode_p
	  (send self :REPLACE cursor_pos cursor_pos key_str)
	  (setq cursor_pos (1+ cursor_pos))
	  (send self :SET_INSERTION_POSITION cursor_pos)
	  )
	 (t
	  (send self :REPLACE cursor_pos (1+ cursor_pos) key_str)
	  ))
	)
      )

(send *calc_display_class* :answer :change_sign '()
      '(
	(cond
	 (positive_p
	  (send self :REPLACE 0 0 "-")
	  (setq cursor_pos (1+ cursor_pos))
	  (send self :SET_INSERTION_POSITION cursor_pos)
	  (setq positive_p nil)
	  )
	 (t
	  (send self :REPLACE 0 1 "")
	  (setq cursor_pos (1- cursor_pos))
	  (send self :SET_INSERTION_POSITION cursor_pos)
	  (setq positive_p t)
	  )))
      )

(send *calc_display_class* :answer :clear '()
      '(
	(setq cursor_pos 0)
	(setq positive_p t)
	(setq ins_mode_p t)
	(send self :set_values
	      :XMN_STRING ""
	      :XMN_CURSOR_POSITION cursor_pos
	      )
	))

(send *calc_display_class* :answer :exec_unary_operator '(operator_symbol)
      '(
	(send self :set_accumulator_and_display 
	      (funcall operator_symbol (send self :get_display_as_flonum)))
	(setq prev_operator_symbol nil)
	))

(send *calc_display_class* :answer :exec_binary_operator '(operator_symbol)
      '(
	(if prev_operator_symbol
	    (send self :set_accumulator_and_display 
		  (funcall prev_operator_symbol (send self :get_accumulator) (send self :get_display_as_flonum)))
	  (send self :set_accumulator_and_display (send self :get_display_as_flonum))
	  )
	(setq prev_operator_symbol operator_symbol)
	))

;; sets the accumulator to result_flonum, and displays that.
;; sets begin_numentry_p to true so that upon numentry, display is cleared and new number input.
(send *calc_display_class* :answer :set_accumulator_and_display '(result_flonum)
      '(
	(setq accumulator result_flonum)
	(setq cursor_pos 0)
	(setq positive_p (not (minusp result_flonum)))
	(setq ins_mode_p t)
	(setq begin_numentry_p t)
	(send self :set_values
	      :XMN_STRING (format NIL "~A" result_flonum)
	      :XMN_CURSOR_POSITION cursor_pos
	      )
	))

(send *calc_display_class* :answer :get_accumulator '()
      '(
	accumulator
	))


(send *calc_display_class* :answer :get_display_as_flonum '()
      '(
	(float (read (make-string-input-stream (send self :get_string))))
	))

(setq *calc_display*
      (send *calc_display_class* :new 0 :managed "disp" paned_w
	    ))

;==============================================================================
;========================= The Numberpad ======================================
;==============================================================================

(defun make-number-button (parent_widget name)
  (send
   (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed name parent_widget
;;;      :XMN_FOREGROUND "Yellow"
;;;      :XMN_BACKGROUND "DimGrey"
	 )
   :add_callback :xmn_activate_callback '()
   `(
     (send *calc_display* :enter_keystroke ,name)
     )
   ))

(defun make-chs-button (parent_widget name)
  (send
   (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed name parent_widget
;;;      :XMN_FOREGROUND "DimGrey"
;;;      :XMN_BACKGROUND "Yellow"
	 )
   :add_callback :xmn_activate_callback '()
   `(
     (send *calc_display* :change_sign)
     )
   ))

(setq numpad_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "numbers" paned_w
	    :XMN_ORIENTATION		:vertical
	    :XMN_PACKING		:pack_column
	    :XMN_NUM_COLUMNS		3
	    :XMN_ADJUST_LAST 		nil
	    :XMN_ENTRY_ALIGNMENT	:alignment_center
	    ))

(make-number-button numpad_w "7")
(make-number-button numpad_w "4")
(make-number-button numpad_w "1")
(make-chs-button    numpad_w "+/-")

(make-number-button numpad_w "8")
(make-number-button numpad_w "5")
(make-number-button numpad_w "2")
(make-number-button numpad_w "0")

(make-number-button numpad_w "9")
(make-number-button numpad_w "6")
(make-number-button numpad_w "3")
(make-number-button numpad_w ".")

;==============================================================================
;========================= Function Keys ======================================
;==============================================================================

(setq funcpad_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "functions" paned_w
	    :XMN_ORIENTATION		:vertical
	    :XMN_PACKING		:pack_column
	    :XMN_NUM_COLUMNS		3
	    :XMN_ADJUST_LAST 		nil
	    :XMN_ENTRY_ALIGNMENT	:alignment_center
	    ))

(defun make-unary-operator (parent_widget operator_symbol name)
  (send
   (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed name parent_widget
;;;	 :XMN_FOREGROUND "White"
;;;	 :XMN_BACKGROUND "Blue"
	 )
   :add_callback :xmn_activate_callback '()
   `(
     (send *calc_display* :exec_unary_operator operator_symbol)
     )
   ))

(defun make-binary-operator (parent_widget operator_symbol name)
  (send (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed name parent_widget
;;;	      :XMN_FOREGROUND "White"
;;;	      :XMN_BACKGROUND "Blue"
	      )
	:add_callback :xmn_activate_callback '()
	`(
	  (send *calc_display* :exec_binary_operator operator_symbol)
	  )
	))

(make-binary-operator funcpad_w #'/    "/")
(make-binary-operator funcpad_w #'*    "*")
(make-binary-operator funcpad_w #'-    "-")
(make-binary-operator funcpad_w #'+    "+")
(make-binary-operator funcpad_w #'expt "x^y")
(make-binary-operator funcpad_w NIL    "=" ) ; NOTE: = is a special NO-OP

(make-unary-operator funcpad_w #'sin   "Sin")
(make-unary-operator funcpad_w #'cos   "Cos")
(make-unary-operator funcpad_w #'tan   "Tan")
(make-unary-operator funcpad_w #'asin  "ArcSin")
(make-unary-operator funcpad_w #'acos  "ArcCos")
(make-unary-operator funcpad_w #'atan  "ArcTan")
(make-unary-operator funcpad_w #'exp   "Exp")
(make-unary-operator funcpad_w #'sqrt  "Sqrt")

(send 
 (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "Clear" funcpad_w
;;;    :XMN_FOREGROUND "White"
;;;    :XMN_BACKGROUND "Blue"
       )
 :add_callback :xmn_activate_callback '()
 `(
   (send *calc_display* :set_accumulator_and_display 0)
   )
 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(send top_w :realize)

;(let (height)
;  (send controlpanel_w :get_values :xmn_height 'height)
;  (send controlpanel_w :set_values
;	:xmn_maximum height
;	:xmn_minimum height
;	)
;  )
