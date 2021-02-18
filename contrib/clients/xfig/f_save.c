/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "w_setup.h"

extern int      num_object;

write_file(file_name)
    char           *file_name;
{
    FILE           *fp;

    if (!ok_to_write(file_name, "SAVE"))
	    return (-1);

    if ((fp = fopen(file_name, "w")) == NULL) {
	put_msg("Couldn't open file %s, %s", file_name, sys_errlist[errno]);
	return (-1);
    }
    num_object = 0;
    if (write_objects(fp)) {
	put_msg("Error writing file %s, %s", file_name, sys_errlist[errno]);
	return (-1);
    }
    put_msg("%d object(s) saved in \"%s\"", num_object, file_name);
    return (0);
}

int
write_objects(fp)
    FILE           *fp;
{
    extern char     file_header[];
    F_arc          *a;
    F_compound     *c;
    F_ellipse      *e;
    F_line         *l;
    F_spline       *s;
    F_text         *t;

    /*
     * Number 2 means that the origin (0,0) is at the upper left corner of
     * the screen (2nd quadrant)
     */

    put_msg("Writing . . .");
    fprintf(fp, "%s\n", file_header);
    fprintf(fp, "%d %d\n", PIX_PER_INCH, 2);
    for (a = objects.arcs; a != NULL; a = a->next) {
	num_object++;
	write_arc(fp, a);
    }
    for (c = objects.compounds; c != NULL; c = c->next) {
	num_object++;
	write_compound(fp, c);
    }
    for (e = objects.ellipses; e != NULL; e = e->next) {
	num_object++;
	write_ellipse(fp, e);
    }
    for (l = objects.lines; l != NULL; l = l->next) {
	num_object++;
	write_line(fp, l);
    }
    for (s = objects.splines; s != NULL; s = s->next) {
	num_object++;
	write_spline(fp, s);
    }
    for (t = objects.texts; t != NULL; t = t->next) {
	num_object++;
	write_text(fp, t);
    }
    if (ferror(fp)) {
	fclose(fp);
	return (-1);
    }
    if (fclose(fp) == EOF)
	return (-1);
    return (0);
}

write_arc(fp, a)
    FILE           *fp;
    F_arc          *a;
{
    F_arrow        *f, *b;

    fprintf(fp, "%d %d %d %d %d %d %d %d %.3f %d %d %d %.3f %.3f %d %d %d %d %d %d\n",
	    O_ARC, a->type, a->style, a->thickness,
	    a->color, a->depth, a->pen, a->area_fill,
	    a->style_val, a->direction,
	    ((f = a->for_arrow) ? 1 : 0), ((b = a->back_arrow) ? 1 : 0),
	    a->center.x, a->center.y,
	    a->point[0].x, a->point[0].y,
	    a->point[1].x, a->point[1].y,
	    a->point[2].x, a->point[2].y);
    if (f)
	fprintf(fp, "\t%d %d %.3f %.3f %.3f\n", f->type, f->style,
		f->thickness, f->wid, f->ht);
    if (b)
	fprintf(fp, "\t%d %d %.3f %.3f %.3f\n", b->type, b->style,
		b->thickness, b->wid, b->ht);
}

write_compound(fp, com)
    FILE           *fp;
    F_compound     *com;
{
    F_arc          *a;
    F_compound     *c;
    F_ellipse      *e;
    F_line         *l;
    F_spline       *s;
    F_text         *t;

    fprintf(fp, "%d %d %d %d %d\n", O_COMPOUND, com->nwcorner.x,
	    com->nwcorner.y, com->secorner.x, com->secorner.y);
    for (a = com->arcs; a != NULL; a = a->next)
	write_arc(fp, a);
    for (c = com->compounds; c != NULL; c = c->next)
	write_compound(fp, c);
    for (e = com->ellipses; e != NULL; e = e->next)
	write_ellipse(fp, e);
    for (l = com->lines; l != NULL; l = l->next)
	write_line(fp, l);
    for (s = com->splines; s != NULL; s = s->next)
	write_spline(fp, s);
    for (t = com->texts; t != NULL; t = t->next)
	write_text(fp, t);
    fprintf(fp, "%d\n", O_END_COMPOUND);
}

