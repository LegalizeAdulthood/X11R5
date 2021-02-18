/*
 *			X D U . C
 *
 * Display the output of "du" in an X window.
 *
 * Phillip C. Dykstra
 * <phil@BRL.MIL>
 * 4 Sep 1991.
 * 
 * Copyright (C)	Phillip C. Dykstra	1991
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the authors name not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The author makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */
#include <stdio.h>

extern char *malloc(), *calloc();

#define	MAXDEPTH	80	/* max elements in a path */
#define	MAXNAME		1024	/* max pathname element length */
#define	MAXPATH		4096	/* max total pathname length */
#define	NCOLS		5	/* default number of columns in display */

/* What we IMPORT from xwin.c */
extern int xsetup(), xmainloop(), xdrawrect(), xrepaint();

/* What we EXPORT to xwin.c */
extern int press(), reset(), repaint();
int ncols = NCOLS;

/* internal routines */
char *strdup();
void addtree();
void parse();
void dumptree();
void clearrects();

/* order to sort paths by */
#define	ORD_FIRST	1
#define	ORD_LAST	2
#define	ORD_ALPHA	3
#define	ORD_SIZE	4
int order = ORD_FIRST;

/*
 * Rectangle Structure
 * Stores window coordinates of a displayed rectangle
 * so that we can "find" it again on key presses.
 */
struct rect {
	int	left;
	int	top;
	int	width;
	int	height;
};

/*
 * Node Structure
 * Each node in the path tree is linked in with one of these.
 */
struct node {
	char	*name;
	long	size;		/* from here down */
	struct	rect rect;	/* last drawn screen rectangle */
	struct	node *peer;	/* siblings */
	struct	node *child;	/* list of children if !NULL */
	struct	node *parent;	/* backpointer to parent */
} top;
struct node *topp = &top;
#define	NODE_NULL ((struct node *)0)

/*
 * create a new node with the given name and size info
 */
struct node *
makenode(name,size)
char *name;
int size;
{
	struct	node	*np;

	np = (struct node *)calloc(1,sizeof(struct node));
	np->name = strdup(name);
	np->size = size;

	return	np;
}

/*
 * Return the node (if any) which has a draw rectangle containing
 * the given x,y point.
 */
struct node *
findnode(treep, x, y)
struct	node *treep;
int	x, y;
{
	struct	node	*np;
	struct	node	*np2;

	if (treep == NODE_NULL)
		return	NODE_NULL;

	if (x >= treep->rect.left && x < treep->rect.left+treep->rect.width
	 && y >= treep->rect.top && y < treep->rect.top+treep->rect.height) {
		/*printf("found %s\n", treep->name);*/
		return	treep;	/* found */
	}

	/* for each child */
	for (np = treep->child; np != NULL; np = np->peer) {
		if ((np2 = findnode(np,x,y)) != NODE_NULL)
			return	np2;
	}
	return	NODE_NULL;
}

/*
 * return a count of the number of children of a given node
 */
int
numchildren(nodep)
struct node *nodep;
{
	int	n;

	if (nodep == NODE_NULL)
		return	0;

	n = 0;
	for (nodep = nodep->child; nodep != NODE_NULL; nodep=nodep->peer)
		n++;

	return	n;
}

static char usage[] = "\
Usage: xdu < du.out\n";

main(argc,argv)
int argc;
char **argv;
{
	char	buf[4096];
	int	size;
	char	name[4096];

	top.name = strdup("[root]");

	if (isatty(fileno(stdin))) {
		fprintf(stderr, usage);
		exit(1);
	}

	while (fgets(buf,sizeof(buf),stdin) != NULL) {
		sscanf(buf, "%d %s\n", &size, name);
		/*printf("%d %s\n", size, name);*/
		parse(name,size);
	}
	/*dumptree(&top,0);*/

	topp = &top;
	/* don't display root if only one child */
	if (numchildren(topp) == 1)
		topp = topp->child;

	xsetup(argc,argv);
	xmainloop();
	exit(0);
}

