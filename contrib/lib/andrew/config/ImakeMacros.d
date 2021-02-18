\begindata{text,268745564}
\textdsversion{12}
\template{default}
\define{italic
menu:[Font~1,Italic~11]
attr:[FontFace Italic Int Set]}
\define{bold
menu:[Font~1,Bold~10]
attr:[FontFace Bold Int Set]}
\define{chapter
menu:[Title~3,Chapter~20]
attr:[Justification LeftJustified Point 0]
attr:[Flags KeepPriorNL Int Set]
attr:[Flags KeepNextNL Int Set]
attr:[FontFace Bold Int Set]
attr:[FontFamily AndySans Int 0]
attr:[FontSize PreviousFontSize Point 4]}
\define{section
menu:[Title~3,Section~21]
attr:[Justification LeftJustified Point 0]
attr:[Flags KeepPriorNL Int Set]
attr:[Flags KeepNextNL Int Set]
attr:[FontFace Bold Int Set]
attr:[FontFamily AndySans Int 0]
attr:[FontSize PreviousFontSize Point 2]}
\define{subsection
menu:[Title~3,Subsection~22]
attr:[Justification LeftJustified Point 0]
attr:[Flags KeepPriorNL Int Set]
attr:[Flags KeepNextNL Int Set]
attr:[FontFace Bold Int Set]
attr:[FontFamily AndySans Int 0]}
\define{majorheading
menu:[Title~3,MajorHeading~10]
attr:[Justification Centered Point 0]
attr:[Flags KeepPriorNL Int Set]
attr:[Flags KeepNextNL Int Set]
attr:[FontFamily AndySans Int 0]
attr:[FontSize PreviousFontSize Point 4]}
\define{center
menu:[Justify~2,Center~10]
attr:[Justification Centered Point 0]
attr:[Flags KeepPriorNL Int Set]
attr:[Flags KeepNextNL Int Set]}
\define{leftindent
menu:[Region~4,LeftIndent~21]
attr:[LeftMargin LeftMargin Inch 32768]
attr:[Flags KeepPriorNL Int Set]
attr:[Flags KeepNextNL Int Set]}
\define{index
menu:[Title,Index~91]
attr:[FontFace Italic Int Set]}
\define{indexi
menu:[Title,InvisibleIndex~92]
attr:[Script PreviousScriptMovement Point -2]
attr:[FontFace Italic Int Set]}
\define{global

attr:[LeftMargin LeftEdge Int 16]
attr:[Indent LeftMargin Int -16]
attr:[Justification LeftJustified Point 0]
attr:[Flags ContinueIndent Int Set]
attr:[FontFace FixedFace Int Set]
attr:[FontFamily AndyType Int 0]
attr:[FontSize ConstantFontSize Point 10]}
\define{comment

attr:[FontFace Italic Int Set]
attr:[FontFace FixedFace Int Clear]
attr:[FontFamily Andy Int 0]
attr:[FontSize PreviousFontSize Point 2]}
\define{function

attr:[FontFace Bold Int Set]}
\define{fieldheader
menu:[Region~4,FieldHeader]
attr:[LeftMargin LeftMargin Cm 41615]
attr:[FontFace Bold Int Set]
attr:[FontFamily AndySans Int 0]}
\define{fieldcontent
menu:[Region~4,FieldContent]
attr:[LeftMargin LeftMargin Cm 101725]
attr:[Indent LeftMargin Cm -16183]
attr:[FontFamily AndySans Int 0]
attr:[FontSize PreviousFontSize Point -2]}
\define{fixedfieldcontent
menu:[Region~4,FixedFieldContent]
attr:[LeftMargin LeftMargin Cm 101725]
attr:[Indent LeftMargin Cm -16183]
attr:[FontFace FixedFace Int Set]
attr:[FontFamily AndyType Int 0]
attr:[FontSize PreviousFontSize Point -2]}
\majorheading{Imakefile Macros}




\fieldcontent{A guide to the available macros for use within Imakefiles, 
written to support the construction of the Andrew software.  For ease of 
perusal - if you are looking at this document in EZ, use Page/Table Of 
Contents (menu) which will bring up another window.  Clicking on the entries 
in that window will bring that section of the document into view in this 
window.

}\
\begindata{bp,268754692}
\enddata{bp,268754692}
\view{bpv,268754692,0,0,0}
\chapter{1	Notes}


\section{1.1	Notes on installation rules}


\fieldcontent{Most of the installation rules expand in two different ways, 
based upon the value of a #define in andrew/config/andyenv.h - 
LINKINSTALL_ENV.  Most of the macros when using LinkInstall expand into 
another macro which is defined only for internal use (and is described in the 
section on internally used macros below)


Many of the installation rules expect one argument to consist of command line 
flags for the installation program these flags are currently:}


\fixedfieldcontent{        INSTUIDFLAGS = -c -m 4755

        INSTLIBFLAGS = -c -m 0664

        INSTPROGFLAGS = -c -m 0555

        INSTPROGRWFLAGS = -c -m 0755

        INSTINCFLAGS = -c -m 0444

        INSTMANFLAGS = -c -m 0444

        INSTAPPFLAGS = -c -m 0444

        INSTDOFLAGS = -c -m 0444}


\section{1.2	Notes on OS dependent rules}


\fieldcontent{Some of the rules when expanded may vary depending on what 
operating system they are being used on - in these cases the operating systems 
are identified and each version of the expansion is shown.}


\section{1.3	Notes on ATK rules}


\fieldcontent{The NormalATKRule (described below) uses two "magic" arguments, 
in that they are not passed as parameters of the macro.  These arguments are 
the make variables:


\leftindent{\bold{\italic{DOBJS}} - list of all dynamic object (.do) files to 
be made.


\bold{\italic{IHFILES}} - list of all import header (.ih) files to be made.


}}\section{1.4	Notes on Imakefile writing}


\fieldcontent{All macro calls use '(' and ')' to define the list of arguments 
- not '\{' and '\}'

Make variables can be referenced with either '$()' or '$\{\}'

When seperating arguments with commas - you can have a space after the comma 
but not before - i.e. (foo, bar) is ok, but (foo ,bar) is not

Imakefiles MUST end with a newline}