write_ellipse(fp, e)
    FILE           *fp;
    F_ellipse      *e;
{
    if (e->radiuses.x == 0 || e->radiuses.y == 0)
	return;

    fprintf(fp, "%d %d %d %d %d %d %d %d %.3f %d %.3f %d %d %d %d %d %d %d %d\n",
	    O_ELLIPSE, e->type, e->style, e->thickness,
	    e->color, e->depth, e->pen, e->area_fill,
	    e->style_val, e->direction, e->angle,
	    e->center.x, e->center.y,
	    e->radiuses.x, e->radiuses.y,
	    e->start.x, e->start.y,
	    e->end.x, e->end.y);
}

write_line(fp, l)
    FILE           *fp;
    F_line         *l;
{
    F_point        *p;
    F_arrow        *f, *b;

    if (l->points == NULL)
	return;
    fprintf(fp, "%d %d %d %d %d %d %d %d %.3f %d %d %d\n",
	    O_POLYLINE, l->type, l->style, l->thickness,
	  l->color, l->depth, l->pen, l->area_fill, l->style_val, l->radius,
	    ((f = l->for_arrow) ? 1 : 0), ((b = l->back_arrow) ? 1 : 0));
    if (f)
	fprintf(fp, "\t%d %d %.3f %.3f %.3f\n", f->type, f->style,
		f->thickness, f->wid, f->ht);
    if (b)
	fprintf(fp, "\t%d %d %.3f %.3f %.3f\n", b->type, b->style,
		b->thickness, b->wid, b->ht);
    if (l->type == T_EPS_BOX)
	fprintf(fp, "\t%d %s\n", l->eps->flipped, l->eps->file);

    fprintf(fp, "\t");
    for (p = l->points; p != NULL; p = p->next) {
	fprintf(fp, " %d %d", p->x, p->y);
    };
    fprintf(fp, " 9999 9999\n");
}

write_spline(fp, s)
    FILE           *fp;
    F_spline       *s;
{
    F_control      *cp;
    F_point        *p;
    F_arrow        *f, *b;

    if (s->points == NULL)
	return;
    fprintf(fp, "%d %d %d %d %d %d %d %d %.3f %d %d\n",
	    O_SPLINE, s->type, s->style, s->thickness,
	    s->color, s->depth, s->pen, s->area_fill, s->style_val,
	    ((f = s->for_arrow) ? 1 : 0), ((b = s->back_arrow) ? 1 : 0));
    if (f)
	fprintf(fp, "\t%d %d %.3f %.3f %.3f\n", f->type, f->style,
		f->thickness, f->wid, f->ht);
    if (b)
	fprintf(fp, "\t%d %d %.3f %.3f %.3f\n", b->type, b->style,
		b->thickness, b->wid, b->ht);
    fprintf(fp, "\t");
    for (p = s->points; p != NULL; p = p->next) {
	fprintf(fp, " %d %d", p->x, p->y);
    };
    fprintf(fp, " 9999 9999\n");/* terminating code  */

    if (s->controls == NULL)
	return;
    fprintf(fp, "\t");
    for (cp = s->controls; cp != NULL; cp = cp->next) {
	fprintf(fp, " %.3f %.3f %.3f %.3f",
		cp->lx, cp->ly, cp->rx, cp->ry);
    };
    fprintf(fp, "\n");
}

write_text(fp, t)
    FILE           *fp;
    F_text         *t;
{
    if (t->length == 0)
	return;
    fprintf(fp, "%d %d %d %d %d %d %d %.3f %d %d %d %d %d %s\1\n",
	    O_TEXT, t->type, t->font, t->size, t->pen,
	    t->color, t->depth, t->angle,
	    t->flags, t->height, t->length,
	    t->base_x, t->base_y, t->cstring);
}

emergency_save(file_name)
    char           *file_name;
{
    FILE           *fp;

    if ((fp = fopen(file_name, "w")) == NULL)
	return (-1);
    num_object = 0;
    if (write_objects(fp))
	return (-1);
    (void) fprintf(stderr, "xfig: %d object(s) saved in \"%s\"\n",
		   num_object, file_name);
    return (0);
}
