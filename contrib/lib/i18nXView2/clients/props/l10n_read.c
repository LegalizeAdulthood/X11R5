/*
 * "l10n_read.c"	Reading the l10n configuration file(s)
 */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<locale.h>
#include	<widec.h>
#include	<stdio.h>
#include	<string.h>
#include	"props.h"
#include	"l10n_props.h"


#define	L10N_CONFIG_PATH	"locale/props"

#define	MAX_LINE_LENGTH		256

#define	NAME_SEPARATOR		'='
#define	FIELD_SEPARATOR		'|'
#define	ITEM_SEPARATOR		';'


extern char	*malloc();
extern char	*calloc();

#ifdef SELF_DEBUG
main()
{
	wchar_t	line[256];
	wchar_t	c;
	l10n_config_list_t	*list;
	l10n_config_list_item_t	*item;
	

	setlocale(LC_ALL, "");

	read_l10n_config_file("C", "basic_setting", l10n_config_basic_setting);
	dump_l10n_config(l10n_config_basic_setting);

	for (item = l10n_config_basic_setting[0].items;
	     item != NULL; item = item->next)
	{
		list = l10n_setup(item->value);
		dump_l10n_config(list);
	}
	for (item = l10n_config_basic_setting[0].items;
	     item != NULL; item = item->next)
	{
		list = l10n_setup(item->value);
		dump_l10n_config(list);
	}
}
#endif


/*
 * FIX_ME! This routine must be sophisticated enough to allow more
 * flexible syntax in file.  Such as allow the white space between
 * each token.
 */
l10n_config_read(locale, file_name, a_list)
char			*locale;
char			*file_name;
l10n_config_list_t	*a_list;
{
	register wchar_t	*p1, *p2;
	register l10n_config_list_t	*list;
	register l10n_config_list_item_t *item;
	l10n_config_list_item_t	*item_new;

	register FILE		*config_file;
	int			i;
	wchar_t			*dft;
	char			*mbs;
	int			lineno;
	int			slotno;
	char			fullpath[MAXPATHLEN];
	wchar_t			line[MAX_LINE_LENGTH+1];
	int			rcode = -1;


	/*  
	 * Find path for localization configuration files under
	 * $OPENWINHOME/share/locale/props
	 */
	sprintf(fullpath, "%s/share/%s/%s/%s",
		getenv("OPENWINHOME"), L10N_CONFIG_PATH,
		locale, file_name);

	if ((config_file = fopen(fullpath, "r")) == NULL)
	{
		perror(fullpath);
		goto ret;
	}

	/* 
	 * Parse configuration file. 
	 */
	for (lineno = 1; fgetws(line, MAX_LINE_LENGTH, config_file) != NULL; lineno++)
	{
		p1 = line;
#if DEBUG > 4
printf("line = %ws", p1);
#endif
		if (*p1 == '#' || *p1 == '\n')
			continue;

		/*
		 * Pickup the category name.
		 */
		if ((p2 = wschr(p1, NAME_SEPARATOR)) == NULL)
		{
			fprintf(stderr, LOCALIZE("Bad format in %s (line#%d): missing name separator '%c')\n"),
					fullpath, lineno, NAME_SEPARATOR);
			goto ret;
		}

		/*
		 * Looking for the category name in list...
		 */
		*p2 = 0;	/* Overwrite '=' with string terminator */
		for (list = a_list; list->name != NULL; list++)
			if (wscmp(list->name, p1) == 0)
				break;
		if (list->name == NULL)
		{
			fprintf(stderr, LOCALIZE("Bad format in %s (line#%d): Unknown category name [%ws]\n"),
					fullpath, lineno, p1);
			goto ret;
		}
		p1 = ++p2;

		/*
		 * Picking up the default value.
		 */
		if ((p2 = wschr(p1, ITEM_SEPARATOR)) == NULL)
		{
			fprintf(stderr, LOCALIZE("Bad format in %s (line#%d): no default value\n"),
					fullpath, lineno);
			goto ret;
		}
		*p2++ = 0;
		dft = p1;	/* Keep it for now */
		p1 = p2;


		/*
		 * Picking up the items.
		 */
		item = NULL;
		for (slotno = 0; *p1 != '\n' && *p1 != 0; slotno++)
		{
			/*
			 * Allocate new space.
			 */
			item_new = (l10n_config_list_item_t *)
				calloc(1, sizeof (l10n_config_list_item_t));
			if (item == NULL)
				list->items = item_new;
			else
				item->next = item_new;
			item = item_new;

			/*
			 * Picking up the "value".
			 */
			if ((p2 = wschr(p1, FIELD_SEPARATOR)) == NULL)
			{
				fprintf(stderr, LOCALIZE("Bad format in %s (line#%d): missing field separator '%c'\n"),
					fullpath, lineno, FIELD_SEPARATOR);
				goto ret;
			}
			*p2++ = 0;
			item->value = malloc(wslen(p1) * sizeof(wchar_t) + 1);
			wstostr(item->value, p1);
			if (dft != NULL && wscmp(p1, dft) == 0)
			{
				list->default_value = slotno;
				dft = NULL;
			}
			p1 = p2;


			/*
			 * Picking up the label.
			 */
			if ((p2 = wschr(p1, ITEM_SEPARATOR)) != NULL
			 || (p2 = wschr(p1, '\n')) != NULL)
			{
				if (*p2 != ITEM_SEPARATOR)
					slotno = -1;
				*p2++ = 0;
			}
			if (list->convert_label != NULL)
				item->label = (*(list->convert_label))
							(item->value, p1);
			else
				item->label = wsdup(p1);
			if (slotno < 0)
				break;
			p1 = p2;
		}
		if (dft != NULL)
		{
			fprintf(stderr, LOCALIZE("Invalid default value in %s (line#%d): [%ws]\n"),
				fullpath, lineno, dft);
			goto ret;
		}
		if (defaults_exists(list->class, list->class) == TRUE)
		{
			mbs = defaults_get_string(list->class, list->class, "");
#if DEBUG > 2
			fprintf(stderr, "Load from defaults DB(Xrm) '%s:%s'\n",
			       list->class, mbs);
#endif
			for (i = 0, item = list->items;
			     item != NULL;
			     i++, item = item->next)
				if (strcmp(item->value, mbs) == 0)
				{
					list->current_value = i;
					break;
				}
			if (item == NULL)
			{
#ifdef notdef
				/*
				 * This could happen all the time, if
				 * you switch the "Basic Setting" from
				 * English to Japanese.  So, I'm
				 * decided do it silently.
				 */
				fprintf(stderr, LOCALIZE("Bad configuration: %s should not be %s for basic setting %s\n"),
						list->class, mbs, file_name);
#endif
				list->current_value = list->default_value;
			}
		}
		else
			list->current_value = list->default_value;
		list->initial_value = list->current_value;
	}
	rcode = 0;

ret:
	(void) fclose(config_file);

	return rcode;
}


#if DEBUG > 0 
l10n_config_dump(list)
register l10n_config_list_t	*list;
{
	register l10n_config_list_item_t	*item;
	register int	slotno;


	for (; list->name != NULL; list++)
	{
		printf("Category name = [%ws]\n", list->name);
		for (slotno = 0, item = list->items;
		     item != NULL;
		     slotno++, item = item->next)
		{
			printf("\titem (v/l) = (%s/%ws)%s\n",
			       item->value, item->label,
			       slotno == list->current_value
						? " <- default" : "");
		}
	}
}
#endif

