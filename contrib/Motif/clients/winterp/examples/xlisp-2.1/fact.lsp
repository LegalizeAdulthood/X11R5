; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         fact.lsp
; RCS:          $Header: $
; Description:  Compute factorial
; Author:       ???
; Created:      Sat Oct  5 21:04:29 1991
; Modified:     Sat Oct  5 21:04:46 1991 (Niels Mayer) mayer@hplnpm
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

(defun fact (n)
       (cond ((= n 1) 1)
	     (t (* n (fact (- n 1))))))
