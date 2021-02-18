#ifdef sccs
static char     sccsid[] = "@(#)sb_menu.c	1.13 91/09/12";
#endif
/*
****************************************************************************

              Copyright 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution of
the software without specific, written prior permission.  
Sun Microsystems, Inc. makes no representations about the suitability
of this software for any purpose.  It is provided "as is" without 
express or implied warranty.

SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL  SUN MICROSYSTEMS, INC. BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Auther: Hiroshi Watanabe	(nabe@japan.sun.com) Sun Microsystems, Inc.
	Tomonori Shioda		(shioda@japan.sun.com) Sun Microsystems, Inc.

****************************************************************************
*/


#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>

#include "sb_shinbun.h"
#include "sb_item.h"
#include "shinbun.h"

/*
 * Already used menu item num in main panel menu.
 */
#define DEFAULT_MENU_ITEM_NUM	2
#define MAX_MENU_NUM		100	/* Max num of the main panel menu */

#define MAX_MENU_LIST		200
#define MAX_LEN_GROUP_NAME	50
/*
 * This is used for holding the same hierarchy of the newsgroup which is
 * separated '.'. Allocating the array statically maybe bad, but this array
 * is often used for the long time usage.
 */
static char     menu_list[MAX_MENU_LIST][MAX_LEN_GROUP_NAME];

typedef struct struct_menu {
	char           *menu_str;
	int             menu_flag;
}               MENU_DATA;

Menu            sb_select_group_menu;
Menu            sb_all_group_menu;

typedef enum {
	SELECT, ALL
}               Sb_menu_type;

static Menu     sb_group_menu_create();
static Menu     sb_group_menu_build();
static Menu     sb_pullright_gen_proc();
static void     sb_pullright_proc();
static char    *sb_dot_str();
static int      sb_get_next_menu();
static int      sb_chk_dup_nwsgrp();

/*
 * Menu item proc.
 */
Menu
sb_group_select_menu_proc(menu, operation)
	Menu_item       menu;
	Menu_generate   operation;
{
	return (sb_select_group_menu =
		sb_group_menu_create(sb_select_group_menu, "", "", SELECT));
}

/*
 * Menu item proc.
 */
Menu
sb_group_all_menu_proc(menu, operation)
	Menu_item       menu;
	Menu_generate   operation;
{
	return (sb_all_group_menu =
		sb_group_menu_create(sb_all_group_menu, "", "", ALL));
}

/*
 * Create the menu of the newsgroup order.
 */
static          Menu
sb_group_menu_create(menu, parent_menu_str, current_menu_str, flag)
	Menu            menu;	/* parent menu */
	char           *parent_menu_str;	/* the parent menu text at
						 * that time. */
	char           *current_menu_str;	/* the menu text at that
						 * time. */
	Sb_menu_type    flag;	/* the flag whether ALL or SELECT. */
{
	MENU_DATA      *menu_data;
	Menu            m;
	char           *str;

	if (menu)
		return (menu);
	else {
		menu_data = (MENU_DATA *) malloc(sizeof(MENU_DATA));
		/*
		 * The parent menu str is added the current menu str.
		 */
		str = (char *) (malloc(strlen(parent_menu_str) +
				       strlen(current_menu_str) + 1));
		(void) strcpy(str, parent_menu_str);
		(void) strcat(str, current_menu_str);
		menu_data->menu_str = str;
		/*
		 * flag is used for whether ALL or SELECT.
		 */
		menu_data->menu_flag = flag;
	}
	m = sb_group_menu_build(menu_data);
	return m;
}

/*
 * Menu create.
 */
