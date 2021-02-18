; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         grep-br.lsp
; RCS:          $Header: grep-br.lsp,v 1.7 91/10/05 17:12:19 mayer Exp $
; Description:  A file search browser using the Unix "grep" command to perform
;		search. Has capability to call $EDITOR or XmText editor to view
;		context	surrounding matching item. See mouse and accelerator bindings
;		comments below for details on using this. Note customization via
;               setting variables *SYSTEM-EDITOR*, *GREP-BR-EXECUTABLE* (see below).
;		For speed, I suggest (setq *GREP-BR-EXECUTABLE* "/usr/local/bin/gnugrep")...
; Author:       Niels Mayer, HPLabs
; Created:      Mon Nov 20 18:13:23 1989
; Modified:     Sat Oct  5 17:11:04 1991 (Niels Mayer) mayer@hplnpm
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

;; Resources:
;;
;; Mwm*winterpGrepBr*iconImage:    /usr/local/include/X11/bitmaps/search-i.h
;; Mwm*WinterpGrepBr*iconImage:    /usr/local/include/X11/bitmaps/search-i.h
;;

;; Mouse bindings on browser:
;; single left click   -- select item for use by $EDITOR button or middle/right click.
;; double left click   -- select item and display corresponding text in text viewer.
;; single middle click -- select item and display corresponding text in text viewer.
;; single right click  -- select item and display corresponding text in $EDITOR

;; Accelerators on browser:
;; (Note: bindings available everywhere other than "search regexp" and 
;; "wildcarded files" editors).
;;
;; 'E'  -- view selected item in user's editor.
;; '^E' -- select next item and view in user's editor.
;; '^N', ^<DownArrow>  -- select next item
;; '^P', ^<UpArrow>    -- select prev item
;; 'N' , <DownArrow>   -- view next item in built-in editor
;; 'P' , <UpArrow>     -- view prev item in built-in editor

;; Accelerators on editor:
;; (Note: bindings available everywhere other than "search regexp" and 
;; "wildcarded files" editors).
;;
;; <space>     -- pages forwards
;; <backspace> -- pages backwards


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; the contents of this file have evolved into something that can only work with
;; Motif 1.1/X11r4. A more rudimentary grep browser is available for Motif 1.0,
;; in file grep-br1.0.lsp, and we load that if we note you're running Motif 1.0...
;; Note that the 1.0 version isn't nearly as good as the grep browser in this file. 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
  (if (not (load "grep-br1.0.lsp"))	;load failure doesn't signal an error.
      (error "Couldn't load grep-br1.0.lsp -- did you forget to set\nresource .lispLibDir or command-line argument -lib_dir\nto specify the path to the WINTERP examples directory??")
    )
(progn

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; *SYSTEM-EDITOR*:
;;; if NIL, then edit functionality will use editor set in environment variable 
;;; $EDITOR. If set to a string, then that string will be used as the name of
;;; the editor to use for the "Edit" button.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defvar *SYSTEM-EDITOR* nil)

#| ;; NPM: commented out because simple-option-menu has motif bug
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; *GREP-EXECUTABLES*
;;; user may want to customize, by adding the following to their "~/.winterp"
;;; (setq *GREP-BR-EXECUTABLE* #("grep1" "grep2" "grep3" "grep4"))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defvar *GREP-EXECUTABLES* #("grep" "fgrep" "egrep" "gnugrep"))
|# ;; NPM: commented out because simple-option-menu has motif bug

; use this in place of above till option menu fixed.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; *GREP-BR-EXECUTABLE*
;;; user may want to customize, by adding the following to their "~/.winterp"
;;; (setq *GREP-BR-EXECUTABLE* "/usr/local/bin/gnugrep")
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defvar *GREP-BR-EXECUTABLE* "grep")

