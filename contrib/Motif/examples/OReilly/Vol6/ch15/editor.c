/* Written by Dan Heller.  Copyright 1991, O'Reilly && Associates.
 * This program is freely distributable without licensing fees and
 * is provided without guarantee or warrantee expressed or implied.
 * This program is -not- in the public domain.
 */

/* editor.c -- create a full-blown Motif editor application complete
 * with a menubar, facilities to read and write files, text search
 * and replace, clipboard support and so forth.
 */
#include <Xm/Text.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <Xm/PanedW.h>
#include <Xm/FileSB.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

Widget text_w, search_w, replace_w, text_output;

main(argc, argv)
int argc;
char *argv[];
{
    XtAppContext  app;
    Widget        toplevel, main_w, menubar, pane, rowcol, pb;
    void          file_cb(), cut_paste(), search_cb();
    Arg           args[5];
    XmString      new, save, quit, quit_acc, file, edit, cut,
                  clear, copy, paste, search, next, find, replace;

    toplevel = XtVaAppInitialize(&app, "Demos",
        NULL, 0, &argc, argv, NULL, NULL);

    main_w = XtVaCreateWidget("main_w",
        xmMainWindowWidgetClass, toplevel, NULL);

    /* Create a simple MenuBar that contains three menus */
    file = XmStringCreateSimple("File");
    edit = XmStringCreateSimple("Edit");
    search = XmStringCreateSimple("Search");
    menubar = XmVaCreateSimpleMenuBar(main_w, "menubar",
        XmVaCASCADEBUTTON, file, 'F',
        XmVaCASCADEBUTTON, edit, 'E',
        XmVaCASCADEBUTTON, search, 'S',
        NULL);
    XmStringFree(file);
    XmStringFree(edit);
    XmStringFree(search);

    /* First menu is the File menu -- callback is file_cb() */
    new = XmStringCreateSimple("New ...");
    save = XmStringCreateSimple("Save ...");
    quit = XmStringCreateSimple("Quit");
    quit_acc = XmStringCreateSimple("Ctrl-C");
    XmVaCreateSimplePulldownMenu(menubar, "file_menu", 0, file_cb,
        XmVaPUSHBUTTON, new, 'N', NULL, NULL,
        XmVaPUSHBUTTON, save, 'S', NULL, NULL,
        XmVaSEPARATOR,
        XmVaPUSHBUTTON, quit, 'Q', "Ctrl<Key>c", quit_acc,
        NULL);
    XmStringFree(new);
    XmStringFree(save);
    XmStringFree(quit);
    XmStringFree(quit_acc);

    /* ...create the "Edit" menu --  callback is cut_paste() */
    cut = XmStringCreateSimple("Cut");      /* create a simple    */
    copy = XmStringCreateSimple("Copy");    /* pulldown menu that */
    clear = XmStringCreateSimple("Clear");  /* has these menu     */
    paste = XmStringCreateSimple("Paste");  /* items in it.       */
    XmVaCreateSimplePulldownMenu(menubar, "edit_menu", 1, cut_paste,
        XmVaPUSHBUTTON, cut, 'C', NULL, NULL,
        XmVaPUSHBUTTON, copy, 'o', NULL, NULL,
        XmVaPUSHBUTTON, paste, 'P', NULL, NULL,
        XmVaSEPARATOR,
        XmVaPUSHBUTTON, clear, 'l', NULL, NULL,
        NULL);
    XmStringFree(cut);
    XmStringFree(copy);
    XmStringFree(paste);

    /* create the "Search" menu -- callback is search_cb() */
    next = XmStringCreateSimple("Find Next");
    find = XmStringCreateSimple("Show All");
    replace = XmStringCreateSimple("Replace Text");
    XmVaCreateSimplePulldownMenu(menubar, "search_menu", 2, search_cb,
        XmVaPUSHBUTTON, next, 'F', NULL, NULL,
        XmVaPUSHBUTTON, find, 'S', NULL, NULL,
        XmVaPUSHBUTTON, replace, 'R', NULL, NULL,
        XmVaSEPARATOR,
        XmVaPUSHBUTTON, clear, 'C', NULL, NULL,
        NULL);
    XmStringFree(next);
    XmStringFree(find);
    XmStringFree(replace);
    XmStringFree(clear);

    XtManageChild(menubar);

    /* create a standard vertical PanedWindow... */
    pane = XtVaCreateWidget("pane",
        xmPanedWindowWidgetClass, main_w, NULL);

    /* create horizontal RowColumn inside the pane... */
    rowcol = XtVaCreateWidget("rowcol",
        xmRowColumnWidgetClass, pane,
        XmNorientation,  XmHORIZONTAL,
        XmNpacking,      XmPACK_TIGHT,
        NULL);
    /* Create two Text widgets with Labels... */
    XtVaCreateManagedWidget("Search Pattern:",
        xmLabelGadgetClass, rowcol, NULL);
    search_w = XtVaCreateManagedWidget("search_text",
        xmTextWidgetClass, rowcol, NULL);
    XtVaCreateManagedWidget("     Replace Pattern:",
        xmLabelGadgetClass, rowcol, NULL);
    replace_w = XtVaCreateManagedWidget("replace_text",
        xmTextWidgetClass, rowcol, NULL);
    XtManageChild(rowcol);

    text_output = XtVaCreateManagedWidget("text_out",
        xmTextWidgetClass, pane,
        XmNeditable,              False,
        XmNcursorPositionVisible, False,
        XmNshadowThickness,       0,
        XmNsensitive,             False,
        NULL);

    XtSetArg(args[0], XmNrows,      10);
    XtSetArg(args[1], XmNcolumns,   80);
    XtSetArg(args[2], XmNeditMode,  XmMULTI_LINE_EDIT);
    text_w = XmCreateScrolledText(pane, "text_w", args, 3);
    XtManageChild(text_w);

    XtManageChild(pane);
    XtManageChild(main_w);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app);
}

