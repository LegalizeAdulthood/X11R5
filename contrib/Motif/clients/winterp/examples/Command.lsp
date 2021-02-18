; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         Command.lsp
; RCS:          $Header: Command.lsp,v 1.2 91/10/05 02:27:14 mayer Exp $
; Description:  Demo of XM_COMMAND_WIDGET_CLASS
; Author:       Niels Mayer, HPLabs
; Created:      Sun Feb 10 20:32:15 1991
; Modified:     Sat Oct  5 02:25:13 1991 (Niels Mayer) mayer@hplnpm
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
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "command-test"
	    :XMN_TITLE		"Command Widget Test"
	    :XMN_ICON_NAME	"CmdTest"
	    ))

(setq command_w
      (send XM_COMMAND_WIDGET_CLASS :new :managed
	    "command" top_w
	    :XMN_PROMPT_STRING		"Geo-Political choices"
	    :XMN_COMMAND		"quagmire"
	    :XMN_HISTORY_ITEMS		#("dogma"
					  "cold war"
					  "new world odor"
					  "orwell's 1984"
					  "certain doom"
					  "imperialism"
					  "stupidity"
					  "interventionism")
	    :XMN_HISTORY_ITEM_COUNT	6
	    :XMN_HISTORY_MAX_ITEMS	10
	    :XMN_HISTORY_VISIBLE_ITEM_COUNT 5
	    ))

(send command_w :set_callback :XmN_command_Changed_Callback 
      '(CALLBACK_WIDGET CALLBACK_REASON CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "Command Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\n"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)
	))

(send command_w :set_callback :XmN_command_Entered_Callback
      '(CALLBACK_WIDGET CALLBACK_REASON	CALLBACK_XEVENT CALLBACK_VALUE CALLBACK_LENGTH)
      '(
	(format T "Command Callback occured.\n\twidget=~A;\n\treason=~A;\n\tevent=~A;\n\tvalue=~A;\n\txmstr-length=~A\nHistory Items:"
		CALLBACK_WIDGET
		CALLBACK_REASON
		CALLBACK_XEVENT
		(xm_string_get_l_to_r CALLBACK_VALUE)
		CALLBACK_LENGTH)

	(let* ((items_array (send command_w :get_history_items))
	       (items_length (length items_array))
	       )
	  (do ((i 0 (1+ i)))
	      ((= i items_length))
	      (format t "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
	  )
	))

(send command_w :get_child :DIALOG_COMMAND_TEXT)

(send command_w :get_child :DIALOG_HISTORY_LIST)

(send command_w :get_child :DIALOG_PROMPT_LABEL)

(send command_w :set_value "freedom ")

(send command_w :append_value "love ")
(send command_w :append_value "and equality ")


(let* ((items_array (send command_w :get_history_items))
       (items_length (length items_array))
       )
  (do ((i 0 (1+ i)))
       ((= i items_length))
       (print (xm_string_get_l_to_r (aref items_array i))))
  )

(send command_w :error "<<ERROR: invalid choice>>")

(send top_w :realize)

)