\begindata{bp,268831896}
\enddata{bp,268831896}
\view{bpv,268831896,1,0,0}
\chapter{2	Standard and often used Make rules}


\section{2.1	Default dependency rules}


\subsection{2.1.1	NormalObjectRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalObjectRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{.c.o:

	$(RM) $@

	$(CC) -c -I. $(LOCALINCLUDES) $(CFLAGS) $*.c}


\fieldheader{Use:}

\fieldcontent{Use whenever you have .c files which will need to be compiled 
into .o files - and eventually into either a .a file or binary (a.out)}


\subsection{2.1.2	NormalHelpRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalHelpRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fieldcontent{NOTE: if CMU_ENV is set - COMMON = "/afs/.andrew.cmu.edu/common" 
- otherwise it = ""

}
\fixedfieldcontent{install.alias:: install.doc do.alias

	@sh -c 'if [ -f ,help.alias ]; then \\

		$(ADDALIASES) ,help.alias \\ 		$(COMMON)/$(DESTDIR)/help.aliases ; \\

		fi; \\

	exit 0'

	

do.alias::

	@$(RM) ,help.alias

}		

\fieldheader{Use:}

\fieldcontent{Use only in conjunction with InstallHelpAlias() rule}


\begindata{bp,268720388}
\enddata{bp,268720388}
\view{bpv,268720388,2,0,0}
\section{2.2	Compilation rules}


\subsection{2.2.1	ProgramTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{ProgramTarget(program, objs, libs, syslibs)}


\fieldheader{Arguments:}

\fieldcontent{program - name to be used for binary file (with extension if 
any)

objs - space separated list of object files (*.o)

libs - libraries (*.a) which can be placed on a dependency list

syslibs - libraries (*.a) which are not to be placed on a dependency list

(most common example: -lm   for the math library)}


\fieldheader{Note:}

\fieldcontent{All arguments need not be present - but the commas must be 
there, and the arguments which are to be filled in should be in the correct 
place with respect to the commas.

The syslibs can include library path search specifications using -L <lib-path> 
interspersed with the -l<x>.<a> notation  (see ld.1). }


\fieldheader{Expanded Macro:}

\fixedfieldcontent{all:: program


program: objs libs

	$(RM) $@

	$(CC) $(CFLAGS) -o $@ objs libs syslibs


clean::

	$(RM) program}


\fieldheader{Use:}



\begindata{bp,268831800}
\enddata{bp,268831800}
\view{bpv,268831800,3,0,0}
\subsection{2.2.2	LibraryTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{LibraryTarget(library, objs)}


\fieldheader{Arguments:}

\fieldcontent{library - name of final archive library (.a) file (with 
extension)

objs - space separated list of .o files}


\fieldheader{Expanded Macro:}

\fieldcontent{On most systems - }

\fixedfieldcontent{all:: library


library: objs

	$(RM) $@

	$(AR) $@ objs

	$(RANLIB) $@}


\fieldcontent{On systems such as AIX and HPUX 'ranlib' is not used so it 
expands to - }

\fixedfieldcontent{all:: library


library: objs

	$(RM) $@

	$(AR) $@ objs

}
\fieldheader{Use:}

\fieldcontent{Use whenever a library archive file (*.a) is to be generated 
from a list of object files (*.o)}



\begindata{bp,268720240}
\enddata{bp,268720240}
\view{bpv,268720240,4,0,0}
\subsection{2.2.3	TestProgramTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{TestProgramTarget(program, objs, libs, syslibs)}


\fieldheader{Arguments:}

\fieldcontent{program - name to be used for binary file (with extension if 
any)

objs - space separated list of object files (*.o)

libs - libraries (*.a) which can be placed on a dependency list

\smaller{syslibs - libraries (*.a) which are not to be placed on a dependency 
list (most common example: -lm   for the math library).

}
}	

	\fieldheader{Note:}

\fieldcontent{All arguments need not be present - but the commas must be 
there, and the arguments which are to be filled in should be in the correct 
place with respect to the commas.

The syslibs can include library path search specifications using -L <lib-path> 
interspersed with the -l<x>.<a> notation  (see ld.1).  

}
\fieldheader{Expanded Macro:}

\fixedfieldcontent{test:: program


program: objs libs

	$(RM) $@

	$(CC) $(CFLAGS) -o $@ objs libs syslibs


clean::

	$(RM) program}


\fieldheader{Use:}


\begindata{bp,268720172}
\enddata{bp,268720172}
\view{bpv,268720172,5,0,0}
\section{2.3	Cleanup and dependency generating rules}


\subsection{2.3.1	CleanTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{CleanTarget(files)}


\fieldheader{Arguments:}

\fieldcontent{files - space separated list of files}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{clean::

	$(RM) files}


\fieldheader{Use:}

\fieldcontent{Use in directories that produce normal binary files (not .do or 
.a files) and/or directories that might have temporary files generated on the 
fly during compilation that would not otherwise get cleaned up by the 
GenericCleanTarget macro (an internally used macro described below)


}
\subsection{2.3.2	DependTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{DependTarget(dependencies)}


\fieldheader{Arguments:}

\fieldcontent{[optional] dependencies - space separated list of Make 
dependencies (i.e. all::, install::, etc.) to take care of before generating 
the dependency information for the directory}


\fieldheader{Expanded Macro:}

\fieldcontent{if AFS_ENV is defined:

}
\fixedfieldcontent{depend:: dependencies

	$(DEPENDSCRIPT) $(XMAKEDEPEND) $(BASEDIR) $(XINCDIR) \\

	"$(CC)" $(CLASS) "-I. $(LOCALINCLUDES) $(INCLUDES)" $\{AFSBASEDIR\}}


\fieldcontent{if AFS_ENV is not defined:

}
\fixedfieldcontent{depend:: dependencies

	$(DEPENDSCRIPT) $(XMAKEDEPEND) $(BASEDIR) $(XINCDIR) \\

	"$(CC)" $(CLASS) "-I. $(LOCALINCLUDES) $(INCLUDES)"}


\fieldheader{Use:}

\fieldcontent{Handy if symbolic links are setup during compilation to handle 
things like OS-type differences}


\begindata{bp,268720104}
\enddata{bp,268720104}
\view{bpv,268720104,6,0,0}
\subsection{2.3.3	MkdirTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{MkdirTarget(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - list of directories to be created - must be in proper 
order of creation}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{install.time:: makedirs

	@echo -n ''

install.doc:: makedirs	

	@echo -n ''

makedirs::

	@sh -c 'for i in dirs;  do  \\

		if [ -f $$i ]; then  \\

			echo $$i is a file; \\

			exit 1; \\

		elif [ ! -d $$i ]; then  \\

			echo Making the directory $$i; \\

			mkdir $$i; \\

		fi; \\

	done; \\

	exit 0'

}

\fieldheader{Use:}

\fieldcontent{For making destination directories, avoiding most typos - and 
avoiding the run-time complaints that mkdir puts out}



\begindata{bp,268720036}
\enddata{bp,268720036}
\view{bpv,268720036,7,0,0}
\section{2.4	Installation rules}


\subsection{2.4.1	InstallFile}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallFile(file, flags, dest)}


\fieldheader{Arguments:}

\fieldcontent{file - the file to be installed

flags - one of the flags listed above

dest - the destination directory only (no filename)}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: file

	$(INSTALL) flags file dest}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{LinkInstallFile(file, dest/file)}


\fieldheader{Use:}

\fieldcontent{Very flexible - could be used for installing an include file, a 
shell-script, a program, a help file, etc. }



\subsection{2.4.2	InstallFileToFile}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallFileToFile(file, flags, destfile)}


\fieldheader{Arguments:}

\fieldcontent{file - the file to be installed

flags - of the the flags listed above

destfile - the destination directory with the name of the file as it is to be 
installed (i.e. <dir>/<file>)}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: file

	$(INSTALL) flags file destfile

}
\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{LinkInstallFile(file, destfile)}


\fieldheader{Use:}

\fieldcontent{Must be used if you want to install a particular file with a 
different name than what it already has.}


\begindata{bp,268719968}
\enddata{bp,268719968}
\view{bpv,268719968,8,0,0}
\subsection{2.4.3	InstallDocs}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallDocs(files, dest)}


\fieldheader{Arguments:}

\fieldcontent{files - a space separated list of documentation files to be 
installed

dest - the path of the destination directory}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.doc:: files

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

	    (set -x; $(INSTALL) $(INSTINCFLAGS) $$i dest); \\

	done}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{install.doc:: files

	LinkInstallMultiFiles(files, dest)

}
\fieldheader{Use:}

\fieldcontent{For installing documentation (docs, help files, man pages, etc)}



\subsection{2.4.4	InstallHelpAlias}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallHelpAlias(file, aliases)}


\fieldheader{Arguments:}

\fieldcontent{file - name of help file (sans extension)

aliases - space separated list of aliases for the file}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{do.alias::

  @case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

  PWD=`pwd` ; \\

  for i in aliases; do \\

    (set -x; echo "$$i	file	!$$PWD" >> ,help.alias ;) \\

  done}


\fieldheader{Use:}

\fieldcontent{For creating the help.alias file used by the help program.

}

\begindata{bp,268823220}
\enddata{bp,268823220}
\view{bpv,268823220,9,0,0}
\subsection{2.4.5	InstallMultiple}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallMultiple(list, flags, dest)}


\fieldheader{Arguments:}

\fieldcontent{list - space separated list of files to install

flags - install flags to use for the installation

dest - the path of the destination directory}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: list

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

		(set -x; $(INSTALL) flags $$i dest); \\

	done}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{install.time:: list

	LinkInstallMultiFiles(list, dest)

}
\fieldheader{Use:}