;;
;; Make it more straightforward to access singleton resources.
;;
(send WIDGET_CLASS :answer :get '(resource-name)
 '(
   (car (send self :GET_VALUES resource-name NIL))
   ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;; List_Browser_Widget_Class ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Make a subclass of XM_LIST_WIDGET_CLASS which holds an additional
;; instance variable 'items'. 'items' is an array of arbitrary objects
;; (BROWSER_OBJECT) to be displayed in a browser made from the list widget.
;;
;; BROWSER-OBJECT can be any arbitrary xlisp object that responds to
;; the message :display_string.
;;
;; Message :display_string must return a string which is used as the
;; textual representation of the object in the browser display.
;;
(setq List_Browser_Widget_Class
      (send Class :new
	    '(items selected_pos)	;new instance vars
	    '()				;no class vars
	    XM_LIST_WIDGET_CLASS))	;superclass
;;
;; Initialization method.
;;
(send List_Browser_Widget_Class :answer :isnew '(&rest args)
      '(
	;; initialize the instance variables
	(setq items nil)
	(setq selected_pos nil)

	;; initialize the widget...
	(apply 'send-super `(:isnew
			     ,@args
			     :XMN_SELECTION_POLICY :browse_select
			     ))

	;; add a callback to set selected_pos instance var.
	(send-super :add_callback :XMN_BROWSE_SELECTION_CALLBACK '(CALLBACK_ITEM_POSITION)
		    '(
		      (setq selected_pos CALLBACK_ITEM_POSITION)
		      )
		    )
	))
;;
;; We add a method to set the items browsed by the list browser
;; and set the 'items' instance variable.
;;
;; (send <List_Browser_Widget_Class_inst> :set_browser_items <items_list>)
;; <items_list> is a list of BROWSER_OBJECTs as described above.
;;
(send List_Browser_Widget_Class :answer :SET_BROWSER_ITEMS '(items_list)
      '(
	(let* (
	       (items_end_idx (length items_list))
	       (display_items (make-array items_end_idx)))

	  (setq selected_pos nil)	;reset ivar to init value.

	  (if (= 0 items_end_idx)	;if items_list empty
	      (setq items nil)		;reset 'items' ivar to init value
	    (progn			;else setup 'items' and 'display_items'
	      ;; initialize the 'items' instance variable so that it
	      ;; holds all the BROWSER_OBJECTs passed in <items_list>
	      (setq items (make-array items_end_idx)) ;create the array
	      (do (			;copy elts from list to array
		   (i    0          (1+ i))
		   (elts items_list (cdr elts)))
		  ;; loop till no more elts
		  ((null elts))
		  ;; loop body
		  (setf (aref items i) (car elts))
		  (setf (aref display_items i) (send (car elts) :display_string))
		  )
	      )
	    )

	  ;; tell the widget about the new browser items to display
	  (send-super :set_values
		      :XMN_ITEMS display_items
		      :XMN_ITEM_COUNT items_end_idx
		      )
	  )
	))
;;
;; Retrieves object at selected position, else NIL. 'selected_pos' is set by
;; :XMN_BROWSE_SELECTION_CALLBACK in :ISNEW method.
;;
(send List_Browser_Widget_Class :answer :GET_SELECTED_ITEM '()
      '(
	(if selected_pos
	    (aref items (1- selected_pos))
	  NIL)
	))
;;
;; Brings up editor on object at selected position
;;
(send List_Browser_Widget_Class :answer :EDIT_SELECTED_ITEM '()
      '(
	(let ((browsed-object (send self :get_selected_item)))
	  (if browsed-object		;set to NIL if no browsed object
	      (system (format nil 
			      "~A +~A ~A &"
			      (if *SYSTEM-EDITOR* *SYSTEM-EDITOR* "$EDITOR")
			      (send browsed-object :line-num)
			      (send browsed-object :file-name)
			      ))
	    ))
	))
;;
;; Select next item in list.
;;
(send List_Browser_Widget_Class :answer :GOTO_NEXT '()
      '(
	(if items			;trap empty list...
	    (progn
	      ;; ensure that the position remains valid
	      (if (not (numberp selected_pos)) ;if no current selection,
		  (setq selected_pos 1)	;then start at beginning of list
		(if (>= selected_pos (length items))
		    (format T "\007\n")	;replace this with XBeep...
		  (setq selected_pos (1+ selected_pos))
		  )
		)

	      ;; select the next position
	      (send-super :select_pos selected_pos t)

	      ;; make sure the item is visible
	      (let (num-br-visible num-br-items)
		(send self :get_values
		      :XMN_ITEM_COUNT 'num-br-items
		      :XMN_VISIBLE_ITEM_COUNT 'num-br-visible)
		(send self :set_bottom_pos
		      (min num-br-items (+ selected_pos (/ num-br-visible 2))))
		)
	      ))
	))
