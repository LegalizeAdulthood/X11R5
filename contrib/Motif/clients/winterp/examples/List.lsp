; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         List.lsp
; RCS:          $Header: List.lsp,v 1.4 91/10/05 03:40:36 mayer Exp $
; Description:  Demo of XM_LIST_WIDGET_CLASS
; Author:       Niels Mayer, HPLabs
; Created:      Sun Feb 10 20:33:16 1991
; Modified:     Sat Oct  5 03:39:21 1991 (Niels Mayer) mayer@hplnpm
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

(defun motif-1.1-p () 
  (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1)))

(setq toplevel_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "listsh"
	    :XMN_TITLE		"Winterp: XmList Test"
	    :XMN_ICON_NAME	"W:XmList-Test"
	    ))

(setq scrl_w
      (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
	    "sc" toplevel_w
	    :XMN_SCROLLING_POLICY	:automatic
	    ))

(setq rc_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	    "rc" scrl_w
	    :XMN_ORIENTATION		:VERTICAL
	    :XMN_PACKING		:PACK_TIGHT
	    :XMN_ENTRY_ALIGNMENT 	:ALIGNMENT_CENTER
;;;	    :XMN_FOREGROUND		"Black"
;;;	    :XMN_BACKGROUND		"LightGray"
	    ))

(send toplevel_w :realize)

