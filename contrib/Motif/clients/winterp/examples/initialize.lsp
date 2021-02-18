; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         .winterp
; RCS:          $Header: initialize.lsp,v 1.3 91/10/05 18:09:06 mayer Exp $
; Description:  WINTERP INITIALIZATION FILE
;		You should do the following:
;		(1) copy this file to <homedir>/.winterp
;		(2) set resource "Winterp.lispInitFile: <homedir>/.winterp"
;		(3) Customize sections commented out at the end of this file.
;			* Uncomment "LOAD" statements
;			* Add XLISP globals that one wants customized.
;			* Load any other personal macros, etc.
; Author:       Niels Mayer, HPLabs
; Created:      Mon Nov 20 18:13:23 1989
; Modified:     Sat Oct  5 18:08:19 1991 (Niels Mayer) mayer@hplnpm
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

; initialization file for XLISP 2.1 & WINTERP

; define some macros
(defmacro defvar (sym &optional val)
  `(if (boundp ',sym) ,sym (setq ,sym ,val)))
(defmacro defparameter (sym val)
  `(setq ,sym ,val))
(defmacro defconstant (sym val)
  `(setq ,sym ,val))

; (makunbound sym) - make a symbol value be unbound
(defun makunbound (sym) (setf (symbol-value sym) '*unbound*) sym)

; (fmakunbound sym) - make a symbol function be unbound
(defun fmakunbound (sym) (setf (symbol-function sym) '*unbound*) sym)

; (mapcan fun list [ list ]...)
(defmacro mapcan (&rest args) `(apply #'nconc (mapcar ,@args)))

; (mapcon fun list [ list ]...)
(defmacro mapcon (&rest args) `(apply #'nconc (maplist ,@args)))

; (set-macro-character ch fun [ tflag ])
(defun set-macro-character (ch fun &optional tflag)
    (setf (aref *readtable* (char-int ch))
          (cons (if tflag :tmacro :nmacro) fun))
    t)

; (get-macro-character ch)
(defun get-macro-character (ch)
  (if (consp (aref *readtable* (char-int ch)))
    (cdr (aref *readtable* (char-int ch)))
    nil))

; (savefun fun) - save a function definition to a file
(defmacro savefun (fun)
  `(let* ((fname (strcat (symbol-name ',fun) ".lsp"))
          (fval (get-lambda-expression (symbol-function ',fun)))
          (fp (open fname :direction :output)))
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

(setq *breakenable* nil)		; T allows entry into breakloop
(setq *tracenable* t)			; set this to T if you want to see a
					; backtrace on error.
(setq *gc-flag* t)			; we want to see garbage collection messages


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ***********   P E R S O N A L  C U S T O M I Z A T I O N S  ************ ;;
;;
;; Uncomment the following expressions in order to set up the appropriate
;; personal customization for WINTERP.
;;
;; Note that the load-filenames used below are correct if you set WINTERP
;; resource Winterp.lispLibDir: "<path>/winterp/examples/ 
;; (where <path> is the full path to wherever you've put the WINTERP dist.)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;
;;; set up editor used by "$EDITOR)" button in w_ctrlpnl.lsp, and grep-br.lsp
;;; If you don't set this then environment variable $EDITOR will be used.
;;;

; ;; For those using Andy Norman's emacs gnuserv/gnuclient/gnudoit package...
; (setq *SYSTEM-EDITOR* "gnuclient -q")
;
; ;; For those using emacs' standard emacsserver/emacsclient package...
; (setq *SYSTEM-EDITOR* "emacsclient") 
;
; ;; VI users will want to use this, even though it is slow, but hey, you use VI, so you must be used to braindamage by now :-)...
; (setq *SYSTEM-EDITOR* "xterm -e vi") 
;


;;;
;;; Load the WINTERP control panel -- provides control over the XLISP debugger,
;;; alongside a file-browser and an XmText editor. The XmText editor may be
;;; used to for entering Lisp expressions into WINTERP's XLISP evaluator --
;;; the expression underneath the cursor is evaluated by using the
;;; "Eval @ Point" button. Note that the editor and Lisp evaluator portions
;;; of w_ctrlpnl.lsp need alot more work. I use gnu-emacs myself...
;;; (also, you should set *SYSTEM-EDITOR* first...)
;;;

; (if (not (load "w_ctrlpnl"))		;load doesn't signal an error.
;     (error "Couldn't load w_ctrlpnl.lsp -- did you forget to set\nresource .lispLibDir or command-line argument -lib_dir\nto specify the path to the WINTERP examples directory??")
;   )


;;; 
;;; Load "prov-req" providing XLISP routines PROVIDE and REQUIRE, which
;;; behave much like their counterparts in Common Lisp. REQUIRE is a much
;;; nicer way to load files than LOAD because it allows you to specify a 
;;; load path. Note that "prov-req" only work for X11r4/Motif1.1
;;;
;;; Set global variable *default-load-path* to a string whose format is
;;; described by the Xtoolkit function XtResolvePathname().
;;; (%N gets substituted for the name REQUIRE'd, %S=.lsp)
;;;

; (setq *default-load-path*
;       (strcat
;        "/users/mayer/%N%S:"
;        "/users/mayer/%N:"
;        "/users/mayer/src/winterp/examples/%N%S:"
;        "/users/mayer/src/winterp/examples/%N:"
;        "/tmp/%N%S:"
;        "/tmp/%N"
;        ))
; (if (not (load "prov-req"))		;load doesn't signal an error.
;     (error "Couldn't load prov-req.lsp -- did you forget to set\nresource .lispLibDir or command-line argument -lib_dir\nto specify the path to the WINTERP examples directory??")
;   )


;;;
;;; Add a method on the widget metaclass WIDGET_CLASS. The method allow use of
;;; simpler notation for doing XtGetValues() for a single resource.
;;; (send <widget-class> :get :<resource-name>) ==> returns the resource value.
;;;

;(send WIDGET_CLASS :answer :get '(resource-name)
; '(
;   (car (send self :GET_VALUES resource-name NIL))
;   ))


;;;
;;; Set the grep program used by the Motif 1.1 version of grep-br.lsp
;;; gnugrep is highly recommended because it is many times faster than standard grep...
;;;

; (setq *GREP-BR-EXECUTABLE* "gnugrep")
