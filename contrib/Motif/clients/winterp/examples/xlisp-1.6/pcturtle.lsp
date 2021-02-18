; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         pcturtle.lsp
; RCS:          $Header: $
; Description:  This is a sample XLISP program. It implements a simple form of
;		programmable turtle for IBM-PC compatible machines.
;		To run it:
;			A>xlisp pt
;		This should cause the screen to be cleared and two turtles to
;		appear. They should each execute their simple programs and then
;		the prompt should return.  Look at the code to see how all of this works.
; Author:       ???
; Created:      Sat Oct  5 20:57:21 1991
; Modified:     Sat Oct  5 20:58:38 1991 (Niels Mayer) mayer@hplnpm
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
; purpose.  It is provided "as is" without express or implied warranty.;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Get some more memory
(expand 1)

; Move the cursor to the currently set bottom position and clear the line
;  under it
(defun bottom ()
    (set-cursor by bx)
    (clear-eos))

; Clear the screen and go to the bottom
(defun cb ()
    (clear)
    (bottom))


; ::::::::::::
; :: Turtle ::
; ::::::::::::

; Define "Turtle" class
(setq Turtle (Class :new '(xpos ypos char)))

; Answer ":isnew" by initing a position and char and displaying.
(Turtle :answer :isnew '() '(
    (setq xpos (setq newx (+ newx 1)))
    (setq ypos 12)
    (setq char "*")
    (self :display)
    self))

; Message ":display" prints its char at its current position
(Turtle :answer :display '() '(
    (set-cursor ypos xpos)
    (princ char)
    (bottom)
    self))

; Message ":char" sets char to its arg and displays it
(Turtle :answer :char '(c) '(
    (setq char c)
    (self :display)))

; Message ":goto" goes to a new place after clearing old one
(Turtle :answer :goto '(x y) '(
    (set-cursor ypos xpos) (princ " ")
    (setq xpos x)
    (setq ypos y)
    (self :display)))

; Message ":up" moves up if not at top
(Turtle :answer :up '() '(
    (if (> ypos 1)
	(self :goto xpos (- ypos 1))
	(bottom))))

; Message ":down" moves down if not at bottom
(Turtle :answer :down '() '(
    (if (< ypos by)
	(self :goto xpos (+ ypos 1))
	(bottom))))

; Message ":right" moves right if not at right
(Turtle :answer :right '() '(
    (if (< xpos 80)
	(self :goto (+ xpos 1) ypos)
	(bottom))))

; Message ":left" moves left if not at left
(Turtle :answer :left '() '(
    (if (> xpos 1)
	(self :goto (- xpos 1) ypos)
	(bottom))))


; :::::::::::::
; :: PTurtle ::
; :::::::::::::

; Define "DPurtle" programable turtle class
(setq PTurtle (Class :new '(prog pc) '() Turtle))

; Message ":program" stores a program
(PTurtle :answer :program '(p) '(
    (setq prog p)
    (setq pc prog)
    self))

; Message ":step" executes a single program step
(PTurtle :answer :step '() '(
    (if (null pc)
	(setq pc prog))
    (if pc
	(progn (self (car pc))
	       (setq pc (cdr pc))))
    self))

; Message ":step#" steps each turtle program n times
(PTurtle :answer :step# '(n) '(
    (dotimes (x n) (self :step))
    self))


; ::::::::::::::
; :: PTurtles ::
; ::::::::::::::

; Define "PTurtles" class
(setq PTurtles (Class :new '(turtles)))

; Message ":make" makes a programable turtle and adds it to the collection
(PTurtles :answer :make '(x y &aux newturtle) '(
    (setq newturtle (PTurtle :new))
    (newturtle :goto x y)
    (setq turtles (cons newturtle turtles))
    newturtle))

; Message ":step" steps each turtle program once
(PTurtles :answer :step '() '(
    (mapcar '(lambda (turtle) (turtle :step)) turtles)
    self))

; Message ":step#" steps each turtle program n times
(PTurtles :answer :step# '(n) '(
    (dotimes (x n) (self :step))
    self))


; Initialize things and start up
(setq bx 1)
(setq by 21)
(setq newx 1)

; Create some programmable turtles
(cb)
(setq turtles (PTurtles :new))
(setq t1 (turtles :make 40 10))
(setq t2 (turtles :make 41 10))
(t1 :program '(:left :right :up :down))
(t2 :program '(:right :left :down :up))


