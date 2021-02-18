.EH ''''
.OH ''''
.EF ''''
.OF ''''
.ps 11
.nr PS 11
\&
.sp 10
.ce 6
\s+6\fBInternationalization\fP\s-6

\s+6\fBof\fP\s-6

\s+6\fBAthena Widget Set\fP\s-6
.bp
\&
.sp 5
.ce 6
\s+2\fBIntroduction\fP\s-2
.sp 6
.PP
This section describes the internationalization of Athena widget set.
A whole libraray Xaw consists of this part and others that is not needed to
changed, still remained in the original Xaw from MIT.
.PP
The i18n part of Xaw consists of the following widgets:
.sp
.RS
.RS
.TS
l.
LabelLocale Widget
CommandLocale Widget
DialogLocale Widget
ListLocale Widget
MenuButtonLocale Widget
SmeBSBLocale Object
ToggleLocale Widget
MultiText Widget
MultiSource Widget
MultiSink Widget
AsciiText Widget
AsciiSource Object
AsciiSink Object
Text Widget
TextSource Widget
TextSink Widget
.TE
.RE
.RE
.PP
where all new names above are from subclassing new widgets, all same names
are from changing X11R5.
.PP
This document is arranged to be made up a whole refrence book of Ahthena
Widget Set with this internationalization part.  So you combine this
document with the original Athena Widget Set to be a new one.
.PP
This implementation is referred to the
following standard or draft:
.IP
Compound Text Encoding, Version 1.1, X11R5.
.IP
ISO DIS 10646(UCS), 4 November 1990.
