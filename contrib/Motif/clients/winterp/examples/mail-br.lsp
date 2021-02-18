; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         mail-browser.lsp
; RCS:          $Header: mail-br.lsp,v 1.3 91/10/05 18:24:09 mayer Exp $
; Description:  A simple MH mail browser written mostly to show the power of
;               subclassing the Motif list widget in WINTERP. Load this file
;               to get a browser of the last 30 MH messages in your inbox.
;               This assumes that (1) you have MH, (2) you have folder +inbox,
;               (3) "scan" is on your $PATH. (4) various other things I forgot.
; Author:       Niels Mayer, HPLabs
; Created:      Mon Nov 20 18:13:23 1989
; Modified:     Sat Oct  5 18:23:48 1991 (Niels Mayer) mayer@hplnpm
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

;;
;; Make a subclass of XM_LIST_WIDGET_CLASS which holds an additional
;; instance variable 'items'. 'items' is an array of arbitrary objects
;; (BROWSER_OBJECT) to be displayed in a browser made from the list widget.
;;
;; BROWSER-OBJECT can be any arbitrary xlisp object that respond to
;; the messages :display_string and :default_action:
;;
;; Message :display_string must return a string which is used as the
;; textual representation of the object in the browser display.
;;
;; Message :default_action is sent to the object whenever the
;; list widget's default action, a double-click, is performed on the item
;; corresponding to the object.
;; 
(setq List_Browser 
      (send Class :new
	    '(items)			;new instance vars
	    '()				;no class vars
	    XM_LIST_WIDGET_CLASS))	;superclass

;;
;; We override the XM_LIST_WIDGET_CLASS's object initializer
;; so that we can process the items list and hand off the
;; browser items to the list widget.
;;
;; (send List_Browser :new <items_list> <args-for-the-list-widget>)
;; <items_list> is a list of BROWSER_OBJECTs as described above.
;; <args-for-the-list-widget> -- these are the arguments that
;;       will be passed on to the list widget
;;
(send List_Browser :answer :isnew '(items_list &rest args)
      '(
	(let* (
	       (items_end_idx (length items_list))
	       (display_items (make-array items_end_idx)))

	  ;; initialize the 'items' instance variable so that it
	  ;; holds all the BROWSER_OBJECTs passed in <items_list>
	  (setq items (make-array items_end_idx)) ;create the array
	  (do (				;copy elts from list to array
	       (i    0          (1+ i))
	       (elts items_list (cdr elts)))
	      ;; loop till no more elts
	      ((null elts))
	      ;; loop body
	      (setf (aref items i) (car elts))
	      (setf (aref display_items i) (send (car elts) :display_string))
	      )

	  ;; initialize the widget, passing in the browser items.
	  (apply 'send-super `(:isnew
			       ,@args
			       :xmn_selection_policy :browse_select
			       :xmn_items ,display_items
			       :xmn_item_count ,items_end_idx
			       ))
	  )

	;; set up a callback on the list widget initialized above such that
	;; a double click on the browser-item will send the message
	;; :default_action to the BROWSER_OBJECT.
	(send-super :set_callback :xmn_default_action_callback
		    '(callback_item_position)
		    '((send (aref items (1- callback_item_position)) :default_action))
		    )
	)
      )

;;
;; override methods on XM_LIST_WIDGET_CLASS so that they work properly
;; with the list browser. Note that all other list methods work fine
;; on the list browser
;;
(send List_Browser :answer :ADD_ITEM '(item position)
      '(
	(setq items (array-insert-pos items (1- position) item))
	(send-super :add_item (send item :display_string) position)
	)
      )

(send List_Browser :answer :ADD_ITEM_UNSELECTED '(item position)
      '(
	(setq items (array-insert-pos items (1- position) item))
	(send-super :add_item_unselected (send item :display_string) position)
	)
      )

(send List_Browser :answer :DELETE_ITEM '(item)
      '(
	;; this is too lame to implement... requires that we compare
	;; item with the result of :display_string done on every element
	;; of ivar 'items'
	(error "Message :DELETE_ITEM not supported in List_Browser")
	)
      )

(send List_Browser :answer :DELETE_POS '(position)
      '(
	(setq items (array-delete-pos items (1- position)))
	(send-super :delete_pos position)
	)
      )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Define a BROWSER_OBJECT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Each BROWSER_OBJECT holds the information summarizing one mail message.
;; the information is split up into individual fields because we may want
;; to be able to sort on one field, or search for mathes on one field.
;;
(setq Mail_Message_Class
      (send Class :new
	    '(folder num anno month date no-date size sender subject)
	    ))

;; this string is passed to the mh 'scan' and 'inc' commands to determine
;; the formatting of the output of the message info summary. Each entry
;; here corresponds to an instance variable in Mail_Message_Class
(setq FOLDER_SCAN_FORMAT 
      (strcat
       "%(msg)"				;output the message number
       "%<{replied}A%|"			;IF msg answered output "A" ELSE
       "%<{forwarded}F%|"		;IF msg forwarded output "F" ELSE
       "%<{resent}R%|"			;IF msg redisted output "R" ELSE
       "%<{printed}P%|"			;IF msg printed output "P"
       " %>%>%>%>"			;ELSE output " "
       "%02(mon{date})/%02(mday{date})"	;output mon/date
       "%<{date} %|*%>"			;IF no date output "*" else " "
       "%(size) "			;output the message's size
       "%<(mymbox{from})To:%14(friendly{to})%|"	;IF my message, output "To: <recipient>"
       "%17(friendly{from})%> "		;ELSE output sender field
       "%{subject}<<"			;output subject followed by ">>"
       "%{body}"			;output beginning of body, limited by SCAN_OUTPUT_WIDTH
       )
      )