\fieldcontent{For installing multiple files all in the same directory}



\begindata{bp,268719872}
\enddata{bp,268719872}
\view{bpv,268719872,10,0,0}
\subsection{2.4.6	InstallByPattern}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallByPattern(pattern, flags, dest)}


\fieldheader{Arguments:}

\fieldcontent{pattern - a simple regular expression pattern

flags - installation flags

dest - the path of the destination directory}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time::

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in pattern; do \\

		(set -x; $(INSTALL) flags $$i dest); \\

	done}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{install.time::

	LinkInstallMultiFiles(pattern, dest)

}
\fieldheader{Use:}

\fieldcontent{For installing various files that all share a common pattern in 
their filenames.

Unfortunately, this is not a very good macro as there is no way to do the 
actual installation based upon time-stamp information.  So if used, it means 
that all the files that match that pattern will be installed every time the 
directory is compiled - whether or no they have changed since the last time 
they were installed.}



\subsection{2.4.7	InstallProgram}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallProgram(file, dest)}


\fieldheader{Arguments:}

\fieldcontent{file - the file to be installed

dest - the path of the destination directory possibly including a new name for 
the file to be installed as}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: file

	$(INSTALL) $(INSTPROGFLAGS) file dest}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{LinkInstallFile(file, dest/file)}

\fieldheader{Use:}

\fieldcontent{For installing programs (binaries or shell scripts) read-only, 
executable}



\subsection{2.4.8	InstallProgramRW}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallProgramRW(file, dest)}


\fieldheader{Arguments:}

\fieldcontent{file - the file to be installed

dest - the path of the destination directory possibly including a new name for 
the file to be installed as}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: file

	$(INSTALL) $(INSTPROGRWFLAGS) file dest}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{LinkInstallFile(file, dest/file)}


\fieldheader{Use:}

\fieldcontent{For installing programs (binaries or shell scripts) read-write, 
executable

}

\begindata{bp,268823100}
\enddata{bp,268823100}
\view{bpv,268823100,11,0,0}
\subsection{2.4.9	InstallLibrary}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallLibrary(file, dest)}


\fieldheader{Arguments:}

\fieldcontent{file - the name of the library archive (.a) file (with 
extension)

dest - the path of the destination directory possibly including a new name to 
install the library as}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: file

	$(INSTALL) $(INSTLIBFLAGS) file dest

	$(RANLIB) dest/file}


\fieldcontent{If this is either an AIX or an HPUX machine we do not need to 
use 'ranlib' so it expands to:}

