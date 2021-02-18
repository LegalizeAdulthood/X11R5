; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         RowColumn.lsp
; RCS:          $Header: RowColumn.lsp,v 1.2 91/10/05 03:57:55 mayer Exp $
; Description:  Some examples of XmCreateSimpleRadioBox(),
;		XmCreateSimpleCheckBox(), and XmCreateSimpleOptionMenu().
;		Note that XmCreateSimpleOptionMenu() invokes window manager
;		"close" bug. See ./../doc/BUGS for details.
; Author:       Niels Mayer, HPLabs
; Created:      Sun Feb 10 20:34:01 1991
; Modified:     Sat Oct  5 03:54:34 1991 (Niels Mayer) mayer@hplnpm
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

(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
    (error "Most features in RowColumn.lsp are present only in Motif 1.1 -- 1.0 doesn't have them yet."))

;;
;; Add a method on the widget metaclass WIDGET_CLASS. The method allow use of
;; simpler notation for doing XtGetValues() for a single resource.
;; (send <widget-class> :get :<resource-name>) ==> returns the resource value.
;;
(send WIDGET_CLASS :answer :get '(resource-name)
      '(
	(car (send self :GET_VALUES resource-name NIL))
	))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Test XmCreateSimpleRadioBox
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(let ()
  (setq top_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "srbshl"
	      :XMN_TITLE	"Simple Radio Box Test"
	      :XMN_ICON_NAME	"Test"
	      ))
  (setq scrl_w
	(send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
	      "sc" top_w
	      :XMN_SCROLLING_POLICY :AUTOMATIC
	      ))
  (setq button-labels
	(do* ((i  100 (1- i))
	      (bl '() (cons (format nil "Button ~A" i) bl))
	      )
	     ((<= i 0)			;test
	      bl			;return
	      )
	     ))
  (setq rc_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_radio_box
	      "rc" scrl_w
	      :XMN_BUTTON_COUNT (length button-labels)
	      :XMN_BUTTONS button-labels
	      ))
  (send rc_w :set_callback :XMN_ENTRY_CALLBACK
	'(CALLBACK_ENTRY_WIDGET
	  CALLBACK_ENTRY_SET)
	'(
	  (if CALLBACK_ENTRY_SET
	      (format T "Radio-Box Select: name==~A label==~A\n"
		      (send CALLBACK_ENTRY_WIDGET :name)
		      (xm_string_get_l_to_r
		       (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
	    )
	  ))

  (send top_w :realize)
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Test XmCreateSimpleCheckBox()
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(let ()
(setq top_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "scbshl"
	    :XMN_TITLE		"Simple Check Box"
	    :XMN_ICON_NAME	"SCB"
	    ))
(setq scrl_w
      (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
	    "sc" top_w
	    :XMN_SCROLLING_POLICY :AUTOMATIC
	    ))
(setq button-labels
      (do* ((i  100 (1- i))
	    (bl '() (cons (format nil "Button ~A" i) bl))
	    )
	   ((<= i 0)			;test
	    bl				;return
	    )
	   ))
(setq rc_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_check_box
	    "rc" scrl_w
	    :XMN_BUTTON_COUNT	(length button-labels)
	    :XMN_BUTTONS	button-labels
	    ))
(send rc_w :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET
	CALLBACK_ENTRY_SET)
      '(
	(format T "Check-Box ~A: name==~A label==~A\n"
		    (if CALLBACK_ENTRY_SET "Select" "Unselect")
		    (send CALLBACK_ENTRY_WIDGET :name)
		    (xm_string_get_l_to_r
		     (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING))
		    )
	))

(send top_w :realize)
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Test XmCreateSimpleOptionMenu()
;;
;; Unfortunately something about all popup menus in Motif 1.1.1 is screwy...
;; everything works fine, but if you close the window using mwm's f.kill,
;; it either won't delete the window, or it will coredump.
;;
;; I've seen odd behavior on Motif's standard popup menu example (in C),
;; so I think this is a motif bug. Fixed in WINTERP version 1.14...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(if (and (= *WINTERP_VERSION* 1) (>= *WINTERP_REVISION* 14))
(let ()
(setq top_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "somshl"
	    :XMN_TITLE		"Simple Option Menu"
	    :XMN_ICON_NAME	"SOM"
	    ))
(setq scrl_w
      (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
	    "sc" top_w
	    :XMN_SCROLLING_POLICY :AUTOMATIC
	    ))
(setq rc_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	    "rc" scrl_w
	    :XMN_ORIENTATION :VERTICAL
	    :XMN_PACKING :PACK_TIGHT
	    :XMN_ENTRY_ALIGNMENT :ALIGNMENT_CENTER
	    ))

(setq op1_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 1"
	    :XMN_OPTION_MNEMONIC	#\1
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		1
	    ))
(setq op2_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 2"
	    :XMN_OPTION_MNEMONIC	#\2
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		2
	    ))
(setq op3_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 3"
	    :XMN_OPTION_MNEMONIC	#\3
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		3
	    ))
(setq op4_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 4"
	    :XMN_OPTION_MNEMONIC	#\4
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		4
	    ))
(setq op5_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 5"
	    :XMN_OPTION_MNEMONIC	#\5
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		5
	    ))
(setq op6_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 6"
	    :XMN_OPTION_MNEMONIC	#\6
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		6
	    ))
(setq op7_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 7"
	    :XMN_OPTION_MNEMONIC	#\7
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		7
	    ))
(setq op8_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 8"
	    :XMN_OPTION_MNEMONIC	#\8
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		8
	    ))
(setq op9_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
	    "rc" rc_w
	    :XMN_OPTION_LABEL		"Option 9"
	    :XMN_OPTION_MNEMONIC	#\9
	    :XMN_BUTTON_COUNT		10
	    :XMN_BUTTONS		#("0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
	    :XMN_BUTTON_MNEMONICS	#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9)
	    :XMN_BUTTON_SET		9
	    ))
;;;
;;; WORK AROUND WINTERP's MISSING support for :XMN_SIMPLE_CALLBACK:
;;; We cannot attach a :XMN_ENTRY_CALLBACK on the widget instance returned from
;;; XM_ROW_COLUMN_WIDGET_CLASS/:simple_option_menu.
;;; The reason for this is that the option menu is composed of a label + cascade
;;; button, with the cascade attached to a pulldown. The :XMN_ENTRY_CALLBACK
;;; would have to occur on the pulldown menu, not on the option r/c. Thus,
;;; we need to attach the entry callback to the widgetobj returned by doing
;;; :get_values on resource XmNsubMenuId. However, due to a weird bug in
;;; Motif 1.1, you have to use method :GET_SUB_MENU_WIDGET instead of retrieving
;;; :XMN_SUB_MENU_ID.
;;;
(send (send op1_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 1 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op2_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 2 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op3_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 3 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op4_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 4 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op5_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 5 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op6_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 6 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op7_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 7 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op8_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 8 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))
(send (send op9_w :GET_SUB_MENU_WIDGET)	;must use :GET_SUB_MENU_WIDGET rather than :get_values/:xmn_sub_menu_id, since that one reveals 1.1 bug.
       :set_callback :XMN_ENTRY_CALLBACK
      '(CALLBACK_ENTRY_WIDGET)
       '(
  	(format T "Option-Menu 9 Select: name==~A label==~A\n"
		(send CALLBACK_ENTRY_WIDGET :name)
  		(xm_string_get_l_to_r
  		 (send CALLBACK_ENTRY_WIDGET :get :XMN_LABEL_STRING)))
  	))

(send top_w :realize)

)
)