;; this method will read a single line summary of a mail message as produced
;; by the mh 'scan' or 'inc' commands and sets the instance variables in the 
;; BROWSER_OBJECT to the individual fields of the message summary.
(send Mail_Message_Class :answer :read-msg-info '(pipe fldr)
      '(
	(if (and
	     (setq folder fldr)
	     (setq num     (fscanf-fixnum pipe "%ld"))
	     (setq anno    (fscanf-string pipe "%c"))
	     (setq month   (fscanf-fixnum pipe "%2ld"))
	     (setq date    (fscanf-fixnum pipe "/%2ld"))
	     (setq no-date (fscanf-string pipe "%c"))
	     (setq size    (fscanf-fixnum pipe "%d%*c"))
	     (setq sender  (fscanf-string pipe "%17[\001-\177]%*c"))
	     (setq subject (fscanf-string pipe "%[^\n]\n"))
	     )
	    self			;return self if succesful
	  NIL				;return NIL if hit EOF
	  )
	)
      )

(send Mail_Message_Class :answer :display_string '()
      '(
	(format nil
		"~A ~A ~A/~A~A ~A ~A ~A"
		num anno month date no-date size sender subject)
	))

(send Mail_Message_Class :answer :default_action '()
      '((find-file (format nil "~A/~A/~A" MAILPATH folder num))))


;;
;; i'm too lazy to add a getenv() interface to WINTERP... this'll do for now.
;;
(setq MAILPATH
      (let*
	  ((pipe (popen "/bin/echo $HOME" "r"))
	   (home (read-line pipe))
	   )
	(pclose pipe)
	(strcat home "/Mail"))		;this is the default directory
					;for MH... this assumes you haven't
					;put the MH directory elsewhere
					;via a ~/.mh_profile entry.
      )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; This returns a list of Mail_Message_Class instances corresponding
;; to the mail messages scanned from <foldername> over range <msgs>.
;;
(defun mh-scan (foldername msgs)
  (do* 
   ((fp (popen (strcat "scan "
		       "+" foldername
		       " " msgs
		       " -noclear -noheader -reverse -width 80"
		       " -format '" FOLDER_SCAN_FORMAT "'")
	       :direction :input))
    (msg (send (send Mail_Message_Class :new) :read-msg-info fp foldername)
	 (send (send Mail_Message_Class :new) :read-msg-info fp foldername))
    (result NIL)
    )
   ((null msg)				;:read-msg-info returns NIL on EOF
    (pclose fp)
    (cdr result)			;last msg was EOF, remove it
    )
   (setq result (cons msg result))
   )
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq top_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
;	    :XMN_GEOMETRY "500x700+1+1"
	    :XMN_TITLE "Mail Browser"
	    :XMN_ICON_NAME "Mail Browser"
	    ))

(setq paned_w
      (send XM_PANED_WINDOW_WIDGET_CLASS :new :managed top_w
	    ))

(setq objs-list (mh-scan "inbox" "last:30"))

(setq list_w
      (send List_Browser :new objs-list :managed :scrolled "browser" paned_w
	    :xmn_visible_item_count 10
	    ))

(setq label_w
      (send XM_LABEL_WIDGET_CLASS :new :managed "label" paned_w
	    :xmn_label_string "None"
	    ))

;;
;; set constraint resources on label widget so that paned window
;; doesn't give it resize sashes.
;;
(let (height)
  (send label_w :get_values :xmn_height 'height)
  ;; In the code below, the kludgery
  ;; "(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0)) ...)"
  ;; is there to work around a name change between Motif 1.0 and 1.1:
  ;; :XMN_MAXIMUM --> :XMN_PANE_MAXIMUM and :XMN_MINIMUM -->:XMN_PANE_MINIMUM
  (send label_w :set_values
	(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
	    :XMN_MAXIMUM :XMN_PANE_MAXIMUM)
	height
	(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
	    :XMN_MINIMUM :XMN_PANE_MINIMUM)
	height
	))

(setq textedit_w 
      (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled "view" paned_w
	    :XMN_EDIT_MODE :MULTI_LINE_EDIT
	    :XMN_HEIGHT 200
	    :XMN_EDITABLE nil		;don't allow user to change text.
	    ))

(send top_w :realize)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun find-file (file)
  (let*
      (;; loc vars
       (fp
	(open file :direction :input)
	)
       inspos
       text_line
       )

    (if (null fp)
	(error "Can't open file." file))

    (send label_w :set_values
	  :xmn_label_string file)
    (send textedit_w :set_string "")	;clear out old text
    (send paned_w :update_display)	;incase reading file takes long time

    (send textedit_w :disable_redisplay NIL) ;don't show changes till done
    (send textedit_w :replace 0 0 (read-line fp))
    (loop
     (if (null (setq text_line (read-line fp)))
	 (return))
     (setq inspos (send textedit_w :get_insertion_position))
     (send textedit_w :replace inspos inspos (strcat "\n" text_line))
     )
    (send textedit_w :enable_redisplay)	;now show changes...
    (close fp)
    )
  )