(setq items-list '("foo" "bar" "baz" "bof" "boof" "frob" "snob" "blog"))

(setq list0_w
      (send XM_LIST_WIDGET_CLASS :new :managed :scrolled
	    "list0" rc_w
	    :XMN_SELECTION_POLICY	:browse_select
	    :XMN_ITEMS			items-list
	    :XMN_ITEM_COUNT		(length items-list)
	    :XMN_VISIBLE_ITEM_COUNT	20
	    ))

(setq retrieved-items-list
      (let* ((items_array (send list0_w :get_items))
	     (items_length (length items_array))
	     (result '())
	     )
	(do ((i 0 (1+ i))
	     )
	    ((= i items_length)
	     (reverse result)
	     )
	    (setq result
		  (cons (xm_string_get_l_to_r (aref items_array i))
			result))
	    )))

(print items-list)
(print retrieved-items-list)

(if (not (equal items-list retrieved-items-list))
    (error (format nil "~A =/= ~A\n" items-list retrieved-items-list)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun directory-list (dir)
  (do* 
   ((fp (popen (strcat "ls -lr " dir)
	       :direction :input))
    (line (read-line fp) (read-line fp))
    (result (list line) (cons line result))
    )
   ((null line)
    (pclose fp)
    (cdr result)
    )
   )
  )

(setq length
      (length
       (setq list (directory-list "$HOME"))
       ))


(setq list1_w
      (send XM_LIST_WIDGET_CLASS :new :managed :scrolled
	    "list1" rc_w 
	    :XMN_WIDTH				500
	    :XMN_SELECTION_POLICY		:extended_select
	    :XMN_LIST_SIZE_POLICY		:constant
	    :XMN_SCROLL_BAR_DISPLAY_POLICY	:static
	    :XMN_TRAVERSAL_ON			T
	    :XMN_ITEMS				list
	    :XMN_ITEM_COUNT			length
	    :XMN_VISIBLE_ITEM_COUNT		20
	    ))

(send list1_w :set_values 
      :XMN_ITEMS	list
      :XMN_ITEM_COUNT	length
      )

(send list1_w :get_values 
      :XMN_ITEM_COUNT nil
      )

(send list1_w :set_values 
      :XMN_SELECTED_ITEMS	list
      :XMN_SELECTED_ITEM_COUNT	length
      )

(setq retrieved-items-list
      (let* ((items_array (send list1_w :get_selected_items))
	     (items_length (length items_array))
	     (result '())
	     )
	(do ((i 0 (1+ i))
	     )
	    ((= i items_length)
	     (reverse result)
	     )
	    (setq result
		  (cons (xm_string_get_l_to_r (aref items_array i))
			result))
	    ))
      )

(print list)
(print retrieved-items-list)

(if (not (equal list retrieved-items-list))
    (error (format nil "~A =/= ~A\n" list retrieved-items-list)))


(send list1_w :set_values 
      :XMN_WIDTH 400
      )


(setq item (send list1_w :ADD_ITEM (symbol-name (gensym)) 12))
(send list1_w :ADD_ITEM_UNSELECTED (symbol-name (gensym)) 12)
(send list1_w :DELETE_ITEM item)
(send list1_w :DELETE_POS 1)
(send list1_w :SELECT_ITEM item)
(send list1_w :DESELECT_ITEM item)
(send list1_w :SELECT_POS 2)
(send list1_w :DESELECT_POS 2)
(send list1_w :DESELECT_ALL_ITEMS)
(send list1_w :SET_POS 3)
(send list1_w :SET_BOTTOM_POS 10)
(send list1_w :SET_ITEM item)
(send list1_w :SET_BOTTOM_ITEM item)
(send list1_w :ITEM_EXISTS "foo")
(send list1_w :ITEM_EXISTS item)
(send list1_w :SET_HORIZ_POS 0)

(if (motif-1.1-p)
(progn

(send list1_w :add_items
      (list (symbol-name (gensym)))
      0)


(send list1_w :add_items
      (list (symbol-name (gensym)))
      10)


(send list1_w :add_items
      (list (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)))
      0)

(send list1_w :add_items
      (list (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)))
      2)

(send list1_w :add_items
      (vector (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)) (symbol-name (gensym)))
      10)

(send list1_w :add_items
      #("FIRST" "SECOND" "THIRD" "FOURTH" "FIFTH" "SIXTH" "SEVENTH" "EIGTH" "NINTH" "TENTH")
      10)

(setq array-of-xmstrings
      (vector (xm_string_create "FIRST")
	      (xm_string_create "SECOND")
	      (xm_string_create "THIRD")
	      (xm_string_create "FOURTH")
	      (xm_string_create "FIFTH")
	      (xm_string_create "SIXTH")
	      (xm_string_create "SEVENTH")
	      (xm_string_create "EIGTH")
	      (xm_string_create "NINTH")
	      (xm_string_create "TENTH"))
      )

(send list1_w :add_items array-of-xmstrings 10)

(send list1_w :delete_items array-of-xmstrings)

(send list1_w :delete_items
      #("FIRST" "SECOND" "THIRD" "FOURTH" "FIFTH" "SIXTH" "SEVENTH" "EIGTH" "NINTH" "TENTH")
      )

(send list1_w :DELETE_ITEMS_POS 1 1)
(send list1_w :DELETE_ITEMS_POS 2 10)

(send list1_w :delete_all_items)

(send list1_w :add_items array-of-xmstrings 1)
(send list1_w :add_items array-of-xmstrings 0)

(send list1_w :REPLACE_ITEMS
      array-of-xmstrings 
      #("1st" "2ond" "3rd" "4th" "5th" "6th" "7th" "8th" "9th" "10th")
      )

(send list1_w :REPLACE_ITEMS_POS
      #("1st" "2ond" "3rd" "4th" "5th" "6th" "7th" "8th" "9th" "10th")
      1)

(send list1_w :REPLACE_ITEMS_POS
      #("1st" "2ond" "3rd" "4th" "5th" "6th" "7th" "8th" "9th" "10th")
      10)

(send list1_w :SET_ADD_MODE t)
(send list1_w :SET_ADD_MODE nil)

(send list1_w :ITEM_POS "10th")

(send list1_w :get_match_pos "1st")
(send list1_w :get_match_pos "FIRST")

(send list1_w :get_selected_pos)

))

(gc)

(setq cb1es
      (send list1_w :set_callback :xmn_extended_selection_callback
	    '(
	      callback_widget
	      callback_reason 
	      callback_xevent
	      callback_item
	      callback_item_length
	      callback_item_position
	      callback_selected_items
	      callback_selected_item_count
	      callback_selection_type
	      callback_selected_item_positions ;new for 1.1
	      )
	    '(
	      (format T "-------------------------------------------\n")
	      (format T "widget=~A, reason=~A, xevent=~A\n" 
		      callback_widget callback_reason callback_xevent)
	      (format T "LIST_ITEM=~A\n"
		      callback_item)
	      (format T "LIST_ITEM_LENGTH=~A\n"
		      callback_item_length)
	      (format T "LIST_ITEM_POSITION=~A\n"
		      callback_item_position)
	      (format T "LIST_SELECTED_ITEMS=~A\n"
		      callback_selected_items)
	      (format T "LIST_SELECTED_ITEM_COUNT=~A\n"
		      callback_selected_item_count)
	      (format T "LIST_SELECTION_TYPE=~A\n"
		      callback_selection_type)
	      (format T "CALLBACK_SELECTED_ITEM_POSITIONS=~A\n"
		      callback_selected_item_positions)
	      )
	    ))
	       
