; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         menutree.lsp
; RCS:          $Header: menutree.lsp,v 1.1 91/10/05 21:29:54 mayer Exp $
; Description:  Simple menu tree program... see below
; Author:       Niels Mayer, HPLabs
; Created:      Sat Oct  5 21:24:29 1991
; Modified:     Sat Oct  5 21:29:04 1991 (Niels Mayer) mayer@hplnpm
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

; To: davis@hplabs.hpl.hp.com (Jim Davis)
; Subject: Re: Want Menu-tree program 
; Newsgroups: comp.windows.x
; In-reply-to: Your message of 23 Aug 91 00:19:09 +0000
; Organization: Hewlett-Packard Labs, Software & Systems Lab, Palo Alto, CA.
; X-Mailer: mh6.7
; --------
; WINTERP is quite handy for building menu trees and associated actions
; without having to mess with C compiles and low-level junk. 
; 
; I have two possible solutions for you. One is the WINTERP Xmu menu server
; package that was contributed by Richard Hess of Consilium, Inc. I also
; quickly cobbled together a Winterp-Lisp file which does the basics of what
; you want.
; 
; > Where can I find such a program?  (Additional points are scored if the
; > program has a concept of string valued 'variables' that can be modified
; > through text widgets so I needn't keep using my cut buffer for such things.
; > More additional points for source, and someone to whom I can convey
; > enhancements.)
; 
; The winterp-lisp program below has this. I put a Motif text editor
; widget in there, and you can access the string in the widget by
; sending it message :GET_STRING. See code below for details.
; 
; 			--------------------
;
; Here's the primitive menu tree I cobbled together. It contains a menu-bar
; with cascade-buttons "hplstl" hplhcid" "hplptd" and each cascase button has
; a pulldown with entries "xterm" "hpterm" "emacs" "xmh" "edit file in
; editor". The latter will use the value you type into the text edit widget
; in order to determine which file to edit.... 
; 
; The menubar and pulldown entries are all mnemonically driven, which means
; you don't have to use a mouse if you don't want to.
; 

(progn

(setq toplevel_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
	    :XMN_TITLE "MenuTree"
	    :XMN_ICON_NAME "MenuTree"
	    ))

(setq rc_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "rc" toplevel_w
	    ))

(setq menubar_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :simple_menu_bar
	      "menubar" rc_w
	      :XMN_BUTTON_COUNT		3
	      :XMN_BUTTONS		#("hplstl" "hplhcid" "hplptd")
	      :XMN_BUTTON_MNEMONICS	#(#\s     #\h        #\p     )
	      ))

(setq edit_w
      (send XM_TEXT_WIDGET_CLASS :new :managed "edit" rc_w
	    ))

(setq hplstl_pd_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	    "pulldown" menubar_w
	    :XMN_POST_FROM_BUTTON 0	;post pulldown from menubar's 0th item
	    :XMN_BUTTON_COUNT 5		;create five buttons in this pulldown
	    :XMN_BUTTONS		   #("xterm" "hpterm" "emacs" "xmh" "edit file in editor")
	    :XMN_BUTTON_MNEMONICS	   #(#\x     #\h      #\e     #\m    #\e)
;;;	    :XMN_BUTTON_ACCELERATORS	   #("Ctrl<Key>C" "Ctrl<Key>F" "Ctrl<Key>O" "Ctrl<Key>S" "Ctrl<Key>W")
;;;	    :XMN_BUTTON_ACCELERATOR_TEXT #("^C" "^F" "^O" "^S" "^W")
	    )
      )

(send hplstl_pd_w :add_callback :xmn_entry_callback ;use this instead of XmNsimpleCallback
      '(CALLBACK_ENTRY_WIDGET)
      '(
	;; (send CALLBACK_ENTRY_WIDGET :name) returns "button_<#>"
	;; where <#> is 0 ... (button-count-1).
	;; we use 'read' to return the FIXNUM <#> after truncating the
	;; 7 chars "button_" from the front of the string.
	(case (read (make-string-input-stream (send CALLBACK_ENTRY_WIDGET :name) 7))
	      (0 (system "xhost hplstl ; remsh hplstl \"env DISPLAY=hplnpm:0.0 /usr/bin/X11/xterm \" &"))
	      (1 (system "xhost hplstl ; remsh hplstl \"env DISPLAY=hplnpm:0.0 /usr/bin/X11/hpterm \" &"))
	      (2 (system "xhost hplstl ; remsh hplstl \"env DISPLAY=hplnpm:0.0 /usr/local/bin/x11emacs \" &"))
	      (3 (system "xhost hplstl ; remsh hplstl \"env DISPLAY=hplnpm:0.0 /usr/local/bin/X11/xmh \" &"))
	      (4 (system (strcat
			  "xhost hplstl ; remsh hplstl \"env DISPLAY=hplnpm:0.0 /usr/local/emacs/bin300/gnuclient "
			  (send edit_w :get_string)
			  " &")))
	      (T (system "Error\n")))
	))

