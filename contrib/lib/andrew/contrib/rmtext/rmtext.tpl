(* File ModName.def, created on <date> by <name>, IBM Rochester. *)
(* Last edited by <name> on <date>. *)

(* Copyright (c) by IBM Corporation, 1989. *)

DEFINITION MODULE ModName;

CONST
    IBMid = "Copyright (C) IBM Corporation, 1989.";
    RCSHeader = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtext.tpl,v 1.1 1989/09/10 14:59:03 ghoti Exp $";
    
TYPE
    Handle;
    
PROCEDURE Nil(): Handle;

(* Always returns an empty Handle. *)

PROCEDURE Equal(h1, h2: Handle): BOOLEAN;

(* Returns TRUE if h1 and h2 are the same Handle and FALSE otherwise. *)

END ModName.

(* CHANGE LOG

$Log: rmtext.tpl,v $
Revision 1.1  1989/09/10  14:59:03  ghoti
Initial revision

Revision 1.1  89/08/25  13:04:38  rlquinn
Initial revision


<date> by <name>

Brief description of the change and why it was made. *)




(* File ModName.mod, created on <date> by <name>, IBM Rochester. *)
(* Last edited by <name> on <date>. *)

(* Copyright (c) by IBM Corporation, 1989. *)

IMPLEMENTATION MODULE ModName;

CONST
    IBMidMod = 'Copyright (c) by IBM Corporation, 1989.';
    RCSModHeader = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtext.tpl,v 1.1 1989/09/10 14:59:03 ghoti Exp $";
    
TYPE
    Handle = ;
    
PROCEDURE Nil(): Handle;

(* Always returns an empty Handle. *)

BEGIN RETURN NIL END Nil;

PROCEDURE Equal(h1, h2: Handle): BOOLEAN;

(* Returns TRUE if h1 and h2 are the same Handle and FALSE otherwise. *)

BEGIN RETURN h1 = h2 END Equal;

BEGIN
    <statements>
END ModName.

(* CHANGE LOG

$Log: rmtext.tpl,v $
Revision 1.1  1989/09/10  14:59:03  ghoti
Initial revision

Revision 1.1  89/08/25  13:04:38  rlquinn
Initial revision


<date> by <name>

Brief description of the change and why it was made. *)