/* bust up a path string and link it into the tree */
void
parse(name,size)
char *name;
int size;
{
	char	*path[MAXDEPTH]; /* break up path into this list */
	char	buf[MAXNAME];	 /* temp space for path element name */
	int	arg, indx;

	if (*name == '/')
		name++;		/* skip leading / */

	arg = 0; indx = 0;
	bzero(path,sizeof(path));
	bzero(buf,sizeof(buf));
	while (*name != NULL) {
		if (*name == '/') {
			buf[indx] = 0;
			path[arg++] = strdup(buf);
			indx = 0;
			if (arg >= MAXDEPTH)
				break;
		} else {
			buf[indx++] = *name;
			if (indx >= MAXNAME)
				break;
		}
		name++;
	}
	buf[indx] = 0;
	path[arg++] = strdup(buf);
	path[arg] = NULL;

	addtree(&top,path,size);
}

int
compare(n1,n2,order)
struct node *n1, *n2;
int order;
{
	int	ret;

	if (order == ORD_SIZE) {
		ret = n2->size - n1->size;
		if (ret == 0)
			return strcmp(n1->name,n2->name);
		else
			return ret;
	}
	if (order == ORD_ALPHA)
		return strcmp(n1->name,n2->name);
	if (order == ORD_FIRST)
		return -1;
	if (order == ORD_LAST)
		return	1;

	/* shouldn't get here */
	fprintf(stderr,"xdu: bad insertion order\n");
	return	0;
}

void
insertchild(nodep,childp,order)
struct node *nodep;	/* parent */
struct node *childp;	/* child to be added */
int order;		/* FIRST, LAST, ALPHA, SIZE */
{
	struct node *np, *np1;

	if (nodep == NODE_NULL || childp == NODE_NULL)
		return;
	if (childp->peer != NODE_NULL) {
		fprintf(stderr, "xdu: can't insert child with peers\n");
		return;
	}

	childp->parent = nodep;
	if (nodep->child == NODE_NULL) {
		/* no children, order doesn't matter */
		nodep->child = childp;
		return;
	}
	/* nodep has at least one child already */
	if (compare(childp,nodep->child,order) < 0) {
		/* new first child */
		childp->peer = nodep->child;
		nodep->child = childp;
		return;
	}
	np1 = nodep->child;
	for (np = np1->peer; np != NODE_NULL; np = np->peer) {
		if (compare(childp,np,order) < 0) {
			/* insert between np1 and np */
			childp->peer = np;
			np1->peer = childp;
			return;
		}
		np1 = np;
	}
	/* at end, link new child on */
	np1->peer = childp;
}

/* add path as a child of top - recursively */
void
addtree(top, path, size)
struct node *top;
char *path[];
int size;
{
	struct	node *np;

	/*printf("addtree(\"%s\",\"%s\",%d)\n", top->name, path[0], size);*/

	/* check all children for a match */
	for (np = top->child; np != NULL; np = np->peer) {
		if (strcmp(path[0],np->name) == 0) {
			/* name matches */
			if (path[1] == NULL) {
				/* end of the chain, save size */
				np->size = size;
				return;
			}
			/* recurse */
			addtree(np,&path[1],size);
			return;
		}
	}
	/* no child matched, add a new child */
	np = makenode(path[0],size);
	insertchild(top,np,order);

	if (path[1] == NULL) {
		/* end of the chain, save size */
		np->size = size;
		return;
	}
	/* recurse */
	addtree(np,&path[1],size);
	return;
}

/* debug tree print */
void
dumptree(np,level)
struct node *np;
int level;
{
	int	i;
	struct	node *subnp;

	for (i = 0; i < level; i++)
		printf("   ");

	printf("%s %d\n", np->name, np->size);
	for (subnp = np->child; subnp != NULL; subnp = subnp->peer) {
		dumptree(subnp,level+1);
	}
}

/*
 * Draws a node in the given rectangle, and all of its children
 * to the "right" of the given rectangle.
 */
drawnode(nodep, rect)
struct node *nodep;	/* node whose children we should draw */
struct rect rect;	/* rectangle to draw all children in */
{
	struct rect subrect;

	/*printf("Drawing \"%s\" %d\n", nodep->name, nodep->size);*/

	xdrawrect(nodep->name, nodep->size,
		rect.left,rect.top,rect.width,rect.height);

	/* save current screen rectangle for lookups */
	nodep->rect.left = rect.left;
	nodep->rect.top = rect.top;
	nodep->rect.width = rect.width;
	nodep->rect.height = rect.height;

