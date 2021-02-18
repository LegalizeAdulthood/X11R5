/* If saveenv() and jmpenv() are working correctly, this program
 * prints the numbers 0 to 9.
 */

char *malloc();
char *env, *env2;
char *stkbase;
int Special;

int i, r = 1;

main () {
    char foo;

    stkbase = &foo;
    i = inner ();
    if (i == 7)
	jmpenv (env2, 9);
    jmpenv (env, r++);
    printf ("There seems to be a problem [1] with saveenv or jmpenv.\n");
    exit (1);
}

inner () {
    int r, len;

    inner2 ();
    len = stksize ();
    env = malloc (len);
    r = saveenv (env);
    printf ("%d\n", r+1);
    return r;
}

inner2 () {
    int r, len = stksize ();
    int a[10000];
    a[0] = 1; a[9999] = 2;

    env2 = malloc (len);
    r = saveenv (env2);
    printf ("%d\n", r);
    if (a[0] != 1 || a[9999] != 2) {
	printf ("There seems to be a problem [2] with saveenv or jmpenv.\n");
	exit (1);
    }
    if (r > 0)
	exit ();
}