(send list1_w :remove_all_callbacks :xmn_extended_selection_callback)

(setq cb1da
      (send list1_w :set_callback :xmn_default_action_callback
	    '(
	      callback_widget
	      callback_reason 
	      callback_xevent
	      callback_item
	      callback_item_length
	      callback_item_position
;;;	      callback_selected_items
;;;	      callback_selected_item_count
;;;	      callback_selection_type
	      )
	    '(
	      (format T "-------------------------------------------\n")
	      (format T "widget=~A, reason=~A, xevent=~A\n" 
		      callback_widget callback_reason callback_xevent)
	      (format T "LIST_ITEM=~A\n"
		      callback_item)
	      (format T "LIST_ITEM_LENGTH=~A\n"
		      callback_item_length)
	      (format T "LIST_ITEM_POSITION=~A\n"
		      callback_item_position)
;;;	      (format T "LIST_SELECTED_ITEMS=~A\n"
;;;		       callback_selected_items)
;;;	      (format T "LIST_SELECTED_ITEM_COUNT=~A\n"
;;;		       callback_selected_item_count)
;;;	      (format T "LIST_SELECTION_TYPE=~A\n"
;;;		        callback_selection_type)
	      )
	    ))
	       


;(xt_remove_callback cb1)
;(gc)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(progn
  (setq list2_w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled
	      "list2" rc_w 
	      :XMN_WIDTH			500
	      :XMN_SELECTION_policy		:single_select
	      :XMN_LIST_SIZE_POLICY		:constant
	      :XMN_SCROLL_BAR_DISPLAY_POLICY	:static
	      :XMN_TRAVERSAL_ON			T
	      :XMN_ITEMS			list
	      :XMN_ITEM_COUNT			length
	      :XMN_VISIBLE_ITEM_COUNT		20
	      ))

  (setq cb2
	(send list2_w :set_callback :XMN_SINGLE_SELECTION_CALLBACK
	      '(
		callback_widget
		callback_reason 
		callback_xevent
		callback_item
		callback_item_length
		callback_item_position
;;; 		callback_selected_items
;;; 		callback_selected_item_count
;;; 		callback_selection_type
;;;		callback_selected_item_positions ;new for 1.1
		)
	      '(
		(format T "-------------------------------------------\n")
		(format T "widget=~A, reason=~A, xevent=~A\n" 
			callback_widget callback_reason callback_xevent)
		(format T "LIST_ITEM=~A\n"
			callback_item)
		(format T "LIST_ITEM_LENGTH=~A\n"
			callback_item_length)
		(format T "LIST_ITEM_POSITION=~A\n"
			callback_item_position)
;;; 	        (format T "LIST_SELECTED_ITEMS=~A\n"
;;; 		        callback_selected_items)
;;; 		(format T "LIST_SELECTED_ITEM_COUNT=~A\n"
;;; 		        callback_selected_item_count)
;;; 	        (format T "LIST_SELECTION_TYPE=~A\n"
;;; 		        callback_selection_type)
;;; 		(format T "CALLBACK_SELECTED_ITEM_POSITIONS=~A\n"
;;; 		        callback_selected_item_positions)
		)
	      ))
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(progn
  (setq list3_w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled
	      "list3" rc_w
	      :XMN_WIDTH			500
	      :XMN_SELECTION_POLICY		:multiple_select
	      :XMN_LIST_SIZE_POLICY		:constant
	      :XMN_SCROLL_BAR_DISPLAY_POLICY	:static
	      :XMN_TRAVERSAL_ON			T
	      :XMN_ITEMS			list
	      :XMN_ITEM_COUNT			length
	      :XMN_VISIBLE_ITEM_COUNT		20
	      ))
  (setq cb3
	(send list3_w :set_callback :XMN_MULTIPLE_SELECTION_CALLBACK
	      (if (motif-1.1-p)
		  '(
		    callback_widget
		    callback_reason 
		    callback_xevent
		    callback_item
		    callback_item_length
		    callback_item_position
		    callback_selected_items
		    callback_selected_item_count
;;;		    callback_selection_type
		    callback_selected_item_positions ;new for 1.1
		    )
		'(
		  callback_widget
		  callback_reason 
		  callback_xevent
		  callback_item
		  callback_item_length
		  callback_item_position
		  callback_selected_items
		  callback_selected_item_count
;;;	  	  callback_selection_type
;;;		  callback_selected_item_positions ;new for 1.1
		  )
		)
	      '(
		(format T "-------------------------------------------\n")
		(format T "widget=~A, reason=~A, xevent=~A\n" 
			callback_widget callback_reason callback_xevent)
		(format T "LIST_ITEM=~A\n"
			callback_item)
		(format T "LIST_ITEM_LENGTH=~A\n"
			callback_item_length)
		(format T "LIST_ITEM_POSITION=~A\n"
			callback_item_position)
		(format T "LIST_SELECTED_ITEMS=~A\n"
			callback_selected_items)
		(format T "LIST_SELECTED_ITEM_COUNT=~A\n"
			callback_selected_item_count)
