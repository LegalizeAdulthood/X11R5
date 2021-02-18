typedef struct {
    char *name;
    unsigned long val;
} SYMDESCR;

extern unsigned long Symbols_To_Bits();
extern Object Bits_To_Symbols();
