; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         popup-menu.lsp
; RCS:          $Header: popup-menu.lsp,v 1.3 91/10/05 18:38:00 mayer Exp $
; Description:  POPUP menu example. This is a 1-to-1 translation of the popup
;               menu example in the Motif Programmer's Guide. Just 'load' this
;               file to see the example.					
;		Note that for Motif 1.1, closing the popup-menu shell created
;		here will give the following message (due to Motif bug):
;		"Warning: Attempt to remove non-existant passive grab"
;		However, if you have WINTERP resource "enableXtWarningBreak" set
;		to TRUE, you will corrupt the internal state of Motif, resulting
;		in subsequent inability to close windows, coredumps, etc.
;		The resource is set to FALSE by default so as to prevent such
;		problems. See ./../doc/BUGS for details. 
; Author:       Niels Mayer, HPLabs
; Created:      Thu Nov 23 12:43:54 1989
; Modified:     Sat Oct  5 18:37:40 1991 (Niels Mayer) mayer@hplnpm
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

(setq toplevel
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "PopUp-Menu"
	    :XMN_TITLE "Popup Menu Example"
	    :XMN_ICON_NAME "Popup"
	    ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Create RowColumn in toplevel with two pushbuttons
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq rc
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "rc" toplevel
	    :XMN_WIDTH 400
	    :XMN_HEIGHT 125
	    :XMN_RESIZE_WIDTH nil
	    :XMN_RESIZE_HEIGHT nil
	    :XMN_ADJUST_LAST nil
	    ))

(let ((buttons (MAKE-ARRAY 2)))

  (setf (aref buttons 0)
	(send XM_PUSH_BUTTON_GADGET_CLASS :new "button0" rc
	      ))
  (send (aref buttons 0) :set_callback :XMN_ACTIVATE_CALLBACK ()
	'(
	  (format T "Button 0 selected.\n")
	  ))

  (setf (aref buttons 1) 
	(send XM_PUSH_BUTTON_GADGET_CLASS :new "button1" rc
	      ))
  (send (aref buttons 1) :set_callback :XMN_ACTIVATE_CALLBACK ()
	'(
	  (format T "Button 1 selected.\n")
	  ))

  (xt_manage_children buttons)
  )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Create Popup Menu
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq popup
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :POPUP_MENU "popup" rc
	    ))

(send rc :set_event_handler BUTTON_PRESS_MASK
      '(EVHANDLER_BUTTON		;gets bound to button of event
	EVHANDLER_XEVENT)		;gets bound to Xevent struct.
      `(
	(if (= EVHANDLER_BUTTON 3)
	    (progn
	      (send ,popup :menu_position EVHANDLER_XEVENT)
	      (send ,popup :manage)
	      )))
      )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Create two submenus and cascadebuttons in the popup menu
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq submenu1
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :PULLDOWN_MENU "submenu1" popup
	    ))

(let ((popupBtn (MAKE-ARRAY 2)))

  (setf (aref popupBtn 0)
	(send XM_CASCADE_BUTTON_GADGET_CLASS :new "cbutton1" popup
	      :XMN_SUB_MENU_ID submenu1
	      :XMN_LABEL_STRING "First Submenu"
	      ))

  (setq submenu2
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :PULLDOWN_MENU "submenu2" popup
	      ))

  (setf (aref popupBtn 1)
	(send XM_CASCADE_BUTTON_GADGET_CLASS :new "cbutton2" popup
	      :XMN_SUB_MENU_ID submenu2
	      :XMN_LABEL_STRING "Second Submenu"
	      ))

  (xt_manage_children popupBtn)
  )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Create pushbuttons in submenu1 and submenu2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq sub1Btn  (MAKE-ARRAY 3))

(setf (aref sub1Btn 0)
      (send XM_PUSH_BUTTON_GADGET_CLASS :new "button1a" submenu1
	    ))
(send (aref sub1Btn 0) :set_callback :XMN_ACTIVATE_CALLBACK '()
      '(
	(format T "Button 1a selected.\n")
	))

(setf (aref sub1Btn 1)
      (send XM_PUSH_BUTTON_GADGET_CLASS :new "button1b" submenu1
	    ))
(send (aref sub1Btn 1) :set_callback :XMN_ACTIVATE_CALLBACK '()
      '(
	(format T "Button 1b selected.\n")
	))


(let ((sub2Btn (MAKE-ARRAY 2)))

  (setf (aref sub2Btn 0)
	(send XM_PUSH_BUTTON_GADGET_CLASS :new "button2a" submenu2
	      ))
  (send (aref sub2Btn 0) :set_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (format T "Button 2a selected.\n")
	  ))

  (setf (aref sub2Btn 1)
	(send XM_PUSH_BUTTON_GADGET_CLASS :new "button2b" submenu2
	      ))
  (send (aref sub2Btn 1) :set_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (format T "Button 2b selected.\n")
	  ))

  (xt_manage_children sub2Btn)
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Create a submenu of submenu 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(setq submenu3
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :PULLDOWN_MENU "submenu3" submenu1
	    ))

(setf (aref sub1Btn 2)
      (send XM_CASCADE_BUTTON_GADGET_CLASS :new "cbutton3" submenu1
	    :XMN_SUB_MENU_ID submenu3
	    :XMN_LABEL_STRING "To Third Submenu"
	    ))

(xt_manage_children sub1Btn)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Create Pushbuttons in submenu 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(let ((sub3Btn (MAKE-ARRAY 2)))

  (setf (aref sub3Btn 0)
	(send XM_PUSH_BUTTON_GADGET_CLASS :new "button3a" submenu3
	      ))
  (send (aref sub3Btn 0) :set_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (format T "Button 3a selected.\n")
	  ))

  (setf (aref sub3Btn 1)
	(send XM_PUSH_BUTTON_GADGET_CLASS :new "button3b" submenu3
	      ))
  (send (aref sub3Btn 1) :set_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (format T "Button 3b selected.\n")
	  ))

  (xt_manage_children sub3Btn)
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Create the windows and Make them visible.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(send toplevel :realize)
