; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         trace.lsp
; RCS:          $Header: $
; Description:  trace functions for xlisp 1.6
; Author:       ???
; Created:      Sat Oct  5 21:03:47 1991
; Modified:     Sat Oct  5 21:04:14 1991 (Niels Mayer) mayer@hplnpm
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

(setq *tracelist* nil)

(defun evalhookfcn (expr &aux val)
       (if (and (consp expr) (member (car expr) *tracelist*))
           (progn (princ ">>> ") (print expr)
                  (setq val (evalhook expr evalhookfcn nil))
                  (princ "<<< ") (print val))
           (evalhook expr evalhookfcn nil)))

(defun trace (fun)
       (if (not (member fun *tracelist*))
	   (progn (setq *tracelist* (cons fun *tracelist*))
                  (setq *evalhook* evalhookfcn)))
       *tracelist*)

(defun untrace (fun)
       (if (null (setq *tracelist* (delete fun *tracelist*)))
           (setq *evalhook* nil))
       *tracelist*)