(setq hplhcid_pd_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	    "pulldown" menubar_w
	    :XMN_POST_FROM_BUTTON 1	;post pulldown from menubar's first button.
	    :XMN_BUTTON_COUNT 5		;create five buttons in this pulldown
	    :XMN_BUTTONS		   #("xterm" "hpterm" "emacs" "xmh" "edit file in editor")
	    :XMN_BUTTON_MNEMONICS	   #(#\x     #\h      #\e     #\m    #\e)
;;;	    :XMN_BUTTON_ACCELERATORS	   #("Ctrl<Key>C" "Ctrl<Key>F" "Ctrl<Key>O" "Ctrl<Key>S" "Ctrl<Key>W")
;;;	    :XMN_BUTTON_ACCELERATOR_TEXT #("^C" "^F" "^O" "^S" "^W")
	    ))

(send hplhcid_pd_w :add_callback :xmn_entry_callback ;use this instead of XmNsimpleCallback
      '(CALLBACK_ENTRY_WIDGET)
      '(
	;; (send CALLBACK_ENTRY_WIDGET :name) returns "button_<#>"
	;; where <#> is 0 ... (button-count-1).
	;; we use 'read' to return the FIXNUM <#> after truncating the
	;; 7 chars "button_" from the front of the string.
	(case (read (make-string-input-stream (send CALLBACK_ENTRY_WIDGET :name) 7))
	      (0 (system "xhost hplhcid ; remsh hplhcid \"env DISPLAY=hplnpm:0.0 /usr/bin/X11/xterm \" &"))
	      (1 (system "xhost hplhcid ; remsh hplhcid \"env DISPLAY=hplnpm:0.0 /usr/bin/X11/hpterm \" &"))
	      (2 (system "xhost hplhcid ; remsh hplhcid \"env DISPLAY=hplnpm:0.0 /usr/local/bin/x11emacs \" &"))
	      (3 (system "xhost hplhcid ; remsh hplhcid \"env DISPLAY=hplnpm:0.0 /usr/local/bin/X11/xmh \" &"))
	      (4 (system (strcat
			  "xhost hplhcid ; remsh hplhcid \"env DISPLAY=hplnpm:0.0 /usr/local/emacs/bin300/gnuclient "
			  (send edit_w :get_string)
			  " &")))
	      (T (system "Error\n")))
	))

(setq hplptd_pd_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :simple_pulldown_menu
	    "pulldown" menubar_w
	    :XMN_POST_FROM_BUTTON 2	;post pulldown from menubar's second button.
	    :XMN_BUTTON_COUNT 5		;create five buttons in this pulldown
	    :XMN_BUTTONS		   #("xterm" "hpterm" "emacs" "xmh" "edit file in editor")
	    :XMN_BUTTON_MNEMONICS	   #(#\x     #\h      #\e     #\m    #\e)
;;;	    :XMN_BUTTON_ACCELERATORS	   #("Ctrl<Key>C" "Ctrl<Key>F" "Ctrl<Key>O" "Ctrl<Key>S" "Ctrl<Key>W")
;;;	    :XMN_BUTTON_ACCELERATOR_TEXT #("^C" "^F" "^O" "^S" "^W")
	    ))
(send hplptd_pd_w
      :add_callback :xmn_entry_callback	;use this instead of XmNsimpleCallback
      '(CALLBACK_ENTRY_WIDGET)
      '(
	;; (send CALLBACK_ENTRY_WIDGET :name) returns "button_<#>"
	;; where <#> is 0 ... (button-count-1).
	;; we use 'read' to return the FIXNUM <#> after truncating the
	;; 7 chars "button_" from the front of the string.
	(case (read (make-string-input-stream (send CALLBACK_ENTRY_WIDGET :name) 7))
	      (0 (system "xhost hplptd ; remsh hplptd \"env DISPLAY=hplnpm:0.0 /usr/bin/X11/xterm \" &"))
	      (1 (system "xhost hplptd ; remsh hplptd \"env DISPLAY=hplnpm:0.0 /usr/bin/X11/hpterm \" &"))
	      (2 (system "xhost hplptd ; remsh hplptd \"env DISPLAY=hplnpm:0.0 /usr/local/bin/x11emacs \" &"))
	      (3 (system "xhost hplptd ; remsh hplptd \"env DISPLAY=hplnpm:0.0 /usr/local/bin/X11/xmh \" &"))
	      (4 (system (strcat
			  "xhost hplptd ; remsh hplptd \"env DISPLAY=hplnpm:0.0 /usr/local/emacs/bin300/gnuclient "
			  (send edit_w :get_string)
			  " &")))
	      (T (system "Error\n")))
	))

(send toplevel_w :realize)

)
