; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         dialogshel.lsp
; RCS:          $Header: dialogshel.lsp,v 1.3 91/10/05 15:25:37 mayer Exp $
; Description:  test out motif dialog shells
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:54:12 1989
; Modified:     Sat Oct  5 15:23:42 1991 (Niels Mayer) mayer@hplnpm
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

;; note use of unmapped *toplevel_widget* (winterp's main widget) as the 
;; parent to pop up dialogues from.

(let ()
(setq fsb_w
      (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new :managed :dialog
	    "file_selection_box" *toplevel_widget*
	    ))
(setq form_w
      (send XM_FORM_WIDGET_CLASS :new :managed :dialog
	    "form" *toplevel_widget*
	    ))
(setq mbox_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed
	    "message_box" *toplevel_widget*
	    :XMN_MESSAGE_STRING "welcome to the night train"
	    ))
(setq md_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :message_dialog
	    "message_dialog" *toplevel_widget*
	    :XMN_MESSAGE_STRING "the show is coming...."
	    ))
(setq ed_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :error_dialog
	    "error_dialog" *toplevel_widget*
	    :XMN_MESSAGE_STRING "snakeskin tracksuit"
	    ))
(setq id_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :information_dialog
	    "information_dialog" *toplevel_widget*
	    :XMN_MESSAGE_STRING "high protein snack"
	    ))
(setq qd_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :question_dialog
	    "question_dialog" *toplevel_widget*
	    :XMN_MESSAGE_STRING "get it together with the younger generation"
	    ))
(setq wrnd_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :warning_dialog
	    "warning_dialog" *toplevel_widget*
	    :XMN_MESSAGE_STRING "beware of dub syndicate"
	    ))
(setq wrkd_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :working_dialog
	    "working_dialog" *toplevel_widget*
	    :XMN_MESSAGE_STRING "african head charge in effect"
	    ))
)

(let ()
(send fsb_w :unmanage)
(send form_w :unmanage)
(send mbox_w :unmanage)
(send md_w :unmanage)
(send ed_w :unmanage)
(send id_w :unmanage)
(send qd_w :unmanage)
(send wrnd_w :unmanage)
(send wrkd_w :unmanage)
)

(let ()
(send fsb_w :manage)
(send form_w :manage)
(send mbox_w :manage)
(send md_w :manage)
(send ed_w :manage)
(send id_w :manage)
(send qd_w :manage)
(send wrnd_w :manage)
(send wrkd_w :manage)
)

(let ()
(send (car (send fsb_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
;;(send (car (send form_w :get_values :XMN_DEFAULT_BUTTON nil))
;;      :set_values :XMN_FONT_LIST "6x10")
(send (car (send mbox_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
(send (car (send md_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
(send (car (send ed_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
(send (car (send id_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
(send (car (send qd_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
(send (car (send wrnd_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
(send (car (send wrkd_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "6x10")
)

(let ()
(send (car (send fsb_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
;;(send (car (send form_w :get_values :XMN_DEFAULT_BUTTON nil))
;;      :set_values :XMN_FONT_LIST "micro")
(send (car (send mbox_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
(send (car (send md_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
(send (car (send ed_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
(send (car (send id_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
(send (car (send qd_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
(send (car (send wrnd_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
(send (car (send wrkd_w :get_values :XMN_DEFAULT_BUTTON nil))
      :set_values :XMN_FONT_LIST "micro")
)