\fixedfieldcontent{install.time:: file

	$(INSTALL) $(INSTLIBFLAGS) file dest}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{LinkInstallFile(file, dest/file)}


\fieldheader{Use:}

\fieldcontent{For installing library archive (.a) files}



\subsection{2.4.10	InstallLink}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallLink(file, link)}


\fieldheader{Arguments:}

\fieldcontent{file - name of actual existing file (may include path)

link - name of link to be created (may include path)}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{install.time::

	$(RM) link

	$(LN) file link

}
\fieldheader{Use:}

\fieldcontent{Generally for installing symbolic links to point to other files}



\begindata{bp,268719776}
\enddata{bp,268719776}
\view{bpv,268719776,12,0,0}
\subsection{2.4.11	InstallHardLink}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallHardLink(file, link)}


\fieldheader{Arguments:}

\fieldcontent{file - name of existing file

link - name of link to be created}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{install.time::

	$(RM) link

	ln file link

}
\fieldheader{Use:}

\fieldcontent{For installing hard links to other files}



\subsection{2.4.12	InstallMultipleByExtension}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallMultipleByExtension(extension, flags, dest)}


\fieldheader{Arguments:}

\fieldcontent{extension - the extensions to look for (sans '.' i.e. 'help' for 
'*.help')}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{	InstallByPattern(*.extension, flags, dest)

}
\fieldheader{Use:}

\fieldcontent{Useful for installing things like documentation or fonts.

Unfortunately it is not possible to do the installations based on time-stamp 
information}



\begindata{bp,268822980}
\enddata{bp,268822980}
\view{bpv,268822980,13,0,0}
\chapter{3	Less standard, or less often used Make rules}


\section{3.1	Default dependency rules}


