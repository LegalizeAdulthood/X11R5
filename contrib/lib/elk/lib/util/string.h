#define Declare_C_Strings int c_str_len; Alloca_Begin

#define Make_C_String(from,to) {\
    if (TYPE(from) == T_Symbol)\
	from = SYMBOL(from)->name;\
    else if (TYPE(from) != T_String)\
	Wrong_Type_Combination (from, "string or symbol");\
    c_str_len = STRING(from)->size;\
    Alloca (to, char*, c_str_len+1);\
    bcopy (STRING(from)->data, to, c_str_len);\
    to[c_str_len] = '\0';\
}

#define Dispose_C_Strings Alloca_End
