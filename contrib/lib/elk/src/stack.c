stksize () {
    extern char *stkbase;
    char foo;
    return 115 + stkbase - &foo;
}