;;
;; Select previous item in list
;;
(send List_Browser_Widget_Class :answer :GOTO_PREV '()
      '(

	(if items			;trap empty list...
	    (progn
	      ;; ensure that the position remains valid
	      (if (not (numberp selected_pos))
		  (setq selected_pos 1)	;if no current selection, start at beginning.
		(if (= selected_pos 1)
		    (format T "\007\n")	;replace this with XBeep...
		  (setq selected_pos (1- selected_pos))
		  )
		)

	      ;; select the prev position
	      (send-super :select_pos selected_pos t)

	      ;; make sure the item is visible
	      (let* (visible-items)
		(send self :get_values :XMN_VISIBLE_ITEM_COUNT 'visible-items)
		(send self :set_pos (max 1 (- selected_pos (/ visible-items 2))))
		)
	      ))
	))
;;
;; Select next item in list and browse it.
;;
(send List_Browser_Widget_Class :answer :BROWSE_NEXT '(XEVENT)
      '(
	(send self :goto_next)
	(send-super :call_action_proc "ListKbdActivate" XEVENT)	;call :XMN_DEFAULT_ACTION_CALLBACK defined below.
	))
;;
;; Select previous item in list and browse it.
;;
(send List_Browser_Widget_Class :answer :BROWSE_PREV '(XEVENT)
      '(
	(send self :goto_prev)
	(send-super :call_action_proc "ListKbdActivate" XEVENT)	;call :XMN_DEFAULT_ACTION_CALLBACK defined below.
	))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Grep_Item_Class ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Define a BROWSER_OBJECT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Each BROWSER_OBJECT holds the information summarizing one mail message.
;; the information is split up into individual fields because we may want
;; to be able to sort on one field, or search for mathes on one field.
;;
(setq Grep_Item_Class
      (send Class :new
	    '(file-name line-num match-line)
	    ))

;; this method will read a single line of grep output.
;; and sets the instance variables in the 
;; BROWSER_OBJECT to the individual fields of the grep output
(send Grep_Item_Class :answer :read-grep-info '(pipe)
      '(
	(if (and
	     (setq file-name (fscanf-string pipe "%[^:]:"))
	     (setq line-num  (fscanf-fixnum pipe "%d:"))
	     (setq match-line (fscanf-string pipe "%[^\n]\n"))
	     )
	    self			;return self if succesful
	  NIL				;return NIL if hit EOF
	  )
	)
      )

(send Grep_Item_Class :answer :display_string '()
      '(
	(format nil "~A: ~A"
		file-name match-line)
	))

(send Grep_Item_Class :answer :file-name '()
      '(
	file-name
	))

(send Grep_Item_Class :answer :line-num '()
      '(
	line-num
	))

