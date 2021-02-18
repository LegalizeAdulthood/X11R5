; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         init.lsp
; RCS:          $Header: $
; Description:  initialization file for XLISP 1.6
; Author:       David Betz
; Created:      Sat Oct  5 20:55:28 1991
; Modified:     Sat Oct  5 20:55:53 1991 (Niels Mayer) mayer@hplnpm
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

; get some more memory
(expand 1)

; some fake definitions for Common Lisp pseudo compatiblity
(setq first  car)
(setq second cadr)
(setq rest   cdr)

; (when test code...) - execute code when test is true
(defmacro when (test &rest code)
          `(cond (,test ,@code)))

; (unless test code...) - execute code unless test is true
(defmacro unless (test &rest code)
          `(cond ((not ,test) ,@code)))

; (makunbound sym) - make a symbol be unbound
(defun makunbound (sym) (setq sym '*unbound*) sym)

; (objectp expr) - object predicate
(defun objectp (x) (eq (type-of x) :OBJECT))

; (filep expr) - file predicate
(defun filep (x) (eq (type-of x) :FILE))

; (unintern sym) - remove a symbol from the oblist
(defun unintern (sym) (cond ((member sym *oblist*)
                             (setq *oblist* (delete sym *oblist*))
                             t)
                            (t nil)))

; (mapcan fun list [ list ]...)
(defmacro mapcan (&rest args) `(apply #'nconc (mapcar ,@args)))

; (mapcon fun list [ list ]...)
(defmacro mapcon (&rest args) `(apply #'nconc (maplist ,@args)))

; (set-macro-character ch fun [ tflag ])
(defun set-macro-character (ch fun &optional tflag)
    (setf (aref *readtable* ch) (cons (if tflag :tmacro :nmacro) fun))
    t)

; (get-macro-character ch)
(defun get-macro-character (ch)
  (if (consp (aref *readtable* ch))
    (cdr (aref *readtable* ch))
    nil))

; (save fun) - save a function definition to a file
(defmacro save (fun)
         `(let* ((fname (strcat (symbol-name ',fun) ".lsp"))
                 (fval (car ,fun))
                 (fp (openo fname)))
                (cond (fp (print (cons (if (eq (car fval) 'lambda)
                                           'defun
                                           'defmacro)
                                       (cons ',fun (cdr fval))) fp)
                          (close fp)
                          fname)
                      (t nil))))

; (debug) - enable debug breaks
(defun debug ()
       (setq *breakenable* t))

; (nodebug) - disable debug breaks
(defun nodebug ()
       (setq *breakenable* nil))

; initialize to enable breaks but no trace back
(setq *breakenable* t)
(setq *tracenable* nil)
