.\" $Header: /usr/local/src/x11r5.mvex/RCS/indexmacros.t,v 1.3 1990/04/06 19:17:43 toddb Exp $
. \" Macros for the index
.de Ib	\" blank major entry
.br
.ne 2v
\\$1:
..
.de I>	\" major entry
.br
\\$1, \\$2
..
.de I<  \" minor entry
.br
   \\$2, \\$3
..
.de LB	\" new letter starts here
.di DT	\" start diverted text
.sp
.sz +2
.b
\\$1
.r
.sz -2
.sp
.di	\" end diverted text
.ne \\n(dnu+1v	\" get enough space for it
.DT		\" output it
..
.\" Set the page number for the index properly.
.so index.page
.\" set up various parameters for the right evironment.
.\" Your taste may be different.
.eh '\s+1\fBVEX \*(nM\fP''\fBX11, Release 4\fP\s-1'
.oh '\s+1\fBVEX \*(nM\fP''\fBX11, Release 4\fP\s-1'
.ef ''\s+1\fB % \fP\s-1''
.of ''\s+1\fB % \fP\s-1''
.++ A
.po 1.0i	\" physical offset
.ta 5iR		\" right alignment tab
.lp		\" initialize -me
.ce
.sz 18
Index
.sp 1
.sz 10
.2c		\" 2 column mode
.sp 3
