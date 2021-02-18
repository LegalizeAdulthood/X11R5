; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         bitmap-br2.lsp
; RCS:          $Header: bitmap-br2.lsp,v 1.2 91/10/05 05:18:00 mayer Exp $
; Description:  Example program -- Browses bitmaps in /usr/include/X11/bitmaps/*
;		click on a bitmap and it becomes the root background bitmap.
; Author:       Niels Mayer, HPLabs
; Created:      Wed Mar 14 21:13:36 1990
; Modified:     Sat Oct  5 05:16:20 1991 (Niels Mayer) mayer@hplnpm
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
;; create a toplevel widget that talks to the window manager.
;;
(setq toplevel_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "bbr2"
            :XMN_TITLE "Bitmap Browser Example"
            :XMN_ICON_NAME "Bitmap-Br"
            ))
;;
;; inside the toplevel_w create a scrolled window widget to allow viewing
;; of a window larger than the toplevel window by panning around with
;; scrollbars.
;;
(setq scrl_w
      (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
            "sc" toplevel_w
            :XMN_SCROLLING_POLICY :automatic))
;;
;; Inside the scrl_w, create a "manager" widget that lays out the entries
;; in the bitmap browser (children of scrl_w) in a vertical fashion.
;;
(setq rowcol_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
            "rc" scrl_w
            :XMN_ORIENTATION     :vertical
            :XMN_PACKING         :pack_tight
            :XMN_ENTRY_ALIGNMENT :alignment_center
            ))
;;
;; Add a callback that sends the message :xsetroot to the child widget
;; of the rowcolumn widget that was activated via mouse click.
;;
(send rowcol_w :set_callback :XMN_ENTRY_CALLBACK
       '(CALLBACK_ENTRY_WIDGET) ;local variable bound to the
                                ;callback-causing widget
       '(
         (send CALLBACK_ENTRY_WIDGET :xsetroot)
         ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; make a trivial subclass of XM_PUSH_BUTTON_GADGET_CLASS
;;;
(setq Niels_Pixmap_Push_Button_Class    ;name of the new subclass
      (send Class :new
            '(pixmap_file)              ;a new ivar for this subclass
            '()                         ;no class variables for subclass
            XM_PUSH_BUTTON_GADGET_CLASS ;name of the superclass
      )) 
;;;
;;; override XM_TOGGLE_BUTTON_GADGET_CLASS's instance initializer (method
;;; :isnew) such that the instance variable pixmap_file is initialized
;;; and such that the created pushbutton widget displays a pixmap.
;;;
(send Niels_Pixmap_Push_Button_Class :answer :isnew '(filename &rest args)
      '(
        (setq pixmap_file filename)
        (apply 'send-super `(:isnew ,@args
                                    :XMN_LABEL_TYPE :pixmap
                                    :XMN_LABEL_PIXMAP ,filename))
        ))
;;;
;;; add a method responding to message :xsetroot that calls the
;;; xsetroot(1) program to set background tile. the 'system' call
;;; is the unix system(3s) call, and the 'format' call is equivalent
;;; to the unix sprintf(3s) call.
;;;
(send Niels_Pixmap_Push_Button_Class :answer :xsetroot '()
      '(
        (system (format nil "xsetroot -bitmap ~A -fg Black -bg DimGrey" 
                        pixmap_file))
        ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; This loop creates a label,bitmap-pushbutton,separator triple for each
;; bitmap file read from the directory specified in the arg to popen(3s).
;;
;; Obvisouly, this loop should become a procedure with the name of the
;; bitmap directory passed in as parameter. However, since this is example
;; code to be read by people not conversant in WINTERP-Lisp, I am going to
;; hold off on introducing proceduralization...
(do* 
 (;; local do-loop variables with initialize and increment expressions.

  ;; Get a list of the bitmap files in matching the pattern
  ;; /usr/local/mayer/src/bitmaps/*.xbm
  ;; We use the unix popen(3s) routine to read the results of ls(1), which returns
  ;; to stdout a list of matching filenames in the shell created by popen(3s). 
  ;; popen(3s) returns a FILE* that can be read by the XLISP primitive 'read-line'
  (ls_reader_pipe
   (popen "/bin/ls /usr/include/X11/bitmaps/*" :direction :input))

  (file-name
   (read-line ls_reader_pipe) (read-line ls_reader_pipe))
  )

 (;; do-loop termination condition and termination code
  (null file-name)                      ;terminate when (read-line) ==> EOF
  (pclose ls_reader_pipe)               ;close the pipe
  (send toplevel_w :realize)            ;create the toplevel window and exit
  )

 ;; loop body
 (send XM_LABEL_GADGET_CLASS :new :managed
       "filename" rowcol_w
       :XMN_LABEL_TYPE :STRING
       :XMN_LABEL_STRING file-name
       )
 (send Niels_Pixmap_Push_Button_Class :new file-name :managed
       "image" rowcol_w
       )
 (send XM_SEPARATOR_GADGET_CLASS :new :managed
       "sep" rowcol_w
       :XMN_SEPARATOR_TYPE :DOUBLE_LINE
       )
 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; end of bitmap browser example -- note that in WINTERP you don't need to
;; mess with initializing the display nor calling XtMainLoop().