/* callback routine for "Ok" button in FileSelectionDialogs */
void
open_file(dialog, save, cbs)
Widget dialog;
int save;         /* actually, item_no from "new" or "save" */
XmFileSelectionBoxCallbackStruct *cbs;
{
    char buf[BUFSIZ], *filename, *text;
    struct stat statb;
    long len;
    FILE *fp;

    if (!XmStringGetLtoR(cbs->value,
        XmSTRING_DEFAULT_CHARSET, &filename))
        return; /* must have been an internal error */

    if (!*filename) {
        XtFree(filename);
        XBell(XtDisplay(text_w), 50);
        XmTextSetString(text_output, "Choose a file.");
        return; /* nothing typed */
    }

    if (save) {
        if (!(fp = fopen(filename, "w"))) {
            perror(filename);
            sprintf(buf, "Can't save to %s.", filename);
            XmTextSetString(text_output, buf);
            XtFree(filename);
            return;
        }
        /* saving -- get text from Text widget... */
        text = XmTextGetString(text_w);
        len = XmTextGetLastPosition(text_w);
        /* write it to file (check for error) */
        if (fwrite(text, sizeof(char), len, fp) != len)
            strcpy(buf, "Warning: did not write entire file!");
        else {
            /* make sure a newline terminates file */
            if (text[len-1] != '\n')
                fputc('\n', fp);
            sprintf(buf, "Saved %ld bytes to %s.", len, filename);
        }
    } else {
        /* make sure the file is a regular text file and open it */
        if (stat(filename, &statb) == -1 ||
                (statb.st_mode & S_IFMT) != S_IFREG ||
                !(fp = fopen(filename, "r"))) {
            perror(filename);
            sprintf(buf, "Can't read %s.", filename);
            XmTextSetString(text_output, buf);
            XtFree(filename);
            return;
        }
        /* put the contents of the file in the Text widget by
         * allocating enough space for the entire file, reading the
         * file into the space, and using XmTextSetString() to show
         * the file.
         */
        len = statb.st_size;
        if (!(text = XtMalloc((unsigned)(len+1)))) /* +1 for NULL */
            sprintf(buf, "%s: XtMalloc(%ld) failed", len, filename);
        else {
            if (fread(text, sizeof(char), len, fp) != len)
                sprintf(buf, "Warning: did not read entire file!");
            else
                sprintf(buf, "Loaded %ld bytes from %s.",
                    len, filename);
            text[len] = 0; /* NULL-terminate */
            XmTextSetString(text_w, text);
        }
    }
    XmTextSetString(text_output, buf); /* purge output message */

    /* free all allocated space. */
    XtFree(text);
    XtFree(filename);
    fclose(fp);
}

