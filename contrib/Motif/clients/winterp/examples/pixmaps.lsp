; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         pixmaps.lsp
; RCS:          $Header: pixmaps.lsp,v 1.5 91/10/05 18:32:58 mayer Exp $
; Description:  Play around with pixmaps. These are just random individual forms
;               I eval'd to play around and test pixmaps, pixmap garbage
;               collection, image cacheing, etc.
;               Many of the pixmaps mentioned in this file do not exist on your
;               machine.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:44:36 1989
; Modified:     Sat Oct  5 18:32:37 1991 (Niels Mayer) mayer@hplnpm
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

(progn
  (setq b1
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "background"  "white" "DarkSlateGrey")))
  (setq b2
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "25_foreground" "white" "DarkSlateGrey")))
  (setq b3
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "50_foreground"  "white" "DarkSlateGrey")))
  (setq b4
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "75_foreground"  "white" "DarkSlateGrey")))
  (setq b5
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "vertical" "white" "DarkSlateGrey")))
  (setq b6
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "horizontal"  "white" "DarkSlateGrey")))
  (setq b7
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "slant_right"  "white" "DarkSlateGrey")))
  (setq b8
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "slant_left" "white" "DarkSlateGrey")))
  (setq b9
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "menu_cascade" "white" "DarkSlateGrey")))
  (setq b10
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "menu_checkmark"  "white" "DarkSlateGrey")))
  )

(progn
  (send b1 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b2 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b3 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b4 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b5 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b6 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b7 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b8 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b9 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  (send b10 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "white" "DarkSlateGrey"))
  )

(progn
  (setq b1
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "background"))
  (setq b2
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "25_foreground" ))
  (setq b3
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "50_foreground"  ))
  (setq b4
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "75_foreground"  ))
  (setq b5
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "vertical" ))
  (setq b6
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "horizontal"  ))
  (setq b7
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "slant_right"  ))
  (setq b8
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "slant_left" ))
  (setq b9
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "menu_cascade" ))
  (setq b10
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "menu_checkmark"  ))
  )





(setq pb_w (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman" "red" "blue")))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/wingdogs" "red" "blue"))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/flagup" "red" "blue"))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/flagdown" "red" "blue"))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/xlogo16" "red" "blue"))
(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/woman")






(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
  	    )
      )
(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/woman"
      )
(gc)





(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/xlogo16"
;	    :XMN_ARM_PIXMAP "/usr/include/X11/bitmaps/flagup"
	    )
      )

(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/flagdown"
	    )
      )

(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/woman"
      )

(setq image1_pixmap (xm_get_pixmap "/usr/include/X11/bitmaps/woman"
				 "white" "DarkSlateGrey"))

(setq pb_w (send xm_push_button_widget_class :new :managed rc_w
		 :XMN_LABEL_TYPE :PIXMAP
		 :XMN_LABEL_PIXMAP (xm_get_pixmap
				    "/usr/include/X11/bitmaps/wingdogs"
				    "white" "DarkSlateGrey")
		 ))

(send pb_w :set_values 
      :XMN_LABEL_PIXMAP (xm_get_pixmap
			 "/usr/include/X11/bitmaps/xlogo16"
			 "white" "DarkSlateGrey")
      )

(gc)

; (setq image1_pixmap nil)
(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP image1_pixmap
      )


(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP image1_pixmap
	    )
      )
(setq image1_pixmap (xm_get_pixmap "/usr/include/X11/bitmaps/flagup"
				 "blue" "red"))

(gc)

(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP image1_pixmap
;	    :XMN_ARM_PIXMAP   image2_pixmap
	    )
      )


(setq rc (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
		       "rc"
		       (send (send TOP_LEVEL_SHELL_WIDGET_CLASS :new :XMN_GEOMETRY "500x500") :realize)
		       :xmn_orientation :vertical
		       :xmn_packing :pack_tight
		       :xmn_entry_alignment :alignment_center
		       :xmn_foreground "Black"
		       :xmn_background "LightGray"))

(setq to_w 
      (send XM_toggle_button_widget_CLASS :new :managed "foo" rc
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/wingdogs"
	    :XMN_select_PIXMAP "/usr/include/X11/bitmaps/flagup"
	    :XMN_select_insensitive_PIXMAP "/usr/include/X11/bitmaps/flagdown"
	    )
      )

(setq to_w 
      (send XM_toggle_button_widget_CLASS :new :managed "foo" rc
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/woman"  "white" "DarkSlateGrey")
	    :XMN_select_PIXMAP (xm_get_pixmap "/usr/include/X11/bitmaps/wingdogs"  "red" "blue")
	    )
      )





(send to_w :set_values 
	    :XMN_LABEL_PIXMAP "/usr/include/X11/bitmaps/xlogo64"
	    :XMN_SELECT_PIXMAP "/usr/include/X11/bitmaps/xlogo16"
	    )

(setq mbox_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :message_dialog top_w
	    :XMN_SYMBOL_PIXMAP "/usr/include/X11/bitmaps/woman"
	    ))



(xm_install_image
 (setq image0_xi (xm_get_ximage_from_file "/usr/include/X11/bitmaps/woman"))
 "image0")

(xm_install_image
 (setq image1_xi (xm_get_ximage_from_file "/usr/include/X11/bitmaps/wingdogs"))
 "image1")

(xm_install_image
 (setq image2_xi (xm_get_ximage_from_file "/usr/include/X11/bitmaps/flagup"))
 "image2")

(xm_install_image
 (setq image3_xi (xm_get_ximage_from_file "/usr/include/X11/bitmaps/flagdown"))
 "image3")

(xm_install_image
 (setq image4_xi (xm_get_ximage_from_file "/usr/include/X11/bitmaps/xlogo16"))
 "image4")

(format T "~A, ~A, ~A, ~A, ~A\n" image0_xi image1_xi image2_xi image3_xi image4_xi)

(progn
  (setq b1
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "image0"  "white" "DarkSlateGrey")))
  (setq b2
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "image1" "white" "DarkSlateGrey")))
  (setq b3
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "image2"  "white" "DarkSlateGrey")))
  (setq b4
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "image3"  "white" "DarkSlateGrey")))
  (setq b5
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "image4" "white" "DarkSlateGrey")))
  )

(xm_uninstall_image image0_xi)
(xm_uninstall_image image1_xi)
(xm_uninstall_image image2_xi)
(xm_uninstall_image image3_xi)
(xm_uninstall_image image4_xi)




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; try out method :update_display in callback.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(setq pb_w (send xm_push_button_widget_class :new :managed rc_w
		 :XMN_LABEL_TYPE :PIXMAP
		 :XMN_LABEL_PIXMAP (xm_get_pixmap
				    "/usr/include/X11/bitmaps/xlogo16"
				    "white" "green")
		 ))
(send pb_w :set_callback :XMN_ACTIVATE_CALLBACK '(callback_widget)
      '(
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-1")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-2")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-3")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-4")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-5")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-6")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-7")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap
				 "/usr/include/X11/bitmaps/xlogo16"
				 "white" "DarkSlateGrey"))
	))
