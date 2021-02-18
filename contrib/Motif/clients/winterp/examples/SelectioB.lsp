; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         SelectioB.lsp
; RCS:          $Header: SelectioB.lsp,v 1.2 91/10/05 04:42:23 mayer Exp $
; Description:  Demo of XM_SELECTION_BOX_WIDGET_CLASS
; Author:       Niels Mayer, HPLabs
; Created:      Sun Feb 10 20:34:42 1991
; Modified:     Sat Oct  5 04:41:49 1991 (Niels Mayer) mayer@hplnpm
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

(let ()

(setq top_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "sbshl"
	    :XMN_TITLE		"SelectionB Widget Test"
	    :XMN_ICON_NAME	"Test"
	    ))

(setq selection_w
      (send XM_SELECTION_BOX_WIDGET_CLASS :new :managed
	    "selection" top_w
	    :XMN_PROMPT_STRING		"Geo-Political choices"
	    :XMN_COMMAND		"quagmire"
	    :XMN_LIST_ITEMS		#("dogma"
					  "cold war"
					  "new world odor"
					  "orwell's 1984"
					  "certain doom"
					  "imperialism"
					  "stupidity"
					  "interventionism")
	    :XMN_LIST_ITEM_COUNT	6
	    :XMN_LIST_VISIBLE_ITEM_COUNT 5
	    ))

(send (send selection_w :get_child :dialog_apply_button) :manage)

(send selection_w :set_callback :XMN_APPLY_CALLBACK
      '(CALLBACK_WIDGET CALLBACK_REASON CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "Apply Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\n"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)
	))

(send selection_w :set_callback :XMN_CANCEL_CALLBACK
      '(CALLBACK_WIDGET CALLBACK_REASON CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "Cancel Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\n"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)
	))

(send selection_w :set_callback :XMN_OK_CALLBACK
      '(CALLBACK_WIDGET CALLBACK_REASON	CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "Ok Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\nHistory Items:"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)

	(let* ((items_array (send CALLBACK_WIDGET :get_list_items))
	       (items_length (length items_array))
	       )
	  (do ((i 0 (1+ i)))
	      ((= i items_length))
	      (format t "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
	  )
	))



(format T ":DIALOG_LIST==~A\n"
	(send selection_w :get_child :DIALOG_LIST))

(format T ":DIALOG_LIST_LABEL==~A\n"
	(send selection_w :get_child :DIALOG_LIST_LABEL))

(format T ":DIALOG_SELECTION_LABEL==~A\n"
	(send selection_w :get_child :DIALOG_SELECTION_LABEL))

(format T ":DIALOG_WORK_AREA==~A\n"
	(send selection_w :get_child :DIALOG_WORK_AREA))

(format T ":DIALOG_TEXT==~A\n"
	(send selection_w :get_child :DIALOG_TEXT))

(format T ":DIALOG_SEPARATOR==~A\n"
	(send selection_w :get_child :DIALOG_SEPARATOR))

(format T ":DIALOG_OK_BUTTON==~A\n"
	(send selection_w :get_child :DIALOG_OK_BUTTON))

(format T ":DIALOG_APPLY_BUTTON==~A\n"
	(send selection_w :get_child :DIALOG_APPLY_BUTTON))

(format T ":DIALOG_CANCEL_BUTTON==~A\n"
	(send selection_w :get_child :DIALOG_CANCEL_BUTTON))

(format T ":DIALOG_HELP_BUTTON==~A\n"
	(send selection_w :get_child :DIALOG_HELP_BUTTON))

(format T ":DIALOG_DEFAULT_BUTTON==~A\n"
	(send selection_w :get_child :DIALOG_DEFAULT_BUTTON))



(let* ((items_array (send selection_w :get_list_items))
       (items_length (length items_array))
       )
  (do ((i 0 (1+ i)))
      ((= i items_length))
      (print (xm_string_get_l_to_r (aref items_array i))))
  )

(send top_w :realize)

)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(let ()

(setq selection1_w
      (send XM_SELECTION_BOX_WIDGET_CLASS :new :managed :dialog
	    "selection" top_w
	    :XMN_SELECTION_LABEL_STRING	"Democracy implies freedom to choose from the above::"
	    :XMN_MUST_MATCH		t
	    :XMN_LIST_ITEMS		#("dogma"
					  "cold war"
					  "new world odor"
					  "orwell's 1984"
					  "certain doom"
					  "imperialism"
					  "stupidity"
					  "interventionism")
	    :XMN_LIST_ITEM_COUNT	6
	    :XMN_LIST_VISIBLE_ITEM_COUNT 5
	    ))

(send selection1_w :set_callback :XMN_NO_MATCH_CALLBACK
      '(CALLBACK_WIDGET CALLBACK_REASON	CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "No Match Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\nHistory Items:"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)

	(let* ((items_array (send CALLBACK_WIDGET :get_list_items))
	       (items_length (length items_array))
	       )
	  (do ((i 0 (1+ i)))
	      ((= i items_length))
	      (format t "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
	  )
	(send CALLBACK_WIDGET :manage)
	))

(send selection1_w :manage)
(send selection1_w :set_callback :XMN_OK_CALLBACK
      '(CALLBACK_WIDGET CALLBACK_REASON	CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "Ok Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\nHistory Items:"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)

	(let* ((items_array (send CALLBACK_WIDGET :get_list_items))
	       (items_length (length items_array))
	       )
	  (do ((i 0 (1+ i)))
	      ((= i items_length))
	      (format t "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
	  )
	))

)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(let ()

(setq selection2_w
      (send XM_SELECTION_BOX_WIDGET_CLASS :new :managed :prompt_dialog
	    "selection" top_w
	    :XMN_SELECTION_LABEL_STRING "Freedom of speech -- Just watch what you say:"
	    ))

(send selection2_w :set_callback :XMN_OK_CALLBACK
      '(CALLBACK_WIDGET CALLBACK_REASON	CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "Ok Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\nHistory Items:"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)
	))

(format T ":DIALOG_LIST==~A\n"
	(send selection2_w :get_child :DIALOG_LIST))

(format T ":DIALOG_LIST_LABEL==~A\n"
	(send selection2_w :get_child :DIALOG_LIST_LABEL))

(format T ":DIALOG_SELECTION_LABEL==~A\n"
	(send selection2_w :get_child :DIALOG_SELECTION_LABEL))

(format T ":DIALOG_WORK_AREA==~A\n"
	(send selection2_w :get_child :DIALOG_WORK_AREA))

(format T ":DIALOG_TEXT==~A\n"
	(send selection2_w :get_child :DIALOG_TEXT))

(format T ":DIALOG_SEPARATOR==~A\n"
	(send selection2_w :get_child :DIALOG_SEPARATOR))

(format T ":DIALOG_OK_BUTTON==~A\n"
	(send selection2_w :get_child :DIALOG_OK_BUTTON))

(format T ":DIALOG_APPLY_BUTTON==~A\n"
	(send selection2_w :get_child :DIALOG_APPLY_BUTTON))

(format T ":DIALOG_CANCEL_BUTTON==~A\n"
	(send selection2_w :get_child :DIALOG_CANCEL_BUTTON))

(format T ":DIALOG_HELP_BUTTON==~A\n"
	(send selection2_w :get_child :DIALOG_HELP_BUTTON))

(format T ":DIALOG_DEFAULT_BUTTON==~A\n"
	(send selection2_w :get_child :DIALOG_DEFAULT_BUTTON))

)
