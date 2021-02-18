#include <stdio.h>
/*

 * Declare our callback functions.
 */
static void button_selected();

/*
 * Miscellaneous Uil boilerplate: next 8 lines
 */
#include <Mrm/MrmAppl.h>                   /* Motif Toolkit */
static MrmHierarchy s_MrmHierarchy;     /* MRM database hierarch id */
static char *vec[]={"xuildemo.uid"};  /* MRM database file list   */
static MrmCode      class ;
static MrmCount     regnum = 1 ;
static MrmRegisterArg  regvec[] = {
    {"button_selected",(caddr_t) button_selected }
};

/*
 * Define our callback functions.
 */
static void button_selected(widget, call_data, client_data)
    Widget  widget;
    XtPointer client_data;
    XtPointer call_data;
{
    XmAnyCallbackStruct *data = (XmAnyCallbackStruct *) call_data;
    char *tag = (char *) client_data;

    printf("button selected\n");
}

int main(argc, argv)
int argc;
char **argv;
{
    XtAppContext    app_context;
    Widget topLevel, appMain;

    /*
     *  Initialize the MRM
     */
    MrmInitialize ();

    topLevel = XtVaAppInitialize(
           &app_context,       /* Application context */
           "XUilDemo",             /* application class name */
           NULL, 0,            /* command line option list */
           &argc, argv,        /* command line args */
           NULL,               /* for missing app-defaults file */
           NULL);              /* terminate varargs list */

    /*
     *  Define the Mrm.hierarchy (only 1 file)
     */

    if (MrmOpenHierarchy (1,        /* number of files      */
            vec,                    /* files        */
            NULL,
            &s_MrmHierarchy)        /* ptr to returned id   */
            != MrmSUCCESS) {
        printf ("Mrm can't open hierarchy\n");
    }

    /*
     *  Register our callback routines so that the resource manager can
     *  resolve them at widget-creation time.
     */

    if (MrmRegisterNames (regvec, regnum)
            != MrmSUCCESS)
         printf("Mrm can't register names\n");

    /*
     *  Call MRM to fetch and create the pushbutton and its container
     */

    if (MrmFetchWidget (s_MrmHierarchy,
            "helloworld_main",
            topLevel,
            &appMain,
            &class) != MrmSUCCESS)
        printf("Mrm can't fetch interface\n");

    /*
     *  Manage the main window of the hierarchy created by Mrm.
     */

    XtManageChild(appMain);

    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
}
