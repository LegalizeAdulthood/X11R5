/* This program tests whether stksize() is producing reasonable
 * results.
 */

#ifdef __GNUC__
#  define alloca __builtin_alloca
#endif

int Special;
char *stkbase;

main () {
    char foo;

    stkbase = &foo;
    f ();
    printf ("stksize() seems to work fine.\n");
    exit (0);
}

f () {
    int s, t;
    char buf[100];

    s = stksize ();
    if (s < 100 || s > 100000) {
	printf ("There seems to be a problem [1] with stksize().\n");
	exit (1);
    }
    (void)alloca (100);
    t = stksize ();
    if (t < s) {
	printf ("There seems to be a problem [2] with stksize().\n");
	exit (1);
    }
    if (t > s + 104) {
	printf ("There seems to be a problem with stksize() or alloca().\n");
	exit (1);
    }
}
