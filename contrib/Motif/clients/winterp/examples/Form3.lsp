; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         Form3.lsp
; RCS:          $Header: Form3.lsp,v 1.4 91/10/05 03:03:26 mayer Exp $
; Description:  Example options panel for Bob Leichner's audio controller...
; Author:       Niels Mayer, HPLabs
; Created:      Fri Jul 27 14:00:53 1990
; Modified:     Sat Oct  5 03:02:44 1991 (Niels Mayer) mayer@hplnpm
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

(let (
      form_w sliderpnl_w btnpnl_w
      balance_w volume_w bass_w treble_w
      pb1_w pb2_w pb3_w pb4_w pb5_w ok_w		 
      )


(if (not (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Code for Motif 1.1 (and later) version -- 1.0 version below has
;;; workarounds for bugs in Motif 1.0 XmForm widget.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    (progn    

      ;; Note use of a dummy toplevel widget (*TOPLEVEL_WIDGET*) below
      (setq form_w
	    (send XM_FORM_WIDGET_CLASS :new :dialog "form" *TOPLEVEL_WIDGET*
		  :XMN_DIALOG_TITLE		"Audio Controller Options"
		  :XMN_AUTO_UNMANAGE		nil ;BulletinBoard resource -- setting to NIL prevents automatic unamange of dialog caused by clicking on pushbutton child of form_w....
		  ))
      (setq sliderpnl_w
	    (send XM_FORM_WIDGET_CLASS :new :managed "sliders" form_w
		  :XMN_LEFT_ATTACHMENT		:attach_form
		  :XMN_BOTTOM_ATTACHMENT	:attach_form
		  :XMN_TOP_ATTACHMENT		:attach_form
		  ))
      (setq btnpnl_w
	    (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "buttons" form_w
		  :XMN_ORIENTATION		:vertical
		  :XMN_ENTRY_ALIGNMENT		:alignment_center
		  :XMN_IS_ALIGNED		t
		  :XMN_PACKING			:pack_tight
		  :XMN_TOP_ATTACHMENT		:attach_form
		  :XMN_BOTTOM_ATTACHMENT	:attach_form
		  :XMN_LEFT_ATTACHMENT		:attach_widget
		  :XMN_LEFT_WIDGET		sliderpnl_w
		  :XMN_RIGHT_ATTACHMENT		:attach_form
		  ))
      ;;
      ;; place a balance slider at bottom, horizontally stretching from
      ;; left side of form to the right side of the treble control
      ;; 
      (setq balance_w 
	    (send XM_SCALE_WIDGET_CLASS :new :managed "balance" sliderpnl_w
		  :XMN_TITLE_STRING		"Balance"
		  :XMN_SENSITIVE		t
		  :XMN_SHOW_VALUE		t
		  :XMN_ORIENTATION		:horizontal
		  :XMN_PROCESSING_DIRECTION	:max_on_right
		  :XMN_MAXIMUM			10
		  :XMN_MINIMUM			-10
		  :XMN_LEFT_ATTACHMENT		:attach_form
		  :XMN_BOTTOM_ATTACHMENT	:attach_form
		  :XMN_RIGHT_ATTACHMENT		:attach_form
		  ))
      ;;
      ;; Create volume, bass, and treble sliders stretching across form
      ;; horizontally...
      ;;
      (setq volume_w 
	    (send XM_SCALE_WIDGET_CLASS :new :managed "volume" sliderpnl_w
		  :XMN_TITLE_STRING		"Vol"
		  :XMN_SENSITIVE		t
		  :XMN_SHOW_VALUE		t
		  :XMN_ORIENTATION		:vertical
		  :XMN_PROCESSING_DIRECTION	:max_on_top
		  :XMN_MAXIMUM			100
		  :XMN_MINIMUM			0
		  :XMN_TOP_ATTACHMENT		:attach_form
		  :XMN_LEFT_ATTACHMENT		:attach_form
		  :XMN_BOTTOM_ATTACHMENT	:attach_widget
		  :XMN_BOTTOM_WIDGET		balance_w
		  ))
      (setq bass_w 
	    (send XM_SCALE_WIDGET_CLASS :new :managed "bass" sliderpnl_w
		  :XMN_TITLE_STRING		"Bass"
		  :XMN_SENSITIVE		t
		  :XMN_SHOW_VALUE		t
		  :XMN_ORIENTATION		:vertical
		  :XMN_PROCESSING_DIRECTION	:max_on_top
		  :XMN_MAXIMUM			10
		  :XMN_MINIMUM			-10
		  :XMN_TOP_ATTACHMENT		:attach_form
		  :XMN_LEFT_ATTACHMENT		:attach_widget
		  :XMN_LEFT_WIDGET		volume_w
		  :XMN_BOTTOM_ATTACHMENT	:attach_widget
		  :XMN_BOTTOM_WIDGET		balance_w
		  ))
      (setq treble_w 
	    (send XM_SCALE_WIDGET_CLASS :new :managed "treble" sliderpnl_w
		  :XMN_TITLE_STRING		"Treble"
		  :XMN_SENSITIVE		t
		  :XMN_SHOW_VALUE		t
		  :XMN_ORIENTATION		:vertical
		  :XMN_PROCESSING_DIRECTION	:max_on_top
		  :XMN_MAXIMUM			10
		  :XMN_MINIMUM			-10
		  :XMN_TOP_ATTACHMENT		:attach_form
		  :XMN_LEFT_ATTACHMENT		:attach_widget
		  :XMN_LEFT_WIDGET		bass_w
		  :XMN_BOTTOM_ATTACHMENT	:attach_widget
		  :XMN_BOTTOM_WIDGET		balance_w
		  ))

      ;;
      ;; place a column of buttons vertically on rhs of form in btnpnl
      ;;
      (setq pb1_w
	    (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb1" btnpnl_w
		  :XMN_LABEL_STRING	"Jack In"
		  ))
      (setq pb2_w
	    (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb2" btnpnl_w
		  :XMN_LABEL_STRING	"Jack Out"
		  ))
      (setq pb3_w
	    (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb3" btnpnl_w
		  :XMN_LABEL_STRING	"Do Watoosi"
		  ))
      (setq pb4_w
	    (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb4" btnpnl_w
		  :XMN_LABEL_STRING	"Enter Hyperspace"
		  ))
      (setq pb5_w
	    (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb5" btnpnl_w
		  :XMN_LABEL_STRING	"Winterp-Mute"
		  ))
      (setq ok_w
	    (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "ok" btnpnl_w
		  :XMN_LABEL_STRING	"Ok"
		  :xmn_show_as_default t
		  ))

      ;;
      ;; this causes the ok button to be pressed when <return> is entered within the form widget.
      ;;
      (send form_w :set_values :xmn_default_button ok_w)

      (send form_w :manage)		;pop it up

      )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; The Motif 1.0 version -- workarounds for bugs in Motif 1.0 XmForm
;;; widget.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  (progn
  
    ;; Note use of a dummy toplevel widget (*TOPLEVEL_WIDGET*) below
    (setq form_w
	  (send XM_FORM_WIDGET_CLASS :new :dialog "form" *TOPLEVEL_WIDGET*
		:XMN_DIALOG_TITLE "Audio Controller Options"
		:XMN_AUTO_UNMANAGE nil	;BulletinBoard resource -- setting to NIL prevents automatic unamange of dialog caused by clicking on pushbutton child of form_w....
		))

    ;;
    ;; Create volume, bass, and treble sliders stretching across form horizontally...
    ;;

    (setq volume_w 
	  (send XM_SCALE_WIDGET_CLASS :new :managed "volume" form_w
		:XMN_TITLE_STRING		"Vol"
		:XMN_SENSITIVE			t
		:XMN_SHOW_VALUE			t
		:XMN_ORIENTATION		:vertical
		:XMN_PROCESSING_DIRECTION	:max_on_top
		:XMN_MAXIMUM			100
		:XMN_MINIMUM			0
		:XMN_TOP_ATTACHMENT		:attach_form
		:XMN_LEFT_ATTACHMENT		:attach_form
		))
    (setq bass_w 
	  (send XM_SCALE_WIDGET_CLASS :new :managed "bass" form_w
		:XMN_TITLE_STRING		"Bass"
		:XMN_SENSITIVE			t
		:XMN_SHOW_VALUE			t
		:XMN_ORIENTATION		:vertical
		:XMN_PROCESSING_DIRECTION	:max_on_top
		:XMN_MAXIMUM			11
		:XMN_MINIMUM			-10
		:XMN_TOP_ATTACHMENT		:attach_form
		:XMN_LEFT_ATTACHMENT		:attach_widget
		:XMN_LEFT_WIDGET		volume_w
		))
    (setq treble_w 
	  (send XM_SCALE_WIDGET_CLASS :new :managed "treble" form_w
		:XMN_TITLE_STRING		"Treble"
		:XMN_SENSITIVE			t
		:XMN_SHOW_VALUE			t
		:XMN_ORIENTATION		:vertical
		:XMN_PROCESSING_DIRECTION	:max_on_top
		:XMN_MAXIMUM			11
		:XMN_MINIMUM			-10
		:XMN_TOP_ATTACHMENT		:attach_form
		:XMN_LEFT_ATTACHMENT		:attach_widget
		:XMN_LEFT_WIDGET		bass_w
		))

    ;;
    ;; place a column of buttons vertically on rhs of form
    ;;

    (setq pb1_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb1" form_w
		:XMN_LABEL_STRING	"Jack In"
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	treble_w
;;;		:XMN_RIGHT_ATTACHMENT	:attach_form
		))
    (setq pb2_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb2" form_w
		:XMN_LABEL_STRING	"Jack Out"
		:XMN_TOP_ATTACHMENT	:attach_widget
		:XMN_TOP_WIDGET		pb1_w
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	treble_w
;;;		:XMN_RIGHT_ATTACHMENT	:attach_form
		))
    (setq pb3_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb3" form_w
		:XMN_LABEL_STRING	"Do Watoosi"
		:XMN_TOP_ATTACHMENT	:attach_widget
		:XMN_TOP_WIDGET		pb2_w
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	treble_w
;;;		:XMN_RIGHT_ATTACHMENT	:attach_form
		))
    (setq pb4_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb4" form_w
		:XMN_LABEL_STRING	"Enter Hyperspace"
		:XMN_TOP_ATTACHMENT	:attach_widget
		:XMN_TOP_WIDGET		pb3_w
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	treble_w
;;;		:XMN_RIGHT_ATTACHMENT	:attach_form
		))
    (setq pb5_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb5" form_w
		:XMN_LABEL_STRING	"Winterp-Mute"
		:XMN_TOP_ATTACHMENT	:attach_widget
		:XMN_TOP_WIDGET		pb4_w
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	treble_w
;;;		:XMN_RIGHT_ATTACHMENT	:attach_form
		))
    (setq ok_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "ok" form_w
		:XMN_LABEL_STRING	"Ok"
		:XMN_SHOW_AS_DEFAULT	2
		:XMN_TOP_ATTACHMENT	:attach_widget
		:XMN_TOP_WIDGET		pb5_w
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	treble_w
;;;		:XMN_RIGHT_ATTACHMENT	:attach_form
		))

    ;;
    ;; place a balance slider horizontally stretching from left side of form to the
    ;; right side of the treble control
    ;; 

    (setq balance_w 
	  (send XM_SCALE_WIDGET_CLASS :new :managed "balance" form_w
		:XMN_TITLE_STRING		"Balance"
		:XMN_SENSITIVE			t
		:XMN_SHOW_VALUE			t
		:XMN_ORIENTATION		:horizontal
		:XMN_PROCESSING_DIRECTION	:max_on_right
		:XMN_MAXIMUM			10
		:XMN_MINIMUM			-11
		:XMN_TOP_ATTACHMENT		:attach_widget
		:XMN_TOP_WIDGET			treble_w
		:XMN_LEFT_ATTACHMENT		:attach_form
		:XMN_RIGHT_ATTACHMENT		:attach_opposite_widget
		:XMN_RIGHT_WIDGET		treble_w
		:XMN_BOTTOM_ATTACHMENT		:attach_opposite_widget
		:XMN_BOTTOM_WIDGET		ok_w
		))

    ;;
    ;; this causes the ok button to be pressed when <return> is entered within the form widget.
    ;;
    (send form_w :set_values :xmn_default_button ok_w)

    ;;
    ;; work around a resize bug for Motif 1.0 only: if these constraint resources are set before the
    ;; form-dialog widget is managed, then the dialog shell comes up too small
    ;; and doesn't display the buttons. If no right attachment is made, then all
    ;; the buttons come up in different sizes which looks ugly.
    ;;

    ;;
    ;; this pops up the form-dialog...
    ;;
    (send form_w :manage)		;pop it up

    (send pb1_w :set_values
	  :XMN_RIGHT_ATTACHMENT :attach_form
	  )
    (send pb2_w :set_values 
	  :XMN_RIGHT_ATTACHMENT :attach_form
	  )
    (send pb3_w :set_values
	  :XMN_RIGHT_ATTACHMENT :attach_form
	  )
    (send pb4_w :set_values
	  :XMN_RIGHT_ATTACHMENT :attach_form
	  )
    (send pb5_w :set_values
	  :XMN_RIGHT_ATTACHMENT :attach_form
	  )
    (send  ok_w :set_values
	   :XMN_RIGHT_ATTACHMENT :attach_form
	   )
    )
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Set up callbacks... (shared between 1.0 and 1.1 versions)
;;