\subsection{3.1.1	NormalAsmRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalAsmRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{.s.o:

	-$(RM) $@

	$(AS) -o $*.o $*.s}


\fieldheader{Use:}


\subsection{3.1.2	NormalYaccRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalYaccRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{.y.o:

	$(YACC) $*.y

	-$(RM) $*.c

	$(MV) y.tab.c $*.c

	$(CC) -c -I. $(LOCALINCLUDES) $(CFLAGS) $*.c

	$(RM) $*.c

}
\fieldheader{Use:}



\begindata{bp,268844668}
\enddata{bp,268844668}
\view{bpv,268844668,14,0,0}
\subsection{3.1.3	NormalAsmPPRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalAsmPPRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{.spp.o:

	-$(RM) $@ ,$*.s ,$*.c

	$(CP) $*.spp ,$*.c

	$(CC) -E -I. $(LOCALINCLUDES) $(CFLAGS) ,$*.c > ,$*.s

	$(AS) -o $*.o ,$*.s

	$(RM) ,$*.c ,$*.s}


\fieldheader{Use:}



\subsection{3.1.4	NormalLexRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalLexRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{.l.o:

	$(LEX) $*.l

	-$(RM) $*.c

	$(MV) lex.yy.c $*.c

	$(CC) -c -I. $(LOCALINCLUDES) $(CFLAGS) $*.c

	$(RM) $*.c

}
\fieldheader{Use:}



\begindata{bp,268719632}
\enddata{bp,268719632}
\view{bpv,268719632,15,0,0}
\subsection{3.1.5	NormalScribeRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalScribeRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{.mss.PS:

	$(SCRIBE) $*.mss}


\fieldheader{Use:}

\fieldcontent{To produce PostScript files from Scribe manuscript (.mss) files 
- useful for printed documentation.}


\begindata{bp,268719588}
\enddata{bp,268719588}
\view{bpv,268719588,16,0,0}
\section{3.2	Compilation rules}


\subsection{3.2.1	YaccWithHeader}


\fieldheader{Syntax:}

\fixedfieldcontent{YaccWithHeader(file)}


\fieldheader{Arguments:}

\fieldcontent{file - name of the file.y to be processed with yacc (sans 
extension)}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{file.o file.h: file.y

	$(YACC) -d file.y

	-$(RM) file.c file.h

	$(MV) y.tab.c file.c

	$(MV) y.tab.h file.h

	$(CC) -c -I. $(LOCALINCLUDES) $(CFLAGS) file.c

	$(RM) file.c


tidy::

	$(RM) file.c

clean::

	$(RM) file.h}


\fieldheader{Use:}



\begindata{bp,268719544}
\enddata{bp,268719544}
\view{bpv,268719544,17,0,0}
\subsection{3.2.2	YaccWithReplacement}


\fieldheader{Syntax:}

\fixedfieldcontent{YaccWithReplacement(file, string)}


\fieldheader{Arguments:}

\fieldcontent{file - name of file.y to be processed with yacc (sans extension)

string - character string (quoted?)  to use in 'sed' command}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{file.o file.h: file.y

	$(YACC) -d file.y

	-$(RM) file.c file.h y.tab._c

	$(SED) -e '/yy/s//string/g' y.tab.c > y.tab._c

	-$(RM) y.tab.c

	$(MV) y.tab._c file.c

	$(MV) y.tab.h file.h

	$(CC) -c -I. $(LOCALINCLUDES) $(CFLAGS) file.c


tidy::

	$(RM) file.c

clean::

	$(RM) file.h}


\fieldheader{Use:}



\subsection{3.2.3	LexWithReplacement}


\fieldheader{Syntax:}

\fixedfieldcontent{LexWithReplacement(file, string)}


\fieldheader{Arguments:}

\fieldcontent{file - name of file.l to be processed with lex (sans extension)

string - character string (quoted?) to use in 'sed' command}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{file.o: file.l

	$(LEX) $*.l

	-$(RM) $*.c

	$(SED) -e '/yy/s//string/g' lex.yy.c > lex.yy._c

	-$(RM) lex.yy.c

	$(MV) lex.yy._c file.c

	$(CC) -c -I. $(LOCALINCLUDES) $(CFLAGS) file.c


tidy::

	$(RM) file.c}


\fieldheader{Use:}


\begindata{bp,268719500}
\enddata{bp,268719500}
\view{bpv,268719500,18,0,0}

\subsection{3.2.4	NCYaccWithHeader}


\fieldheader{Syntax:}

\fixedfieldcontent{NCYaccWithHeader(yfile)}


\fieldheader{Arguments:}

\fieldcontent{yfile - yacc file

}
\fieldheader{Expanded Macro:}

\fixedfieldcontent{y.tab.c y.tab.h: yfile

	$(RM) y.tab.c y.tab.h

	$(YACC) -d yfile

}
\fieldheader{Use:}

\fieldcontent{For simple yacc processing - NC = Not Compiled.

This does a fraction of what YaccWithHeader() does.

}

\subsection{3.2.5	NCLex}


\fieldheader{Syntax:}

\fixedfieldcontent{NCLex(lfile)}


\fieldheader{Arguments:}

\fieldcontent{lfile - lex file

}
\fieldheader{Expanded Macro:}

\fixedfieldcontent{lex.yy.c: lfile

	$(RM) lex.yy.c

	$(LEX) lfile

}
\fieldheader{Use:}

\fieldcontent{For simple lex processing - NC = Not Compiled.

}
\begindata{bp,268719456}
\enddata{bp,268719456}
\view{bpv,268719456,19,0,0}
\subsection{3.2.6	CppAndFilter}


\fieldheader{Syntax:}

\fixedfieldcontent{CppAndFilter(file, lyfile, incfiles, filter)

}
\fieldheader{Arguments:}

\fieldcontent{file - target filename (with extension)

lyfile - a lex or yacc file to be processed with cpp

incfiles - include files that are pulled in during processing

filter - process to run cpp output through before inserting into file

}
\fieldheader{Expanded Macro:}

\fixedfieldcontent{file: lyfile incfiles

	$(RM) file

	$(CPP) lyfile | filter > file

}
\fieldheader{Use:}

\fieldcontent{Target is a generated C file.  lyfile is the output of Yacc or 
Lex; namely, y.tab.c or lex.yy.c.  Inclfiles are the include files on which 
lyfile depends, and filter is a program through which the filter the CPP 
output.

}

\subsection{3.2.7	AppendFiles}


\fieldheader{Syntax:}

\fixedfieldcontent{AppendFiles(target, sources)

}
\fieldheader{Arguments:}

\fieldcontent{target - file to create (with extension, if any)

sources - files to concatenate into target

}
\fieldheader{Expanded Macro:}

\fixedfieldcontent{target: sources

	$(RM) target

	cat sources > target

}
\fieldheader{Use:}

\fieldcontent{Target is any old filename and sources is a list of any old 
filenames.  This simply cats them together.

}
\begindata{bp,268719412}
\enddata{bp,268719412}
\view{bpv,268719412,20,0,0}
\subsection{3.2.8	TagsTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{TagsTarget()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{tags::

	$(TAGS) -w *.[ch]

	$(TAGS) -xw *.[ch] > TAGS}


\fieldheader{Use:}




\begindata{bp,268719368}
\enddata{bp,268719368}
\view{bpv,268719368,21,0,0}
\chapter{4	ATK compilation rules}


\section{4.1	Default dependency rules}


\subsection{4.1.1	NormalATKRule}


\fieldheader{Syntax:}

\fixedfieldcontent{NormalATKRule()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{all:: $(IHFILES)


.ch.ih:

	$(CLASS) -s -I. $(LOCALINCLUDES) $(CLASSFLAGS) $*.ch


.ch.eh:

	$(CLASS) -s -I. $(LOCALINCLUDES) $(CLASSFLAGS) $*.ch


.o.do:

	$(MAKEDO) $(MAKEDOFLAGS) -o $*.do $*.o


.c.do:

	$(RM) $*.do

	$(CC) -c -I. $(LOCALINCLUDES) $(CFLAGS) $*.c

	$(MAKEDO) $(MAKEDOFLAGS) -i $*.do $*.o}


\fieldheader{Use:}

\fieldcontent{When compiling any ATK sources that will produce .ih, .eh, 
and/or .do files}


\begindata{bp,268719324}
\enddata{bp,268719324}
\view{bpv,268719324,22,0,0}
\section{4.2	Compilation rules}


\subsection{4.2.1	DynamicObject}


\fieldheader{Syntax:}

\fixedfieldcontent{DynamicObject(dobj, libs, syslibs)}


\fieldheader{Arguments:}

\fieldcontent{dobj - name of dynamic object .do file (sans extension)

libs - space separated list of library (.a) files

syslibs - space separated list of system library files

(most common use: -lm for math library)}


\fieldheader{Note:}

\fieldcontent{All arguments need not be present - but the commas must be 
there, and the arguments which are to be filled in should be in the correct 
place with respect to the commas.}

\fieldcontent{The syslibs can include library path search specifications using 
-L <lib-path> interspersed with the -l<x>.<a> notation  (see ld.1).  }


\fieldheader{Expanded Macro:}

\fixedfieldcontent{all:: dobj.do


dobj.do: dobj.o libs

	$(MAKEDO) $(MAKEDOFLAGS) -o dobj.do \\

	dobj.o libs syslibs}


\fieldheader{Use:}

\fieldcontent{Use for creating dynamic object files that are dependent upon 
only one object (.o) file which has the same name, sans extension, as the 
resulting dynamic object (.do) file}



\begindata{bp,268719280}
\enddata{bp,268719280}
\view{bpv,268719280,23,0,0}
\subsection{4.2.2	DynamicMultiObject}


\fieldheader{Syntax:}

\fixedfieldcontent{DynamicMultiObject(dobj, objs, libs, syslibs)}


\fieldheader{Arguments:}

\fieldcontent{dobj - final name for resulting dynamic object (with extension)

objs - space separated list of object (.o) files

libs - space separated list of library (.a) files which dobj can be dependent 
upon

syslibs - space separated list of system library files to be linked against

(most common use: -lm  for the math library)}


\fieldheader{Note:}

\fieldcontent{All arguments need not be present - but the commas must be 
there, and the arguments which are to be filled in should be in the correct 
place with respect to the commas.}

\fieldcontent{The syslibs can include library path search specifications using 
-L <lib-path> interspersed with the -l<x>.<a> notation  (see ld.1).  }


\fieldheader{Expanded Macro:}

\fixedfieldcontent{all:: dobj


dobj: objs libs

	$(MAKEDO) $(MAKEDOFLAGS) -o dobj objs libs syslibs}


\fieldheader{Use:}

\fieldcontent{Use for creating dynamic object files that are dependent upon 
several object (.o) files and/or library (.a) files and/or system library 
files.}




\begindata{bp,268719236}
\enddata{bp,268719236}
\view{bpv,268719236,24,0,0}
\section{4.3	Installation rules}


\subsection{4.3.1	InstallClassFiles}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallClassFiles(dolist, ihlist)}


\fieldheader{Arguments:}

\fieldcontent{dolist - list of dynamic object (.do) files

ihlist - list of import header (.ih) files corresponding dolist}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: dolist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

	    (set -x; $(INSTALL) $(INSTDOFLAGS) $$i \\

	    $(DESTDIR)/dlib/atk ;)\\

	done

	$(BASEDIR)/bin/doindex -d$(DESTDIR)/dlib/atk dolist


install.time:: ihlist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

	    (set -x; \\

	    $(INSTALL) $(INSTINCFLAGS) `basename $$i .ih`.ch \\

	    $(DESTDIR)/include/atk ; \\

	    $(INSTALL) $(INSTINCFLAGS) $$i \\

	    $(DESTDIR)/include/atk ) \\

	done}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{install.time:: dolist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

	    (set -x; $(RM) $(DESTDIR)/dlib/atk/$$i ;\\

	    $(LN) `pwd`/$$i $(DESTDIR)/dlib/atk ;)\\

	done

	$(BASEDIR)/bin/doindex -d$(DESTDIR)/dlib/atk dolist


install.time:: ihlist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

	    (set -x; $(RM) $(DESTDIR)/include/atk/$$i ;\\

	    $(LN) `pwd`/$$i $(DESTDIR)/include/atk ;\\

	    $(RM) $(DESTDIR)/include/atk/`basename $$i .ih`.ch ;\\

	    $(LN) `pwd`/`basename $$i .ih`.ch \\

		$(DESTDIR)/include/atk ;)\\

	done

}
\fieldheader{Use:}

\fieldcontent{Use for installing dynamic object (.do) files and associated 
class header (.ch) and import header (.ih) files}



\subsection{4.3.2	InstallExampleClassFiles}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallExampleClassFiles(dolist, ihlist, dest)}


\fieldheader{Arguments:}

\fieldcontent{dolist -list of dynamic object (.do) files

ihlist - list of import header (.ih) files corresponding to dolist}


\fieldheader{Expanded Macro:}

\fieldcontent{1) Without LinkInstall}

\fixedfieldcontent{install.time:: dolist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

	    (set -x; $(INSTALL) $(INSTDOFLAGS) $$i dest ;\\

	done

	$(BASEDIR)/bin/doindex -d dest dolist


install.time:: ihlist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

	    (set -x; \\

	    $(INSTALL) $(INSTINCFLAGS) `basename $$i .ih`.c \\

	    dest ;\\

	    $(INSTALL) $(INSTINCFLAGS) `basename $$i .ih`.ch \\

	    dest ;\\

	    $(INSTALL) $(INSTINCFLAGS) $$i dest ;\\

	    $(INSTALL) $(INSTINCFLAGS) `basename $$i .ih`.eh \\

	    dest ;\\

	    $(INSTALL) $(INSTINCFLAGS) Imakefile \\

	    dest )\\

	done}


\fieldcontent{2) With LinkInstall}

\fixedfieldcontent{install.time:: dolist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

		(set -x; $(RM) dest/$$i ;\\

		$(LN) `pwd`/$$i dest ;)\\

	done

	$(BASEDIR)/bin/doindex -d dest dolist


install.time:: ihlist

	@case '$(MFLAGS)' in *[i]*) set +e;; esac; \\

	for i in $?; do \\

		(set -x; $(RM) dest$$i ;\\

		$(LN) `pwd`/$$i dest ;\\

		$(RM) dest/`basename $$i .ih`.ch ;\\

		$(LN) `pwd`/`basename $$i .ih`.ch dest ;\\

		$(RM) dest/`basename $$i .ih`.eh ;\\

		$(LN) `pwd`/`basename $$i .ih`.eh dest ;\\

		$(RM) dest/`basename $$i .ih`.c ;\\

		$(LN) `pwd`/`basename $$i .ih`.c dest ;\\

		$(RM) dest/Imakefile ;\\

		$(LN) `pwd`/Imakefile dest )\\

	done

}
\fieldheader{Use:}

\fieldcontent{For installing the andrew/atk/examples/ex* directories which 
basically means installing the source, .[ie]h files, and .do files all into 
the same destination directory (a separate one for each ex* directory)

}
\begindata{bp,268719176}
\enddata{bp,268719176}
\view{bpv,268719176,25,0,0}
\chapter{5	Font rules

}
\section{5.1	Default dependency rules}


\subsection{5.1.1	DeclareFont}


\fieldheader{Syntax:}

\fixedfieldcontent{DeclareFont(fontname)}


\fieldheader{Arguments:}

\fieldcontent{fontname - name of font file (sans extension)}


\fieldheader{Expanded Macro:}

\fieldcontent{This macro has many different forms depending upon the 
configurations for which the fonts are being built.}


\fieldcontent{A) for X11}

\fieldcontent{1) for AIX

a) on PS/2 machines}

