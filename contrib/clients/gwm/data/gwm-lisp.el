;;;file: gwm-lisp.el
;;;============================================================
;;;
;;; GWM Lisp Mode
;;;
;;; This package provides a GWM Lisp major mode in Epoch.
;;;
;;; by Barry Kaplan
;;;    kaplanb@ajpo.sei.cmu.edu
;;;============================================================

(defvar gwm::lisp-mode-map ())
(if gwm::lisp-mode-map
  ()
  (setq gwm::lisp-mode-map (make-sparse-keymap))
  (lisp-mode-commands gwm::lisp-mode-map)
)

(defun gwm::lisp-mode ()
"Entry to this mode calls the value of gwm-lisp-mode-hook if that 
value is non-nil."
  (interactive)
  (kill-all-local-variables)
  (use-local-map gwm::lisp-mode-map)
  (setq major-mode 'gwm::lisp-mode)
  (setq mode-name "GWM-Lisp")
  (lisp-mode-variables t)
  (run-hooks 'gwm::lisp-mode-hook)
)


(defun gwm::eval-defun ()
  (interactive)
  (save-excursion
    (end-of-defun)
    (let ((end (point)))
      (beginning-of-defun)
      (epoch::set-property "GWM_EXECUTE" 
	(buffer-substring (point) end))
)))

(defun gwm::eval-region () ; should be independent of Epoch drag button...
  (interactive)
  (epoch::set-property "GWM_EXECUTE"
    (buffer-substring 
      (epoch::button-start drag-button)
      (epoch::button-end drag-button)
)))

(setq gwm::eval-expression 'eval-expression) ;; Not implemented yet.

(defun gwm::eval-current-buffer ()
  (interactive)
  (epoch::set-property "GWM_EXECUTE" (buffer-string))
)
	

;;;
;;; Put the gwm file extension on the auto-mode-list.
;;;
(setq auto-mode-alist
  (append
    '(("\\.gwm$" . gwm::lisp-mode))
    auto-mode-alist
))


;;;------------------------------------------------------------
(provide 'gwm-lisp)

;;;  ---EOF---




I also redefine some of the standard emacs key definitions in
the \e map. For me the actual keys the functions are defined to
is abitrary since I use them only to define function keys later
on. The following are fragmants from my .emacs.

;;;file fragment: .emacs

(require 'keybind-functions)
(require 'gwm-lisp         )
(require 'amc-lisp         )

;;;----
;;; Lisp keys (non-standard, used by function keys below)
;;;----
(define-key lisp-mode-map             "\e\C-x" 'eval-defun     )
(define-key emacs-lisp-mode-map       "\e\C-x" 'eval-defun     )
(define-key lisp-interaction-mode-map "\e\C-x" 'eval-defun     )
(define-key gwm::lisp-mode-map        "\e\C-x" 'gwm::eval-defun)

(define-key lisp-mode-map             "\e\e" 'eval-expression     )
(define-key emacs-lisp-mode-map       "\e\e" 'eval-expression     )
(define-key lisp-interaction-mode-map "\e\e" 'eval-expression     )
(define-key gwm::lisp-mode-map        "\e\e" 'gwm::eval-expression)

(define-key lisp-mode-map             "\e\C-@" 'eval-region     )
(define-key emacs-lisp-mode-map       "\e\C-@" 'eval-region     )
(define-key lisp-interaction-mode-map "\e\C-@" 'eval-region     )
(define-key gwm::lisp-mode-map        "\e\C-@" 'gwm::eval-region)

(define-key lisp-mode-map             "\e\C-h" 'eval-current-buffer     )
(define-key emacs-lisp-mode-map       "\e\C-h" 'eval-current-buffer     )
(define-key lisp-interaction-mode-map "\e\C-h" 'eval-current-buffer     )
(define-key gwm::lisp-mode-map        "\e\C-h" 'gwm::eval-current-buffer)


(rebind-key-0-l    "F5"  "\e\C-x")                      ;     F5 = eval-defun
(rebind-key-s-l    "F5"  "\e\C-@")                      ;   s-F5 = eval-region
(rebind-key-c-l    "F5"  "\e\e")                        ;   c-F5 = eval-expression
(rebind-key-s-c-l  "F5"  "\e\C-h")                      ; s-c-F5 = eval-current-buffer
(rebind-key-0-l    "F6"  "\M-xdescribe-function\C-m")   ;     F6 = describe-function
(rebind-key-s-l    "F6"  "\M-xdescribe-variable\C-m")   ;   s-F6 = describe-variable
(rebind-key-0-l    "F7"  "\M-xdescribe-key\C-m")        ;     F6 = describe-key
(rebind-key-s-l    "F7"  "\M-xhelp-with-mouse\C-m")     ;   s-F6 = describe-mouse

;;;  ---EOF---




The rebind-key-* functions above are defined shown below. They are
only convenience routines to make it easy to bind function keys
whether the caps lock is on or off. If you of a better to accomplish
this please let me know.

;;; file fragment: keybind-functions.el

(setq epoch::function-key-mapping nil)

(defun rebind-key-all (keyname keystring)
  (interactive)
  (rebind-key keyname 0                            keystring)
  (rebind-key keyname 'lock                        keystring)
  (rebind-key keyname 'shift                       keystring)
  (rebind-key keyname 'control                     keystring)
  (rebind-key keyname (list 'control 'shift)       keystring)
  (rebind-key keyname (list 'lock 'shift)          keystring)
  (rebind-key keyname (list 'lock 'control)        keystring)
  (rebind-key keyname (list 'lock 'control 'shift) keystring)
  )

(defun rebind-key-0-l (keyname keystring)
  (interactive)
  (rebind-key keyname 0     keystring)
  (rebind-key keyname 'lock keystring)
  )

(defun rebind-key-s-l (keyname keystring)
  (interactive)
  (rebind-key keyname 'shift              keystring)
  (rebind-key keyname (list 'shift 'lock) keystring)
  )

(defun rebind-key-c-l (keyname keystring)
  (interactive)
  (rebind-key keyname 'control              keystring)
  (rebind-key keyname (list 'control 'lock) keystring)
  )

(defun rebind-key-s-c-l (keyname keystring)
  (interactive)
  (rebind-key keyname (list 'shift 'control)       keystring)
  (rebind-key keyname (list 'shift 'control 'lock) keystring)
  )

;;;------------------------------------------------------------
(provide 'keybind-functions)

;;; ---EOF---


Also, if you are using amc-lisp.el, add the following to the
end of amc-lisp.el.

;;;file fragment: amc-lisp.el

(defun amc-gwm-lisp-hook () ;; This may not be necessary since gwm::lisp-mode-map
			    ;; is derived from the standard lisp-mode-map (I think).
  (define-key gwm::lisp-mode-map "(" 'electric-lisp-paren)
  (define-key gwm::lisp-mode-map ")" 'electric-lisp-paren)
)

(setq gwm::lisp-mode-hook 'amc-gwm-lisp-hook)

;;;  ---EOF---