	/* draw children in subrectangle */
	subrect.left = rect.left+rect.width;
	subrect.top = rect.top;
	subrect.width = rect.width;
	subrect.height = rect.height;
	drawchildren(nodep, subrect);
}

/*
 * Draws all children of a node within the given rectangle.
 * Recurses on children.
 */
drawchildren(nodep, rect)
struct node *nodep;	/* node whose children we should draw */
struct rect rect;	/* rectangle to draw all children in */
{
	int	totalsize;
	int	totalheight;
	struct	node	*np;
	double	fractsize;
	int	height;
	int	top;

	/*printf("Drawing children of \"%s\", %d\n", nodep->name, nodep->size);*/
	/*printf("In [%d,%d,%d,%d]\n", rect.left,rect.top,rect.width,rect.height);*/

	top = rect.top;
	totalheight = rect.height;
	totalsize = nodep->size;
	if (totalsize == 0) {
		/* total the sizes of the children */
		totalsize = 0;
		for (np = nodep->child; np != NULL; np = np->peer)
			totalsize += np->size;
		nodep->size = totalsize;
	}

	/* for each child */
	for (np = nodep->child; np != NULL; np = np->peer) {
		fractsize = np->size / (double)totalsize;
		height = fractsize * totalheight + 0.5;
		if (height > 1) {
			struct rect subrect;
			/*printf("%s, drawrect[%d,%d,%d,%d]\n", np->name,
				rect.left,top,rect.width,height);*/
			xdrawrect(np->name, np->size,
				rect.left,top,rect.width,height);

			/* save current screen rectangle for lookups */
			np->rect.left = rect.left;
			np->rect.top = top;
			np->rect.width = rect.width;
			np->rect.height = height;

			/* draw children in subrectangle */
			subrect.left = rect.left+rect.width;
			subrect.top = top;
			subrect.width = rect.width;
			subrect.height = height;
			drawchildren(np, subrect);

			top += height;
		}
	}
}

/*
 * clear the rectangle information of a given node
 * and all of its decendents
 */
void
clearrects(nodep)
struct	node *nodep;
{
	struct	node	*np;

	if (nodep == NODE_NULL)
		return;

	nodep->rect.left = 0;
	nodep->rect.top = 0;
	nodep->rect.width = 0;
	nodep->rect.height = 0;

	/* for each child */
	for (np = nodep->child; np != NULL; np = np->peer) {
		clearrects(np);
	}
}

pwd()
{
	struct node *np;
	struct node *stack[MAXDEPTH];
	int num = 0;
	struct node *rootp;
	char path[MAXPATH];

	rootp = &top;
	if (numchildren(rootp) == 1)
		rootp = rootp->child;

	np = topp;
	while (np != NODE_NULL) {
		stack[num++] = np;
		if (np == rootp)
			break;
		np = np->parent;
	}

	path[0] = '\0';
	while (--num >= 0) {
		strcat(path,stack[num]->name);
		if (num != 0)
			strcat(path,"/");
	}
	printf("%-40s %8d (%6.2f)\n", path, topp->size,
		100.0*topp->size/rootp->size);
}

char *
strdup(s)
char *s;
{
	int	n;
	char	*cp;

	n = strlen(s);
	cp = malloc(n+1);
	strcpy(cp,s);

	return	cp;
}

/**************** External Entry Points ****************/

int
press(x,y)
int x, y;
{
	struct node *np;

	/*printf("press(%d,%d)...\n",x,y);*/
	np = findnode(&top,x,y);
	/*printf("Found \"%s\"\n", np?np->name:"(null)");*/
	if (np == topp) {
		/* already top, go up if possible */
		if (np->parent != &top || numchildren(&top) != 1)
			np = np->parent;
		/*printf("Already top, parent = \"%s\"\n", np?np->name:"(null)");*/
	}
	if (np != NODE_NULL) {
		topp = np;
		xrepaint();
	}
}

int
reset()
{
	topp = &top;
	if (numchildren(topp) == 1)
		topp = topp->child;
	xrepaint();
}

int
repaint(width,height)
int width, height;
{
	struct	rect rect;

	/* define a rectangle to draw into */
	rect.top = 0;
	rect.left = 0;
	rect.width = width/ncols;
	rect.height = height;

	clearrects(&top);	/* clear current rectangle info */
	drawnode(topp,rect);	/* draw tree into given rectangle */
#if 0
	pwd();			/* display current path */
#endif
}