\fixedfieldcontent{\leftindent{all:: fontname.snf

install.time:: fontname.snf

	$(INSTALL) $(INSTINCFLAGS) fontname.snf $(DESTDIR)/X11fonts}}


\fieldcontent{b) on PC-RT machines}

\fixedfieldcontent{\leftindent{all:: fontname.rtx

instal.time:: fontname.rtx}}

\fixedfieldcontent{\leftindent{	$(INSTALL) $(INSTINCFLAGS) fontname.rtx 
$(DESTDIR)/X11fonts}}


\fieldcontent{2) supporting both X11 and WM (non AIX)}

\fixedfieldcontent{\leftindent{all:: fontname.snf fontname.fwm

instal.time:: fontname.snf

	$(INSTALL) $(INSTINCFLAGS) fontname.snf $(DESTDIR)/X11fonts

isntall.time:: fontname.fwm

	$(INSTALL) $(INSTINCFLAGS) fontname.fwm $(DESTDIR)/fonts

}}

\fieldcontent{3) X11 only (non AIX)}

\fixedfieldcontent{\leftindent{all:: fontname.snf

instal.time:: fontname.snf

	$(INSTALL) $(INSTINCFLAGS) fontname.snf $(DESTDIR)/X11fonts

}}
\fieldcontent{B) for WM only}

