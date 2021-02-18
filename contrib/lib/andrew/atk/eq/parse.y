/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/eq/RCS/parse.y,v 2.6 1991/10/03 16:13:47 bobg Exp $ */
/* $ACIS:parse.y 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/eq/RCS/parse.y,v $ */
 
/*
 * parse.y
 * This module parses the datastream.
 */
 
%token Y_GOESTO Y_D Y_INT Y_EQ Y_OP Y_UNOP Y_BINOP
%token Y_ITER Y_LIM Y_FUNC Y_ATOM Y_PRIME Y_DOT Y_BAD
 
%token Y_OPEN Y_CLOSE Y_ROOT Y_BEGIN Y_OVER Y_END
%token Y_SUP Y_SUB Y_PILE Y_EQSTYLE Y_SPACE
 
%start choose
 
%{
 
union ptr {
    struct node *n;
    char *s;
};
 
#define YYSTYPE union ptr
union ptr x(), value;
 
#include <class.h>
#include <eq.ih>
#include <stdio.h>
 
%}
 
%%
 
 
/*
 * Switch between the grammars depending on output desired
 * yylex inserts one of 'c', 'e', etc. as the first token
 */
 
choose  : 'c' c_prog                                    { value = $2; }
        | 'e' e_prog                                    { value = $2; }
        ;
 
/*
 * C translator
 */

c_prog	: Y_BEGIN Y_PILE Y_EQSTYLE c_lines Y_END		{$$=x("$4",&$0);}
	;

c_lines	: c_group					{$$=x("$1;\n",&$0);}
	| c_lines c_group				{$$=x("$1$2;\n",&$0);}
	;

c_group	: Y_BEGIN c_expr Y_END				{$$=x("$2",&$0);}
	;

c_paren	: Y_OPEN c_group Y_CLOSE			{$$=x("$2",&$0);}
	| Y_OPEN c_expr Y_CLOSE				{$$=x("$2",&$0);}
	;

c_cplx	: Y_ROOT c_group				{$$=x("sqrt($2)",&$0);}
	| Y_BEGIN Y_PILE c_group Y_OVER c_group Y_END	{$$=x("(($3)/($5))",&$0);}
	;

c_sup	: Y_SUP c_group					{$$=x("$2",&$0);} ;
c_sub	: Y_SUB c_group					{$$=x("$2",&$0);} ;
c_atom	: Y_ATOM | Y_D;
c_func	: c_atom | Y_FUNC;


c_term	: c_xterm
	| c_paren					{$$=x("($1)",&$0);}
	| c_paren c_term				{$$=x("($1)*$2",&$0);}
	;

c_unit	: c_atom c_paren				{$$=x("$1($2)",&$0);}
	| c_atom c_sub c_paren				{$$=x("$1[$2]($3)",&$0);}
	| c_atom c_sup c_paren				{$$=x("pow($1($3),$2)",&$0);}
	| c_atom c_sub c_sup c_paren			{$$=x("pow($1[$2]($4),$3)",&$0);}
	| c_atom c_sup c_sub c_paren			{$$=x("pow($1[$3]($4),$2)",&$0);}

	| c_cplx					{$$=x("$1",&$0);}
	| c_cplx c_sub					{$$=x("$1[$2]",&$0);}
	| c_cplx c_sup					{$$=x("pow($1,$2)",&$0);}
	| c_cplx c_sub c_sup				{$$=x("pow($1[$2], c_$3)",&$0);}
	| c_cplx c_sup c_sub				{$$=x("pow($1[$3], c_$2)",&$0);}
	;

c_unitx	: c_atom					{$$=x("$1",&$0);}
	| c_atom c_sub					{$$=x("$1[$2]",&$0);}
	| c_atom c_sup					{$$=x("pow($1,$2)",&$0);}
	| c_atom c_sub c_sup				{$$=x("pow($1[$2], c_$3)",&$0);}
	| c_atom c_sup c_sub				{$$=x("pow($1[$3], c_$2)",&$0);}
	;

c_xterm	: c_unitx
	| c_unit
	| c_unitx c_xterm				{$$=x("$1*$2",&$0);}
	| c_unit c_term					{$$=x("$1*$2",&$0);}

	| Y_FUNC c_term					{$$=x("$1($2)",&$0);}
	| Y_FUNC c_sup c_term				{$$=x("pow($1($3),$2)",&$0);}

	| Y_ITER c_term					{$$=x("$1($2)",&$0);}
	| Y_ITER Y_SUB Y_BEGIN c_atom Y_EQ c_expr Y_END c_sup c_term
							{$$=x("$1($4=$6,$8,$9)",&$0);}
	| Y_ITER c_sup Y_SUB Y_BEGIN c_atom Y_EQ c_expr Y_END c_term
							{$$=x("$1($5=$7,$2,$9)",&$0);}

	| Y_INT c_term Y_SPACE Y_D c_atom		{$$=x("$1($5,$2)",&$0);}
	| Y_INT c_sub c_term Y_SPACE Y_D c_atom		{$$=x("$1($6=$2,INF,$3)",&$0);}
	| Y_INT c_sup c_term Y_SPACE Y_D c_atom		{$$=x("$1($6=-INF,$2,$3)",&$0);}
	| Y_INT c_sub c_sup c_term Y_SPACE Y_D c_atom	{$$=x("$1($7=$2,$3,$4)",&$0);}
	| Y_INT c_sup c_sub c_term Y_SPACE Y_D c_atom	{$$=x("$1($7=$3,$2,$4)",&$0);}

	| Y_LIM c_xterm					{$$=x("$1($2)",&$0);}
	| Y_LIM Y_SUB Y_BEGIN c_atom Y_GOESTO c_expr Y_END c_xterm
							{$$=x("$1($4->$6,$8)",&$0);}
	;