(send pb1_w :set_callback :xmn_activate_callback '(CALLBACK_WIDGET)
      '(
	(format T "callback called on button: ~A\n"
		(xm_string_get_l_to_r
		 (car (send CALLBACK_WIDGET :get_values :xmn_label_string nil))))
	))

(send pb2_w :set_callback :xmn_activate_callback '(CALLBACK_WIDGET)
      '(
	(format T "callback called on button: ~A\n"
		(xm_string_get_l_to_r
		 (car (send CALLBACK_WIDGET :get_values :xmn_label_string nil))))
	))

(send pb3_w :set_callback :xmn_activate_callback '(CALLBACK_WIDGET)
      '(
	(format T "callback called on button: ~A\n"
		(xm_string_get_l_to_r
		 (car (send CALLBACK_WIDGET :get_values :xmn_label_string nil))))
	))

(send pb4_w :set_callback :xmn_activate_callback '(CALLBACK_WIDGET)
      '(
	(format T "callback called on button: ~A\n"
		(xm_string_get_l_to_r
		 (car (send CALLBACK_WIDGET :get_values :xmn_label_string nil))))
	))

(send pb5_w :set_callback :xmn_activate_callback '(CALLBACK_WIDGET)
      '(
	(format T "callback called on button: ~A\n"
		(xm_string_get_l_to_r
		 (car (send CALLBACK_WIDGET :get_values :xmn_label_string nil))))
	))

;;
;; unmanage the form dialog when the ok button is pressed --> pops down.
;; after unmanaging, read values of sliders....
;;
(send ok_w :set_callback :xmn_activate_callback '()
      '(
	(send form_w :unmanage)
	(format T "Volume=~A; Bass=~A; Treble=~A; Balance=~A\n"
		(send volume_w :get_value)
		(send bass_w :get_value)
		(send treble_w :get_value)
		(send balance_w :get_value)
		)
	))

(defun popup-audio-options-panel ()
  (send form_w :manage)
  )


)