\fixedfieldcontent{\leftindent{all:: fontname.fwm

isntall.time:: fontname.fwm

	$(INSTALL) $(INSTINCFLAGS) fontname.fwm $(DESTDIR)/fonts

}}

\fieldheader{Use:}

\fieldcontent{For Declaring font dependencies and installation rules (Replaces 
InstallWMFonts and InstallX11Fonts)

(Installs font as a link if LINKINSTALL_ENV is defined)}


\subsection{5.1.2	NSDeclareFont}


\fieldheader{Syntax:}

\fixedfieldcontent{NSDeclareFont(fontname)}


\fieldheader{Arguments:}

\fieldcontent{fontname - name of font file (sans extension)}


\fieldheader{Expanded Macro:}

\fieldcontent{1) for WM

}\fixedfieldcontent{\leftindent{all:: fontname.fwm

fontname.fwm: fontname.fdb

	$(FDBWM) -g -F fontname.fdb

install.time:: fontname.fwm

	$(INSTALL) $(INSTINCFLAGS) fontname.fwm $(DESTDIR)/fonts}}


\fieldcontent{2) for non-WM}

\fixedfieldcontent{\leftindent{DeclareFont(fontname)

}}
\fieldheader{Use:}

\fieldcontent{For Declaring a [wm] font  to be generated without being 
stripped of extra whitespace (Replaces WMNoStripFontRule)

(Installs font as a link if LINKINSTALL_ENV is defined)}

\begindata{bp,268822356}
\enddata{bp,268822356}
\view{bpv,268822356,26,0,0}
\section{5.2	Compilation rules}


\subsection{5.2.1	FontRule}


\fieldheader{Syntax:}

\fixedfieldcontent{FontRule()}


\fieldheader{Arguments:}

\fieldcontent{fontname - name of font file (sans extension)}


\fieldheader{Expanded Macro:}

\fieldcontent{This macro has many different forms depending upon the 
configurations for which the fonts are being built.}


\fieldcontent{A) for X11}

\fieldcontent{1) for AIX

a) on PS/2 machines}

\fixedfieldcontent{.fdb.snf:

	$(SED) -e 's/^$$spacing \\(.*\\),.*$$/$$spacing \\1,0/' $*.fdb > /tmp/$*.tfdb

	$(FDBBDF) /tmp/$*.tfdb >/tmp/$*.bdf

	$(XFC) /tmp/$*.bdf >$*.snf

	$(RM) /tmp/$*.bdf /tmp/$*.tfdb}


\fieldcontent{b) on PC-RT machines}

\fixedfieldcontent{.fdb.rtx:

	$(SED) -e 's/^$$spacing \\(.*\\),.*$$/$$spacing \\1,0/' $*.fdb > /tmp/$*.tfdb

	$(FDBBDF) /tmp/$*.tfdb >/tmp/$*.bdf

	$(XFC) /tmp/$*.bdf $*.rtx

	$(RM) /tmp/$*.bdf /tmp/$*.tfdb

}
\fieldcontent{2) supporting both X11 and WM (non AIX)}

\fixedfieldcontent{.fdb.snf:

	$(SED) -e 's/^$$spacing \\(.*\\),.*$$/$$spacing \\1,0/' $*.fdb > /tmp/$*.tfdb

	$(FDBBDF) /tmp/$*.tfdb >/tmp/$*.bdf

	$(XFC) /tmp/$*.bdf >$*.snf

	$(RM) /tmp/$*.bdf /tmp/$*.tfdb}


\fixedfieldcontent{.fdb.fwm: ; $(FDBWM) -O -F $*.fdb}


\fieldcontent{3) X11 only (non AIX)}

\fixedfieldcontent{.fdb.snf:

	$(SED) -e 's/^$$spacing \\(.*\\),.*$$/$$spacing \\1,0/' $*.fdb > /tmp/$*.tfdb

	$(FDBBDF) /tmp/$*.tfdb >/tmp/$*.bdf

	$(XFC) /tmp/$*.bdf >$*.snf

	$(RM) /tmp/$*.bdf /tmp/$*.tfdb

}
\fieldcontent{B) for WM only}

\fixedfieldcontent{.fdb.fwm: ; $(FDBWM) -O -F $*.fdb}



\fieldheader{Use:}

\fieldcontent{Whenever your are going to be compiling fonts

(replaces WMFontRule and XFontRule)

}
\begindata{bp,268822312}
\enddata{bp,268822312}
\view{bpv,268822312,27,0,0}
\chapter{6	Internal (to template) rules}


\section{6.1	LinkInstall rules}


\subsection{6.1.1	LinkInstallMultiFiles}


\fieldheader{Syntax:}

\fixedfieldcontent{LinkInstallMultiFiles(pattern, dest)}


\fieldheader{Arguments:}

\fieldcontent{pattern - a space separated list of patterns or files

dest - the path of the directory in which to link the files to}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{	$(BASEDIR)/bin/linkinst "$(RM)" "$(LN)" "pattern" "dest"}


\fieldheader{Use:}

\fieldcontent{Used only when LINKINSTALL_ENV is set}



\subsection{6.1.2	LinkInstallFile}


\fieldheader{Syntax:}

\fixedfieldcontent{LinkInstallFile(file, destfile)}


\fieldheader{Arguments:}

\fieldcontent{file - the name of a file (with extension)

dest - the path of the directory to link it to}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{install.time:: file

	$(RM) destfile

	$(LN) `pwd`/file destfile}


\fieldheader{Use:}

\fieldcontent{Used only when LINKINSTALL_ENV is set}