/* a menu item from the "File" pulldown menu was selected */
void
file_cb(w, item_no, cbs)
Widget w;
int item_no;  /* pulldown menu item number */
XmAnyCallbackStruct *cbs;  /* unused here */
{
    static Widget open_dialog, save_dialog;
    Widget        dialog = NULL;
    XmString      str, title;

    if (item_no == 2)
        exit(0);

    if (item_no == 0 && open_dialog)
        dialog = open_dialog;
    else if (item_no == 1 && save_dialog)
        dialog = save_dialog;

    if (dialog) {
        XtManageChild(dialog);
        /* make sure that dialog is raised to top of window stack */
        XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
        return;
    }
    dialog = XmCreateFileSelectionDialog(text_w, "Files", NULL, 0);
    XtAddCallback(dialog, XmNcancelCallback, XtUnmanageChild, NULL);
    XtAddCallback(dialog, XmNokCallback, open_file, item_no);
    if (item_no == 0) {
        str = XmStringCreateSimple("Open");
        title = XmStringCreateSimple("Open File");
        open_dialog = dialog;
    } else {
        str = XmStringCreateSimple("Save");
        title = XmStringCreateSimple("Save File");
        save_dialog = dialog;
    }
    XtVaSetValues(dialog,
        XmNokLabelString, str,
        XmNdialogTitle,   title,
        NULL);
    XmStringFree(str);
    XmStringFree(title);
    XtManageChild(dialog);
}

/* a menu item from the "Search" pulldown menu was selected */
void
search_cb(w, item_no, cbs)
Widget w;
int item_no;  /* pulldown menu item number */
XmAnyCallbackStruct *cbs;  /* unused here */
{
#define FIND_NEXT 0
#define FIND_ALL  1
#define REPLACE   2
#define CLEAR     3
    char *search_pat, *p, *string, *new_pat, buf[32];
    XmTextPosition pos = 0;
    int len, nfound = 0;
    int search_len, pattern_len;

    if (item_no == CLEAR) {
        pos = XmTextGetLastPosition(text_w);
        XmTextSetHighlight(text_w, 0, pos, XmHIGHLIGHT_NORMAL);
        return;
    }

    if (!(string = XmTextGetString(text_w)) || !*string) {
        XmTextSetString(text_output, "No text to search.");
        return;
    }
    if (!(search_pat = XmTextGetString(search_w)) || !*search_pat) {
        XmTextSetString(text_output, "Specify a search pattern.");
        XtFree(string);
        return;
    }

    new_pat = XmTextGetString(replace_w);
    search_len = strlen(search_pat);
    pattern_len = strlen(new_pat);

    /* start searching at current cursor position + 1 */
    if (item_no == FIND_NEXT)
        pos = XmTextGetCursorPosition(text_w) + 1;
    for (p = &string[pos]; p = index(p, *search_pat); p++)
        if (!strncmp(p, search_pat, search_len)) {
            nfound++;
            /* get the position where pattern was found */
            pos = (XmTextPosition)(p-string);
            if (item_no == REPLACE) {
                /* replace the text position + strlen(new_pat) */
                XmTextReplace(text_w, pos, pos + search_len, new_pat);
                /* "string" has changed -- get the new value */
		XtFree(string);
		string = XmTextGetString(text_w);
                /* continue search -after- replacement */
                p = &string[pos + pattern_len];
            } else if (item_no == FIND_ALL)
                XmTextSetHighlight(text_w, pos, pos+search_len,
                    XmHIGHLIGHT_SELECTED);
            else
                break;
        }
    if (item_no == FIND_NEXT && nfound == 0) {
        /* search from beginning till we've passed "pos" */
        for (p = string; p = index(p, *search_pat); p++)
            if (p - string > pos ||
                    !strncmp(p, search_pat, search_len)) {
                nfound++;
                break;
            }
    }
    if (nfound == 0)
        XmTextSetString(text_output, "Pattern not found.");
    else {
        switch (item_no) {
            case FIND_NEXT :
                pos = (XmTextPosition)(p - string);
                sprintf(buf, "Pattern found at position %ld.", pos);
                XmTextSetInsertionPosition(text_w, pos);
                break;
            case FIND_ALL :
                sprintf(buf, "Found %d occurrences.", nfound);
                break;
            case REPLACE :
            default :
                sprintf(buf, "Made %d replacements.", nfound);
        }
        XmTextSetString(text_output, buf);
    }
    XtFree(string);
    XtFree(search_pat);
    XtFree(new_pat);
}

/* the callback routine for the items in the edit menu */
void
cut_paste(widget, num)
Widget widget;  /* the menu item (pushbutton) that was selected */
int num;        /* the menu item number */
{
    Boolean result = True;

    switch (num) {
        case 0 : result = XmTextCut(text_w, CurrentTime); break;
        case 1 : result = XmTextCopy(text_w, CurrentTime); break;
        case 2 : result = XmTextPaste(text_w);
        case 3 : XmTextClearSelection(text_w, CurrentTime); break;
    }
    if (result == False)
        XmTextSetString(text_output, "There is no selection.");
    else
        XmTextSetString(text_output, NULL);
}
