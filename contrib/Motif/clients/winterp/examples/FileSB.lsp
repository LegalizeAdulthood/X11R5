; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         FileSB.lsp
; RCS:          $Header: FileSB.lsp,v 1.2 91/10/05 02:32:53 mayer Exp $
; Description:  Demo of XM_FILE_SELECTION_BOX_WIDGET_CLASS
; Author:       Niels Mayer, HPLabs
; Created:      Sun Feb 10 20:32:40 1991
; Modified:     Sat Oct  5 02:30:51 1991 (Niels Mayer) mayer@hplnpm
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
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "fsb"
	    :XMN_TITLE "FileSB Widget Test"
	    :XMN_ICON_NAME "Test"
	    ))

(setq fsb_w 
      (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new :managed
	    "files" top_w
    ))

(setq cb1 (send fsb_w :set_callback :XMN_APPLY_CALLBACK
		(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
		    '(			;for Motif 1.1
		      CALLBACK_WIDGET
		      CALLBACK_REASON
		      CALLBACK_XEVENT
		      CALLBACK_VALUE
		      CALLBACK_LENGTH
		      CALLBACK_MASK
		      CALLBACK_MASK_LENGTH
		      CALLBACK_DIR
		      CALLBACK_DIR_LENGTH
		      CALLBACK_PATTERN
		      CALLBACK_PATTERN_LENGTH
		      )
		  '(			;for Motif 1.0
		    CALLBACK_WIDGET
		    CALLBACK_REASON
		    CALLBACK_XEVENT
		    CALLBACK_VALUE
		    CALLBACK_LENGTH
		    CALLBACK_MASK
		    CALLBACK_MASK_LENGTH
		    )
		  )
		'(
		  (format T "-------------------------------------------\n")
		  (format T "widget=~A, reason=~A, xevent=~A\n" 
			  callback_widget callback_reason callback_xevent)
		  (format T "CALLBACK_VALUE=~A\n"
			  (xm_string_get_l_to_r CALLBACK_VALUE))
		  (format T "CALLBACK_LENGTH=~A\n"
			  CALLBACK_LENGTH)
		  (format T "CALLBACK_MASK=~A\n"
			  (xm_string_get_l_to_r CALLBACK_MASK))
		  (format T "CALLBACK_MASK_LENGTH=~A\n"
			  CALLBACK_MASK_LENGTH)
		  (cond ((and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
			 (format T "CALLBACK_DIR=~A\n"
				 (xm_string_get_l_to_r CALLBACK_DIR))
			 (format T "CALLBACK_DIR_LENGTH=~A\n"
				 CALLBACK_DIR_LENGTH)
			 (format T "CALLBACK_PATTERN=~A\n"
				 (xm_string_get_l_to_r CALLBACK_PATTERN))
			 (format T "CALLBACK_PATTERN_LENGTH=~A\n"
				 CALLBACK_PATTERN_LENGTH)
			 (let* ((items_array (send fsb_w :GET_DIR_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "Dir-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 (let* ((items_array  (send fsb_w :GET_FILE_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "File-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 ))
		  )
		))

(setq cb2 (send fsb_w :set_callback :XMN_OK_CALLBACK
		(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
		    '(			;for Motif 1.1
		      CALLBACK_WIDGET
		      CALLBACK_REASON
		      CALLBACK_XEVENT
		      CALLBACK_VALUE
		      CALLBACK_LENGTH
		      CALLBACK_MASK
		      CALLBACK_MASK_LENGTH
		      CALLBACK_DIR
		      CALLBACK_DIR_LENGTH
		      CALLBACK_PATTERN
		      CALLBACK_PATTERN_LENGTH
		      )
		  '(			;for Motif 1.0
		    CALLBACK_WIDGET
		    CALLBACK_REASON
		    CALLBACK_XEVENT
		    CALLBACK_VALUE
		    CALLBACK_LENGTH
		    CALLBACK_MASK
		    CALLBACK_MASK_LENGTH
		    )
		  )
		'(
		  (format T "-------------------------------------------\n")
		  (format T "widget=~A, reason=~A, xevent=~A\n" 
			  callback_widget callback_reason callback_xevent)
		  (format T "CALLBACK_VALUE=~A\n"
			  (xm_string_get_l_to_r CALLBACK_VALUE))
		  (format T "CALLBACK_LENGTH=~A\n"
			  CALLBACK_LENGTH)
		  (format T "CALLBACK_MASK=~A\n"
			  (xm_string_get_l_to_r CALLBACK_MASK))
		  (format T "CALLBACK_MASK_LENGTH=~A\n"
			  CALLBACK_MASK_LENGTH)
		  (cond ((and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
			 (format T "CALLBACK_DIR=~A\n"
				 (xm_string_get_l_to_r CALLBACK_DIR))
			 (format T "CALLBACK_DIR_LENGTH=~A\n"
				 CALLBACK_DIR_LENGTH)
			 (format T "CALLBACK_PATTERN=~A\n"
				 (xm_string_get_l_to_r CALLBACK_PATTERN))
			 (format T "CALLBACK_PATTERN_LENGTH=~A\n"
				 CALLBACK_PATTERN_LENGTH)
			 (let* ((items_array (send fsb_w :GET_DIR_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "Dir-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 (let* ((items_array  (send fsb_w :GET_FILE_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "File-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 ))
		  )
		))

(setq cb3 (send fsb_w :set_callback :XMN_CANCEL_CALLBACK
		(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
		    '(			;for Motif 1.1
		      CALLBACK_WIDGET
		      CALLBACK_REASON
		      CALLBACK_XEVENT
		      CALLBACK_VALUE
		      CALLBACK_LENGTH
		      CALLBACK_MASK
		      CALLBACK_MASK_LENGTH
		      CALLBACK_DIR
		      CALLBACK_DIR_LENGTH
		      CALLBACK_PATTERN
		      CALLBACK_PATTERN_LENGTH
		      )
		  '(			;for Motif 1.0
		    CALLBACK_WIDGET
		    CALLBACK_REASON
		    CALLBACK_XEVENT
		    CALLBACK_VALUE
		    CALLBACK_LENGTH
		    CALLBACK_MASK
		    CALLBACK_MASK_LENGTH
		    )
		  )
		'(
		  (format T "-------------------------------------------\n")
		  (format T "widget=~A, reason=~A, xevent=~A\n" 
			  callback_widget callback_reason callback_xevent)
		  (format T "CALLBACK_VALUE=~A\n"
			  (xm_string_get_l_to_r CALLBACK_VALUE))
		  (format T "CALLBACK_LENGTH=~A\n"
			  CALLBACK_LENGTH)
		  (format T "CALLBACK_MASK=~A\n"
			  (xm_string_get_l_to_r CALLBACK_MASK))
		  (format T "CALLBACK_MASK_LENGTH=~A\n"
			  CALLBACK_MASK_LENGTH)
		  (cond ((and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
			 (format T "CALLBACK_DIR=~A\n"
				 (xm_string_get_l_to_r CALLBACK_DIR))
			 (format T "CALLBACK_DIR_LENGTH=~A\n"
				 CALLBACK_DIR_LENGTH)
			 (format T "CALLBACK_PATTERN=~A\n"
				 (xm_string_get_l_to_r CALLBACK_PATTERN))
			 (format T "CALLBACK_PATTERN_LENGTH=~A\n"
				 CALLBACK_PATTERN_LENGTH)
			 (let* ((items_array (send fsb_w :GET_DIR_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "Dir-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 (let* ((items_array  (send fsb_w :GET_FILE_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "File-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 ))
		  )
		))

(setq cb4 (send fsb_w :set_callback :XMN_NO_MATCH_CALLBACK
		(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
		    '(			;for Motif 1.1
		      CALLBACK_WIDGET
		      CALLBACK_REASON
		      CALLBACK_XEVENT
		      CALLBACK_VALUE
		      CALLBACK_LENGTH
		      CALLBACK_MASK
		      CALLBACK_MASK_LENGTH
		      CALLBACK_DIR
		      CALLBACK_DIR_LENGTH
		      CALLBACK_PATTERN
		      CALLBACK_PATTERN_LENGTH
		      )
		  '(			;for Motif 1.0
		    CALLBACK_WIDGET
		    CALLBACK_REASON
		    CALLBACK_XEVENT
		    CALLBACK_VALUE
		    CALLBACK_LENGTH
		    CALLBACK_MASK
		    CALLBACK_MASK_LENGTH
		    CALLBACK_DIR
		    CALLBACK_DIR_LENGTH
		    CALLBACK_PATTERN
		    CALLBACK_PATTERN_LENGTH
		    )
		  )
		'(
		  (format T "-------------------------------------------\n")
		  (format T "widget=~A, reason=~A, xevent=~A\n" 
			  callback_widget callback_reason callback_xevent)
		  (format T "CALLBACK_VALUE=~A\n"
			  (xm_string_get_l_to_r CALLBACK_VALUE))
		  (format T "CALLBACK_LENGTH=~A\n"
			  CALLBACK_LENGTH)
		  (format T "CALLBACK_MASK=~A\n"
			  (xm_string_get_l_to_r CALLBACK_MASK))
		  (format T "CALLBACK_MASK_LENGTH=~A\n"
			  CALLBACK_MASK_LENGTH)
		  (cond ((and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
			 (format T "CALLBACK_DIR=~A\n"
				 (xm_string_get_l_to_r CALLBACK_DIR))
			 (format T "CALLBACK_DIR_LENGTH=~A\n"
				 CALLBACK_DIR_LENGTH)
			 (format T "CALLBACK_PATTERN=~A\n"
				 (xm_string_get_l_to_r CALLBACK_PATTERN))
			 (format T "CALLBACK_PATTERN_LENGTH=~A\n"
				 CALLBACK_PATTERN_LENGTH)
			 (let* ((items_array (send fsb_w :GET_DIR_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "Dir-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 (let* ((items_array  (send fsb_w :GET_FILE_LIST_ITEMS))
				(items_length (length items_array))
				)
			   (format T "File-List Items:\n")
			   (do ((i 0 (1+ i)))
			       ((= i items_length))
			       (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
			   )
			 ))

		  )
		))

(send top_w :realize)

(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
    (let* ((items_array (send fsb_w :GET_DIR_LIST_ITEMS))
	   (items_length (length items_array))
	   )
      (do ((i 0 (1+ i)))
	  ((= i items_length))
	  (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
      )
  )

(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
    (let* ((items_array  (send fsb_w :GET_FILE_LIST_ITEMS))
	   (items_length (length items_array))
	   )
      (do ((i 0 (1+ i)))
	  ((= i items_length))
	  (format T "\t~A\n" (xm_string_get_l_to_r (aref items_array i))))
      )
  )

)


(send fsb_w :do_search "/tmp/*")

(format T ":DIALOG_FILTER_LABEL==~A\n"
	(send fsb_w :get_child :DIALOG_FILTER_LABEL))
(format T ":DIALOG_FILTER_TEXT==~A\n"
	(send fsb_w :get_child :DIALOG_FILTER_TEXT))
(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
    (format T ":DIALOG_DIR_LIST==~A\n"
	    (send fsb_w :get_child :DIALOG_DIR_LIST)))
(if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1))
    (format T ":DIALOG_DIR_LIST_LABEL==~A\n"
	    (send fsb_w :get_child :DIALOG_DIR_LIST_LABEL)))
(format T ":DIALOG_LIST==~A\n"
	(send fsb_w :get_child :DIALOG_LIST))
(format T ":DIALOG_LIST_LABEL==~A\n"
	(send fsb_w :get_child :DIALOG_LIST_LABEL))
(format T ":DIALOG_SELECTION_LABEL==~A\n"
	(send fsb_w :get_child :DIALOG_SELECTION_LABEL))
(format T ":DIALOG_WORK_AREA==~A\n"
	(send fsb_w :get_child :DIALOG_WORK_AREA))
(format T ":DIALOG_TEXT==~A\n"
	(send fsb_w :get_child :DIALOG_TEXT))
(format T ":DIALOG_SEPARATOR==~A\n"
	(send fsb_w :get_child :DIALOG_SEPARATOR))
(format T ":DIALOG_OK_BUTTON==~A\n"
	(send fsb_w :get_child :DIALOG_OK_BUTTON))
(format T ":DIALOG_APPLY_BUTTON==~A\n"
	(send fsb_w :get_child :DIALOG_APPLY_BUTTON))
(format T ":DIALOG_CANCEL_BUTTON==~A\n"
	(send fsb_w :get_child :DIALOG_CANCEL_BUTTON))
(format T ":DIALOG_HELP_BUTTON==~A\n"
	(send fsb_w :get_child :DIALOG_HELP_BUTTON))
(format T ":DIALOG_DEFAULT_BUTTON==~A\n"
	(send fsb_w :get_child :DIALOG_DEFAULT_BUTTON))

