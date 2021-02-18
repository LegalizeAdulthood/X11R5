.\"#! troff -ms $1		-*- Nroff -*-
.\" "IXterm Control Sequences" document
.\" ictlseqs.ms,v 1.0 91/08/23 knba
.\" 
.\" Copyright 1991 by the Massachusetts Institute of Technology
.\"
.\" Permission to use, copy, modify, and distribute this documentation
.\" for any purpose and without fee is hereby granted, provided 
.\" that the above copyright notice appears in all copies,
.\" and that the name of M.I.T. not be used in advertising
.\" or publicity pertaining to distribution of the software without specific, 
.\" written prior permission.  M.I.T. makes no representations about the 
.\" suitability of the software described here for any purpose, nor
.\" about the accuracy of this documentation.  It is provided "as is"
.\" without express or implied warranty.
.\"
.\" The X Window System is a Trademark of MIT.
.\"
.\" Originally written by Edward Moy, University of California,
.\" Berkeley, edmoy@violet.berkeley.edu, for the X.V10R4 xterm.
.\" The X Consortium staff has since updated it for X11.
.\"
.\"
.\" 91/08/23
.\" SCS sequences for several national replacement character sets
.\" added by Kjetil Narum Bakken, Tandberg Data A/S, Norway.
.\"
.\"
.\" Run this file through troff and use the -ms macro package.
.ND
.de St
.sp
.nr PD 0
.nr PI 1.5i
.nr VS 16
..
.de Ed
.nr PD .3v
.nr VS 12
..
.rm CH
.ds LH IXterm Control Sequences
.nr s 6*\n(PS/10
.ds L \s\nsBEL\s0
.ds E \s\nsESC\s0
.ds T \s\nsTAB\s0
.ds X \s\nsETX\s0
.ds N \s\nsENQ\s0
.ds e \s\nsETB\s0
.ds C \s\nsCAN\s0
.ds S \s\nsSUB\s0
.nr [W \w'\*L'u
.nr w \w'\*E'u
.if \nw>\n([W .nr [W \nw
.nr w \w'\*T'u
.if \nw>\n([W .nr [W \nw
.nr w \w'\*X'u
.if \nw>\n([W .nr [W \nw
.nr w \w'\*N'u
.if \nw>\n([W .nr [W \nw
.nr w \w'\*e'u
.if \nw>\n([W .nr [W \nw
.nr w \w'\*C'u
.if \nw>\n([W .nr [W \nw
.nr w \w'\*S'u
.if \nw>\n([W .nr [W \nw
.nr [W +\w'\|\|'u
.de []
.nr w \w'\\$2'
.nr H \\n([Wu-\\nwu
.nr h \\nHu/2u
.ds \\$1 \(br\v'-1p'\(br\v'1p'\h'\\nhu'\\$2\h'\\nHu-\\nhu'\(br\l'-\\n([Wu\(ul'\v'-1p'\(br\l'-\\n([Wu\(rn'\v'1p'\|
..
.[] Et \v'-1p'\*X\v'1p'
.[] En \v'-1p'\*N\v'1p'
.[] Be \v'-1p'\*L\v'1p'
.[] Bs \v'-1p'\s\nsBS\s0\v'1p'
.[] Ta \v'-1p'\*T\v'1p'
.[] Lf \v'-1p'\s\nsLF\s0\v'1p'
.[] Vt \v'-1p'\s\nsVT\s0\v'1p'
.[] Ff \v'-1p'\s\nsFF\s0\v'1p'
.[] Cr \v'-1p'\s\nsCR\s0\v'1p'
.[] So \v'-1p'\s\nsSO\s0\v'1p'
.[] Si \v'-1p'\s\nsSI\s0\v'1p'
.[] Eb \v'-1p'\*e\v'1p'
.[] Ca \v'-1p'\*C\v'1p'
.[] Su \v'-1p'\*S\v'1p'
.[] Es \v'-1p'\*E\v'1p'
.[] Fs \v'-1p'\s\nsFS\s0\v'1p'
.[] Gs \v'-1p'\s\nsGS\s0\v'1p'
.[] Rs \v'-1p'\s\nsRS\s0\v'1p'
.[] Us \v'-1p'\s\nsUS\s0\v'1p'
.[] ! !
.[] # #
.[] % %
.[] (( (
.[] ) )
.[] * *
.[] + +
.[] 0 0
.[] 1 1
.[] 2 2
.[] 3 3
.[] 4 4
.[] 5 5
.[] 6 6
.[] 7 7
.[] 8 8
.[] 9 9
.[] : :
.[] ; ;
.[] = =
.[] > >
.[] ? ?
.[] @ @
.[] A A
.[] cB B
.[] C C
.[] D D
.[] E E
.[] F F
.[] H H
.[] J J
.[] K K
.[] L L
.[] M M
.[] N N
.[] O O
.[] P P
.[] Q Q
.[] R R
.[] S S
.[] T T
.[] Y Y
.[] Z Z
.[] [[ [
.[] bS \\e
.[] ] ]
.[] ^ ^
.[] _ _
.[] ` \`
.[] a a
.[] b b
.[] c c
.[] d d
.[] f f
.[] g g
.[] h h
.[] i i
.[] j j
.[] k k
.[] l l
.[] m m
.[] n n
.[] o o
.[] p p
.[] q q
.[] r r
.[] s s
.[] t t
.[] x x
.[] | |
.[] } }
.[] c~ ~
.[] Sc \fIc\fP
.ds Cc \fIC\fP
.ds Cb \fIC\v'.3m'\h'-.2m'\s-2b\s0\v'-.3m'\fP
.ds Cx \fIC\v'.3m'\h'-.2m'\s-2x\s0\v'-.3m'\fP
.ds Cy \fIC\v'.3m'\h'-.2m'\s-2y\s0\v'-.3m'\fP
.ds Ps \fIP\v'.3m'\h'-.2m'\s-2s\s0\v'-.3m'\fP
.ds Pm \fIP\v'.3m'\h'-.2m'\s-2m\s0\v'-.3m'\fP
.ds Pt \fIP\v'.3m'\h'-.2m'\s-2t\s0\v'-.3m'\fP
.ds Ix \fIx\fP
.ds Iy \fIy\fP
.ds Iw \fIw\fP
.ds Ih \fIh\fP
.ds Ir \fIr\fP
.ds Ic \fIc\fP
.nr LL 6.5i
.TL
IXterm Control Sequences
.AU
Edward Moy
.AI
University of California, Berkeley
.sp
SCS sequences included by
.AU
Kjetil Narum Bakken
.AI
Tandberg Data A/S, Norway

.am NP
.ds CF %
..
.SH
Definitions
.IP \*(Sc
The literal character \fIc\fP.
.IP \*(Cc
A single or multiple (required) character(s).
.IP \*(Ps
A single (usually optional) numeric parameter, composed of one of more digits.
.IP \*(Pm
A multiple numeric parameter composed of any number of single numeric
parameters, separated by \*; character(s).
.IP \*(Pt
A text parameter composed of printable characters.
.SH
VT100 Mode
.ds RH VT100 Mode
.LP
Most of these control sequences are standard VT102 control sequences,
but there are some sequences here from later DEC VT terminals, too.
Major VT102 features not supported are smooth scrolling, double size
characters, blinking characters, and VT52 mode.
There are additional control sequences to provide
\fIxterm-\fPdependent functions, like the scrollbar or window size.
Where the function is specified by DEC or ISO 6429, the code assigned
to it is given in parentheses.  Some of the escape codes to designate
character sets are specified by ISO 2022 (the others are specified by DEC);
see that document for a discussion of character sets.
.St
.IP \\*(So
Shift Out (Ctrl-N) \(-> Switch to Alternate Character Set: invokes the
G1 character set.
.IP \\*(Si
Shift In (Ctrl-O) \(-> Switch to Standard Character Set: invokes the
G0 character set (the default).
.IP \\*(Es\\*(((\\*(Cc
Designate G0 Character Set
  \*(Cc = \*0 \(-> DEC Special Character and Line Drawing Set
  \*(Cc = \*A \(-> United Kingdom (UK)
  \*(Cc = \*(cB \(-> United States (USASCII)
  \*(Cc = \*4 \(-> Dutch
  \*(Cc = \*5 \(-> Finnish
  \*(Cc = \*C \(-> Finnish
  \*(Cc = \*Q \(-> French
  \*(Cc = \*R \(-> French
  \*(Cc = \*9 \(-> French Canadian
  \*(Cc = \*K \(-> German
  \*(Cc = \*i \(-> Hungarian
  \*(Cc = \*Y \(-> Italian
  \*(Cc = \*6 \(-> Norwegian/Danish
  \*(Cc = \*E \(-> Norwegian/Danish
  \*(Cc = \*` \(-> Norwegian/Danish
  \*(Cc = \*%\*6 \(-> Portuguese
  \*(Cc = \*Z \(-> Spanish
  \*(Cc = \*7 \(-> Swedish
  \*(Cc = \*H \(-> Swedish
  \*(Cc = \*= \(-> Swiss
.IP \\*(Es\\*)\\*(Cc
Designate G1 Character Set
  \*(Cc = Same as for G0
.IP \\*(Es\\**\\*(Cc
Designate G2 Character Set
  \*(Cc = Same as for G0
.IP \\*(Es\\*+\\*(Cc
Designate G3 Character Set
  \*(Cc = Same as for G0
.IP \\*(Es\\*N
Single Shift Select of G2 Character Set (SS2): affects next character only
.IP \\*(Es\\*O
Single Shift Select of G3 Character Set (SS3): affects next character only
.IP \\*(Es\\*n
Select the G2 Character Set (LS2)
.IP \\*(Es\\*o
Select the G3 Character Set (LS3)
.IP \\*(Es\\*|
Invoke the G3 Character Set as GR (LS3R).  Has no visible effect in \fIxterm\fP.
.IP \\*(Es\\*}
Invoke the G2 Character Set as GR (LS2R).  Has no visible effect in \fIxterm\fP.
.IP \\*(Es\\*(c~
Invoke the G1 Character Set as GR (LS1R).  Has no visible effect in \fIxterm\fP.
.Ed
