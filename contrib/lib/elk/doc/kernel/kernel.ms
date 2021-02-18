.so ../tmac/tmac.scheme
.RP
.ds R "R\v'-.3m'\s-13\s0\v'.3m'RS
.ds 1 "\v'.3m'\s-11\s0\v'-.3m'
.ds 2 "\v'.3m'\s-12\s0\v'-.3m'
.TL
Reference Manual for the
.br
Elk Extension Language Interpreter
.AU
Oliver Laumann
.AB
This document provides a complete list of all primitive procedures
and special forms implemented by the Elk Extension Language.
Only those procedures and special forms that are not defined in the
\f2Revised\v'-.3m'\s-13\s0\v'.3m' Report on the Algorithmic
Language Scheme\fP by Jonathan Rees and William Clinger (editors)
are described in detail.
The procedures that are mentioned in the report are only listed
without description or examples.
.AE
.
.Ch Lambda Expressions, Procedures
.
.Sy lambda formals body
See \*R.
.
.Pr procedure-lambda procedure
Returns a copy of the \f2lambda\fP expression which has been
evaluated to create the given procedure.
.br
Example:
.Ss
(define (square x) (* x x))
(procedure-lambda square)	==>  (lambda (x) (* x x))
.Se
.
.Pr procedure? obj
See \*R.
.
.Pr primitive? obj
Returns #t if \f2obj\fP is a primitive procedure, #f otherwise.
.
.Pr compound? obj
Returns #t if \f2obj\fP is a compound procedure (a procedure that
has been created by evaluating a lambda expression), #f otherwise.
.
.Ch Local Bindings
.
.Sy let bindings body
.Up
.Sy let* bindings body
.Up
.Sy letrec bindings body
See \*R.
.
.Ch Fluid Binding
.
.Sy fluid-let bindings body
\f2bindings\fP is of the form ((\f2variable\*1\fP \f2init1\fP) ...).
The \f2init\fPs are temporarily assigned to the \f2variable\fPs
and the \f2body\fP is executed.
The variables must be bound in an enclosing scope.
When the body is exited normally or by invoking a control point,
the old values of the variables are restored.
In the latter case, when the control returns back to the body
of the fluid-let by invocation of a control point created within
the body, the bindings are changed again to the values they had
when the body exited.
.br
Examples:
.Ss
((lambda (x)
  (+ x (fluid-let ((x 3)) x))) 1)	==>  4
