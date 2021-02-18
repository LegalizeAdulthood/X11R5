; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         fake-app.lsp
; RCS:          $Header: fake-app.lsp,v 1.2 91/10/05 15:55:10 mayer Exp $
; Description:  Example application using XM_MAIN_WINDOW_WIDGET_CLASS +
;               XM_ROW_COLUMN_WIDGET_CLASS/:simple_menu_bar +
;               XM_ROW_COLUMN_WIDGET_CLASS/:simple_pulldown_menu
;		to create a window with a menubar and pulldowns, etc.
; Author:       Niels Mayer, HPLabs
; Created:      Fri Feb  8 19:59:47 1991
; Modified:     Sat Oct  5 15:52:34 1991 (Niels Mayer) mayer@hplnpm
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
    (error "Most features in fake-app.lsp are present only in Motif 1.1\
            -- 1.0 doesn't have the required functions (yet)."))

(let (toplevel_w main_w menubar_w commandwindow_w edit_w)

  (setq toplevel_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
	      :XMN_TITLE "Winterp: Fake Application"
	      :XMN_ICON_NAME "W:Fake-App"
	      ))

  (setq main_w
	(send XM_MAIN_WINDOW_WIDGET_CLASS :new :managed
	      "mainw" toplevel_w
	      ))

  (setq menubar_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_menu_bar
	      "menubar" main_w
	      :XMN_BUTTON_COUNT		5
	      :XMN_BUTTONS		#("Files" "Edit" "Fold" "Spindle" "Mutilate")
	      :XMN_BUTTON_MNEMONICS	#(#\F     #\E    #\o    #\S       #\M)
	      ))

  (send
   (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	 "pulldown" menubar_w
	 :XMN_POST_FROM_BUTTON	0 ;post pulldown from menubar's "Files" button.
	 :XMN_BUTTON_COUNT	5 ;create five buttons in this pulldown
	 :XMN_BUTTONS		#("Quit" "Open" "Open in New Window" "Save" "Save As")
	 :XMN_BUTTON_MNEMONICS	#(#\Q    #\O    #\N                  #\S    #\A)
	 :XMN_BUTTON_MNEMONIC_CHAR_SETS	#("" "" "ISO8859-1" "ISO8859-1" "ISO8859-1")
	 :XMN_BUTTON_ACCELERATORS #("Ctrl<Key>C" "Ctrl<Key>F" "Ctrl<Key>O" "Ctrl<Key>S" "Ctrl<Key>W")
	 :XMN_BUTTON_ACCELERATOR_TEXT #("^C" "^F" "^O" "^S" "^W")
	 )
   :add_callback :xmn_entry_callback	;use this instead of XmNsimpleCallback
   '(CALLBACK_ENTRY_WIDGET)
   '(
     ;; (send CALLBACK_ENTRY_WIDGET :name) returns "button_<#>"
     ;; where <#> is 0 ... (button-count-1).
     ;; we use 'read' to return the FIXNUM <#> after truncating the
     ;; 7 chars "button_" from the front of the string.
     (case (read (make-string-input-stream (send CALLBACK_ENTRY_WIDGET :name) 7))
	   (0 (format T "Quit Function Called\n"))
	   (1 (format T "Open Function Called\n"))
	   (2 (format T "Open in New Window Function Called\n"))
	   (3 (format T "Save Function Called\n"))
	   (4 (format T "Save As Function Called\n"))
	   (T (format T "Error\n")))
     ))

  (setq edit_pd_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	      "pulldown" menubar_w
	      :XMN_POST_FROM_BUTTON	1 ;post from menubar's "Edit"
	      :XMN_BUTTON_COUNT		8
	      :XMN_BUTTONS		#("One" "Two" "Three" "Four" "Five" "Six" "Seven" "Eight")
	      :XMN_BUTTON_MNEMONICS	#(#\O   #\T   #\h     #\F    #\i    #\S   #\e     #\g)
	      :XMN_BUTTON_TYPE		#(:PUSHBUTTON :TOGGLEBUTTON :CHECKBUTTON :RADIOBUTTON :CASCADEBUTTON :SEPARATOR :DOUBLE_SEPARATOR :TITLE)
	      ))
  (send edit_pd_w :add_callback :xmn_entry_callback '(CALLBACK_ENTRY_WIDGET)
	'(
	  (process-pulldown-buttonclick CALLBACK_ENTRY_WIDGET)
	  ))

  (send
   (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	 "pulldown" edit_pd_w
	 :XMN_POST_FROM_BUTTON	4	
	 :XMN_BUTTON_COUNT	8
	 :XMN_BUTTONS		#("One" "Two" "Three" "Four" "Five" "Six" "Seven" "Eight")
	 :XMN_BUTTON_MNEMONICS	#(#\O   #\T   #\h     #\F    #\i    #\S   #\e     #\g)
	 :XMN_BUTTON_TYPE	#(:TITLE :DOUBLE_SEPARATOR :SEPARATOR :CASCADEBUTTON :RADIOBUTTON :CHECKBUTTON :TOGGLEBUTTON :PUSHBUTTON)
	 )
   :add_callback :xmn_entry_callback '(CALLBACK_ENTRY_WIDGET)
   '(
     (process-pulldown-buttonclick CALLBACK_ENTRY_WIDGET)
     ))


  (send
   (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	 "pulldown" menubar_w
	 :XMN_POST_FROM_BUTTON	2	;post from menubar's "Fold"
	 :XMN_BUTTON_COUNT	5
	 :XMN_BUTTONS		#("One" "Two" "Three" "Four" "Five")
	 :XMN_BUTTON_MNEMONICS	#(#\O   #\T   #\h     #\F    #\i   )
	 )
   :add_callback :xmn_entry_callback '(CALLBACK_ENTRY_WIDGET)
   '(
     (process-pulldown-buttonclick CALLBACK_ENTRY_WIDGET)
     ))

  (send
   (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	 "pulldown" menubar_w
	 :XMN_POST_FROM_BUTTON	3	;post from menubar's "Spindle"
	 :XMN_BUTTON_COUNT	5
	 :XMN_BUTTONS		#("One" "Two" "Three" "Four" "Five")
	 :XMN_BUTTON_MNEMONICS	#(#\O   #\T   #\h     #\F    #\i   )
	 )
   :add_callback :xmn_entry_callback '(CALLBACK_ENTRY_WIDGET)
   '(
     (process-pulldown-buttonclick CALLBACK_ENTRY_WIDGET)
     ))

  (send
   (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	 "pulldown" menubar_w
	 :XMN_POST_FROM_BUTTON	4	;post from menubar's "Mutilate"
	 :XMN_BUTTON_COUNT	5
	 :XMN_BUTTONS		#("One" "Two" "Three" "Four" "Five")
	 :XMN_BUTTON_MNEMONICS	#(#\O   #\T   #\h     #\F    #\i   )
	 )
   :add_callback :xmn_entry_callback '(CALLBACK_ENTRY_WIDGET)
   '(
     (process-pulldown-buttonclick CALLBACK_ENTRY_WIDGET)
     ))

  ;; NOTE: Motif 1.1 bug appears if we use :scrolled opt.
  ;;	  (strange interaction between scrolled list widget
  ;;       and main_w manager)
  ;;  (setq commandwindow_w
  ;;	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled
  ;;	      "commandwindow" main_w
  ;;	      :XMN_SELECTION_POLICY :extended_select
  ;;	      :XMN_LIST_SIZE_POLICY :constant
  ;;	      :XMN_SCROLL_BAR_DISPLAY_POLICY :static
  ;;	      :XMN_TRAVERSAL_ON T
  ;;	      :XMN_ITEMS list
  ;;	      :XMN_ITEM_COUNT length
  ;;	      :XMN_VISIBLE_ITEM_COUNT 10
  ;;	      ))

  (setq commandwindow_w
	(send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new :managed
	      "commandwindow" main_w
	      ))
  (send (send commandwindow_w :get_child :DIALOG_HELP_BUTTON) :unmanage)
  (send (send commandwindow_w :get_child :DIALOG_CANCEL_BUTTON) :unmanage)
  (send commandwindow_w :set_callback :XMN_OK_CALLBACK
	'(callback_value)		;returns selected file as XmString 
	'(
	  (send edit_w :find_file
		(xm_string_get_l_to_r callback_value) 0)
	  ))

  (setq edit_w
	(send XM_TEXT_WIDGET_CLASS :new :managed :scrolled
	      "edit" main_w
	      :XMN_EDIT_MODE :MULTI_LINE_EDIT
	      ))

  (send main_w :set_areas
	menubar_w			;set XmNmenuBar
	commandwindow_w			;set XmNcommandWindow
	NIL				;don't want a XmNhorizontalScrollBar
	NIL				;dont' want a XmNverticalScrollBar
	(send edit_w :parent)		;XmNworkWindow -- note, :parent retrieves scrolled-text's scrolled-window parent
	)

  (send toplevel_w :realize)
  )

(defun process-pulldown-buttonclick (w)
  (format T "Process-pulldown-buttonclick(~A)\n" w)
  (format T "\tName==~A Label==~A\n"
	  (send w :name)
	  (xm_string_get_l_to_r (car (send w :get_values :xmn_label_string nil))))
  )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Add a :FIND_FILE method to the Motif Text widget.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(send XM_TEXT_WIDGET_CLASS :answer :FIND_FILE '(filename linenum)
      '(
	(let*
	    (;; loc vars
	     (fp
	      (open filename :direction :input)
	      )
	     inspos
	     text_line
	     )

	  (if (null fp)
	      (error "Can't open file." filename))

	  (send self :set_string "")	;clear out old text
	  (send self :disable_redisplay NIL) ;don't show changes till done
	  (loop
	   (if (null (setq text_line (read-line fp)))
	       (return))
	   (setq inspos (send self :get_insertion_position))
	   (send self :replace inspos inspos (strcat text_line "\n"))
	   )

	  (send self :scroll linenum)	;make <linenum> be the top of screen

	  (send self :enable_redisplay)	;now show changes...

	  (close fp)
	  )
	)
      )