c_binop	: Y_BINOP | Y_OP | Y_EQ | Y_GOESTO;
c_unop	: Y_UNOP | Y_OP;

c_opterm: c_term
        | c_unop c_term                                 {$$=x("$1$2",&$0);}
        ;
 
c_expr  : c_opterm
        | c_opterm c_binop c_expr                       {$$=x("$1 $2 $3",&$0);}
        ;
 
 
/*
 * Eqn translator
 */
 
e_prog  : Y_BEGIN Y_PILE Y_EQSTYLE e_lines Y_END
{$$=x(".EQ\ndelim ##\ndefine above \"to\"\ndefine below \"from\"\ndefine zilch \"\"\ndefine thick \" ~ \"\ndefine med \" ^ \"\ndefine thin \" ^ \"\ndefine vartheta \"theta\"\ndefine varsigma \"sigma\"\ndefine varphi \"phi\"\ndefine varomega \"omega\"\ndefine Alpha \"A\"\ndefine Beta \"B\"\ndefine Epsilon \"E\"\ndefine Zeta \"Z\"\ndefine Eta \"H\"\ndefine Iota \"I\"\ndefine Kappa \"K\"\ndefine Mu \"M\"\ndefine Nu \"N\"\ndefine Omicron \"O\"\ndefine Rho \"P\"\ndefine Tau \"T\"\ndefine Chi \"X\"\ndefine Gamma \"GAMMA\"\ndefine Delta \"DELTA\"\ndefine Theta \"THETA\"\ndefine Lambda \"LAMBDA\"\ndefine Xi \"XI\"\ndefine Sigma \"SIGMA\"\ndefine Upsilon \"UPSILON\"\ndefine Phi \"PHI\"\ndefine Psi \"PSI\"\ndefine Omega \"OMEGA\"\ndefine cint \"int\"\ndefine product \"prod\"\ndefine union_op \"union\"\ndefine inter_op \"inter\"\ndefine bold-a \"fat a\"\ndefine * \"times\"\ndefine down \"\\(da\"\ndefine up \"\\(ua\"\ndefine slash \"\\(sl\"\ndefine approx \"\\(ap\"\ndefine / \"\\(di\"\ndefine cthick \" ~ \"\ndefine cmed \" ^ \"\ndefine cthin \" ^ \"\ndefine or_op \"or\"\ndefine and_op \"and\"\ndefine -+ \"+-\"\ngsize 12\n.EN\n$4",&$0);}
	;

e_group	: Y_BEGIN e_eqstyle e_expr Y_END			{$$=x("$3",&$0);}
	;

e_lines	: e_group					{$$=x("\n#$1#\n",&$0);}
	| e_lines e_group				{$$=x("$1\n#$2#\n",&$0);}
	;

e_pile	: e_group					{$$=x("{$1}",&$0);}
	| e_pile e_group				{$$=x("% <$1> above % <$2>",&$0);}
	;

e_eqstyle	: /* empty */
	| Y_EQSTYLE
	;

e_thing	: Y_GOESTO | Y_D | Y_EQ | Y_OP | Y_UNOP | Y_BINOP
	| Y_LIM | Y_FUNC | Y_ATOM | Y_BAD | Y_OPEN | Y_CLOSE
	| Y_INT | Y_ITER | Y_SPACE			{$$=x("$1",&$0);}
	| Y_BEGIN Y_PILE e_eqstyle e_pile Y_END		{$$=x("$2 {$4}",&$0);}
	| Y_BEGIN Y_PILE e_group Y_OVER e_group Y_END	{$$=x("% <$3> over % <$5>",&$0);}
	| Y_ROOT e_group				{$$=x("sqrt {$2}",&$0);}
	| Y_OPEN e_group Y_CLOSE			{$$=x("left $1 $2 right $3",&$0);}
	;

e_scr	: e_thing
	| e_thing Y_SUP Y_BEGIN e_accs Y_END		{$$=x("$1 $4",&$0);}
	| e_thing Y_SUB e_group				{$$=x("$1 $2 <$3>",&$0);}
	| e_thing Y_SUP e_group				{$$=x("$1 $2 <$3>",&$0);}
	| e_thing Y_SUP e_group Y_SUB e_group		{$$=x("$1 $4 <$5> $2 <$3>",&$0);}
	| e_thing Y_SUB e_group Y_SUP e_group		{$$=x("$1 $2 <$3> $4 <$5>",&$0);}
	;

