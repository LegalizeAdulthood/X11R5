; -*-Emacs-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         winterp.el
; RCS:          $Header: winterp.el,v 1.2 89/12/15 17:48:27 mayer Exp $
; Description:  GNUEMACS lisp-mode interface to WINTERP.
; Author:       Niels Mayer, HPLabs
; Created:      Tue Nov 14 23:14:54 1989
; Modified:     Sat Oct  5 22:44:53 1991 (Niels Mayer) mayer@hplnpm
; Language:     Emacs-Lisp
; Package:      N/A
; Status:       X11r5 contrib tape release
;
; WINTERP Copyright 1989-1991 Hewlett-Packard Company (by Niels Mayer).
; XLISP version 2.1, Copyright (c) 1989, by David Betz.
;
; Permission to use, copy, modify, distribute, and sell this software and its
; documentation for any purpose is hereby granted without fee, provided that
; the above copyright notice appear in all copies and that both that
; copyright notice and this permission notice appear in supporting
; documentation, and that the name of Hewlett-Packard and David Betz not be
; used in advertising or publicity pertaining to distribution of the software
; without specific, written prior permission.  Hewlett-Packard and David Betz
; make no representations about the suitability of this software for any
; purpose. It is provided "as is" without express or implied warranty.
;
; HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
; SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
; IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
; INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
; LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
; OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
; PERFORMANCE OF THIS SOFTWARE.
;
; See ./winterp/COPYRIGHT for information on contacting the authors.
; 
; Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
; Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Make sure shell.el is loaded, since this file uses stuff defined there.
;; None of this will work unless you load shell.el from emacs
;; versions >= 18.54. I also expect that lisp-mode has been preloaded into
;; gnuemacs (it is automatically loaded by default in "normal" gnuemacsen)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(require 'shell)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defvar winterp-client-shell "sh" 
  "A shell in which to run the winterp-server client 'wl' any shell will do
e.g. sh, csh, ksh....")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defvar winterp-client-program "wl"
  "The name of the client program that sends lisp expressions to the
winterp lisp server. You may want to change this if you use a different
client program, or if you  don't have 'wl' on your search path, you
may want to specify a full path to 'wl'.") 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defvar winterp-client-program-args ""
  "This sets the arguments sent to 'wl'. You may want to set this to
\"-h <hostname> -p <portnum>\" if you are running  winterp on a different
host than gnuemacs, or if you want to run winterp on a TCP port other than
the default, which is 23751.")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun winterp-send-defun ()
  "A version of lisp-send-defun that talks to WINTERP's lisp server via the
winterp client program 'wl' (which must be on your search path)."
  (interactive)
  (if (not (get-process "winterp-client-shell"))
      (make-shell "winterp-client-shell" winterp-client-shell)
    )
  (let ((loadfile (format "/tmp/wl%d.lsp"
			  (process-id (get-process "winterp-client-shell")))))
    (save-excursion (write-region
		     (progn (beginning-of-defun) (point))
		     (progn (end-of-defun) (point))
		     loadfile nil 'nomessage))
    (process-send-string "winterp-client-shell"
			 (format "%s %s '(load \"%s\" :verbose nil :print t)'\n"
				 winterp-client-program winterp-client-program-args
				 loadfile))
    )
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun winterp-send-exit ()
  "Calling this function will send WINTERP a command to exit the current
breaklevel. If not at a breaklevel, then WINTERP will exit."
  (interactive)
  (if (not (get-process "winterp-client-shell"))
      (make-shell "winterp-client-shell" winterp-client-shell)
    )
  ;;sending wl with no args sends an EOF to WINTERP which signals XLISP to
  ;;exit the current breaklevel, or exit if not in the breakloop.
  (process-send-string "winterp-client-shell" 
		       (format "%s %s\n"
			       winterp-client-program winterp-client-program-args))
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun winterp-send-buffer ()
  "A version of winterp-send-defun that sends the entire buffer off to WINTERP
for evaluation. This routine talks to WINTERP's lisp server via the winterp client
program 'wl' (which must be on your search path)."
  (interactive)
  (if (not (get-process "winterp-client-shell"))
      (make-shell "winterp-client-shell" winterp-client-shell)
    )
  (let ((loadfile (format "/tmp/wl%d.lsp"
			  (process-id (get-process "winterp-client-shell")))))
    (save-excursion (write-region (point-min) (point-max)
		     loadfile nil 'nomessage))
    (process-send-string "winterp-client-shell"
			 (format "%s %s '(load \"%s\" :verbose nil :print t)'\n"
				 winterp-client-program winterp-client-program-args
				 loadfile))
    )
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; rebind some keys in lisp-mode-map (assumed preloaded in gnuemacs).
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define-key lisp-mode-map "\C-c\C-d" 'winterp-send-exit)
(define-key lisp-mode-map "\e\C-x" 'winterp-send-defun)
