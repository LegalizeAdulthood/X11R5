; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         scooter.lsp
; RCS:          $Header: scooter.lsp,v 1.3 91/10/05 18:51:17 mayer Exp $
; Description:  a silly example that scoots (moves) windows around the
;		screen while changing their colors. This can really tie up your
;		X server and window manager, so be careful...
; Author:       Niels Mayer, HPLabs
; Created:      Sat Oct  5 18:49:55 1991
; Modified:     Sat Oct  5 18:50:52 1991 (Niels Mayer) mayer@hplnpm
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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(setq top_w
      (send OVERRIDE_SHELL_WIDGET_CLASS :new 
	    :xmn_geometry "+1+1"
	    ))
(setq pb_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed top_w
	    :XMN_LABEL_TYPE :pixmap
	    :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/xlogo64"
	    ))
(send top_w :realize)

(defun randomove()
  (do
   ((i 0 (1+ i)))
   ((eq 100 i)
    )
   (send top_w :set_values
	 :xmn_x (random 1000)
	 :xmn_y (random 1000)
	 )
   )
  )
(defun scoot(d)
  (do
   ((i 0 (1+ i))
    (x 0 (+ d x))
    (y 0 (+ d y)))
   ((eq 100 i)
    )
   (send top_w :set_values
	 :xmn_x x
	 :xmn_y y
	 )
   )
  )


(scoot 1)
(scoot 5)
(scoot 10)


(defun colorize ()
  (do* 
   ((fp (open "/usr/lib/X11/rgb.txt" :direction :input))
    (color
     (fscanf-string fp "%*d %*d %*d %[^\n]\n")
     (fscanf-string fp "%*d %*d %*d %[^\n]\n"))
    (x 1 (1+ x))
    (y 1 (1+ y))
    )
   ((null color)
    (close fp)
    )

   (send top_w :set_values 
	 :xmn_x x
	 :xmn_y y
	 )
   (send pb_w :set_values
	 :xmn_background color)
;   (send pb_w :update_display)
   ))

(colorize)

