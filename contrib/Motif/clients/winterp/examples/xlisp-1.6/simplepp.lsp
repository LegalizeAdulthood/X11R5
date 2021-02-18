; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         simplepp.lsp
; RCS:          $Header: $
; Description:  a pretty-printer, with hooks for the editor
; Author:       ???
; Created:      Sat Oct  5 21:03:25 1991
; Modified:     Sat Oct  5 21:03:40 1991 (Niels Mayer) mayer@hplnpm
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

; First, the terminal width and things to manipulate it
(setq pp$terminal-width 79)

(defmacro get-terminal-width nil
  pp$terminal_width)

(defmacro set-terminal-width (new-width)
  (let ((old-width pp$terminal-width))
    (setq pp$terminal-width new-width)
    old-width))
;
; Now, a basic, simple pretty-printer
; pp$pp prints expression, indented to indent-level, assuming that things
; have already been indented to indent-so-far. It *NEVER* leaves the cursor
; on a new line after printing expression. This is to make the recursion
; simpler. This may change in the future, in which case pp$pp could vanish.
;
(defun pp$pp (expression indent-level indent-so-far)
; Step one, make sure we've indented to indent-level
  (dotimes (x (- indent-level indent-so-far)) (princ " "))
; Step two, if it's an atom or it fits just print it
  (cond ((or (not (consp expression))
	     (> (- pp$terminal-width indent-level) (flatsize expression)))
	 (prin1 expression))
; else, print open paren, the car, then each sub expression, then close paren
	(t (princ "(")
	   (pp$pp (car expression) (1+ indent-level) (1+ indent-level))
	   (if (cadr expression)
	       (progn
		 (if (or (consp (car expression))
			 (> (/ (flatsize (car expression)) 3)
			    pp$terminal-width))
		     (progn (terpri)
			    (pp$pp (cadr expression) 
				   (1+ indent-level)
				   0))
		     (pp$pp (cadr expression)
			    (+ 2 indent-level (flatsize (car expression)))
			    (+ 1 indent-level (flatsize (car expression)))))
		 (dolist (current-expression (cddr expression))
			 (terpri)
			 (pp$pp current-expression
				(+ 2 indent-level 
				   (flatsize (car expression)))
				0))))
	   (princ ")")))
  nil)
;
; Now, the thing that outside users should call
; We have to have an interface layer to get the final terpri after pp$pp.
; This also allows hiding the second and third args to pp$pp. Said args
; being required makes the pp recursion loop run faster (don't have to map
; nil's to 0).
;	The where arg to pp is ingnored, as the obvious hack to pp$pp [adding
; an extra arg to every call to a print routine or pp$pp] doesn't work,
; printing nothing when where is nil.
;
(defun pp (expression &optional where)
"Print EXPRESSION on STREAM, prettily"
  (pp$pp expression 0 0)
  (terpri))
