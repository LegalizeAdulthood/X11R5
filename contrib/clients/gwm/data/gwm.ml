; Macros emacs de gestion souris pour BWM
; (emulateur de Peter Collinson)

; la banniere des windows:
; ========================
(setq default-mode-line-format
"%[[EXIT]%][DOWN][ UP ] %b%* (%f) %M (%m) %p")

(declare-global mode-line-sep-1	      
		mode-line-sep-2
		mode-line-sep-3
		x y
		total-height)

(setq total-height (window-height))
(setq mode-line-sep-1 6)
(setq mode-line-sep-2 12)
(setq mode-line-sep-3 18)

; Interpretation de la position ^X7x;y;n
; -----------------------------

(defun (decode-mouse-click b
    (setq x 0) 
    (while (> 58 (setq b (get-tty-character)))
	   (setq x (+ (* 10 x) (- b 48))))
    (setq y 0)
    (while (> 58 (setq b (get-tty-character)))
	   (setq y (+ (* 10 y) (- b 48))))
    (setq x (+ x 1))
    (setq y (+ y 1))
    (if (= 49 (setq b (get-tty-character)))
	(left-mouse-click)
	(right-mouse-click))))    
	      	  
; LEFT CLICK 
; ==========

; le tri entre le click dans une window ou dans une banniere
; ----------------------------------------------------------

(defun (left-mouse-click 
     (if (error-occurred
         (move-dot-to-x-y x y))
      (left-mouse-clicked-in-mode-line)
      (left-mouse-clicked-in-text))))

; cette fonction est la semantique du click dans une fenetre
; ----------------------------------------------------------

(defun (left-mouse-clicked-in-text
  (if (< (current-column) x)
     (to-col x)
  )))

; et celle-ci du click dans une mode-line
; ---------------------------------------

(defun (left-mouse-clicked-in-mode-line buffer
  (save-window-excursion
    (move-dot-to-x-y x (- y 1))
    (setq buffer (current-buffer-name)))
  (pop-to-buffer buffer)
  (if (<= x mode-line-sep-1)
      (if (>= (window-height) total-height)   ;  on est seul sur l'ecran
          (exit-window)
	  (delete-window))
      (if (<= x mode-line-sep-2)
          (next-page)
          (if (<= x mode-line-sep-3)
              (previous-page)
     	      (delete-other-windows)
	  )))))

(defun (exit-window
   (decremente-mode-line-seps)
   (exit-emacs)))

(defun (incremente-mode-line-seps
  (setq mode-line-sep-1 (+ mode-line-sep-1 2))
  (setq mode-line-sep-2 (+ mode-line-sep-2 2))
  (setq mode-line-sep-3 (+ mode-line-sep-3 2))))

(defun (decremente-mode-line-seps
  (setq mode-line-sep-1 (- mode-line-sep-1 2))
  (setq mode-line-sep-2 (- mode-line-sep-2 2))
  (setq mode-line-sep-3 (- mode-line-sep-3 2))))

; RIGHT BUTTON
; ============

; le tri entre le click dans une window ou dans une banniere
; ----------------------------------------------------------

(defun (right-mouse-click
     (if (error-occurred
	 (set-mark)
         (move-dot-to-x-y x y))
      (left-mouse-clicked-in-mode-line)
      (right-mouse-clicked-in-text))))


(defun (right-mouse-clicked-in-text
  (if (< (current-column) x)
     (to-col x))
  (sit-for 10)
  (exchange-dot-and-mark)))

; Les codes que doit envoyer la souris
; ====================================
; (via le multiplexeur clavier-souris)

(bind-to-key "decode-mouse-click" "\^X7")

; pour enlever les blancs en fin de ligne
; =======================================
; (qui ont pu etre rajoutes par des pointages souris)

(defun (clean-white-spaces-at-eol
  (save-excursion
    (beginning-of-file)
    (while (! (eobp))
      (end-of-line)
      (delete-white-space)
      (next-line)))))

; pour popper plein de fenetres
; =============================

(setq pop-up-windows 1)
(setq split-height-threshhold 1)