\begindata{bp,268822268}
\enddata{bp,268822268}
\view{bpv,268822268,28,0,0}
\section{6.2	Cleanup rule}


\subsection{6.2.1	GenericCleanTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{GenericCleanTarget()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{tidy::

	$(RM) \\#* ,* *~ *.CKP *.BAK *.bas errs core

	$(RM) *.ln *.o *.dog tags TAGS make.log

clean::

	$(RM) *.do *.eh *.ih *.a *.fwm *.snf *.rtx

	$(RM) install.time install.doc

}
\fieldheader{Use:}


\section{6.3	Makefile rule}


\subsection{6.3.1	MakefileTarget}


\fieldheader{Syntax:}

\fixedfieldcontent{MakefileTarget()}


\fieldheader{Arguments:}

\fieldcontent{None}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{Makefile:: Imakefile \\

	$(IRULESRC)/andyenv.h \\

	$(IRULESRC)/imake.tmpl \\

	$(IRULESRC)/andrew.rls \\

	$(IRULESRC)/allsys.h \\

	$(IRULESRC)/allsys.mcr \\

	$(IRULESRC)/site.h \\

	$(IRULESRC)/site.mcr \\

	$(IRULESRC)/$(SYSTEM_H_FILE) \\

	$(IRULESRC)/$(MACROFILE)

		$(IMAKE_CMD) -DTOPDIR=$(TOP) ; \\

		touch .depends ; \\

		cat .depends >>Makefile}


\fieldheader{Use:}


\begindata{bp,268822224}
\enddata{bp,268822224}
\view{bpv,268822224,29,0,0}
\section{6.4	Subdir rules}


\subsection{6.4.1	MakeSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{MakeSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{All::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i ; echo "making (`pwd`)"; \\

			$(MAKE) $(MFLAGS)); \\

	done}


\fieldheader{Use:}



\subsection{6.4.2	DependMakeSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{DependMakeSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{dependAll::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i ; echo "depend/making (`pwd`)"; \\

			$(MAKE) $(MFLAGS) dependAll ); \\

	done}


\fieldheader{Use:}



\begindata{bp,268822180}
\enddata{bp,268822180}
\view{bpv,268822180,30,0,0}
\subsection{6.4.3	InstallSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{Install::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i ; echo "installing (`pwd`)"; \\

			$(MAKE) $(MFLAGS) \\

				DESTDIR='$(DESTDIR)' Install ); \\

	done}


\fieldheader{Use:}



\subsection{6.4.4	InstallDocSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallDocSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{Doc::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i ; echo "installing docs (`pwd`)"; \\

			$(MAKE) $(MFLAGS) \\

				DESTDIR='$(DESTDIR)' Doc ); \\

	done}


\fieldheader{Use:}



\begindata{bp,268822136}
\enddata{bp,268822136}
\view{bpv,268822136,31,0,0}
\subsection{6.4.5	InstallAliasSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{InstallAliasSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{Aliases::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

	    (cd $$i ; echo "installing aliases (`pwd`)"; \\

		$(MAKE) $(MFLAGS) \\

		DESTDIR='$(DESTDIR)' Aliases ); \\

	done}


\fieldheader{Use:}



\subsection{6.4.6	DependInstallSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{DependInstallSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{dependInstall::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i;echo "building (depend/All/Install) (`pwd`)"; \\

			$(MAKE) $(MFLAGS) \\

			    DESTDIR='$(DESTDIR)' dependInstall); \\

	done}


\fieldheader{Use:}



\begindata{bp,268918564}
\enddata{bp,268918564}
\view{bpv,268918564,32,0,0}
\subsection{6.4.7	WorldInstallSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{WorldInstallSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{World::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i;echo "building (World) (`pwd`)"; \\

			$(MAKE) $(MFLAGS) \\

			    DESTDIR='$(DESTDIR)' World); \\

	done}


\fieldheader{Use:}



\subsection{6.4.8	CleanSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{CleanSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{Clean::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i ; echo "cleaning (`pwd`)"; \\

			$(MAKE) $(MFLAGS)  Clean ); \\

	done}


\fieldheader{Use:}


\subsection{6.4.9	TidySubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{TidySubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{Tidy::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i ; echo "tidying (`pwd`)"; \\

			$(MAKE) $(MFLAGS)  Tidy ); \\

	done}


\fieldheader{Use:}

\fieldcontent{Specfically used (but not dependent on) when LINKINSTALL_ENV is 
set - so you can remove all not installed generated files - without removing 
the installed ones. 

}
\begindata{bp,268885676}
\enddata{bp,268885676}
\view{bpv,268885676,33,0,0}
\subsection{6.4.10	TagSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{TagSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{tags::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		(cd $$i ; echo "tagging (`pwd`)"; \\

			$(MAKE) $(MFLAGS) TAGS='$(TAGS)' tags ); \\

	done}


\fieldheader{Use:}



\subsection{6.4.11	MakeMakeSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{MakeMakeSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{Makefiles::

	@case '$(MFLAGS)' in *[ik]*) set +e;; esac; \\

	for i in dirs ;\\

	do \\

		echo "Making Makefiles (`pwd`/$$i)..."; \\

		$(MAKE) subdirMakefiles NEWTOP=../ \\

			MAKE_SUBDIR=$$i;\\

	done}


\fieldheader{Use:}



\begindata{bp,268822020}
\enddata{bp,268822020}
\view{bpv,268822020,34,0,0}
\subsection{6.4.12	MakefileSubdirs}


\fieldheader{Syntax:}

\fixedfieldcontent{MakefileSubdirs(dirs)}


\fieldheader{Arguments:}

\fieldcontent{dirs - space separated list of subdirectories}


\fieldheader{Expanded Macro:}

\fixedfieldcontent{MakeMakeSubdirs(dirs)


subdirMakefiles:

	cd $(MAKE_SUBDIR) ; \\

	$(DOTDOTIMAKE_CMD) -DTOPDIR=../$(TOP) ; \\

	touch .depends ;  \\

	cat .depends >>Makefile ; \\

	$(MAKE) $(MFLAGS) Makefiles

}
\fieldheader{Use:}


\enddata{text,268745564}
