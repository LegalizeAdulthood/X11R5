#ifndef EXTERN_H
#define EXTERN_H

/* From the C-library:
 */
extern errno;
extern char *sbrk(), *malloc(), *getenv(), *mktemp();
#ifndef alloca
extern char *alloca();
#endif

/* autoload.c:
 */
extern Object V_Autoload_Notifyp;
extern Object P_Autoload(), Do_Autoload();

/* bignum.c:
 */
extern Bignum_Zero(), Bignum_Positive(), Bignum_Negative(), Bignum_Even();
extern Object Make_Bignum(), Integer_To_Bignum(), Bignum_Divide();
extern Object Bignum_Abs(), Bignum_Plus(), Bignum_Minus();
extern Object Bignum_Fixnum_Multiply(), Bignum_Multiply();
extern Object Bignum_Fixnum_Divide(), Double_To_Bignum();
extern Object Unsigned_To_Bignum(), Bignum_To_String();
extern double Bignum_To_Double();

/* bool.c:
 */
extern Object P_Booleanp(), P_Not(), P_Eq(), P_Eqv(), P_Equal();
extern Eqv(), Equal();

/* char.c:
 */
extern Object Make_Char(), P_Charp(), P_Char_Upcase(), P_Char_Downcase();
extern Object P_Chr_Eq(), P_Chr_Less(), P_Chr_Greater(), P_Chr_Eq_Less();
extern Object P_Chr_Eq_Greater(), P_Chr_CI_Eq(), P_Chr_CI_Less();
extern Object P_Chr_CI_Greater(), P_Chr_CI_Eq_Less(), P_Chr_CI_Eq_Greater();
extern Object P_Char_Upper_Case(), P_Char_Lower_Case();
extern Object P_Char_Alphabetic(), P_Char_Numeric(), P_Char_Whitespace();
extern Object P_Char_To_Integer(), P_Integer_To_Char();

/* cont.c:
 */
#ifdef PORTABLE
extern jmp_buf last_jmpbuf;
#endif
extern WIND *First_Wind, *Last_Wind;
extern Object P_Call_CC(), P_Dynamic_Wind(), P_Control_Pointp();
extern Object P_Control_Point_Env(), Make_Control_Point();
extern Object P_Reentrant_Continuationsp();

/* debug.c:
 */
extern Object P_Backtrace_List();

/* dump.c:
 */
extern Object P_Dump();
extern Object Dump_Control_Point;

/* env.c:
 */
extern Object The_Environment, Global_Environment;
extern Object P_The_Environment(), P_Define(), P_Set(), P_Env_List();
extern Object Add_Binding(), P_Define_Macro(), P_Boundp();
extern Object P_Global_Environment(), P_Environmentp();

/* error.c:
 */
extern Object Arg_True;
extern Intr_Handler();
extern Object P_Error(), P_Reset();

/* feature.c:
 */
extern Object P_Featurep(), P_Provide(), P_Require();

/* fixmul.c:
 */
extern Object Fixnum_Multiply();

/* heap.c:
 */
extern char *Hp, *Heap_Start, *Heap_End, *Free_Start, *Free_End;
extern GC_In_Progress;
extern GCNODE *GC_List;
extern char *Get_Bytes(), *Get_Const_Bytes();
extern Object P_Collect();
extern char *Safe_Malloc();

/* io.c:
 */
extern Object Curr_Input_Port, Curr_Output_Port;
extern Object Standard_Input_Port, Standard_Output_Port;
extern Object P_Curr_Input_Port(), P_Curr_Output_Port(), P_Input_Portp();
extern Object P_Curr_Input_Port(), P_Curr_Output_Port(), P_Input_Portp();
extern Object P_Output_Portp(), P_Open_Input_File(), P_Open_Output_File();
extern Object P_Close_Port(), P_Eof_Objectp(), P_With_Input(), P_With_Output();
extern Object P_Call_With_Input(), P_Call_With_Output(), General_Open_File();
extern Object P_Open_Input_String(), P_Open_Output_String();
extern Object P_Port_File_Name(), P_Tilde_Expand(), P_File_Existsp();

/* load.c:
 */
extern char Loader_Input[];
extern Object General_Load(), P_Load();

/* list.c:
 */
extern Object P_Cons(), P_Car(), P_Cdr(), P_Setcar(), P_Setcdr(), P_Listp();
extern Object Const_Cons();
extern Object P_List(), P_Length(), P_Nullp(), P_Pairp(), P_Cxr();
extern Object P_Cddr(), P_Cdar(), P_Cadr(), P_Caar(), P_Cdddr(), P_Cddar();
extern Object P_Cdadr(), P_Cdaar(), P_Caddr(), P_Cadar(), P_Caadr(), P_Caaar();
extern Object P_Append(), P_Append_Set(), P_Last_Pair(), P_Reverse();
extern Object P_Reverse_Set(), P_List_Tail(), P_List_Ref();
extern Object General_Assoc(), P_Assq(), P_Assv(), P_Assoc();
extern Object P_Memq(), P_Memv(), P_Member(), P_Make_List(), Copy_List();

