; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         art.lsp
; RCS:          $Header: $
; Description:  This is an example using the object-oriented programming support in
;		XLISP.  The example involves defining a class of objects representing
;		dictionaries.  Each instance of this class will be a dictionary in
;		which names and values can be stored.  There will also be a facility
;		for finding the values associated with names after they have been
;		stored.
; Author:       ???
; Created:      Sat Oct  5 20:47:56 1991
; Modified:     Sat Oct  5 20:48:55 1991 (Niels Mayer) mayer@hplnpm
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

; Create the 'Dictionary' class and establish its instance variable list.
; The variable 'entries' will point to an association list representing the
; entries in the dictionary instance.

(setq Dictionary (Class :new '(entries)))

; Setup the method for the ':isnew' initialization message.
; This message will be send whenever a new instance of the 'Dictionary'
; class is created.  Its purpose is to allow the new instance to be
; initialized before any other messages are sent to it.  It sets the value
; of 'entries' to nil to indicate that the dictionary is empty.

(Dictionary :answer :isnew '()
	    '((setq entries nil)
	      self))

; Define the message ':add' to make a new entry in the dictionary.  This
; message takes two arguments.  The argument 'name' specifies the name
; of the new entry; the argument 'value' specifies the value to be
; associated with that name.

(Dictionary :answer :add '(name value)
	    '((setq entries
	            (cons (cons name value) entries))
	      value))

; Create an instance of the 'Dictionary' class.  This instance is an empty
; dictionary to which words may be added.

(setq d (Dictionary :new))

; Add some entries to the new dictionary.

(d :add 'mozart 'composer)
(d :add 'winston 'computer-scientist)

; Define a message to find entries in a dictionary.  This message takes
; one argument 'name' which specifies the name of the entry for which to
; search.  It returns the value associated with the entry if one is
; present in the dictionary.  Otherwise, it returns nil.

(Dictionary :answer :find '(name &aux entry)
	    '((cond ((setq entry (assoc name entries))
	      (cdr entry))
	     (t
	      nil))))

; Try to find some entries in the dictionary we created.

(d :find 'mozart)
(d :find 'winston)
(d :find 'bozo)

; The names 'mozart' and 'winston' are found in the dictionary so their
; values 'composer' and 'computer-scientist' are returned.  The name 'bozo'
; is not found so nil is returned in this case.
