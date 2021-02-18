\begindata{text,2373192}
\textdsversion{12}
\template{default}
\define{concat
}
\define{sans
}
\flushright{7 December  1989}



\italic{\{\{Future: This version of the manual includes planned but 
non-existent features.  These are delimited by double curly brackets so they 
can be removed to form the manual itself.\}\}}



\majorheading{Ness Hacker's Guide}


\center{by Wilfred J. Hansen}


\indent{\flushright{\smaller{_________________________________


Information Technology Center,

Carnegie Mellon University


Copyright IBM Corporation 1988, 1989

__________________________________


}}
Ness is a programming language for the Andrew ToolKit.  With it, documents can 
be processed and can even contain active elements controlled by Ness scripts. 
 The language features an innovative substring algebra for string processing. 
 


This document is intended for people who want to do unusual things with Ness. 
 It describes how to extend the keyboard and menus with Ness, how to call 
methods and class procedures from Ness, how to access instance variables of 
objects, and how to utilize Ness from C code.


For a description of the other Ness documents, see the \bold{Ness User's 
Manual}.


   1. NessUseDialogBoxInsteadOfWarning

   2. Accessing ATK objects: methods, class procedures, and instance variables

   3. Redefining keys and menus for ATK Applications

   4. Calling Ness from C code

   5. Ness system files


\smaller{CAVEAT: This manual describes an early version of the Ness language 
and system.  You are invited to contribute suggestions for change to the 
author (\italic{wjh+@andrew.cmu.edu}) or the bboard (\italic{andy.ness}).  But 
please realize that your suggestions and those of others may evolve the 
language enough to force you to revise or abandon programs written to this 
document.}\bold{

}}\bold{
}\formatnote{.bp}


        Copyright IBM Corporation 1988, 1989 - All Rights Reserved        


\smaller{Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided that 
the above copyright notice appear in all copies and that both that copyright 
notice and this permission notice appear in supporting documentation, and that 
the name of IBM not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior permission.


\smaller{IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL IBM 
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

}
/* $Header: /afs/andrew.cmu.edu/usr13/wjh/h/atk/ness/doc/RCS/nessuser.d,v 1.1 
89/09/13 10:38:05 wjh Exp Locker: wjh $ */

/* $ACIS:$ */

/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/doc/RCS/nesshack.d,v $ */

/* $Log: nesshack.d,v $
Revision 1.2  1989/12/12  15:03:53  ghoti
sync with MIT tape

Revision 1.2  89/12/07  12:15:29  xguest
added author's name
 Revision 1.1  89/12/07  11:50:30  xguest Initial 
revision

Revision 1.1  89/10/11  10:56:49  wjh Initial revision  */

}\formatnote{.bp}



\section{1. Ness preference options}


Ness supports two preference options.


When Ness scripts appear in documents or applications, they are normally 
surrounded with a warning text describing the dangers of freely using scripts 
from others.  You may prefer to have a dialog box appear instead of the 
warning message.  To get this the preference option is 
"NessUseDialogBoxInsteadOfWarning".  To activate it, put in your ~/preferences 
file the line


\leftindent{*.NessUseDialogBoxInsteadOfWarning: on}


When you have installed this option, any Ness you encounter in documents or 
applications will be announced to you in a dialog box with four options:


\leftindent{\sans{\bold{Ness script here.  Do you trust the user you got it 
from?


\leftindent{Cancel - Keep Ness script inactive

Help - Add warning text to script

Scan - Check for dangerous statements


Empower - I trust the source of this script}}}}


If you just want to eyeball the script, choose the first option and scroll 
through the Ness inset.  When you are ready to compile, you can choose the 
compilation options from the Ness menu.



Another preference option is NessPath.  The value of this preference is a 
comma separated list of directories to be searched for library files.  The 
default value is 


	*.NessPath:/usr/andrew/lib/ness


Incidentally:  These directories are scanned once when Nes is initiated in a 
process.  If new files are later added to the library, Ness will not learn 
about them;  they will however, be available to proceses started after they 
are added.



\section{2. Accessing ATK objects: methods, class procedures, and instance 
variables}


Ness scripts may call functions defined by the ATK "class" preprocessor.  Let 
<var>, <class>, <method>, and <proc> be identifiers and \italic{obj} an 
expression with an object as its value.  Then:


To retrieve the value of instance variable <var> of object \italic{obj} in 
class <class> write


	<class>_Get<var>(\italic{obj})


and to set the value write 


	<class>_Set<var>(\italic{obj}, value)


To call  <method>  in  <class>  on object \italic{obj} write


	<class>_<method>(\italic{obj}, other parameters)


and to call the classprocedure <proc>  in  <class>  write


	<class>_<proc>(parameter list)


Before any of these can operate, the script must declare that it is going to 
utilize functions defined in class.  This is done by writing a comment saying 
(exactly):


	--$enable class access



\section{3. Redefining keys and menus for ATK Applications}


During initialization, each ATK application reads ~/.atkinit or 
~/.<applicationname>init if it exists.  This file can contain specifications 
that redefine the keys and menus provided for various objects when viewed by 
the application.  Ness can be used to write the code that is executed for 
these specifications.  


The first step is to have the ness processor available during initialization. 
 This is done by adding to the .XYZinit file the line


	load ness


which searches the CLASSPATH for ness.do and loads it.  As it is loaded, Ness 
installs in the proctable the entry ness-load, which is used to load and 
compile Ness files which can themselves define entries in the proctable.


Once Ness is loaded, the .XYZinit file can utilize ness-load to load 
specifications written in Ness:


	call ness-load /usr/andrew/lib/wc.n


where the last component of the line is the filename of the Ness program. 
 (This is Ness source code as text;  it is not a Ness object.)  Note that no 
security is applied;  if your .XYZinit file loads a ness script it is compiled 
and its event handlers are activated without asking your permission.


Ness programs utilize special \bold{extend} and \bold{on} syntax to bind key 
sequences and menu options to Ness code.  The script extends the special name 
"view:\italic{viewname}", where \italic{viewname} is the name of a view class. 
 To apply to all views, the string can be "view:view".  Within such an extend 
block, event specifications are acceptable for \bold{on keys} and \bold{on 
menu}.  This must be compiled BEFORE the window is created, so it is best done 
from 

	call ness-load <filename>

in .atkinit or one of your other .XYZinit files.


For example, here is the section of /usr/andrew/lib/ness/wc.n which 
establishes a menu option for counting words:

\example{\smaller{
extend "view:textview"\leftindent{

on menu"Search/Spell,Count Words"\leftindent{

wc_showcount()}

end menu}

end extend}}


It is this event specification which is invoked by the lines shown above for 
the .XYZinit file.  (As of this writing, this file contains 13993 words.)


\{\{Future: The string after /call ness-load/ may begin with  \concat{~} to 
mean home directory, or without a slash to mean a lookup in the libraries 
 listed in the NessPath preference.\}\}



\section{4.  Calling Ness from C code}


A program in the C language can construct a Ness object, load it with a 
script, and cause its execution.  First create a new instance of a ness 
object:


\leftindent{ness_Ness()  returns a struct ness * pointing to a brand new Ness. 
}



The Ness object is a subclass of text, so all methods of the text data object 
can be used to construct the Ness script.  


If an argument is to be passed to the function main(), a pointer to it must be 
stored into the Ness.  The pointer must refer to a struct nessmark value which 
provides the text of the argument.


\leftindent{ness_SupplyMarkerArg(ness, markerptr)}


The execution environement for a ness program consists of a defaulttext value 
and an arbiter value.  The defaulttext is the one the program will refer to 
with the predefined variable of the same name.  The arbiter is the name 
resolver that Ness will utilize for the \bold{extend} construct and the 
inset() function.  There are two ways to establish this environment.  Either 
variable can be set explicitly, or the EstablishViews method can be called to 
set them both.


\leftindent{ness_SetArbiter(ness, arbiterp)  Sets the pointer to the 
surrounding arbiter.


ness_Set DefaultText(ness, textview)  Sets the pointer to the defaulttext.


ness_EstablishViews(ness, child)  where the child is some view which is a 
child of the appropriate arbiter (or outside all arbiters, in which case it 
gets the default outer arbiter.)  This function first sets the arbiter by 
scanning up the view tree from the child view.  It then uses various 
hueristics to determine what is the defaulttext.}


The ness is compiled automatically before Execution, but can be explicitly 
compiled:


\leftindent{ness_Execute(ness, function-name) returns the error list.  The 
ness is compiled and execution is initiated by calling the named function.  By 
convention, the name is usually "main".  If the return value is NULL, there 
were no errors.


ness_Compile(ness) returns the error list.  The Ness is compiled and not 
executed.  If the return value is NULL, there were no errors.}


To debug errors, it is prefereable to install a nessview (if you have windows, 
a call to ness_Expose(ness) will usually display the ness).  


\leftindent{ness_Expose(ness)  Causes the ness to be displayed if possible.}



If you want the compilation to check for  statements which might damage the 
user's environment, you can set the access level with


	ness_SetAccessLevel(nessobject, integer)


where the access levels are defined in ness.ch:


	ness_codeRed,	/* do not even compiled */

	ness_codeOrange,	/* give compile errors for all modification ops */

	ness_codeYellow,	/* give compile errors for file system ops */

	ness_codeGreen,	/* prompt before first executing */

	ness_codeBlue	/* compile without prompting */

	ness_codeUV	/* no warningNotice and no dialog box */


When a compilation or execution has errors, a list of them is recorded in the 
struct ness object.  Your program can manage this list with ClearError, 
NextError, and PreviousError.  


ness_ClearErrors(ness)  Removes the list of errors; discards their space.


ness_NextError(ness) returns a struct errornode *.  Advances a pointer and 
returns the next errornode on the list.  After visiting the last error, the 
next call returns NULL.  A subsequent call will return the first error again.


ness_PreviousError(ness) returns a struct errornode *.  This function is the 
same as NexError, but it moves the pointer in the opposite direction.


ness_printerrors(ness, file) returns the number of errors.  Prints the errors 
to file.


ness_PrintAllErrors(msg) returns the number of errors found. This 
classprocedure prints and counts all the error messages for all nesses in the 
process.



\section{5. Ness system files}


The files listed below constitute the installation of Ness. 



\bold{/usr/andrew/bin/nessrun}  -  Standalone execution of Ness programs. 


\bold{/usr/andrew/lib/ness}  -  Library of Ness functions.  Also contains 
demos (next item).


\bold{/usr/andrew/lib/ness/demos } -  Demonstrations.  See Ness: A Short 
Tutorial


\bold{/usr/andrew/dlib/atk/}  -  Eleven dynamic objects constitute Ness. They 
are listed here with their superclasses


lex.do		()	lexical analyzers; one for each compilation

lexdef.do		()	classes of lexical analyzer; one for each grammar

ness.do		(text)	the Ness compiler and interpreter

nessmark.do	(mark)	the implementation of Ness markers

nesssym.do	(toksym)	Ness symbol table entries

nessv.do		(scroll)	user interface to ness.do

sym.do		()	generic symbol table entries

toksym.do		(sym)	symbol table entries for lex

type.do		(sym)	parses .ch files and builds tables


\bold{goodness}


The Imakefile for andrew/atk/ness/objects alos includes the target 'goodness.' 
 This program is identical to 'nessrun' except that in goodness many of the 
dynamic objects are statically linked instead of dynamically linked.  This 
difference has two effects:


run time:  The runtime for nessrun isabout 2 seconds longer than goodness, 
primarily for the dynamic loading and linking.


object module size:  The goodness object module is 1411072 bytes unstripped 
and 456704 bytes when stripped.  The nessrun module is 1030144 bytes 
unstripped and reduces to 362496 when stripped.  Here are the sizes of each:

\leftindent{
% \bold{size ../objects/nessrun}

text	data	bss	dec	hex

221184	139264	22700	383148	5d8ac

% \bold{size ../objects/goodness}

text	data	bss	dec	hex

288768	165888	26756	481412	75884}


The maximum memory during nessrun execution on one program was 247K, while it 
was only 228K for goodness on the same program.  It is my guess that goodness 
avoids memory fragmentation by the loaded modules.

\enddata{text,2373192}
