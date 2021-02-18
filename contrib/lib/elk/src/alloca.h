#ifdef PORTABLE

struct unwind_info {
    struct unwind_info *next;
    jmp_buf last_target;
};

#define Alloca_Begin struct unwind_info *first_unwind = 0

#define Alloca(ret,type,size) {\
    struct unwind_info *p_;\
    Disable_Interrupts;\
    p_ = (struct unwind_info *)Safe_Malloc (sizeof (struct unwind_info)\
	+ (size));\
    p_->next = first_unwind;\
    first_unwind = p_;\
    copy_jmpbuf (last_jmpbuf, p_->last_target);\
    if (setjmp (last_jmpbuf)) {\
	unwind_free (first_unwind);\
	longjmp (last_jmpbuf, 1);\
    }\
    Enable_Interrupts;\
    (ret) = (type)((char *)p_ + sizeof (struct unwind_info));\
}

#define Alloca_End {\
    Disable_Interrupts;\
    unwind_free (first_unwind);\
    Enable_Interrupts;\
}

#else

#define Alloca_Begin extern unused_   /* must expand to valid declaration */
#define Alloca(ret,type,size) ((ret) = (type)alloca(size))
#define Alloca_End

#endif
