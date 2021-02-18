\begindata{text, 1}
\textdsversion{11}
\define{comment
attr:[FontFace Italic Int Set]
attr:[FontFace FixedFace Int Clear]
attr:[FontSize PreviousFontSize Point 2]
attr:[FontFamily Andy Int 0]}
\define{keyword
attr:[FontFace Bold Int Set]}
\define{identifier
attr:[FontFace Italic Int Set]
attr:[FontFace Bold Int Set]}
\define{function
attr:[FontFace Bold Int Set]
attr:[FontSize PreviousFontSize Point 2]
attr:[FontFamily Andy Int 0]}
\define{global
attr:[FontFace FixedFace Int Set]
attr:[Justification LeftJustified Point 0]
attr:[LeftMargin LeftEdge Int 16]
attr:[Indent LeftMargin Int -16]
attr:[Flags ContinueIndent Int Set]
attr:[FontSize ConstantFontSize Point 10]
attr:[FontFamily AndyType Int 0]]]}
\enddata{text,1}
(* ---------- This is a Modula-3 Interface Template. ---------- *)

(* File Name.m, created by <name> on <date>, IBM Rochester. *)
(* Last edited by <name> on <date>. *)

(* Copyright (c) by IBM Corporation, 1989. *)

INTERFACE Name;
    
    CONST
        NameCopyright = "Copyright (c) by IBM Corporation, 1989.";
        NameRCSHeader = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/m3text/RCS/m3text.tpl,v 1.1 1989/09/10 14:58:35 ghoti Exp $";
        NameDate = "$Date: 1989/09/10 14:58:35 $";
        NameRevision = "$Revision: 1.1 $";
        
    TYPE
        Node = REF NodeBody;
        NodeBody = RECORD
            data: INTEGER := 0;
            next: Node := NIL;
        END;
        T = OBJECT
            body: Node := NIL;
        METHODS
            push(x: INTEGER) := Push;
            pop(): INTEGER := Pop;
            empty(): BOOLEAN := Empty;
        END;
        
    PROCEDURE Push(VALUE stack: T; x: INTEGER); 
        (* Places the integer x at the top of stack while retaining the remaining elements of the stack.  *)
        
    PROCEDURE Pop(VALUE stack: T): INTEGER;
        (* Removes the top element from the stack and returns this value.  *)
        
    PROCEDURE Empty(VALUE stack: T): BOOLEAN;
        (* Returns TRUE if stack is empty and FALSE otherwise.  *)
        
END Name.

(* CHANGE LOG
   
   $Log: m3text.tpl,v $
Revision 1.1  1989/09/10  14:58:35  ghoti
Initial revision

Revision 1.4  89/08/25  13:21:37  rlquinn
*** empty log message ***

   
   <date> by <name>
   
   Brief description of the change and why it was made. *)



(* ---------- This is a Modula-3 Module Template. ---------- *)

(* File NameImpl.m, created by <name> on <date>, IBM Rochester. *)
(* Last edited by <name> on <date>. *)

(* Copyright (c) by IBM Corporation, 1989. *)

MODULE NameImpl
    
    EXPORTS Name;
    
    IMPORT Text;
    
    CONST
        NameImplCopyright = "Copyright (c) by IBM Corporation, 1989.";
        NameImplRCSHeader = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/m3text/RCS/m3text.tpl,v 1.1 1989/09/10 14:58:35 ghoti Exp $";
        NameImplDate = "$Date: 1989/09/10 14:58:35 $";
        NameImplRevision = "$Revision: 1.1 $";
        
    PROCEDURE Push(VALUE stack: T; x: INTEGER) =
        VAR tmp: Node;
        BEGIN
            new(tmp);
            tmp.next := stack.body;
            tmp.data := x;
            stack.body := tmp
        END Push;
        
    PROCEDURE Pop(VALUE stack: T): INTEGER =
        VAR
            answer: INTEGER:= 0;
        BEGIN
            answer := stack.body.data;
            stack.body := stack.body.next;
            RETURN answer
        END Pop;
        
    PROCEDURE Empty(VALUE stack: T): BOOLEAN =
        BEGIN
            RETURN stack.body = NIL
        END Empty;
        
BEGIN
    <statements>
END NameImpl.

(* CHANGE LOG
   
   $Log: m3text.tpl,v $
Revision 1.1  1989/09/10  14:58:35  ghoti
Initial revision

Revision 1.4  89/08/25  13:21:37  rlquinn
*** empty log message ***

   
   <date> by <name>
   
   Brief description of the change and why it was made. *)