static          Menu
sb_group_menu_build(menu_data)
	MENU_DATA      *menu_data;
{
	register int    i;
	int             list_num;
	Menu            m;
	Menu_item       mi;
	char           *menu_str;

	xv_set(sb_frame, FRAME_BUSY, TRUE, 0);
	xv_set(XV_SERVER_FROM_WINDOW(sb_frame), SERVER_SYNC, FALSE, 0);

	sb_get_next_menu(menu_data->menu_str, menu_data->menu_flag, &list_num);

	if (list_num == 0) {
		list_num = 1;
	}
	if (list_num <= 3)
		i = 1;
	else if (list_num <= 12)
		i = 2;
	else if (list_num <= 27)
		i = 3;
	else if (list_num <= 48)
		i = 4;
	else if (list_num <= 75)
		i = 5;
	else if (list_num <= 108)
		i = 6;
	else if (list_num <= 147)
		i = 7;
	else
		i = 8;

	m = xv_create(NULL, MENU,
		      MENU_NCOLS, i,
		      MENU_CLIENT_DATA, menu_data,
		      0);

	for (i = 0; i < list_num; i++) {
		if (menu_str = malloc(strlen(menu_list[i]) + 1))
			strcpy(menu_str, menu_list[i]);
		/*
		 * else menu_str = &menu_list[i];
		 */

		mi = xv_create(NULL, MENUITEM,
			       MENU_STRING, menu_str,
			       MENU_RELEASE_IMAGE,
			       NULL);
		xv_set(m, MENU_APPEND_ITEM, mi, NULL);
	}

	for (i = 0; i < list_num; i++) {
		char           *str;
		int             len;

		mi = xv_get(m, MENU_NTH_ITEM, i + 1);
		len = (int) strlen(menu_list[i]);
		str = &menu_list[i][0];
		if (*(str + len - 1) == '.')
			xv_set(mi, MENU_GEN_PULLRIGHT, sb_pullright_gen_proc, NULL);

		xv_set(mi, MENU_ACTION_PROC, sb_pullright_proc, NULL);
	}

	xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
	return (m);
}

/*
 * Create menus and menus if need.
 */
static          Menu
sb_pullright_gen_proc(mi, operation)
	Menu_item       mi;
	Menu_generate   operation;
{
	Menu            pull_right;
	Menu            parent;
	MENU_DATA      *menu_data;

	parent = (Menu) (xv_get(mi, MENU_PARENT));
	pull_right = (Menu) (xv_get(mi, MENU_CLIENT_DATA));

	switch (operation) {
	case MENU_DISPLAY:
		menu_data = (MENU_DATA *) xv_get(parent, MENU_CLIENT_DATA);
		pull_right = sb_group_menu_create(pull_right, menu_data->menu_str,
			     xv_get(mi, MENU_STRING), menu_data->menu_flag);
		(void) xv_set(mi, MENU_CLIENT_DATA, pull_right, NULL);
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY:
	case MENU_NOTIFY_DONE:
		break;
	}
	return (pull_right);
}

/*
 * Input the str to the panel text field.
 */
static void
sb_pullright_proc(m, mi)
	Menu            m;
	Menu_item       mi;
{

	char            str[128];
	MENU_DATA      *menu_data;

	*str = '\0';
	menu_data = (MENU_DATA *) xv_get(m, MENU_CLIENT_DATA);
	(void) strcpy(str, menu_data->menu_str);
	(void) strcat(str, xv_get(mi, MENU_STRING));

	(void) xv_set(sb_group_txt, PANEL_VALUE, str, NULL);
}

/*
 * Get the menu strs. This strs are constructed the same hierarchy of the
 * newsgroup and must be guranteed only the one.
 */
static int
sb_get_next_menu(str, flag, list_num)
	char           *str;
	Sb_menu_type    flag;
	int            *list_num;
{

	FILE           *fp;
	int             len;
	char            buf[128];
	char           *str2;
	int             i = 0;
	int             j = 0;
	char            str_buf[256];

	menu_list[0][0] = NULL;
	len = strlen(str);

	if (flag == ALL) {
		if ((fp = fopen(ACTIVE_FLIST_NAM, "r")) == NULL) {
			fprintf(stderr, "Can not read file %s", ACTIVE_FLIST_NAM);
			return;
		}
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strncmp(buf, str, strlen(str)) == 0) {
				str2 = sb_dot_str(buf, len);
				sb_chk_dup_nwsgrp(&i, str2);
			}
		}
		fclose(fp);
	} else if (flag == SELECT) {
		while (j < num_of_ngrp) {
			strncpy(str_buf, return_newsrc_data(j), 255);
			if (strstr(str_buf, ":") &&
			    (strncmp(str_buf, str, strlen(str)) == 0)) {
				str2 = sb_dot_str(str_buf, len);
				sb_chk_dup_nwsgrp(&i, str2);
			}
			j++;
		}
	} else {
		fprintf(stderr, "ERR menu function\n");
		exit(1);
	}

	*list_num = i;
	menu_list[i + 1][0] = NULL;
}

