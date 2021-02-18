; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         graphcalc-sliderdisp.lsp
; RCS:          $Header: graphcalc2.lsp,v 1.2 91/10/05 16:03:37 mayer Exp $
; Description:  Add a scale widget to display results from graphcalc.lsp
; Author:       Niels Mayer, HPLabs
; Created:      Tue Jul 10 10:35:58 1990
; Modified:     Sat Oct  5 16:03:14 1991 (Niels Mayer) mayer@hplnpm
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

; (send (get_moused_widget) :destroy)
; (send calc_keyboard_w :set_values :xmn_editable t)

(setq  disp_slider_w
 (send XM_SCALE_WIDGET_CLASS :new :managed "disp-slider" calc_keyboard_w
      :XMN_ORIENTATION		:horizontal
      :XMN_PROCESSING_DIRECTION :MAX_ON_RIGHT 
      :XMN_SENSITIVE		nil
      :XMN_MINIMUM		0
      :XMN_MAXIMUM		+100
      :XMN_VALUE		0
      :xmn_width 500
      :xmn_scale_width 500
      :xmn_scale_height 30
      :xmn_height 30
      :xmn_x 0
      :xmn_y 200
      ))

(send disp_slider_w :set_values
      :xmn_width 500
      :xmn_scale_width 500
      :xmn_scale_height 30
      :xmn_height 30
      )



;;
;; this gets called when a graph node gets clicked, it displays the value
;; of the clicked graph node and sets that as a possible operand for other
;; operators
(send *calc_display_class* :answer :set_display_value_from_graphnode '(value-widget)
      '(
	(let ((value
	       (truncate (send value-widget :get_value)))
	      min_val
	      max_val)
	  (setq display-value-widget value-widget)
	  (setq modify-verify-callback-enabled nil)
	  (send-super :set_string (format nil "~A" value))
	  (setq modify-verify-callback-enabled t)
	  (setq begin-entry-p t)
	
	  (send disp_slider_w :get_values 
		:XMN_MINIMUM 'min_val
		:XMN_MAXIMUM 'max_val)
	  (format T "value=~A;min=~A; max=~A\n" value min_val max_val)
	  (cond
	   ((< value min_val)
	    (send disp_slider_w :set_value min_val)
	    )
	   ((> value max_val)
	    (send disp_slider_w :set_value max_val)
	    )
	   (t
	    (send disp_slider_w :set_value value)
	    )
	   )
	)
      )
      )


(send *calc_display_class* :answer :exec_binary_operator '(operator-symbol)
      '(
	;; if display-value-widget is non-null, then a result has been set by clicking an operator node in the graph widget;
	;; on first entering an expression in the display, the :XMN_MODIFY_VERIFY_CALLBACK fires which clears the disp and
	;; sets display-value-widget to NIL. when display-value-widget is NIL we convert the expression in the display into a sexp and graph it.
	(if (null display-value-widget)
	    (setq display-value-widget
		  (display-equation (read (make-string-input-stream (strcat "( " (send-super :get_string) " )"))))))
	;; display-value-widget is now guaranteed to hold widget assoc'd with displayed value

	(let (value
	      min_val
	      max_val)

	  (cond
	   ;; if there is a prev operator, then we want to create a new node corresponding to prev-op-symbol
	   ;; whose args are the current value of the accumulator and the current display.
	   ;; if the accumilator is NIL, the we don't pass that arg to widget.
	   (prev-operator-symbol
	    (let ((operator-representor-class (get-operator-class prev-operator-symbol))
		  w)
	      (cond
	       (operator-representor-class ;get-operator-class returns NIL if the operator was not defined with make-operator
		(setq w (send operator-representor-class :new NIL))
		(send w :add_arg display-value-widget)
		(setq display-value-widget w)
		(if accumulator-value-widget
		    (send display-value-widget :add_arg accumulator-value-widget))
		(setq modify-verify-callback-enabled nil)
		(send-super :set_string (format nil "~A" (truncate (send display-value-widget :get_value)))) ;display the result
		(setq modify-verify-callback-enabled t)
		(setq accumulator-value-widget display-value-widget)
		)))
	    )
	   ;; else there was no prev operator, meaning last operation was an ==
	   ;; just display the value, and save it in the accumulator for next time
	   (t
	    (setq modify-verify-callback-enabled nil)
	    (send-super :set_string (format nil "~A" (truncate (send display-value-widget :get_value))))
	    (setq modify-verify-callback-enabled t)
	    (setq accumulator-value-widget display-value-widget)    
	    ))

	  (setq begin-entry-p t)
	  (setq prev-operator-symbol operator-symbol)
	  (setq value (truncate (send display-value-widget :get_value)))
	  (send disp_slider_w :get_values 
		:XMN_MINIMUM 'min_val
		:XMN_MAXIMUM 'max_val)
	  (cond
	   ((< value min_val)
	    (send disp_slider_w :set_value min_val)
	    )
	   ((> value max_val)
	    (send disp_slider_w :set_value max_val)
	    )
	   (t
	    (send disp_slider_w :set_value value)
	    )
	   )
	  )
	))
