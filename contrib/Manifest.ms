.\" Contrib manifest  Time-stamp: <91/10/11 14:35:23 gildea>  -*- Nroff -*-
.\" #! troff -ms
.			\" .DH - directory heading
.\" .DH level name
.de DH
.nr DL \\$1
.ds D\\n(DL \\$2
.\" create the section name
.ds SN \\*(D1
.if \\n(DL-1 .as SN /\\*(D2
.if \\n(DL-2 .as SN /\\*(D3
.if \\n(DL-3 .as SN /\\*(D4
.if \\n(DL-4 .as SN /\\*(D5
.SH
.if 2-\\n(DL .LG	\" make level 1 headings larger
\\*(SN
.NL
.LP
..
.			\" AT - apropos table
.de AT
.ta 24n
.nf
.\" end table with .fi
..

.\" move page number to bottom
.ds CF - \\n(PN -
.ds CH \"

.ce 100
\s+2\fBX Window System, Version 11, Release 5\fP\s-2

\s+1\fBContrib Manifest\fP\s-1
.ce 0
.sp .5i
Copyright \(co\ 1991 by the Massachusetts Institute of Technology.
.LP
.SM
Permission to use, copy, modify, and distribute this document 
for any purpose and without fee is hereby granted, provided 
that the above copyright notice and this permission notice
appear in all copies, and that the name of MIT not be used 
in advertising or publicity pertaining to this document without 
specific, written prior permission.
MIT makes no representations about the suitability of this
document for any purpose.
It is provided ``as is'' without express or implied warranty.
.NL
.LP
\fIX Window System\fP is a trademark of MIT.
.sp 3
.LP
This document is a walk-through of the directories on the X11R5 contrib tape.
.LP
Where programs have provided a top-level \fI.man\fP file, the
line that \fIapropos\fP(1) would extract from that file is shown.
For other packages, if there is a succinct description of
the software near the top of a \fIREADME\fP file, that is given here.
Otherwise only the name of the package is given.
.LP
The descriptions of the software given here are simply how
the software describes itself; MIT has not tested that the software
performs as described.
.LP
We do not claim to have built or tested any of this software.  We do
not know what hardware (if any) it will work on.  All questions
regarding contributed software should be directed to the authors, not
to the MIT X Consortium.
.sp 2
.LP
And now the software, directory by directory:

.DH 1 clients
Additional clients and internationalized versions of MIT clients.

additional clients:

.AT
AnswerGarden
cpicker
cxterm
emu
ImageMagick/XtoPS	dump an image of an X window as encapsulated Postscript
ImageMagick/animate	display a sequence of images on any workstation running X
ImageMagick/display	display an image on any workstation running X
ImageMagick/import	dump an image of an X window as MIFF
ImageMagick/montage	creates a composite image by combining several separate images
exterm/Exterm	terminal emulator for X
exterm/resize	utility to set TERMCAP and terminal settings to current window size
gwm	Generic Window Manager for the X Window System
i18nOlwmV2/olwm	OPEN LOOK window manager for OpenWindows
ixterm	international terminal emulator for X
kterm	kanji terminal emulator for X
lclock	display time of day
palette	X11 color FG/BG palette selection tool 
pbmplus
pixmap
shinbun
snftobdf	SNF to BDF font decompiler for X11
tvtwm	Tom's Virtual Tab Window Manager for the X Window System
vine
xalarm
xcalendar	calendar with a notebook for X11
xcdplayer
xco	display X11 color names and colors 
xcolors	display all X11 color names and colors
xcrtca	CRT color analyzer driver
xcrtca/xsccd	Xcms property builder
xdaliclock	melting digital clock
xdu	display the output of "du" in an X window
xfed	font editor for X window systems bdf fontfiles 
xfig	Facility for Interactive Generation of figures under X11
xforecast	X window system interface to the weather program
xforecast/weather	display current weather for a city or state
xhelp	X Window System Help Facility 
xlbiff	mailbox message previewer for X
xlogin	login for X
xmail	X11 visual interface to the mail program
xmailwatcher
xmessage	display a message or query in a window (X-based /bin/echo)
xmeter	rstat display for X
xmh-icons
xmh.editor
xmon	interactive X protocol monitor
xphoon	displays the PHase of the mOON on the root window
xplot	plot(5) filter for X
xpostit	X window system Postit notes
xprompt	prompt a user for input
xproof	preview typesetter-independent troff using X Windows
xrsh	start an X program on a remote machine
xrsh/xrlogin	start an xterm that uses rlogin or telnet to connect to a remote host
xscope	X Window System Protocol Monitor
xtalk	talk to another user
xtiff	view a TIFF file in an X window
xtv
.fi

.ne 4
Internationalized versions of MIT clients:

.AT
bitmap
editres
listres
twm	Tab Window Manager for the X Window System
viewres
xclipboard
xedit
xmh
.fi

.DH 1 demos
Graphics demo programs for X.

.AT
beach_ball2
muncher	draw interesting patterns in an X window
plaid	paint some plaid-like patterns in an X window
psycho	animate an icosahedron
pyramid
scdisp	display scaled text 
sctext	display scaled text 
vandal
xtree	tree graphing filter
.fi

.DH 1 doc
Xbibliography, a bibliography of good, publically available X Window System
technical material.
.LP
color.bib, a bibliography of references on color.
.LP
SyncExt, a specification for an X synchronization extension.

.DH 1 examples
.DH 2 CLX
Common Lisp programs using the CLX interface.

.DH 2 OReilly
Examples from O'Reilly X books, volumes 1 and 4, and the R5
supplement.

.DH 2 PEX
PEX example programs.

.DH 2 Xaw
Simple Athena Widgets examples.  These programs were in the core
distribution in R4.

.DH 2 Xcms
.AT
hello_xcms	The client to display "Hello Xcms"
.nf

.DH 2 i18n
xtail

.DH 2 text
ttextext

.DH 1 extensions
.DH 2 mvex
a minimal video extension to X
.DH 2 xidle
get server idle time
.DH 2 xpli
X Paper-Like-Interface extension (pen-based interface)
.DH 2 xtrap
X protocol trapper, allowing event capture and playback.
.DH 2 xv
X video extension

.DH 1 fonts

.DH 2 bdf
.AT
9x13-3.bdf	ISO 8859-3 encoded 9x15 font
chld8x13.bdf	child's scrawl 8x13 font
.fi

.DH 2 lib/font
Server code to support rasterization and scaling of Type 1 (Postscript)
font outlines.

.DH 2 scaled/Type1
Type 1 outlines of IBM Courier font.

.DH 2 Utopia
Adobe Utopia font in both Type 1 and BDF formats.

.DH 1 ftn
PEX FORTRAN binding

.DH 1 games
.AT
acm
spider	play double deck solitaire 
xboard	Xt/Athena user interface for GNU Chess, version 1.2
xpipeman	connect the pipes to stop the leaks (X Windows)
xtetris	X Window block dropping game
.fi

.DH 1 im
Implementations of Input Methods.

.DH 2 Canna
A unified user interface for Japanese input.  Uses a client-server
model for kana-to-kanji conversion.  Includes nemacs.  Supports kinput2.

.DH 2 Ximp
XIMP protocol input method servers.
Includes ximdemo and xterm.

.DH 2 Xsi
An internationalized Wnn.  Includes Xwnmo (an input manager).

.DH 2 kinput2
A front-end for X11 applications that want Japanese text input.
Upward compatible with kinput (from R4 contrib).  Can use Wnn or Canna
conversion server.

.DH 1 lib
.DH 2 DPS
Client libraries and utilities for writing applications that use the Display
PostScript extension to X.  Includes the pswrap preprocessor.
.DH 2 PEXt
PEX toolkit
.DH 2 Selfmoving
A widget with grips that allow it to be moved or resized.
.DH 2 Wcl
Widget Creation Library, a simple package allowing users to define the
X toolkit widget hierarchies and their binding to the application
using X resource files.
.DH 2 X
Additional Xcms conversion routines.
.DH 2 XO
O'Neal X Library, a library of X and Xt-based convenience routines.
.DH 2 Xaw
Internationalized Athena Widget Set.
.DH 2 Xcu
Cornell University widget set.
.DH 2 Xpex
Low-level PEX client library from Sony.
.DH 2 Xt
Experimental extension event processing for Xt.
.DH 2 Xwchar
Wide character set support.
.DH 2 andrew
The Andrew Messaging System, the Andrew Toolkit.
.DH 2 dec-pexlib
Low-level PEX client library from Digital.
.DH 2 elk
A Scheme interpreter intended to be used as a general extension
language.
.DH 2 graphlib
A library and collection of programs to support X programming.
.DH 2 i18nXView2
Internationalized XView library version 2.
Includes cmdtool and textedit programs.
.DH 2 iv
Interviews 3.0, a C++ based toolkit.
.DH 2 pexim
A subset of PHIGS and the proposed PHIGS Immediate Mode Extensions.
.DH 2 vroot
Virtual root support.
.DH 2 xpm
Support for X Pixmap files.
.DH 2 xview3
XView library version 3.
Includes the following clients: clock, cmdtool, olwm, and textedit.

.DH 1 util
.AT
bm-convert
cpp
sdbm
soelim
.fi

.DH 1 Motif
Software that requires Motif.  Motif is not free software and is not
available from the MIT X Consortium.

.DH 2 clients
programs that use Motif:

.AT
cpicker
winterp
xmphone	while you were out phone message pad
xmpostit	X window system (Motif-like) Postit notes
.fi

.DH 2 config
imake configuration files for Motif

.DH 2 demos
.AT
XMdemos	app-defaults for example programs
xmfonts	font browser, no UIL
xmter	shape & animation example, no UIL
xmform	form constraint example, no UIL
xmgendi	generic dialog prompter, no UIL
motifanim	animation style program.
periodic	The Periodic Table of Motif Widgets
.fi

.DH 2 doc
Frequently Asked Questions posting from comp.windows.x.motif.

.DH 2 examples/OReilly
Examples from the O'Reilly books on Motif.

.DH 2 widgets
FsbGQS

.\" the end