;;;		(format T "LIST_SELECTION_TYPE=~A\n"
;;;			callback_selection_type)
		(if (motif-1.1-p)
		    (format T "CALLBACK_SELECTED_ITEM_POSITIONS=~A\n"
			    callback_selected_item_positions)
		  )
		)
	      ))
  )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(progn
  (setq list4_w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled
	      "list4" rc_w 
	      :XMN_WIDTH			500
	      :XMN_SELECTION_POLICY		:extended_select
	      :XMN_LIST_SIZE_POLICY		:constant
	      :XMN_SCROLL_BAR_DISPLAY_POLICY	:static
	      :XMN_TRAVERSAL_ON			T
	      :XMN_ITEMS			list
	      :XMN_ITEM_COUNT			length
	      :XMN_VISIBLE_ITEM_COUNT		20
	      ))
  (setq cb4
	(send list4_w :set_callback :XMN_EXTENDED_SELECTION_CALLBACK
	      (if (motif-1.1-p)
		  '(
		    callback_widget
		    callback_reason 
		    callback_xevent
		    callback_item
		    callback_item_length
		    callback_item_position
		    callback_selected_items
		    callback_selected_item_count
		    callback_selection_type
		    callback_selected_item_positions ;new for 1.1
		    )
		'(
		  callback_widget
		  callback_reason 
		  callback_xevent
		  callback_item
		  callback_item_length
		  callback_item_position
		  callback_selected_items
		  callback_selected_item_count
		  callback_selection_type
		  ))
	      '(
		(format T "-------------------------------------------\n")
		(format T "widget=~A, reason=~A, xevent=~A\n" 
			callback_widget callback_reason callback_xevent)
		(format T "LIST_ITEM=~A\n"
			callback_item)
		(format T "LIST_ITEM_LENGTH=~A\n"
			callback_item_length)
		(format T "LIST_ITEM_POSITION=~A\n"
			callback_item_position)
		(format T "LIST_SELECTED_ITEMS=~A\n"
			callback_selected_items)
		(format T "LIST_SELECTED_ITEM_COUNT=~A\n"
			callback_selected_item_count)
		(format T "LIST_SELECTION_TYPE=~A\n"
			callback_selection_type)
		(if (motif-1.1-p)
		    (format T "CALLBACK_SELECTED_ITEM_POSITIONS=~A\n"
			    callback_selected_item_positions)
		  )
		)
	      ))
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(progn
  (setq list5_w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled
	      "list5" rc_w 
	      :XMN_WIDTH		500
	      :XMN_SELECTION_POLICY	:browse_select
	      :XMN_LIST_SIZE_POLICY	:constant
	      :XMN_SCROLL_BAR_DISPLAY_POLICY :static
	      :XMN_TRAVERSAL_ON		T
	      :XMN_ITEMS		list
	      :XMN_ITEM_COUNT		length
	      :XMN_VISIBLE_ITEM_COUNT	20
	      ))
  (setq cb5
	(send list5_w :set_callback :XMN_BROWSE_SELECTION_CALLBACK
	      '(
		callback_widget
		callback_reason 
		callback_xevent
		callback_item
		callback_item_length
		callback_item_position
;;; 		callback_selected_items
;;; 		callback_selected_item_count
;;; 		callback_selection_type
;;; 		callback_selected_item_positions ;new for 1.1
		)
	      '(
		(format T "-------------------------------------------\n")
		(format T "widget=~A, reason=~A, xevent=~A\n" 
			callback_widget callback_reason callback_xevent)
		(format T "LIST_ITEM=~A\n"
			callback_item)
		(format T "LIST_ITEM_LENGTH=~A\n"
			callback_item_length)
		(format T "LIST_ITEM_POSITION=~A\n"
			callback_item_position)
;;; 		(format T "LIST_SELECTED_ITEMS=~A\n"
;;; 			callback_selected_items)
;;; 		(format T "LIST_SELECTED_ITEM_COUNT=~A\n"
;;; 			callback_selected_item_count)
;;; 		(format T "LIST_SELECTION_TYPE=~A\n"
;;; 			callback_selection_type)
;;; 		(format T "CALLBACK_SELECTED_ITEM_POSITIONS=~A\n"
;;; 			callback_selected_item_positions)
		)
	      ))
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(progn
  (setq list6_w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled
	      "list6" rc_w 
	      :XMN_WIDTH		500
;;;           :XMN_SELECTION_POLICY	:extended_select
	      :XMN_LIST_SIZE_POLICY	:constant
	      :XMN_SCROLL_BAR_DISPLAY_POLICY :static
	      :XMN_TRAVERSAL_ON		T
	      :XMN_ITEMS		list
	      :XMN_ITEM_COUNT		length
	      :XMN_VISIBLE_ITEM_COUNT	20
	      ))
  (setq cb6
	(send list6_w :set_callback :XMN_DEFAULT_ACTION_CALLBACK
	      '(
		callback_widget
		callback_reason 
		callback_xevent
		callback_item
		callback_item_length
		callback_item_position
;;; 		callback_selected_items
;;; 		callback_selected_item_count
;;; 		callback_selection_type
;;; 		callback_selected_item_positions ;new for 1.1
		)
	      '(
		(format T "-------------------------------------------\n")
		(format T "widget=~A, reason=~A, xevent=~A\n" 
			callback_widget callback_reason callback_xevent)
		(format T "LIST_ITEM=~A\n"
			callback_item)
		(format T "LIST_ITEM_LENGTH=~A\n"
			callback_item_length)
		(format T "LIST_ITEM_POSITION=~A\n"
			callback_item_position)
;;; 		(format T "LIST_SELECTED_ITEMS=~A\n"
;;; 			callback_selected_items)
;;; 		(format T "LIST_SELECTED_ITEM_COUNT=~A\n"
;;; 			callback_selected_item_count)
;;; 		(format T "LIST_SELECTION_TYPE=~A\n"
;;; 			callback_selection_type)
;;; 		(format T "CALLBACK_SELECTED_ITEM_POSITIONS=~A\n"
;;; 			callback_selected_item_positions)
		)
	      ))
  )