.Se
.Ss
(fluid-let ((print-length 2))
  (write '(a b c d)))	==>  '(a b ...)
.Se
.Ss
(define (errset thunk)
  (call-with-current-continuation
    (lambda (catch)
      (fluid-let
          ((error-handler
            (lambda msg (catch #f))))
        (list (thunk))))))
.sp
(errset (lambda () (+ 1 2)))	==>  (3)
(errset (lambda () (/ 1 0)))	==>  #f
.Se
.
.Ch Definitions
.
.Sy define variable expression
.Up
.Sy define (variable formals) body
.Up
.Sy define (variable . formal) body
See \*R.
.br
Returns a symbol, the identifier that has been bound.
Definitions may appear anywhere within a local body (e.\|g. a lambda
body or a \f2let\fP).
If the \f2expression\fP is omitted, \f2void\fP (the non-printing
object) is used.
.br
Examples:
.Ss
(define nil #f)
.Se
.Ss
(define ((f x) y) (cons x y))
(define (g x) ((f x) 5))
(g 'a)	==>  (a . 5)
.Se
.
.Ch Assignment
.
.Sy set! variable expression
See \*R.
.br
Returns the previous value of \f2variable\fP.
.br
Examples:
.Ss
(define-macro (swap x y)
  `(set! ,x (set! ,y ,x)))
.Se
.
.Ch Procedure Application
.
.Sy operator operand\*1 ...
See \*R.
\f2operator\fP can be a macro (see below).
.
.Pr apply arg\*1 ... args
See \*R.
.
.Ch Quotation, Quasiquotation
.
.Sy quote datum
.Up
.SH
.tl ,\f3'\fP\f2datum\fP,,\f3syntax\fP,
.LP
.Up
.SH
.tl ,\f2constant\fP,,\f3syntax\fP
.Id constant
.LP
See \*R.
.br
Vectors need not be quoted; a vector evaluates to itself.
.
.Sy quasiquote expression
.Up
.Sy unquote expression
.Up
.Sy unquote-splicing expression
See \*R.
.
.Ch Sequencing
.
.Sy begin expression\*1 expression\*2 ...
See \*R.
.
.Sy begin1 expression\*1 expression\*2 ...
Identical to \f2begin\fP, except that the result of the first
\f2expression\fP is returned.
.
.Ch Conditionals
.
.Sy if test consequent alternate
.Up
.Sy if test consequent
See \*R.
.br
In the first form, \f2alternate\fP can be a sequence of expressions
(implicit \f2begin\fP).
.
.Sy case key clause\*1 clause\*2 ...
See \*R.
.br
Each \f2clause\fP not beginning with \f2else\fP can be of the form
.DS
((\f2datum\*1\fP ...) \f2expression\*1\fP \f2expression\*2\fP ...)
.DE
or
.DS
(\f2datum\fP \f2expression\*1\fP \f2expression\*2\fP ...)
.DE
In the latter case, the \f2key\fP is matched against the \f2datum\fP.
.
.Sy cond clause\*1 clause\*2 ...
See \*R.
.
.Sy and test\*1 ...
.Up
.Sy or test\*1 ...
See \*R.
.
.Ch Booleans
.
.Pr not obj
See \*R.
.
.Pr boolean? obj
See \*R.
.
.Ch Iteration
.
.Sy let variable bindings body
``Named \f2let\fP''.  See \*R.
.
.Pr map procedure list\*1 list\*2 ...
.Up
.Pr for-each procedure list\*1 list\*2 ...
See \*R.  \f2for-each\fP returns the empty list.
.
.Sy do initializations test body
See \*R.
.
.Ch Continuations
.
.Pr call-with-current-continuation procedure
See \*R.
.
.Pr control-point? obj
Returns #t if \f2obj\fP is a control point (a continuation),
#f otherwise.
.
.Pr dynamic-wind thunk thunk thunk
\f2dynamic-wind\fP is a generalization of the \f2unwind-protect\fP
facility provided by many Lisp systems.
.br
All three arguments are procedures of no arguments.
In the normal case, all three thunks are applied in order.
The first thunk is also applied when the body (the second thunk)
is entered by the application of a control point created within
the body (by means of \f2call-with-current-continuation\fP).
Similarly, the third thunk is also applied whenever the body is
exited by invocation of a control point created outside the body.
.br
Examples:
.Ss
(define-macro (unwind-protect body . unwind-forms)
  `(dynamic-wind
    (lambda () #f)
    (lambda () ,body)
    (lambda () ,@unwind-forms)))
.Se
.Ss
(let ((f (open-input-file "foo")))
  (dynamic-wind
    (lambda () #f)
    (lambda () \f2do something with\fP f)
    (lambda () (close-port f))))
.Se
.
.Ch Delayed Evaluation
.
.Sy delay expression
.Up
.Pr force promise
See \*R.
.
.Pr promise? obj
Returns #t if \f2obj\fP is a promise, an object returned by the
application of \f2delay\fP.
Otherwise #f is returned.
.
.Ch Equivalence Predicates
.
.Pr eq? obj\*1 obj\*2
.Up
.Pr eqv? obj\*1 obj\*2
.Up
.Pr equal? obj\*1 obj\*2
See \*R.
.
.Ch Pairs and Lists
.
.Pr cons obj\*1 obj\*2
See \*R.
.
.Pr car pair
.Up
.Pr cdr pair
See \*R.
.
.Pr cxr pair pattern
\f2pattern\fP is either a symbol or a string consisting of a combination
of the characters `a' and `d'.
It encodes a sequence of \f2car\fP and \f2cdr\fP operations;
each `a' denotes the application of \f2car\fP, and each `d' denotes
the application of \f2cdr\fP.
For example, \f2(cxr p "ada")\fP is equivalent to \f2(cadar p)\fP.
.
.Pr caar pair
.br
   ...
.br
.Pr cdddr pair
See \*R.
.
.Pr set-car! pair obj
.Up
.Pr set-cdr! pair obj
See \*R.
.br
Both procedures return \f2obj\fP.
.
.Pr make-list k obj
Returns a list of length \f2k\fP initialized with \f2obj\fP.
.br
Examples:
.Ss
(make-list 0 'a)	==>  ()
(make-list 2 (make-list 2 1))	==>  ((1 1) (1 1))
.Se
.
.Pr list obj ...
See \*R.
.
.Pr length list
See \*R.
.
.Pr list-ref list k
See \*R.
.
.Pr list-tail list k
See \*R.
.
.Pr last-pair list
See \*R.
.
.Pr append list ...
See \*R.
.
.Pr append! list ...
Like \f2append\fP, except that the original
arguments are modified (destructive \f2append\fP).
The cdr of each argument is changed to point to the next argument.
.br
Examples:
.Ss
(define x '(a b))
(append x '(c d))	==>  (a b c d)
x	==>  (a b)
(append! x '(c d))	==>  (a b c d)
x	==>  (a b c d)
.Se
.
.Pr reverse list
See \*R.
.
.Pr reverse! list
Destructive \f2reverse\fP.
.
.Pr memq obj list
.Up
.Pr memv obj list
.Up
.Pr member obj list
See \*R.
.
.Pr assq obj alist
.Up
.Pr assv obj alist
.Up
.Pr assoc obj alist
See \*R.
.
.Pr null? obj
.Up
.Pr pair? obj
See \*R.
.
.Pr list? obj
See \*R.
.
.Ch Numbers
.
.Pr = z\*1 z\*2 ...
.Up
.Pr < z\*1 z\*2 ...
.Up
.Pr > z\*1 z\*2 ...
.Up
.Pr <= z\*1 z\*2 ...
.Up
.Pr >= z\*1 z\*2 ...
See \*R.
.
.Pr 1+ z
.Up
.Pr 1- z
Returns \f2z\fP plus 1 or \f2z\fP minus 1, respectively.
.
.Pr + z\*1 ...
.Up
.Pr * z\*1 ...
See \*R.
.
.Pr - z\*1 z\*2 ...
.Up
.Pr / z\*1 z\*2 ...
See \*R.
.
.Pr zero? z
.Up
.Pr positive? z
.Up
.Pr negative? z
.Up
.Pr odd? z
.Up
.Pr even? z
.Up
.Pr exact? z
.Up
.Pr inexact? z
See \*R.
.br
\f2exact?\fP returns always #f; \f2inexact?\fP returns always #t.
.
.Pr abs z
See \*R.
.
.Pr quotient n\*1 n\*2
.Up
.Pr remainder n\*1 n\*2
.Up
.Pr modulo n\*1 n\*2
See \*R.
.
.Pr gcd n\*1 ...
.Up
.Pr lcm n\*1 ...
See \*R.
.
.Pr floor x
.Up
.Pr ceiling x
.Up
.Pr truncate x
.Up
.Pr round x
See \*R.
.
.Pr sqrt z
See \*R.
.
.Pr exp z
.Up
.Pr log z
.Up
.Pr sin z
.Up
.Pr cos z
.Up
.Pr tan z
.Up
.Pr asin z
.Up
.Pr acos z
.Up
.Pr atan z
.Up
.Pr atan y x
See \*R.
.
.Pr min x\*1 x\*2 ...
.Up
.Pr max x\*1 x\*2 ...
See \*R.
.
.Pr random
Returns an integer pseudo-random number in the range from 0 to
2\v'-.3m'\s-131\s0\v'.3m'-1.
.
.Pr srandom n
Sets the random number generator to the starting point \f2n\fP.
\f2srandom\fP returns \f2n\fP.
.
.Pr number? obj
.Up
.Pr complex? obj
.Up
.Pr real? obj
.Up
.Pr rational? obj
.Up
.Pr integer? obj
See \*R.
.
.Pr number\(mi>string number
.Up
.Pr number\(mi>string number radix
See \*R.
.
.Pr string\(mi>number string
.Up
.Pr string\(mi>number string radix
See \*R.
.
.Ch Characters
.
.Pr char\(mi>integer char
.Up
.Pr integer\(mi>char n
See \*R.
.
.Pr char-upper-case? char
.Up
.Pr char-lower-case? char
See \*R.
.
.Pr char-alphabetic? char
.Up
.Pr char-numeric? char
.Up
.Pr char-whitespace? char
See \*R.
.
.Pr char-upcase char
.Up
.Pr char-downcase char
See \*R.
.
.Pr char=? char\*1 char\*2
.Up
.Pr char<? char\*1 char\*2
.Up
.Pr char>? char\*1 char\*2
.Up
.Pr char<=? char\*1 char\*2
.Up
.Pr char>=? char\*1 char\*2
See \*R.
.
.Pr char-ci=? char\*1 char\*2
.Up
.Pr char-ci<? char\*1 char\*2
.Up
.Pr char-ci>? char\*1 char\*2
.Up
.Pr char-ci<=? char\*1 char\*2
.Up
.Pr char-ci>=? char\*1 char\*2
See \*R.
.
.Pr char? obj
See \*R.
.
.Ch Strings
.
.Pr string char ...
Returns a string containing the specified characters.
.br
Examples:
.Ss
(string)	==>  ""
(string #\ea #\espace #\eb)	==>  "a b"
.Se
.
.Pr string? obj
See \*R.
.
.Pr make-string k char
See \*R.
.
.Pr string-length string
See \*R.
.
.Pr string-ref string k
See \*R.
.
.Pr string-set! string k char
See \*R.
.br
Returns the previous value of element \f2k\fP of the given string.
.
.Pr substring string start end
See \*R.
.
.Pr string-copy string
See \*R.
.
.Pr string-append string ...
See \*R.
.
.Pr list\(mi>string chars
.Up
.Pr string\(mi>list string
See \*R.
.
.Pr string-fill! string char
See \*R.
.br
Returns \f2string\fP.
.
.Pr substring-fill! string start end char
Stores \f2char\fP in every element of \f2string\fP from \f2start\fP
(inclusive) to \f2end\fP (exclusive).
Returns \f2string\fP.
.
.Pr string=? string\*1 string\*2
.Up
.Pr string<? string\*1 string\*2
.Up
.Pr string>? string\*1 string\*2
.Up
.Pr string<=? string\*1 string\*2
.Up
.Pr string>=? string\*1 string\*2
See \*R.
.
.Pr string-ci=? string\*1 string\*2
.Up
.Pr string-ci<? string\*1 string\*2
.Up
.Pr string-ci>? string\*1 string\*2
.Up
.Pr string-ci<=? string\*1 string\*2
.Up
.Pr string-ci>=? string\*1 string\*2
See \*R.
.
.Pr substring? string\*1 string\*2
.Up
.Pr substring-ci? string\*1 string\*2
If \f2string\*1\fP is a substring of \f2string\*2\fP, these
procedures return the starting position of the first occurrence of the
substring within \f2string\*2\fP.
Otherwise #f is returned.
\f2substring-ci?\fP is the case insensitive version of \f2substring?\fP.
.br
Examples:
.Ss
(define s "Hello world")
(substring? "foo" x)	==>  #f
(substring? "hello" x)	==>  #f
(substring-ci? "hello" x)	==>  0
(substring? "!" x)	==>  11
.Se
.
.Ch Vectors
.
.Pr vector? obj
See \*R.
.
.Pr make-vector k
.Up
.Pr make-vector k fill
See \*R.
.
.Pr vector obj ...
See \*R.
.
.Pr vector-length vector
See \*R.
.
.Pr vector-ref vector k
See \*R.
.
.Pr vector-set! vector k obj
See \*R.
.br
Returns the previous value of element \f2k\fP of the vector.
.
.Pr vector\(mi>list vector
.Up
.Pr list\(mi>vector list
See \*R.
.
.Pr vector-fill! vector fill
See \*R.
.br
Returns \f2vector\fP.
.
.Pr vector-copy vector
Returns a copy of \f2vector\fP.
.
.Ch Symbols
.
.Pr string\(mi>symbol string
.Up
.Pr symbol\(mi>string symbol
See \*R.
.
.Pr put symbol key value
.Up
.Pr put symbol key
Associates \f2value\fP with \f2key\fP in the property list of the
given symbol.
\f2key\fP must be a symbol.
Returns \f2key\fP.
.br
If \f2value\fP is omitted, the property is removed from the symbol's
property list.
.
.Pr get symbol key
Returns the value associated with \f2key\fP in the property
list of \f2symbol\fP.
\f2key\fP must be a symbol.
If no value is associated with \f2key\fP in the symbol's property
list, #f is returned.
.br
Examples:
.Ss
(put 'norway 'capital "Oslo")
(put 'norway 'continent "Europe")
(get 'norway 'capital)	==>  "Oslo"
.Se
.
.Pr symbol-plist symbol
Returns a copy of the property list of \f2symbol\fP as an \f2alist\fP.
.br
Examples:
.Ss
(put 'norway 'capital "Oslo")
(put 'norway 'continent "Europe")
(symbol-plist 'norway)
   ==>  ((capital . "Oslo") (continent . "Europe"))
(symbol-plist 'foo)	==>  ()
.Se
.
.Pr symbol? obj
See \*R.
.
.Pr oblist
Returns a list of lists containing all currently interned symbols.
Each sublist represents a bucket of the interpreters internal
hash array.
.br
Examples:
.Ss
(define (apropos what)
  (let ((ret ()))
    (do ((tail (oblist) (cdr tail))) ((null? tail))
      (do ((l (car tail) (cdr l))) ((null? l))
        (if (substring? what (symbol->string (car l)))
            (set! ret (cons (car l) ret)))))
    ret))
.Se
.Ss
.ta 7c
(apropos "let")	==>  (let* let letrec fluid-let)
(apropos "make")	==>  (make-list make-vector make-string)
(apropos "foo")	==>  ()
.Se
.
.Ch Environments
.
.Pr the-environment
Returns the current environment.
.
.Pr global-environment
Returns the global environment (the ``root'' environment in which
all predefined procedures are bound).
.
.Pr environment\(mi>list environment
Returns a list representing the specified environment.
The list is a list of \f2frames\fP, each frame is a list of bindings
(an \f2alist\fP).
The car of the list represents the most recently established environment.
The list returned by \f2environment\(mi>list\fP can contain cycles.
.br
Examples:
.Ss
(let ((x 1) (y 2))
  (car (environment->list
    (the-environment))))	==>  ((y . 2) (x . 1))
.Se
.Ss
((lambda (foo)
   (caar (environment->list
     (the-environment)))) "abc")	==>  (foo . "abc")
.Se
.Ss
(eq?
  (car (last-pair (environment->list
    (the-environment))))
  (car (environment->list
    (global-environment))))	==>  #t
.Se
.
.Pr procedure-environment procedure
.Up
.Pr promise-environment promise
.Up
.Pr control-point-environment control-point
Returns the environment in which the the body of the \f2procedure\fP
is evaluated, the environment in which a value for the \f2promise\fP
is computed when \f2force\fP is applied to it, or the environment in
which the \f2control-point\fP has been created, respectively.
.
.Pr environment? obj
Returns #t if \f2obj\fP is an environment, #f otherwise.
.
.Ch Ports and Files
.LP
Generally, a file name can either be a string or a symbol.
If a symbol is given, it is converted into a string by
applying \f2symbol\(mi>string\fP.
A tilde at the beginning of a file name is expanded according
to the rules employed by the C-Shell (see \f2csh\fP(1)).
.
.Pr call-with-input-file file procedure
.Up
.Pr call-with-output-file file procedure
See \*R.
.
.Pr input-port? obj
.Up
.Pr output-port? obj
See \*R.
.
.Pr current-input-port
.Up
.Pr current-output-port
See \*R.
.
.Pr with-input-from-file file thunk
.Up
.Pr with-output-to-file file thunk
See \*R.
.br
\f2file\fP can be a string as well as a symbol.
.
.Pr open-input-file file
.Up
.Pr open-output-file file
See \*R.
.br
\f2file\fP can be a string as well as a symbol.
.
.Pr close-port port
See \f2close-input-port\fP and \f2close-output-port\fP in \*R.
.
.Pr clear-output-port
.Up
.Pr clear-output-port output-port
If the argument is omitted, it defaults to the current output port.
.br
In case of ``buffered'' output, this procedure is used to discard
all characters that have been
output to the port but have not yet been sent to the file associated
with the port.
.
.Pr flush-output-port
.Up
.Pr flush-output-port output-port
If the argument is omitted, it defaults to the current output port.
.br
In case of ``buffered'' output, this procedure is used to force
all characters that have been output to the port to be printed
immediately.
This may be necessary to force output that is not terminated with a newline 
to appear on the terminal.
An output port is flushed automatically when it is closed.
.
.Pr clear-input-port
.Up
.Pr clear-input-port input-port
If the argument is omitted, it defaults to the current input port.
.br
In case of ``buffered'' input,
this procedure discards all characters that have already been read
from the file associated with the port but have not been processed
using \f2read\fP or similar procedures.
.
.Pr port-file-name port
Returns the name of the file associated with \f2port\fP if it is
a file port, #f otherwise.
.
.Pr tilde-expand file
If \f2file\fP starts with a tilde, performs tilde expansion as
described above and returns the result of the expansion
(a string); returns \f2file\fP otherwise.
\f2file\fP is a string or a symbol.
.
.Pr file-exists? file
Returns #t if \f2file\fP is accessible, #f otherwise.
\f2file\fP is a string or a symbol; tilde expansion is not performed.
.
.Ch Input
.
.Pr read
.Up
.Pr read input-port
See \*R.
.
.Pr read-char
.Up
.Pr read-char input-port
See \*R.
.
.Pr read-string
.Up
.Pr read-string input-port
If the argument is omitted, it defaults to the current input port.
.br
Returns the rest of the current input line as a string (not
including the terminating newline).
.
.Pr unread-char char
.Up
.Pr unread-char char input-port
If the second argument is omitted, it defaults to the current input port.
.br
Pushes \f2char\fP back on the stream of input characters.
It is \f2not\fP an error for \f2char\fP not to be the last character
read from the port.
The procedure returns \f2char\fP.
.br
Examples:
.Ss
(define (peek-char port)
  (unread-char (read-char port) port))
.Se
.
.Pr eof-object? obj
See \*R.
.
.Ch Output
.
.Va print-length
.Up
.Va print-depth
These variables are defined in the global environment.
They control the maximum length (and maximum depth, respectively) of
a list or vector that is printed.
If one of the variables is not bound to an integer, a default value
is taken.
The default value for \f2print-length\fP is 1000, and the default
value for \f2print-depth\fP is 20.
.
.Pr write obj
.Up
.Pr write obj output-port
See \*R.
.
.Pr display obj
.Up
.Pr display obj output-port
See \*R.
.
.Pr write-char char
.Up
.Pr write-char char output-port
See \*R.
.
.Pr newline
.Up
.Pr newline output-port
See \*R.
.
.Pr print obj
.Up
.Pr print obj output-port
If the second argument is omitted, it defaults to the current output port.
.br
Prints \f2obj\fP using \f2write\fP and then prints a newline.
\f2print\fP returns \f2void\fP.
.
.Pr format destination format-string obj ...
Prints the third and the following arguments according to the
specifications in the string \f2format-string\fP.
Characters from the format string are copied to the output.
When a tilde is encountered in the format string, the tilde and
the immediately following character are replaced in the output
as follows:
.IP "~s"
is replaced by the printed representation of the next \f2obj\fP
in the sense of \f2write\fP.
.IP "~a"
is replaced by the printed representation of the next \f2obj\fP
in the sense of \f2display\fP.
.IP "~~"
is replaced by a single tilde.
.IP "~%"
is replaced by a newline.
.LP
An error is signaled if fewer \f2obj\fPs are provided than
required by the given format string.
If the format string ends in a tilde, the tilde is ignored.
.LP
If \f2destination\fP is #t, the output is sent to the current
output port; if #f is given, the output is returned as a string;
otherwise, \f2destination\fP must be an output port.
.br
Examples:
.Ss
(format #f "Hello world!")	==>  "Hello world"
(format #f "~s world!" "Hello")	==>  "\e"Hello\e" world"
(format #f "~a world!" "Hello")	==>  "Hello world"
(format #f "Hello~a")	==>  "Hello!"
.Se
.Ss
(define (flat-size s)
  (fluid-let ((print-length 1000) (print-depth 100))
    (string-length (format #f "~a" s))))
.Se
.Ss
(flat-size 1.5)	==>  3
(flat-size '(a b c))	==>  7
.Se
.
.Ch String Ports
.LP
String ports are similar to file ports, except that characters are
appended to a string instead of being sent to a file, or taken
from a string instead of being read from a file.
It is not necessary to close string ports.
When an string input port has reached the end of the input string,
successive read operations return end-of-file.
.
.Pr open-input-string string
Returns a new string input port initialized with \f2string\fP.
.br
Examples:
.Ss
(define p (open-input-string "Hello world!"))
(read-char p)	==>  #\eH
(read p)	==>  ello
(read p)	==>  world!
(read p)	==>  \f2end of file\fP
.Se
.Ss
(define p (open-input-string "(cons 'a 'b)"))
(eval (read p))	==>  (a . b)
.Se
.
.Pr open-output-string
Returns a new string output port.
.
.Pr get-output-string string-output-port
Returns the string currently associated with the specified string
output port.
As a side-effect, the string is reset to zero length.
.br
Examples:
.Ss
(define p (open-output-string))
(display '(a b c) p)
(get-output-string p)	==>  "(a b c)"
(get-output-string p)	==>  ""
.Se
.Ss
(define (flat-size s)
  (let ((p (open-output-string)))
    (display s p)
    (string-length (get-output-string p))))
.Se
.
.Ch Loading
.
.Pr load file
.Up
.Pr load file environment
Loads a source file or an object file.
If the file contains source code, the expressions in the file are
read and evaluated.
If the file contains object code, the contents of the file is linked
together with the running interpreter and with additional libraries
that are specified by the variable \f2load-libraries\fP (see below).
Object files must end in the suffix ``.o''.
\f2load\fP returns \f2void\fP.
\f2file\fP can be a string as well as a symbol.
.br
If an optional \f2environment\fP is specified, the contents of the file
is evaluated in this environment instead of the current environment.
.br
Example:
.Ss
(fluid-let ((load-noisily? #t))
  (load 'test.scm))
.Se
.
.Va load-path
This variable is defined in the global environment.
It is bound to a list of directories in which files to be loaded are
searched for.
Each element of the list (a string or a symbol) is used in turn as
a prefix for the file name passed to \f2load\fP until opening succeeds.
.LP
The initial value of \f2load-path\fP is a list of the three elements
``.'' (i.\|e. the current directory), ``$(TOP)/scm'', and ``$(TOP)/lib'',
where $(TOP) is the top-level directory of the Elk installation.
.
.Va load-noisily?
This variable is defined in the global environment.
When a file is loaded and the value of \f2load-noisily?\fP is true,
the result of the evaluation of each expression is printed.
The initial value of \f2load-noisily\fP is #f.
.
.Va load-libraries
This variable is defined in the global environment.
If \f2load-libraries\fP is bound to a string, its value specifies
additional load libraries to be linked together with an object file
that is loaded into the interpreter (see \f2load\fP above).
Its initial value is ``-lc''.
.
.Pr autoload symbol file
Binds \f2symbol\fP in the current environment (as with \f2define\fP).
When \f2symbol\fP is evaluated the first time, \f2file\fP is loaded.
The definitions loaded from the file must provide a definition
for \f2symbol\fP different from \f2autoload\fP, otherwise an error
is signaled.
.
.Va autoload-notify?
This variable is defined in the global environment.
If the value of \f2autoload-notify?\fP is true, a message is printed
whenever evaluation of a symbol triggers autoloading of a file.
\f2autoload-notify?\fP is bound to #t initially.
.
.Ch Macros
.
.Sy macro formals body
Creates a macro.
The syntax is identical to the syntax of \f2lambda\fP expressions.
When a macro is called, the actual arguments are bound to
the formal arguments of the \f2macro\fP expression \f2in the current
environment\fP (they are \f2not\fP evaluated), then the \f2body\fP is evaluated.
The result of this evaluation is considered the \f2macro expansion\fP
and is evaluated in place of the macro call.
.
.Sy define-macro (variable formals) body
.Up
.Sy define-macro (variable . formal) body
Like \f2define\fP, except that \f2macro\fP is used instead of \f2lambda\fP.
.br
Examples:
.Ss
(define-macro (++ x) `(set! ,x (1+ ,x)))
(define foo 5)
foo	==>  5
(++ foo)
foo	==>  6
.Se
.Ss
(define-macro (while test . body)
  `(let loop ()
     (cond (,test ,@body (loop)))))
.Se
.
.Pr macro? obj
Returns #t if \f2obj\fP is a macro, #f otherwise.
.
.Pr macro-body macro
Returns a copy of the \f2macro\fP expression which has been evaluated to
created the given macro (similar to \f2procedure-lambda\fP).
.br
Examples:
.Ss
(define-macro (++ x) `(set! ,x (1+ ,x)))
.Sp
(macro-body ++)
  ==>  (macro (x) (quasiquote (set! (unquote x) (1+ (unquote x)))))
.Se
.
.Pr macro-expand list
If the expression \f2list\fP is a macro call, the macro call
is expanded.
.br
Examples:
.Ss
(define-macro (++ x) `(set! ,x (1+ ,x)))
.sp
(macro-expand '(++ foo))	==>  (set! foo (1+ foo))
.Se
.sp
The following function can be used to expand \f2all\fP macro calls
in an expression, i.\|e. not only at the outermost level:
.Ss
(define (expand form)
  (if (or (not (pair? form)) (null? form))
      form
      (let ((head (expand (car form)))
            (args (expand (cdr form)))
	    (result))
        (if (and (symbol? head) (bound? head))
            (begin
              (set! result (macro-expand (cons head args)))
              (if (not (equal? result form))
                  (expand result)
                  result))
              (cons head args)))))
.Se
.
.Ch Error and Exception Handling
.
.Va error-handler
This variable is defined in the global environment.
When an error occurs or when the procedure \f2error\fP is invoked
and the variable \f2error-handler\fP is bound to a compound procedure
(the \f2error handler\fP), the interpreter invokes this procedure.
The error handler is called with an object (either the first argument
that has been passed to \f2error\fP or a symbol identifying the
primitive procedure that has caused the error), and an error
message consisting of a format string
and a list of objects suitable to be passed to \f2format\fP.
.LP
Typically, a user-defined error handler prints the error message and then
calls a control point that has been created outside the error handler.
If the error handler terminates normally or if \f2error-handler\fP
is not bound to a procedure, the error message is printed in a
default way, and then a \f2reset\fP is performed.
.
.Va interrupt-handler
This variable is defined in the global environment.
When an interrupt occurs (typically as a result of typing the
interrupt character on the keyboard), and the variable
\f2interrupt-handler\fP is bound to a procedure (the \f2interrupt
handler\fP), this procedure is called with no arguments.
If \f2interrupt-handler\fP is not bound to a procedure or if
the procedure terminates normally, a message is printed, and
a \f2reset\fP is performed.
.br
Examples:
.Ss
(set! interrupt-handler
  (lambda ()
    (newline)
    (backtrace)
    (reset)))
.Se
.
.Pr error obj string obj ...
Signals an error.
The arguments of \f2error\fP are passed to the \f2error-handler\fP.
.br
Examples:
.Ss
(define (foo sym)
  (if (not (symbol? sym))
      (error 'foo "argument not a symbol: ~s" sym))
  ...
.Se
.
.Va top-level-control-point
.Up
.Pr reset
Performs a reset by calling the control point to which the variable
\f2top-level-control-point\fP is bound in the global environment.
The control point is called with the argument #t.
If \f2top-level-control-point\fP is not bound to a control point,
an error message is printed and the interpreter is terminated.
.br
Examples:
.Ss
(if (call-with-current-continuation
      (lambda (x)
        (fluid-let ((top-level-control-point x))
          \f2do something\fP
          #f)))
    (print "Got a reset!"))
.Se
.
.Pr exit
.Up
.Pr exit n
Terminates the interpreter.
The optional argument \f2n\fP indicates the exit code;
it defaults to zero.
.
.Ch Garbage Collection
.
.Pr collect
Causes a garbage collection.
.
.Va garbage-collect-notify?
This variable is defined in the global environment.
If the value of \f2garbage-collect-notify?\fP is true,
a message indicating the amount of free memory on the heap and
the size of the heap is displayed whenever a garbage collection
is performed.
\f2garbage-collect-notify?\fP is bound to #t initially.
.
.Ch Features
.
.Pr feature? symbol
Returns #t if \f2symbol\fP is a feature, i.\|e. \f2provide\fP has
been called to indicate that the feature \f2symbol\fP is present;
#f otherwise.
.
.Pr provide symbol
Indicates that the feature \f2symbol\fP is present.
Returns \f2void\fP.
.
.Pr require symbol
.Up
.Pr require symbol file
.Up
.Pr require symbol file environment
If the feature \f2symbol\fP is not present (i.\|e.
(feature? \f2symbol\fP) evaluates to #f), 
\f2file\fP is loaded.
A message is displayed prior to loading the file if the value of the
global variable \f2autoload-notify?\fP is true.
If the feature is still not present after the file has been loaded,
an error is signaled.
If the \f2file\fP argument is omitted, it defaults to \f2symbol\fP.
If an \f2environment\fP argument is supplied, the file is loaded
into given environment.
if the \f2environment\fP argument is omitted, it defaults to the
current environment.
.
.Ch Miscellaneous
.
.Pr dump file
Writes a snapshot of the running interpreter to \f2file\fP and
returns #f.
When \f2file\fP is executed, execution of the interpreter resumes such
that the call to \f2dump\fP returns #t
(i.e., \f2dump\fP actually returns twice).
\f2dump\fP closes all ports except the current input and current
output port.
.
.Pr eval list
.Up
.Pr eval list environment
Evaluates the expression \f2list\fP in the specified environment.
If \f2environment\fP is omitted, the expression is evaluated
in the current environment.
.br
Examples:
.Ss
(let ((car 1))
  (eval 'car (global-environment)))	==>  \f2primitive\fP \f1car\fP
.Se
.Ss
(define x 1)
(define env
  (let ((x 2)) (the-environment)))
(eval 'x)	==>  1
(eval 'x env)	==>  2
.Se
.
.Pr bound? symbol
Returns #t if \f2symbol\fP is bound in the current environment,
#f otherwise.
.
.Pr type obj
Returns a symbol indicating the type of \f2obj\fP.
.br
Examples:
.Ss
(type 13782343423544)	==>  integer
(type 1.5e8)	==>  real
(type (lambda (x y) (cons x y)))	==>  compound
(type #\ea)	==>  character
(type '(a b c))	==>  pair
(type ())	==>  null
(type (read
  (open-input-string "")))	==>  end-of-file
.Se
.
.Pr void? obj
Returns true if \f2obj\fP is the non-printing object, false otherwise.
.
.Pr command-line-args
Returns the command line arguments of the interpreter's invocation,
a list of strings.
.
.Ch Incompatibilities with the \*R
.LP
The following list enumerates the points where the Elk Extension Language
does not conform to the \*R.
These are language features which could cause a Scheme program to not
properly run under Elk, although it does run under a \*R-conforming
implementation.
.IP \(bu
Upper and lower case letters in symbols are always distinguished
(i.\|e. Elk is \f2case-sensitive\fP), except when the interpreter is
compiled with a special compile-time option.
.IP \(bu
Quasiquotation can currently not be used to construct vectors.
.IP \(bu
Rational and complex numbers are not implemented.
.IP \(bu
All numbers are inexact.
.IP \(bu
#b #o #d #x
Radix prefixes (#b, #o, #d, and #x) for real numbers are currently
not implemented.
.IP \(bu
Prefixes for exact and inexact constants (#e and #i) are not implemented.
.IP \(bu
\f2exact\(mi>inexact\fP and \f2inexact\(mi>exact\fP are not implemented.
.IP \(bu
\f2close-input-port\fP and \f2close-output-port\fP do not exist.
Use \f2close-port\fP instead; it can be applied to input ports as
well as output ports.
.IP \(bu
\f2char-ready?\fP is currently not implemented.
.IP \(bu
\f2transcript-on\fP and \f2transcript-off\fP do not exist.
Use the UNIX \f2script(1)\fP command instead.