;;
;; This returns a list of Grep_Item_Class instances corresponding
;; to the items matching the search pattern and file list given
;; in argument <grep-arg-string>
;;
(defun grep (executable-string flags-string grep-args-string)
  (do* 
   (;; loop variables, initializers, and increments.
    (fp (popen (strcat
		executable-string " "
		flags-string " "
		"-n "			;force grep to output line numbers
		grep-args-string
		" /dev/null"		;incase there's only one arg, forces filename to be output
		)
	       :direction :input))
    (line (send (send Grep_Item_Class :new) :read-grep-info fp)
	  (send (send Grep_Item_Class :new) :read-grep-info fp))
    (result '())			;init to an empty list
    )
   ;; loop test and return
   ((null line)				;:read-grep-info returns NIL on EOF
    (pclose fp)				;close the pipe opened above
    (reverse result)			;return list of grep objects.
    )
   ;; loop body
   (setq result (cons line result))	;prepend grep-obj to list
   )
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;; Text_Viewer_Widget_Class ;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; Make a subclass of XM_TEXT_WIDGET_CLASS which holds an additional
;; instance variable 'file-path'. 'file-path' is a string representing
;; the full name of the file in the text editor widget.
;;
;; Method :FIND_FILE uses this filename to decide whether it must
;; read the file into the text widget, or whether it's already 
;; there. We don't want it to reread uncecessarily since for large
;; files, this can be slow...
;;
(setq Text_Viewer_Widget_Class
      (send Class :new
	    '(file-path)		;new instance vars
	    '()				;no class vars
	    XM_TEXT_WIDGET_CLASS))	;superclass

;;
;; Override superclass's instance initializer so we can set
;; instance variable, and supply some default arguments.
;;
(send Text_Viewer_Widget_Class :answer :isnew
      '(managed_k widget_name widget_parent &rest args)
      '(
	(setq file-path "")		;initialize instance var
	(apply 'send-super		;call superclass's init to create widget
	       `(:isnew ,managed_k :scrolled ,widget_name ,widget_parent
			,@args
			:XMN_EDIT_MODE :MULTI_LINE_EDIT
			:XMN_EDITABLE  nil ;don't allow user to change text.
			))
	))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Add a :FIND_FILE method to the Motif Text widget.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(send Text_Viewer_Widget_Class :answer :FIND_FILE '(filename linenum)
      '(
	(cond
	 ((string= filename file-path)	;if the file was already read into widget
	  (send-super :set_insertion_position 0) ;then just make <linenum> be center
	  (send-super :scroll (-	;scroll to current line num
			       linenum
			       (/ (send-super :get :XMN_ROWS) 2)
			       1))
	  )
	 (t				;else read the file into the widget...
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

	    (send-super :set_string "")	;clear out old text
	    (send-super :disable_redisplay NIL)	;don't show changes till done
	    (loop
	     (if (null (setq text_line (read-line fp)))
		 (return))
	     (setq inspos (send-super :get_insertion_position))
	     (send-super :replace inspos inspos (strcat text_line "\n"))
	     )

	    (send-super :scroll (-	;scroll to current line num
				 linenum
				 (/ (send-super :get :XMN_ROWS) 2)
				 1))

	    (send-super :enable_redisplay) ;now show changes...

	    (close fp)
	    (setq file-path filename)
	    )
	  )
	 )
	))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; GREP-BROWSER ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; The Main program -- note that this doesn't use any global variables, so
;; you can have many grep browsers up all at once without having them
;; interact.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun grep-browser()
  (let
      (;; declare local variables
       (motif-v-1-0-p (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0)))
       top_w paned_w controlpanel_w controlpanel3_w clone_button_w
#| ;; NPM: commented out because simple-option-menu has motif bug
       grepprog_opt_w
|# ;; NPM: commented out because simple-option-menu has motif bug
       doit_button_w case_toggle_w
       search_label_w  files_label_w 
       controlpanel2_w prev_button_w next_button_w
       srchsel_button_w edit_file_button_w
       filename_label_label_w filename_label_w
       list_w viewtext_w search_editor_w files_editor_w
       )

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;; Create A Widget Hierarchy ;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    (setq top_w
	  (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "winterpGrepBr" "WinterpGrepBr"
		:XMN_TITLE "Grep Browser"
		:XMN_ICON_NAME "Grep Browser"
		))
    (setq paned_w
	  (send XM_PANED_WINDOW_WIDGET_CLASS :new :managed
		"paned_w" top_w 
		))
    (setq controlpanel_w
	  (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
		"controlpanel_w" paned_w 
		:XMN_ORIENTATION	:horizontal
		:XMN_PACKING		:pack_tight
		))
    (setq clone_button_w
	  (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed
		"clone_button_w" controlpanel_w 
		:XMN_LABEL_STRING	"New Search Window"
		))
    (setq doit_button_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed ;NOTE -- this must be a WIDGET (not a GADGET) because ArmAndActivate() action is called below.
		"doit_button_w" controlpanel_w 
		:XMN_LABEL_STRING	"Do Search"
		:XMN_FILL_ON_ARM	t
		))
    (setq srchsel_button_w
	  (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed
		"srchsel_button_w" controlpanel_w 
		:XMN_LABEL_STRING	"Search for Selected Text"
		))
#| ;; NPM: commented out because simple-option-menu has motif bug
    (if (not motif-v-1-0-p)
	(setq grepprog_opt_w
	      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_option_menu
		    "grep-opt" controlpanel_w
		    ;; :XMN_OPTION_LABEL "Search\nProgram:"
		    :XMN_OPTION_MNEMONIC	#\P
		    :XMN_BUTTON_COUNT		(length *GREP-EXECUTABLES*)
		    :XMN_BUTTONS		*GREP-EXECUTABLES*
		    :XMN_BUTTON_MNEMONICS	#(#\g #\f #\e #\n)
		    :XMN_BUTTON_SET		0
		    )))
|# ;; NPM: commented out because simple-option-menu has motif bug

    (setq case_toggle_w
	  (send XM_TOGGLE_BUTTON_GADGET_CLASS :new :managed
		"case_toggle_w" controlpanel_w 
		:XMN_LABEL_STRING	"Case Sensitive Search"
		:XMN_SET		T
		))
    (setq controlpanel3_w
	  (send XM_FORM_WIDGET_CLASS :new :managed
		"controlpanel3_w" paned_w 
		))
    (setq search_label_w
	  (send XM_LABEL_GADGET_CLASS :new :managed
		"search_label_w" controlpanel3_w
		:XMN_LABEL_STRING	"Search Regexp:"
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_form
		))
    (setq search_editor_w
	  (send XM_TEXT_WIDGET_CLASS :new :managed ;note that XM_TEXT_FIELD_WIDGET_CLASS is glitchy with :XMN_RESIZE_WIDTH & :XMN_ALLOW_RESIZE set
		"search_editor_w" controlpanel3_w
		:XMN_EDIT_MODE	:SINGLE_LINE_EDIT
		:XMN_RESIZE_WIDTH	t
		:XMN_ALLOW_RESIZE	t
		:XMN_CURSOR_POSITION_VISIBLE t
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	search_label_w
		))
    (setq files_label_w
	  (send XM_LABEL_GADGET_CLASS :new :managed
		"files_label_w" controlpanel3_w
		:XMN_LABEL_STRING	"Wildcarded Files:"
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	search_editor_w
		))
    (setq files_editor_w
	  (send XM_TEXT_WIDGET_CLASS :new :managed ;note that XM_TEXT_FIELD_WIDGET_CLASS is glitchy with :XMN_RESIZE_WIDTH & :XMN_ALLOW_RESIZE set
		"files_editor_w" controlpanel3_w
		:XMN_EDIT_MODE	:SINGLE_LINE_EDIT
		:XMN_RESIZE_WIDTH	t
		:XMN_ALLOW_RESIZE	t
		:XMN_CURSOR_POSITION_VISIBLE t
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	files_label_w
		:XMN_RIGHT_ATTACHMENT	:attach_form
		))
    (setq list_w
	  (send List_Browser_Widget_Class :new :managed :scrolled
		"list_w" paned_w
		:XMN_VISIBLE_ITEM_COUNT	8
		:XMN_LIST_SIZE_POLICY	:RESIZE_IF_POSSIBLE
		))
    (setq controlpanel2_w
	  (send XM_FORM_WIDGET_CLASS :new :managed
		"controlpanel2_w" paned_w 
		))
    (setq prev_button_w
	  (send XM_ARROW_BUTTON_GADGET_CLASS :new :managed
		"prev_button_w" controlpanel2_w 
		:XMN_ARROW_DIRECTION	:arrow_up
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_form
		))
    (setq next_button_w
	  (send XM_ARROW_BUTTON_GADGET_CLASS :new :managed
		"prev_button_w" controlpanel2_w 
		:XMN_ARROW_DIRECTION	:arrow_down
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	prev_button_w
		))
    (setq edit_file_button_w
	  (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed
		"edit_button_w" controlpanel2_w 
		:XMN_LABEL_STRING	"$EDITOR(sel)"
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	next_button_w
		))
    (setq filename_label_label_w
	  (send XM_LABEL_GADGET_CLASS :new :managed
		"filename_label_label_w" controlpanel2_w
		:XMN_LABEL_STRING	" Viewed File:"
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	edit_file_button_w
		:XMN_ALIGNMENT		:alignment_end
		))
    (setq filename_label_w
	  (send XM_LABEL_GADGET_CLASS :new :managed
		"filename_label_w" controlpanel2_w
		:XMN_LABEL_STRING	"[ None ]"
		:XMN_TOP_ATTACHMENT	:attach_form
		:XMN_BOTTOM_ATTACHMENT	:attach_form
		:XMN_LEFT_ATTACHMENT	:attach_widget
		:XMN_LEFT_WIDGET	filename_label_label_w
		:XMN_RIGHT_ATTACHMENT	:attach_form
		:XMN_ALIGNMENT		:alignment_beginning
		))
    (setq viewtext_w
	  (send Text_Viewer_Widget_Class :new :managed
		"viewtext_w" paned_w
		:XMN_ROWS		10
		:XMN_COLUMNS		80
		))

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;; Setup Keyboard Accelerators ;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    (send list_w :set_values
	  :XMN_TRANSLATIONS	"#override \
		<Btn2Down>:   	ListBeginSelect() \
		<Btn2Up>:	ListEndSelect() ListKbdActivate() \
		<Btn3Down>:	ListBeginSelect() \
		<Btn3Up>:	ListEndSelect() Lisp(send ACTION_WIDGET :edit_selected_item)"
	  :XMN_ACCELERATORS	"#override \
		<Key>E:		Lisp(send ACTION_WIDGET :edit_selected_item) \
		Ctrl<Key>N:	Lisp(send ACTION_WIDGET :goto_next) \
		Ctrl<Key>osfDown: Lisp(send ACTION_WIDGET :goto_next) \
		Ctrl<Key>P:	Lisp(send ACTION_WIDGET :goto_prev) \
		Ctrl<Key>osfUp:	Lisp(send ACTION_WIDGET :goto_prev) \
		<Key>N:		Lisp(send ACTION_WIDGET :browse_next ACTION_XEVENT) \
		<Key>osfDown:	Lisp(send ACTION_WIDGET :browse_next ACTION_XEVENT) \
		<Key>P:		Lisp(send ACTION_WIDGET :browse_prev ACTION_XEVENT) \
		<Key>osfUp:	Lisp(send ACTION_WIDGET :browse_prev ACTION_XEVENT)"
	  )

    (send viewtext_w :set_values
	  :XMN_ACCELERATORS	"#override \
		<Key>space:	   next-page() \
		<Key>osfBackSpace: previous-page()"
	  )

     (send list_w :install_all_accelerators top_w)
     (send viewtext_w :install_all_accelerators top_w)
     (send controlpanel_w :install_all_accelerators top_w)
     (send controlpanel2_w :install_all_accelerators top_w)
     (send controlpanel3_w :install_all_accelerators top_w)
     (send paned_w :install_all_accelerators top_w)
     (send doit_button_w :install_all_accelerators top_w) ;this is a widget, so it needs accels, all other buttons are gadgets so they don't...

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;; Realize Widgets to find out real sizes, then diddle constraints... ;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    (send top_w :realize)

    ;;
    ;; set constraint resources on controlpanels so that paned window
    ;; doesn't give them resize sashes.
    ;;
    (let (height)

      (send controlpanel_w :get_values :xmn_height 'height)
      (send controlpanel_w :set_values
	    (if motif-v-1-0-p :XMN_MAXIMUM :XMN_PANE_MAXIMUM) height
	    (if motif-v-1-0-p :XMN_MINIMUM :XMN_PANE_MINIMUM) height
	    )

      (send controlpanel2_w :get_values :xmn_height 'height)
      (send controlpanel2_w :set_values
	    (if motif-v-1-0-p :XMN_MAXIMUM :XMN_PANE_MAXIMUM) height
	    (if motif-v-1-0-p :XMN_MINIMUM :XMN_PANE_MINIMUM) height
	    )

      (send controlpanel3_w :get_values :xmn_height 'height)
      (send controlpanel3_w :set_values
	    (if motif-v-1-0-p :XMN_MAXIMUM :XMN_PANE_MAXIMUM) height
	    (if motif-v-1-0-p :XMN_MINIMUM :XMN_PANE_MINIMUM) height
	    )
      )

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;  Setup Callbacks ... ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    ;;
    ;; The "Clone" button creates another browser
    ;;
    (send clone_button_w :add_callback :XMN_ACTIVATE_CALLBACK '()
	  `(
	    (grep-browser)
	    ))
    ;;
    ;; The doit_button initiates a grep search.
    ;;
    (send doit_button_w :add_callback :XMN_ARM_CALLBACK '() ;NOTE -- using :XMN_ARM_CALLBACK rather than :XMN_ACTIVATE_CALLBACK because the former will keep the button filled (indicating search-in-progress) while grep is doing it's thing.
	  `(
	    ;; clear out old list while 'grep' is thinking
	    (send ,list_w :set_browser_items '())
	    (send ,list_w :update_display)
	    
	    ;; set new list contents to result of grep
	    (send ,list_w :set_browser_items
		  (grep 
		   *GREP-BR-EXECUTABLE*	;use this in place of commented out option menu code below ... for now.
#| ;; NPM: commented out because simple-option-menu has motif bug
                   (xm_string_get_l_to_r (send (send ,grepprog_opt_w :get :xmn_menu_history) :get :xmn_label_string))
|# ;; NPM: commented out because simple-option-menu has motif bug
                   (if (send ,case_toggle_w :get_state) "" "-i")
		   (strcat
		    "'"			;quotify string to protect regexps from being expanded by shell
		    (send ,search_editor_w :get_string) ;string to search for
		    "' "		;quotify string to protect regexps from being expanded by shell
		    (send ,files_editor_w :get_string)) ;wildcarded files
		   )
		  )
	    ))
    ;;
    ;; Bind doit_button arm callback to <return> key in search_editor_w.
    ;;
    (send search_editor_w :add_callback :XMN_ACTIVATE_CALLBACK '(CALLBACK_XEVENT)
	  `(
	    (send ,doit_button_w :call_action_proc "ArmAndActivate" CALLBACK_XEVENT) 
	    ))
    ;;
    ;; Bind doit_button arm callback to <return> key in files_editor_w.
    ;;
    (send files_editor_w :add_callback :XMN_ACTIVATE_CALLBACK '(CALLBACK_XEVENT)
	  `(
	    (send ,doit_button_w :call_action_proc "ArmAndActivate" CALLBACK_XEVENT) 
	    ))
    ;; 
    ;; This callback will get the selection from the viewtext_w first, set
    ;; that to the search string, and then call doit_button arm callback.
    ;;
    (send srchsel_button_w :add_callback :XMN_ACTIVATE_CALLBACK '(CALLBACK_XEVENT)
	  `(
	    (send ,search_editor_w :set_string (send ,viewtext_w :get_selection))
	    (send ,doit_button_w :call_action_proc "ArmAndActivate" CALLBACK_XEVENT) 
	    ))
    ;;
    ;; On pressing "View Selection in $EDITOR" button, call the *SYSTEM-EDITOR*,
    ;; else call $EDITOR on file in view area.
    ;;
    (send edit_file_button_w :add_callback :XMN_ACTIVATE_CALLBACK '()
	  `(
	    (send ,list_w :edit_selected_item)
	    ))
    ;;
    ;; Add callbacks for buttons browsing prev/next items in list
    ;;
    (send prev_button_w :add_callback :XMN_ACTIVATE_CALLBACK '(CALLBACK_XEVENT)
	  `(
	    (send ,list_w :browse_prev CALLBACK_XEVENT)
	    ))
    (send next_button_w :add_callback :XMN_ACTIVATE_CALLBACK '(CALLBACK_XEVENT)
	  `(
	    (send ,list_w :browse_next CALLBACK_XEVENT)
	    ))
    ;;
    ;; set up a callback on the list widget such that a double click (button 1)
    ;; on the browser-item will browse the object.
    ;;
    (send list_w :add_callback :XMN_DEFAULT_ACTION_CALLBACK '()
	  `(
	    (let ((browsed-object (send ,list_w :get_selected_item)))
	      (if browsed-object	;set to NIL if no browsed object
		  (let ((filename (send browsed-object :file-name))
			(linenum  (send browsed-object :line-num)))
		    (send ,filename_label_w :set_values :XMN_LABEL_STRING filename)
		    (send ,filename_label_w :update_display) ;incase reading file takes long time
		    (send ,viewtext_w :find_file filename linenum)
		    ))
	      )
	    ))

    ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; bring up an instance of the grep browser upon loading this file.
(grep-browser)

) ;; end -- progn					
) ;; end -- "if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))"
