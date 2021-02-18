; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         example.lsp
; RCS:          $Header: $
; Description:  Another OOP example
; Author:       ???
; Created:      Sat Oct  5 20:49:30 1991
; Modified:     Sat Oct  5 20:49:49 1991 (Niels Mayer) mayer@hplnpm
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

; Make the class ship and its instance variables be known

(setq ship (Class :new '(x y xv yv m name captain registry)))


(ship :answer :getx		'() '( x ))	; just evaluate x
(ship :answer :getxv		'() '( xv ))	; note that the method is a
(ship :answer :gety		'() '( y ))	; list of forms, the value
(ship :answer :getyv		'() '( yv ))	; of the last one being the
(ship :answer :getm		'() '( m ))	; value of the method
(ship :answer :getname		'() '( name ))
(ship :answer :getcaptain	'() '( captain ))
(ship :answer :getregistry	'() '( registry ))

;			   formal
;			   param
;			   of
;			   method
(ship :answer :setx  	   '(to) '( (setq x to) ) )
(ship :answer :setxv 	   '(to) '( (setq xv to) ) )
(ship :answer :sety  	   '(to) '( (setq y to) ) )
(ship :answer :setyv	   '(to) '( (setq yv to) ) )
(ship :answer :setm	   '(to) '( (setq m to) ) )
(ship :answer :setname     '(to) '( (setq name to) ) )
(ship :answer :setcaptain  '(to) '( (setq captain to) ) )
(ship :answer :setregistry '(to) '( (setq registry to) ) )

(ship :answer :sail '(time) 
	; the METHOD for sailing
	'( (princ (list "sailing for " time " hours\n"))
	   ; note that this form is expressed in terms of objects:  "self"
	   ; is bound to the object being talked to during the execution
	   ; of its message.  It can ask itself to do things.
	   (self :setx (+  (self :getx)
			   (* (self :getxv) time)))
	   ; This form performs a parallel action to the above, but more
	   ; efficiently, and in this instance, more clearly
	   (setq y (+ y (* yv time)))
	   ; Cute message for return value.  Tee Hee.
	   "Sailing, sailing, over the bountiful chow mein..."))

; <OBJECT: #12345667> is not terribly instructive.  How about a more
; informative print routine?

(ship :answer :print '() '((princ (list
				"SHIP NAME: " (self :getname) "\n"
				"REGISTRY: " (self :getregistry) "\n"
				"CAPTAIN IS: " (self :getcaptain) "\n"
				"MASS IS: " (self :getm) " TONNES\n"
				"CURRENT POSITION IS: " 
					(self :getx)	" X BY "
					(self :gety)	" Y\n"
				"SPEED IS: "
					(self :getxv)	" XV BY "
					(self :getyv)	" YV\n") ) ))

; a function to make life easier

(defun newship (mass name registry captain &aux new)
	(setq new (ship :new))
	(new :setx 0)
	(new :sety 0)
	(new :setxv 0)
	(new :setyv 0)
	(new :setm mass)
	(new :setname name)
	(new :setcaptain captain)
	(new :setregistry registry)
	(new :print)
	new)

; and an example object.

(setq Bounty (newship 50 'Bounty 'England 'Bligh))