e_accs	: e_acc
	| e_accs e_acc					{$$=x("$1 $2",&$0);}
	;

e_acc	: Y_DOT						{$$=x("dot",&$0);}
	| Y_PRIME					{$$=x("prime",&$0);}
	;
	
e_expr	: e_scr
	| e_expr e_scr					{$$=x("$1 $2",&$0);}
	;

%%
static char *parse_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/eq/RCS/parse.y,v 2.6 1991/10/03 16:13:47 bobg Exp $";

 
 
#define NNODES 20              /* nodes to allocate at a time */

/*NCHILDREN must be as large as the maximum number of components
yacc will find on the right side of a rule */
#define NCHILDREN 10            /* children per node */
 
 
static struct node {
    char *fmt;
    union ptr children[10];
} *first_node, *next_node, *last_node;
 
static struct node_table {
    struct node *first;
    struct node *last;
    struct node_table *next;
} *all_nodes, *cur_nodes;

static long posn;
static struct eq *eqptr;
static long which;
 
 
eq__Parse(self, file, w)
struct eq *self;
FILE *file;
long w;
{
    extern int yydebug;
 
    /* make initial table of nodes */
    all_nodes = (struct node_table *)malloc (sizeof(struct node_table));
    all_nodes->next = NULL;
    all_nodes->first = (struct node *) calloc(NNODES, sizeof(struct node));
    all_nodes->last = &(all_nodes->first[NNODES-1]);

    cur_nodes = all_nodes;
    next_node = all_nodes->first;

    eqptr = self;
    which = w;
    posn = -1;
 
    if (!yyparse());
        print(file, value);
    fflush(file);
 
    /* free all the nodes */
    cur_nodes = all_nodes;
    while (cur_nodes != NULL)
        {
	struct node_table *temp;
	if (cur_nodes->first) free (cur_nodes->first);
	temp = cur_nodes->next;
	free (cur_nodes);
	cur_nodes = temp;
        }
}
 
 
static union ptr
x(fmt, a0)
char *fmt;
union ptr *a0;
{
    union ptr ret;
    register int i;
 
    for (i=0; i<NCHILDREN; i++)
        next_node->children[i] = a0[i];
    next_node->fmt = fmt;
    ret.n = next_node;
    next_node += 1;

    /* If the current table is full, allocate another and fill it. */
    if (next_node > cur_nodes->last)
        {
	cur_nodes->next = (struct node_table *)malloc (sizeof(struct node_table));
	cur_nodes = cur_nodes->next;
	cur_nodes->next = NULL;
	cur_nodes->first = (struct node *) calloc(NNODES, sizeof(struct node));
	cur_nodes->last = &(cur_nodes->first[NNODES-1]);
	next_node = cur_nodes->first;
        }
    return ret;
}
 
static int eqstyle = 1;  /* 1=display, 0=text, -1=script, ... */
 
#define is_in_table(addr,table) \
    ((table)->first<=(addr) && (addr)<=(table)->last)

/*
 * Go over list of all tables of nodes.
 * See if this address is in any of the tables.
 */

static int is_node(addr)
    struct node *addr;

{
    struct node_table *this;
    for (this = all_nodes; this != NULL; this = this->next)
        {
	if (is_in_table (addr, this)) return 1;
        }
    return 0;
}

static
print(file, ptr)
FILE *file;
union ptr ptr;
{
    if (is_node(ptr.n)) {
	register char *s;
	for (s = ptr.n->fmt;  *s;  s++) {
	    if (*s == '$') {
		register int i;
		for (i=0; '0'<=s[1] && s[1]<='9'; s++)
		    i = 10*i + s[1] - '0';
		if (i<NCHILDREN)
		    print(file, ptr.n->children[i]);
		else
		    fprintf(stderr, "Too many children!\n");
	    } else if (*s == '<') {
		fputs("{", file);
		eqstyle--;
	    } else if (*s == '>') {
		eqstyle++;
		fputs("}", file);
	    } else if (*s == '%') {
/*
                if (eqstyle<=0)
                    fprintf(file, "size -2");
*/
            } else
                putc(*s, file);
        }
    } else if (ptr.s) {
        fputs(ptr.s, file);
    }
}
 
static
yylex()
{
    register struct formula *f;
    if (posn==-1) {
        posn = 0;
        return which;
    }
    f = eq_Access(eqptr, posn);
    posn++;
    if (f) {
        yylval.s = f->symbol->name;
        return f->symbol->y_genre;
    } else {
        return 0;
    }
}
 
 
yyerror(s)
char *s;
{
    int i;
    printf("%s\n", s);
    if (posn>=1) {
        for (i=posn-1; !eq_Access(eqptr, i)->has_hot_spot; i++);
/*      eq_SetDot(eqptr, i, 0); */ /* does this ever get called? */
    } else {
        printf("totally malformed\n");
    }
}