static char    *
sb_dot_str(line, len)
	char           *line;
	int             len;
{
	static char     dot_tmp[128];
	char           *str = &line[len];
	char           *p = &dot_tmp[0];

	while (*str) {
		if (*str == '.') {
			*p++ = *str++;
			*p = '\0';
			break;
		}
		if (*str == ':') {
			*p = '\0';
			break;
		}
		*p++ = *str++;
	}
	return (dot_tmp);
}

static int
sb_chk_dup_nwsgrp(i, str)
	int            *i;
	char           *str;
{
	int             j = 0;

	while (*i > j) {
		if ((int) strcmp(str, menu_list[j]) == 0)
			return;
		j++;
	}
	strcpy(menu_list[j++], str);
	*i = j;
}

/*
 * Destroy the menu of the main pane.
 */
void
sb_destroy_group_menu(menu)
	Menu            menu;
{
	register int    i;
	Menu            pull_right;
	MENU_DATA      *menu_data;

	if (!menu)
		return;

	for (i = (int) xv_get(menu, MENU_NITEMS); i > 0; i--) {
		pull_right = (Menu) (xv_get(xv_get(menu, MENU_NTH_ITEM, i),
					    MENU_CLIENT_DATA));
		if (pull_right)
			sb_destroy_group_menu(pull_right);
	}
	/*
	 * Should be freed the alocating memory.
	 */
	menu_data = (MENU_DATA *) xv_get(menu, MENU_CLIENT_DATA);
	free(menu_data->menu_str);
	free(menu_data);

	menu_destroy(menu);

	/*
	 * Gurantee that sb_select_group_menu is NULL when the
	 * sb_select_group_menu menu is destroyed.
	 */
	if (menu == sb_select_group_menu)
		sb_select_group_menu = NULL;
	if (menu == sb_all_group_menu)
		sb_all_group_menu = NULL;
}

/*
 * Add the menu history
 */
void
sb_add_menu_item(str)
	char           *str;
{
	int             num;
	int             i, j;
	Menu_item       mi;
	void            sb_add_menu_proc();
	char           *menu_str;

	if (str == NULL || *str == '\n')
		return;

	num = (int) xv_get(sb_group_menu, MENU_NITEMS);

	j = DEFAULT_MENU_ITEM_NUM + 1;
	for (i = num; i >= j; i--) {
		mi = (Menu_item) xv_get(sb_group_menu, MENU_NTH_ITEM, i);
		if (strcmp(str, (char *) xv_get(mi, MENU_STRING)) == 0)
			return;
	}

	num -= (DEFAULT_MENU_ITEM_NUM + 1);
	if (num == MAX_MENU_NUM) {
		mi = (Menu_item) xv_get(sb_group_menu,
				  MENU_NTH_ITEM, DEFAULT_MENU_ITEM_NUM + 2);
		xv_set(sb_group_menu, MENU_REMOVE_ITEM, mi, NULL);
	}
	if (menu_str = malloc(strlen(str) + 1))
		strcpy(menu_str, str);
	else
		menu_str = str;

	mi = xv_create(XV_NULL, MENUITEM,
		       MENU_STRING, menu_str,
		       MENU_RELEASE_IMAGE,
		       MENU_NOTIFY_PROC, sb_add_menu_proc,
		       NULL);
	xv_set(sb_group_menu, MENU_APPEND_ITEM, mi, NULL);

	if (num == 0) {
		num = 1;
	}
	if (num <= 3)
		i = 1;
	else if (num <= 12)
		i = 2;
	else if (num <= 27)
		i = 3;
	else if (num <= 48)
		i = 4;
	else if (num <= 75)
		i = 5;
	else
		i = 6;

	xv_set(sb_group_menu, MENU_NCOLS, i, NULL);
}

void
sb_add_menu_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	xv_set(sb_group_txt, PANEL_VALUE,
	       (char *) xv_get(menu_item, MENU_STRING), NULL);
}
