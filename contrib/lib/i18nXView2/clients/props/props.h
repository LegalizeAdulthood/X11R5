/*
 * @(#)props.h 1.8 90/12/12
 */

/*
 * props.h - declarations for external interfaces to the props program.
 */

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/defaults.h>

#define BORDER_WIDTH	1

#define	COLOR_PANEL 	0
#define	ICON_PANEL 	0
#define	MENU_PANEL 	1
#define	MISC_PANEL 	2
#define	MOUSE_SET_PANEL 3
#ifdef OW_I18N
#define LOCALIZATION_PANEL 4
#define	TOTAL_PANELS	5	/* or 6 if color terminal */
#else
#define	TOTAL_PANELS	4	/* or 5 if color terminal */
#endif

typedef enum {
    D_number, D_string, D_boolean, D_nop
}           Deftype;

typedef struct {
    char       *name;
    char       *class;
    Deftype     type;
    caddr_t     default_value;
    caddr_t     misc;
    int         change_mark;	/* TRUE or FALSE */
    Panel_message_item change_mark_item;
    Panel_item  panel_item;
}           Description;

extern Frame frame;
extern Panel flavor_panel;
extern Panel_item flavor_choice;
extern Panel panel_group[TOTAL_PANELS + 1];
extern Panel current_panel;
extern Panel_item apply_button[TOTAL_PANELS + 1];
extern Panel_item reset_button[TOTAL_PANELS + 1];
extern int  showing_factory;
extern Display *dsp;
extern char *strtok();
#ifdef OW_I18N
extern int  color;
#endif

extern Description *allocate_desc_struct();
extern Panel_setting add_change_bar();


#ifdef OW_I18N
#define LOCALIZE(s)     dgettext("props",s)
#else
#define LOCALIZE(s)     s
#endif