/* main.c:
 */
extern char *stkbase, *A_Out_Name;
extern Max_Stack, Interpreter_Initialized, Was_Dumped, GC_Debug, Bug_Compat;
extern Verbose;
extern SYMTAB *The_Symbols;
extern Object P_Command_Line_Args(), P_Exit();

/* math.c:
 */
extern Object Make_Integer(), Make_Fixnum(), Make_Reduced_Flonum(), P_Numberp();
extern Object P_Complexp(), P_Realp(), P_Rationalp(), P_Integerp(), P_Abs();
extern Object P_Zerop(), P_Positivep(), P_Negativep(), P_Oddp(), P_Evenp();
extern Object P_Exactp(), P_Inexactp(), P_Inc(), P_Dec();
extern Object P_Generic_Equal(), P_Generic_Less(), P_Generic_Greater();
extern Object P_Generic_Eq_Less(), P_Generic_Eq_Greater();
extern Object P_Generic_Plus(), P_Generic_Minus(), P_Generic_Multiply();
extern Object P_Generic_Divide(), P_Quotient(), P_Remainder(), P_Modulo();
extern Object P_Gcd(), P_Lcm(), P_Floor(), P_Ceiling(), P_Truncate();
extern Object P_Round(), P_Sqrt(), P_Exp(), P_Log(), P_Sin(), P_Cos(), P_Tan();
extern Object P_Asin(), P_Acos(), P_Atan(), P_Min(), P_Max(), P_Random();
extern Object P_Srandom(), P_Make_Unsigned(), P_Number_To_String();
extern double Get_Double();

/* prim.c:
 */

/* print.c:
 */
extern Object P_Write(), P_Display(), P_Write_Char(), P_Newline(), P_Format();
extern Object P_Clear_Output_Port(), P_Flush_Output_Port(), P_Print();
extern Object P_Get_Output_String();
extern Saved_Errno;

/* proc.c:
 */
extern char *Error_Tag;
extern Tail_Call;
extern Object Eval();
extern Object Sym_Lambda, Sym_Macro;
extern Object P_Eval(), P_Apply(), Funcall(), P_Lambda(), P_Map();
extern Object P_Procedure_Env(), P_Procedure_Lambda(), Make_Primitive();
extern Object P_For_Each(), P_Procedurep(), Funcall_Compound();
extern Object P_Macro(), P_Macro_Body(), P_Macro_Expand();
extern Object P_Primitivep(), P_Compoundp(), P_Macrop();

/* promise.c:
 */
extern Object P_Delay(), P_Force(), P_Promisep(), P_Promise_Env();

/* read.c:
 */
extern Object Sym_Quote, Sym_Quasiquote, Sym_Unquote, Sym_Unquote_Splicing;
extern Object General_Read(), P_Read(), P_Read_Char();
extern Object P_Unread_Char(), P_Read_String(), P_Clear_Input_Port();
extern Object Parse_Number();

/* special.c:
 */
extern Object P_Quote(), P_If(), P_Let(), P_Letseq(), P_Letrec(), P_Case();
extern Object P_Cond(), P_And(), P_Or(), P_Do(), P_Quasiquote(), P_Fluid_Let();
extern Object P_Begin(), P_Begin1();

/* stab.c:
 */
extern SYMTAB *Snarf_Symbols(), *Open_File_And_Snarf_Symbols();

/* string.c:
 */
extern char Char_Map[];
extern Object General_Make_String(), Make_String(), P_Make_String();
extern Object Make_Const_String();
extern Object P_Stringp(), P_String_To_Number();
extern Object P_Str_Eq(), P_Str_Less(), P_Str_Greater(), P_Str_Eq_Less();
extern Object P_Str_Eq_Greater(), P_Str_CI_Eq(), P_Str_CI_Less();
extern Object P_Str_CI_Greater(), P_Str_CI_Eq_Less(), P_Str_CI_Eq_Greater();
extern Object P_String_Length(), P_String_Ref(), P_String_Set(), P_Substring();
extern Object P_String_Copy(), P_String_Append(), P_List_To_String();
extern Object P_String_To_List(), P_Substring_Fill(), P_String_Fill();
extern Object P_String(), P_Substringp(), P_CI_Substringp();

/* symbol.c:
 */
extern Object Null, True, False, Unbound, Special, Void, Newline, Eof;
extern Object Zero, One;
extern Object Lookup_Symbol(), Intern(), P_Oblist();
extern Object P_Symbolp(), P_Symbol_To_String(), P_String_To_Symbol();
extern Object P_Put(), P_Get(), P_Symbol_Plist();

/* type.c:
 */
extern TYPEDESCR Types[];
extern Object P_Type(), P_Voidp();

/* vector.c:
 */
extern Object Make_Vector(), P_Make_Vector(), P_Vectorp(), P_Vector();
extern Object Make_Const_Vector();
extern Object P_Vector_Length(), P_Vector_Ref(), P_Vector_Set();
extern Object P_Vector_To_List(), P_List_To_Vector(), P_Vector_Fill();
extern Object P_Vector_Copy(), List_To_Vector();

#endif
