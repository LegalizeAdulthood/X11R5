/* $Id: xtalk.c,v 1.0 1991/10/03 18:18:11 lindholm Exp $ */
/*
  
  xtalk.c
  
  This is an X-based 4.3 BSD talk program.
  
  Written by Paul Whaley, University Computing Services, UBC.
  whaley@ucs.ubc.ca
  
  Bug fixes by George Lindholm, University Computing Services, UBC.
  lindholm@ucs.ubc.ca
  
  
  Copyright 1991 by the University of British Columbia.
  
  All rights reserved.
  
  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the University of
  British Columbia (U.B.C.) not be used in advertising or publicity
  pertaining to distribution of the software without specific, written
  prior permission.
  
  U.B.C. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
  U.B.C. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
  SOFTWARE.
  
  */

/*
  This source was based on the xtext example program for the
  Athena widgets, hence the following copyright notice:
  */

/*
 * November 14, 1989 - Chris D. Peterson 
 *
 * XConsortium: xtext.c,v 1.13 89/12/15 19:37:25 kit Exp
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
  There is also a lot of code borrowed from the 4.3 talk program, so:
  */

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <X11/Xos.h>
#include <X11/IntrinsicP.h>    /* For the version number */
#include <X11/Xfuncs.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <sys/socket.h>
#ifdef sun
#include <sys/filio.h>
#else
#include <sys/ioctl.h>
#endif
#include <sys/param.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Dialog.h>
#include "TalkSrc.h"
#include "talkd.h"
#include "xtalk.xbm"

#include <X11/Xaw/Cardinals.h>


extern int errno;
extern int sys_nerr;
extern char *sys_errlist[];

char	*getlogin();
char	*ttyname();
void	exit();

void do_dialog();
void name_dialog_ok();
void name_dialog_ok_action();
void name_dialog_cancel();
void start_protocol();
void get_names();
Boolean any();
void get_addrs();
void setname();
void open_ctl();
void open_sockt();
void ctl_send();
void ctl_resend();
void read_ctl_sockt();
void reinvite();
void sockt_connected();
void read_sockt();
void start_talking();
void read_talk();
void herchars();
void replacecb();
void mychars();
void p_error();
void display_state();
void slow_quit();
void quit();

String fallback_resources[] = {
    "*geometry:               500x250",
    "*input:                  True",
    "*resize:                 False",
    "*text*scrollVertical:    whenNeeded",
    "*quit*label:             Quit",
    "*quit*right:             ChainLeft",
    "*status*label:           Initializing.",
    "*status*justify:         center",
    "*status*resize:          False",
    "*status*borderWidth:     0",
    "*header*top:             ChainTop",
    "*header*bottom:          ChainTop",
    "*header*left:            ChainLeft",
    "*status*right:           ChainRight",
    "*nameOk*label:               Ok",
    "*nameOk*right:               ChainLeft",
    "*nameText*scrollVertical:    never", 
    "*nameText*value:             ",
    "*nameText*resizable:         True",
    "*nameText*resize:            both",
    "*nameText*translations:      #override \\n <Key>Return: ok()",
    "*nameText*right:             ChainRight",
    NULL,
};
static Arg textSrc_args [] = {
    {XtNeditType, (XtArgVal) XawtextAppend},
};
static Arg header_args [] = {
    {XtNshowGrip, (XtArgVal) False},
    {XtNresizeToPreferred, (XtArgVal) True},
};
static Arg text_args [] = {
    {XtNshowGrip, (XtArgVal) False},
    {XtNallowResize, (XtArgVal) True},
    {XtNresize, (XtArgVal) XawtextResizeNever},
    {XtNautoFill, (XtArgVal) False},
    {XtNwrap, (XtArgVal) XawtextWrapWord},
    {XtNscrollHorizontal, (XtArgVal) XawtextScrollNever},
};
static Arg nameText_args [] = {
    {XtNeditType, (XtArgVal) XawtextEdit},
};

XtActionsRec action_table[] = {
    {"ok",	name_dialog_ok_action}
};

#define C_ERASE 0
#define L_ERASE 1
#define W_ERASE 2
static char editchars[W_ERASE + 1];
static int editchars_read = 0;
#define CTL_H '\010'
#define CTL_U '\025'
#define CTL_W '\027'

Widget toplevel, paned, header, quitb, lab, name_text, name_ok, text, textSrc;

#define NAMELEN 9
char myname[9], hername[9];
char hfn[100];
char *his_full_name, *his_tty;

#define NOBODY 0
#define ME 1
#define HER 2
static char lastspeaker;

static int quitting = False;
static int started_talking = False;

static XawTextBlock textblock;

struct	sockaddr_in daemon_addr = { AF_INET };
struct	sockaddr_in ctl_addr = { AF_INET };
struct	sockaddr_in my_addr = { AF_INET };

/* inet addresses of the two machines */
struct	in_addr my_machine_addr;
struct	in_addr his_machine_addr;

u_short daemon_port;	/* port number of the talk daemon */

int ctl_sockt;
int sockt;
Boolean his_invitation_waiting = False;
Boolean my_invitation_waiting = False;
int local_id, remote_id;

static char *message_text[] = {
    "leave invitation with server",
    "check for invitation by callee",
    "delete invitation by caller",
    "announce invitation by caller",
};

CTL_MSG msg;
CTL_RESPONSE ctl_response;
int ctl_response_type;
#define CTL_WAIT 2	/* time to wait for a response, in seconds */

enum ctl_action_type {ca_look_up_him, ca_invite_remote, ca_announced,
			  ca_left_invite, ca_delete_his_invitation,
			  ca_nothing_more} ctl_action;
static char *ctl_action_text [] = {
    "look up him",
    "invite remote",
    "announced",
    "left_invite",
    "delete his invitation",
    "nothing more",
};

enum data_action_type {da_accept, da_read_edit_chars, da_talk} data_action;
static char *data_action_text [] = {
    "accept",
    "read edit chars",
    "talk",
};

static	char *answers[] = {
    "answer #0 - success!",				/* SUCCESS */
    "Your party is not logged on.",			/* NOT_HERE */
    "Target machine is too confused to talk to us.",	/* FAILED */
    "Target machine does not recognize us.",		/* MACHINE_UNKNOWN */
    "Your party is refusing messages.",			/* PERMISSION_REFUSED*/
    "Target machine can not handle remote talk.",	/* UNKNOWN_REQUEST */
    "Target machine says protocol mismatch.",		/* BADVERSION */
    "Target machine says protocol botch (addr).",	/* BADADDR */
    "Target machine says protocol botch (ctl_addr).",	/* BADCTLADDR */
};
#define	NANSWERS	(sizeof (answers) / sizeof (answers[0]))


static XtActionsRec xtalk_actions[] = {
    { "Quit", quit },
};

static XrmOptionDescRec xtalk_options[] = {
    { "-trace", "*trace", XrmoptionNoArg, (XtPointer) "on"},
    { "-sun", "*sun", XrmoptionNoArg, (XtPointer) "on"},
};

static struct resources {
    Boolean  trace;              /* Trace protocol */
    Boolean  sun_talk;           /* Use 4.2 BSD protocol (not supported yet) */
} app_resources;

#define offset(field) XtOffsetOf(struct resources, field)
static XtResource Resources[] = {
    {"trace", "Trace", XtRBoolean, sizeof(Boolean),
	 offset(trace), XtRImmediate, (XtPointer) False},
    {"sun", "Sun", XtRBoolean, sizeof(Boolean),
	 offset(sun_talk), XtRImmediate, (XtPointer) False},
};
#undef offset

static Atom wm_delete_window;
XtInputId sockt_id = NULL;
XtInputId ctl_sockt_id = NULL;
XtInputId conn_id = NULL;
XtIntervalId ctl_resend_id = NULL;
XtIntervalId ring_id = NULL;
XtAppContext app;

void main(argc, argv)
    int argc;
    char **argv;
{
    /* Main routine of xtalk. */
    
    Widget asciiSrc;
    Arg args[2];
    XWMHints wmhints;
    
    toplevel = XtAppInitialize(&app, "XTalk",
			       xtalk_options, XtNumber(xtalk_options),
			       &argc, argv, fallback_resources, NULL, 0);
    
    XtGetApplicationResources(toplevel, (XtPointer)&app_resources, Resources,
			      XtNumber(Resources), (ArgList) NULL, ZERO);
    
    XtAppAddActions(app, action_table, XtNumber(action_table));
    
    paned = XtCreateManagedWidget("paned", panedWidgetClass, toplevel, 
				  NULL, 0);
    
    header = XtCreateManagedWidget("header", formWidgetClass, paned,
				   header_args, XtNumber(header_args));
    
    quitb = XtCreateManagedWidget("quit", commandWidgetClass, header,
				  NULL, 0);
    
    XtSetArg(args[0], XtNfromHoriz, quitb);
    lab = XtCreateManagedWidget("status", labelWidgetClass, header,
				args, 1);
    
    textSrc = XtCreateWidget("textSrc", talkSrcObjectClass, toplevel,
			     textSrc_args, XtNumber(textSrc_args));
    text = XtCreateWidget("text", asciiTextWidgetClass, paned, 
			  text_args, XtNumber(text_args));
    XtSetArg(args[0], XtNtextSource, &asciiSrc);
    XtGetValues(text, args, 1);
    XtSetArg(args[0], XtNtextSource, textSrc);
    XtSetValues(text, args, 1);
    XtDestroyWidget(asciiSrc);
    
    XtAddCallback(textSrc, XtNreplaceCallback, replacecb, NULL);
    XtAddCallback(quitb, XtNcallback, quit, NULL);
    
    XtRealizeWidget(toplevel);
    
    /* Set up our preferred icon image */
    wmhints.icon_pixmap = XCreateBitmapFromData(XtDisplay(toplevel),
						XtWindow(toplevel),
						xtalk_bits, xtalk_width,
						xtalk_height);
    wmhints.flags = IconPixmapHint;
    XSetWMHints(XtDisplay(toplevel), XtWindow(toplevel), &wmhints);
    
    /* Handle delete requests */
    XtAppAddActions(app, xtalk_actions, XtNumber(xtalk_actions));
    XtOverrideTranslations(toplevel,
			   XtParseTranslationTable("<Message>WM_PROTOCOLS: Quit()"));
    wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
				   False);
    (void)XSetWMProtocols(XtDisplay(toplevel), XtWindow(toplevel),
			  &wm_delete_window, 1);
    
    if (argc < 2) {
	/* Didn't tell us who to talk to. Pop up a dialog to find out. */
	do_dialog();
    }
    else {
	/* Got at least who. */
	his_full_name = argv[1];
	if (argc > 2) {
	    /* Also specified a tty. */
	    his_tty = argv[2];
	} else {
	    his_tty = "";
	}
	/* Initialize the talk protocol and data. */
	start_protocol();
    }
    
    
    /* Do the event processing loop. */
    XtAppMainLoop(app);
    /* NOTREACHED */
}

void do_dialog()
{
    /* Put up a dialog to get the remote party name and optionally the tty.
       We don't do this as a popup, since this isn't a result of a user
       action. */
    
    Arg args[2];
    static char *always_params[] = {"Always"};
    
    display_state("Who do you wish to talk to?");
    XtSetArg(args[0], XtNfromVert, quitb);
    name_ok = XtCreateManagedWidget("nameOk", commandWidgetClass,
				    header, args, 1);
    XtAddCallback(name_ok, XtNcallback, name_dialog_ok, NULL);
    name_text = XtCreateManagedWidget("nameText", asciiTextWidgetClass,
				      header,
				      nameText_args, XtNumber(nameText_args));
    XtSetArg(args[0], XtNfromHoriz, name_ok);
    XtSetArg(args[1], XtNfromVert, quitb);
    XtSetValues(name_text, args, 2);
    
    XtCallActionProc(name_ok, "highlight", NULL, always_params, 1);
    XtSetKeyboardFocus(paned, name_text); /* Text gets keyboard input. */
}

/* ARGSUSED */
void name_dialog_ok(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    /* Callback for the ok button in the name dialog. */
    Arg args[2];
    char *str;
    int i;
    
    XtSetArg(args[0], XtNstring, &str);
    XtGetValues(name_text, args, 1);
    
    strncpy(hfn, str, sizeof(hfn) - 1);
    for (i = strlen(hfn) - 1; i > 0 && isspace((unsigned)hfn[i]); i--) {}
    hfn[i+1] = '\0';     /* Ignore trailing spaces */
    his_full_name = hfn + strspn(hfn, " "); /* ignore leading spaces */
    his_tty = strchr(his_full_name, ' ');
    if (his_tty) {
	/* Found a blank - isolate the tty name. */
	char *end_tty;
	*his_tty = 0;
	his_tty += strspn(++his_tty, " ");
	end_tty = strchr(his_tty, ' ');
	if (!end_tty) {
	    *end_tty = 0;
	}
    } else {
	his_tty = "";
    }
    XtDestroyWidget(name_text);
    XtDestroyWidget(name_ok);
    start_protocol();
}

/* ARGSUSED */
void name_dialog_ok_action(w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    /* An action routine for the dialog widget.
       This acts like the ok button. */
    name_dialog_ok(w, (XtPointer)NULL, (XtPointer)NULL);
}

void start_protocol()
{
    /* Finish initializing and start the talk control protocol. */
    Arg args[2];
    char title[100];
    
    /* Set the window title and icon name. */
    strcpy(title, "xtalk ");
    strncat(title, his_full_name, sizeof(title) - strlen(title) - 1);
    XtSetArg(args[0], XtNtitle, title);
    XtSetArg(args[1], XtNiconName, title);
    XtSetValues(toplevel, args, 2);
    
    get_names();
    setname(myname, msg.l_name);
    setname(hername, msg.r_name);
    lastspeaker = NOBODY;
    open_ctl();
    open_sockt();
    
    /* Finish initializing the msg. */
    msg.ctl_addr = *(struct sockaddr *)&ctl_addr;
    msg.ctl_addr.sa_family = htons(msg.ctl_addr.sa_family);
    
    /* Start by checking for an invite on his/her machine. */
    display_state("Checking for invitation on other machine.");
    ctl_send(his_machine_addr, LOOK_UP);
    ctl_action = ca_look_up_him;
    data_action = da_read_edit_chars;
    editchars_read = 0;
}

void get_names()
{
    /* Determine the local and remote user, tty, and machines. */
    char hostname[MAXHOSTNAMELEN];
    char *his_name, *my_name;
    char *my_machine_name, *his_machine_name;
    register char *cp;
    
    if ((my_name = getlogin()) == NULL) {
	struct passwd *pw;
	
	if ((pw = getpwuid(getuid())) == NULL) {
	    fprintf(stderr, "You don't exist. Go away.\n");
	    exit(-1);
	}
	my_name = pw->pw_name;
    }
    gethostname(hostname, sizeof (hostname));
    my_machine_name = hostname;
    
    /* check for, and strip out, the machine name of the target */
    for (cp = his_full_name; *cp && !any(*cp, "@:!."); cp++)
	;
    if (*cp == '\0') {
	/* this is a local to local talk */
	his_name = his_full_name;
	his_machine_name = my_machine_name;
    } else {
	if (*cp++ == '@') {
	    /* user@host */
	    his_name = his_full_name;
	    his_machine_name = cp;
	} else {
	    /* host.user or host!user or host:user */
	    his_name = cp;
	    his_machine_name = his_full_name;
	}
	*--cp = '\0';
    }
    
    get_addrs(my_machine_name, his_machine_name);
    
    /*
     * Initialize the message template.
     */
    msg.vers = TALK_VERSION;
    msg.addr.sa_family = htons(AF_INET);
    msg.ctl_addr.sa_family = htons(AF_INET);
    msg.id_num = htonl(0);
    strncpy(msg.l_name, my_name, NAME_SIZE);
    msg.l_name[NAME_SIZE - 1] = '\0';
    strncpy(msg.r_name, his_name, NAME_SIZE);
    msg.r_name[NAME_SIZE - 1] = '\0';
    strncpy(msg.r_tty, his_tty, TTY_SIZE);
    msg.r_tty[TTY_SIZE - 1] = '\0';
}

Boolean any(c, cp)
    register char c, *cp;
{
    /* Return true iff c is in the string cp. */
    while (*cp) {
	if (c == *cp++) {
	    return True;
	}
    }
    return False;
}

void get_addrs(my_machine_name, his_machine_name)
    char *my_machine_name, *his_machine_name;
{
    /* Get the addresses for the machine names. */
    struct hostent *hp;
    struct servent *sp;
    
    msg.pid = htonl(getpid());
    /* look up the address of the local host */
    hp = gethostbyname(my_machine_name);
    if (hp == NULL) {
	fprintf(stderr, "xtalk: %s: ", my_machine_name);
	perror((char *)NULL);
	exit(-1);
    }
    bcopy(hp->h_addr, (char *)&my_machine_addr, hp->h_length);
    /*
     * If the callee is on-machine, just copy the
     * network address, otherwise do a lookup...
     */
    if (strcmp(his_machine_name, my_machine_name)) {
	hp = gethostbyname(his_machine_name);
	if (hp == NULL) {
	    fprintf(stderr, "xtalk: %s: ", his_machine_name);
	    perror((char *)NULL);
	    exit(-1);
	}
	bcopy(hp->h_addr, (char *) &his_machine_addr, hp->h_length);
    } else {
	his_machine_addr = my_machine_addr;
    }
    /* find the server's port */
    sp = getservbyname("ntalk", "udp");
    if (sp == 0) {
	fprintf(stderr, "xtalk: %s/%s: service is not registered.\n",
		"ntalk", "udp");
	exit(-1);
    }
    daemon_port = sp->s_port;
}

void setname(nameptr, namestr)
    char *nameptr, *namestr;
{
    /* Set the nameptr from namestr. This right justifies and adds a colon. */
    int filllen;
    filllen = NAMELEN - strlen(namestr) - 1;
    if (filllen > 0) {
	memset(nameptr, ' ', filllen);
    } else {
	filllen = 0;
    }
    strncpy(nameptr + filllen, namestr, NAMELEN - 1 - filllen);
    nameptr[NAMELEN - 1] = ':';
}

void open_ctl() 
{
    /* Open and setup the control socket. */
    int length;
    
    ctl_addr.sin_port = htons(0);
    ctl_addr.sin_addr = my_machine_addr;
    ctl_sockt = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctl_sockt <= 0) {
	p_error("Bad socket");
    }
    if (bind(ctl_sockt, (struct sockaddr *)&ctl_addr, sizeof(ctl_addr)) != 0) {
	p_error("Couldn't bind to control socket");
    }
    length = sizeof(ctl_addr);
    if (getsockname(ctl_sockt, (struct sockaddr *)&ctl_addr, &length) == -1) {
	p_error("Bad address for ctl socket");
    }
    /* Arrange to be called when there is something to read. */
    ctl_sockt_id = XtAppAddInput(app, ctl_sockt, (XtPointer)XtInputReadMask,
				 read_ctl_sockt, NULL);
}

void open_sockt()
{
    /* Open and setup the data socket. */
    int length, nbio;
    int opt = 1;
    
    my_addr.sin_addr = my_machine_addr;
    my_addr.sin_port = htons(0);
    sockt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockt <= 0) {
	p_error("Bad socket");
    }
    
    if (bind(sockt, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0) {
	p_error("Binding local socket");
    }
    
    length = sizeof(my_addr);
    if (getsockname(sockt, (struct sockaddr *)&my_addr, &length) != 0) {
	p_error("Bad address for socket");
    }
    
    if (setsockopt(sockt, SOL_SOCKET, SO_DEBUG, &opt, sizeof(opt)) != 0) {
	p_error("Debugging local socket");
    }
    
    if (1) {
	if (fcntl(sockt, F_SETFL, FNDELAY) != 0) {
	    p_error("fcntl FNONBLOK on data socket");
	}
    } else {
	nbio = 1;
	if (ioctl(sockt, FIONBIO, &nbio) != 0) {
	    p_error("ioctl FIONBIO on data socket");
	}
    }
    
    /* Arrange to be called when there is something to read on sockt. */
    sockt_id = XtAppAddInput(app, sockt, (XtPointer)XtInputReadMask,
			     read_sockt, NULL);
}

void ctl_send(target, type)
    struct in_addr target;
    int type;
{
    /* Send the control message to the daemon. */
    msg.type = ctl_response_type = type;
    daemon_addr.sin_addr = target;
    daemon_addr.sin_port = daemon_port;
    ctl_resend((XtPointer)NULL, (XtIntervalId *)NULL);
}

/* ARGSUSED */
void ctl_resend(data, id)
    XtPointer data;
    XtIntervalId *id;
{
    /* Send or resend the current control message. */
    int sendlen;
    
    if (app_resources.trace) {
	fprintf(stderr, "ctl_(re)send: %s\n", message_text[msg.type]);
    }
    sendlen = sendto(ctl_sockt, (char *)&msg, sizeof (msg), 0, 
		     (struct sockaddr *)&daemon_addr, sizeof (daemon_addr));
    if (sendlen != sizeof(msg)) {
	p_error("Write to talk daemon");
    }
    
    /* Retry this every CTL_WAIT seconds. */
    ctl_resend_id = XtAppAddTimeOut(app, CTL_WAIT * 1000, ctl_resend, NULL);
}

/* ARGSUSED */
void read_ctl_sockt(client_data, file_num, id)
    caddr_t client_data;
    int *file_num;
    XtInputId *id;
{
    /* Read from the control socket. */
    
    int reclen;
    CTL_RESPONSE *rp;
    
    rp = &ctl_response;
    reclen = recv(ctl_sockt, (char *)rp, sizeof (*rp), 0);
    if (reclen < 0) {
	p_error("Read from talk daemon");
    }
    /* If we don't like this response, ignore it. */
    if (reclen < sizeof (*rp) || rp->vers != TALK_VERSION ||
	rp->type != ctl_response_type) {
	return;
    }
    
    /* Cancel the resend of our message. */
    XtRemoveTimeOut(ctl_resend_id);
    ctl_resend_id = NULL;
    
    /* Process the response. */
    rp->id_num = ntohl(rp->id_num);
    rp->addr.sa_family = ntohs(rp->addr.sa_family);
    
    for(;;) {
	if (app_resources.trace) {
	    fprintf(stderr, "ctl_read: state=%s, response=%s\n",
		    ctl_action_text[ctl_action], answers[rp->answer]);
	}
	switch (ctl_action) {
	  case ca_look_up_him:
	    switch(rp->answer) {
	      case SUCCESS:
		/* Found an invitation on the other machine. */
		msg.id_num = htonl(rp->id_num);
		display_state("Waiting to connect with caller.");
		conn_id = XtAppAddInput(app, sockt,
					(XtPointer)XtInputWriteMask,
					sockt_connected, NULL);
		started_talking = False;
		do {
		    if (rp->addr.sa_family != AF_INET) {
			p_error("Response uses invalid network address");
		    }
		    errno = 0;
		    if (connect(sockt, &rp->addr, sizeof (rp->addr)) != -1) {
			/* We have immediately connected. */
			XtRemoveInput(conn_id);
			conn_id = NULL;
			if (app_resources.trace) {
			    fprintf(stderr, "Connected.\n");
			}
			if (!started_talking) {
			    start_talking();
			}
			return;
		    }
		} while (errno == EINTR);
		if (errno == EINPROGRESS) {
		    /* Not completed immediately. We use the select on
		       writing to tell us when this has completed. */
		    if (app_resources.trace) {
			fprintf(stderr, "ctl_read: Connection in progress.\n");
		    }
		    return;
		}
		if (errno == ECONNREFUSED) {
		    /*
		     * The caller gave up, but his invitation somehow
		     * was not cleared. Clear it and initiate an 
		     * invitation. (We know there are no newer invitations,
		     * the talkd works LIFO.)
		     */
		    XtRemoveInput(conn_id);
		    conn_id = NULL;
		    ctl_send(his_machine_addr, DELETE);
		    display_state("Deleting stale invitation.");
		    XtRemoveInput(sockt_id);
		    close(sockt);
		    open_sockt();
		    ctl_action = ca_invite_remote;
		    break; /* proceeds to ca_invite_remote */
		}
		p_error("Unable to connect with initiator");
		/* NOTREACHED */
		
	      default:
		/* No invitation waiting for us. */
		ctl_action = ca_invite_remote; /* Try to invite him/her. */
		if (app_resources.trace) {
		    fprintf(stderr, "ctl_read: No connection for us.\n");
		}
		break;
	    }
	    break;
	    
	  case ca_invite_remote:
	    /* Invite the other party to talk to us. */
	    
	    if (listen(sockt, 5) != 0) {
		p_error("Listen for caller");
	    }
	    msg.addr = *(struct sockaddr *)&my_addr;
	    msg.addr.sa_family = htons(msg.addr.sa_family);
	    msg.id_num = htonl(-1);		/* an impossible id_num */
	    my_invitation_waiting = True;	/* so we will clean up */
	    
	    /* Tell the data socket reader to do an accept first. */
	    data_action = da_accept;
	    
	    ctl_send(his_machine_addr, ANNOUNCE);
	    display_state("Trying to connect to your party's talk daemon.");
	    ctl_action = ca_announced;
	    /* Keep ringing until we get a connection or we quit. */
	    ring_id = XtAppAddTimeOut(app, RING_WAIT * 1000, reinvite, NULL);
	    return;
	    
	  case ca_announced:
	    remote_id = rp->id_num;
	    if (rp->answer != SUCCESS) {
		if (rp->answer < NANSWERS) {
		    display_state(answers[rp->answer]);
		}
		slow_quit();
		/* NOTREACHED */
	    }
	    /* Leave the actual invitation on my talk daemon. */
	    his_invitation_waiting = True;
	    ctl_send(my_machine_addr, LEAVE_INVITE);
	    display_state("Leaving invitation on this talk daemon.");
	    ctl_action = ca_left_invite;
	    return;
	    
	  case ca_left_invite:
	    local_id = rp->id_num;
	    display_state("Waiting for your party to respond.");
	    return;
	    
	  case ca_delete_his_invitation:
	    msg.id_num = htonl(remote_id);
	    ctl_action = ca_nothing_more;
	    ctl_send(his_machine_addr, DELETE);
	    my_invitation_waiting = False;
	    if (app_resources.trace) {
		fprintf(stderr, "ctl_read: removing my invitation\n");
	    }
	    return;
	    
	  case ca_nothing_more:
	    if (app_resources.trace) {
		fprintf(stderr, "ctl_read: ctl_socket all done.\n");
	    }
	    return;
	}
    }
}

/* ARGSUSED */
void reinvite(data, id)
    XtPointer data;
    XtIntervalId *id;
{
    /* Re-invite the callee, with a forced re-announce. */
    msg.id_num = htonl(remote_id + 1);
    ctl_send(his_machine_addr, ANNOUNCE);
    display_state("Trying to connect to your party's talk daemon.");
    ctl_action = ca_announced;
    ring_id = XtAppAddTimeOut(app, RING_WAIT * 1000, reinvite, NULL);
}

/* ARGSUSED */
void sockt_connected(client_data, file_num, id)
    XtPointer client_data;
    int *file_num;
    XtInputId *id;
{
    /* This routine should be called when the data socket has connected. */
    XtRemoveInput(conn_id);
    conn_id = NULL;
    if (!started_talking) {
	start_talking();
    }
}

/* ARGSUSED */
void read_sockt(client_data, file_num, id)
    caddr_t client_data;
    int *file_num;
    XtInputId *id;
{
    /* Some input is available on the data socket. Process it. */
    int new_sockt, buflen;
    
    switch (data_action) {
      case da_accept:
	while ((new_sockt = accept(sockt, (struct sockaddr *)NULL,
				   (int *)NULL)) < 0) {
	    if (errno == EINTR) {
		continue;
	    }
	    if (errno == EWOULDBLOCK) {
		return;
	    }
	    p_error("Unable to connect with your party");
	    /* NOTREACHED */
	}
	close(sockt);
	XtRemoveInput(sockt_id);
	sockt_id = XtAppAddInput(app, new_sockt, (XtPointer)XtInputReadMask,
				 read_sockt, NULL);
	sockt = new_sockt;
	XtRemoveTimeOut(ring_id);
	
	/* Now that we have connected, have the daemons delete the
	   invitations. */
	display_state("Deleting his invitation.");
	msg.id_num = htonl(local_id);
	ctl_send(my_machine_addr, DELETE);
	ctl_action = ca_delete_his_invitation;
	start_talking();
	return;
	
      case da_read_edit_chars:
	buflen = read(sockt, &editchars[editchars_read],
		      sizeof(editchars) - editchars_read);
	if (buflen <= 0) {
	    p_error("Reading edit characters");
	}
	editchars_read += buflen;
	if (editchars_read == sizeof(editchars)) {
	    data_action = da_talk;
	}
	return;
	
      case da_talk:
	/* This is the usual case. */
	read_talk(*file_num);
	return;
    }
}

void start_talking()
{
    /* Start the data transfer part of the protocol.
       By agreement the first three characters each talk transmits
       after connection are the three edit characters. */
    int cc;
    
    /* Switch the socket to blocking I/O so our writes don't have to
       be retried. */
    cc = 0;
    if (ioctl(sockt, FIONBIO, &cc) != 0 && 0) {
	p_error("ioctl data socket to blocking I/O");
    }
    
    display_state("Talking.");
    started_talking = True;
    
    /* Although we only send CTL_H, we must set all the codes. */
    editchars[C_ERASE] = CTL_H;
    editchars[L_ERASE] = CTL_U;
    editchars[W_ERASE] = CTL_W;
    cc = write(sockt, editchars, sizeof(editchars));
    if (cc != sizeof(editchars)) {
	p_error("Writing edit characters");
    }
    data_action = da_read_edit_chars;
    
    XtManageChild(text); /* Fire up our end. */
    XtSetKeyboardFocus(paned, text); /* Text gets keyboard input. */
}

void read_talk(file_num)
    int file_num;
{
    /* Read some talk characters from the data socket. */
    char buf[100];
    int buflen, curp, lastp;
    static char lastrecv = '\n';
    
    buflen = read(file_num, buf, sizeof(buf) - 1);
    if (buflen < 0) {
	p_error("Read of data socket");
	return;
    }
    
    if (buflen == 0) {
	/* EOF - The other end has quit. We go into a semi-comatose
	   state here in case we want to save the conversation in a
	   file or paste it somewhere. */
	quitting = True;
	XtRemoveInput(sockt_id); /* Don't loop trying to read more. */
	display_state("The other party has ended the conversation.");
	return;
    }
    
    lastp = curp = 0;
    if (lastspeaker != HER && buf[0] == '\n' && lastrecv != '\n') {
	/* Special glitch here. Since we are "inducing" a newline here
	   because of the conversation switch, don't insert a leading
	   newline. */
	lastp = ++curp;
	lastrecv = '\n';
    }
    lastrecv = buf[buflen - 1];
    for (; curp < buflen; curp++) {
	int chartype;
	if (buf[curp] == '\n') {
	    /* A newline from her end. Put her name at the
	       start of the line. */
	    /* Put out any delayed ordinary characters and the newline. */
	    herchars(&buf[lastp], curp - lastp + 1);
	    lastp = curp + 1;
	    /* Put out the line header on the next thing received. */
	    lastspeaker = NOBODY;
	    lastrecv = '\n';
	    continue;
	}
	lastrecv = buf[curp];
	if (buf[curp] == 0) {
	    /* Ingore nulls. */
	    if (lastp < curp) {
		/* Process delayed ordinary characters. */
		herchars(&buf[lastp], curp - lastp);
	    }
	    lastp = curp + 1;
	    continue;
	}
	for (chartype = C_ERASE; chartype <= W_ERASE; chartype++) {
	    if (buf[curp] == editchars[chartype]) {
		/* Process the editting character, stopping at the
		   line header after newlines. */
		XawTextPosition lastnl, prevwhite, lastpos, cutpos;
		int rc;
		if (lastp < curp) {
		    /* Process delayed ordinary characters. */
		    herchars(&buf[lastp], curp - lastp);
		}
		lastp = curp + 1; /* Processed this editting char too. */
		if (lastspeaker != HER) {
		    /* Nothing of hers to erase. */
		    break;
		}
		/* Do the editting actions. */
		lastpos = XawTextSourceScan(textSrc, (XawTextPosition)0,
					    XawstAll, XawsdRight, 1, False);
		lastnl = XawTextSourceScan(textSrc, lastpos, XawstEOL,
					   XawsdLeft, 1, False);
		if (lastnl >= lastpos) {
		    /* Nothing to do. */
		    break;
		}
		
		/* Replace back to boundary. */
		switch (chartype) {
		  case C_ERASE:
		    cutpos = lastpos - 1;
		    break;
		  case W_ERASE:
		    for (prevwhite = lastpos; prevwhite > lastnl;
			 prevwhite--) {
			cutpos = XawTextSourceScan(textSrc, prevwhite,
						   XawstWhiteSpace,
						   XawsdLeft, 1, False);
			if (cutpos != prevwhite) break;
		    }
		    break;
		  case L_ERASE:
		    cutpos = lastnl;
		    break;
		}
		
		/* fprintf(stderr, "Replacing from %d to %d.\n",
		 *	cutpos, lastpos);
		 */
		if (cutpos <= lastnl + sizeof(hername)) {
		    /* Remove the line header too, but remember we
		       need one. */
		    cutpos = lastnl;
		    lastspeaker = NOBODY;
		}
		textblock.firstPos = 0;
		textblock.length = 0;
		textblock.ptr = NULL;
		textblock.format = FMT8BIT;
		rc = XawTextReplace(text, cutpos, lastpos, &textblock);
		if (rc != XawEditDone) {
		    fprintf(stderr,
			    "XawTextReplace(text, %d, %d, &textblock): %d\n",
			    cutpos, lastpos, rc);
		}
		XawTextSetInsertionPoint(text, cutpos);
		break;
	    }
	}
	/* Just an ordinary character. We delay inserting it. */
    }
    if (lastp < curp) {
	/* Process delayed ordinary characters. */
	herchars(&buf[lastp], curp - lastp);
    }
}

void herchars(buf, len)
    char *buf;
    int len;
{
    /* Insert characters from the "other end". Any newline character can
       only be at the end of these characters. */
    int rc;
    XawTextPosition lastPos;
    
    textblock.firstPos = 0;
    textblock.format = FMT8BIT;
    lastPos = XawTextSourceScan(textSrc, 0, XawstAll, XawsdRight, 1, False);
    
    if (lastspeaker != HER) {
	/* Identify the speaker. */
	if (lastspeaker == ME) {
	    /* Need a newline too. */
	    textblock.length = 1;
	    textblock.ptr = "\n";
	    rc = XawTextReplace(text, lastPos, lastPos, &textblock);
	    if (rc != XawEditDone) {
		fprintf(stderr, "XawTextReplace at %d: %d\n", lastPos, rc);
	    }
	    lastPos ++;
	}
	textblock.length = sizeof(hername);
	textblock.ptr = hername;
	rc = XawTextReplace(text, lastPos, lastPos, &textblock);
	if (rc != XawEditDone) {
	    fprintf(stderr, "XawTextReplace at %d: %d\n", lastPos, rc);
	}
	lastPos += textblock.length;
	lastspeaker = HER;
    }
    
    /* Add the characters to the text. */
    textblock.length = len;
    textblock.ptr = buf;
    rc = XawTextReplace(text, lastPos, lastPos, &textblock);
    if (rc != XawEditDone) {
	fprintf(stderr, "XawTextReplace at %d: %d\n", lastPos, rc);
    }
    XawTextSetInsertionPoint(text, lastPos + len);
}

/* ARGSUSED */
void replacecb(w, app_data, cbs)
    Widget w;
    caddr_t app_data;
    TalkSrcCallbackStruct *cbs;
{
    /* This callback is called from the TalkSrc widget whenever the
       text is to be changed. We use this to capture the changes to
       be sent to the other end. */
    
    static char lastsent = '\n';
    
    if (cbs->text == &textblock) {
	/* This is one of our changes. We do nothing with it. */
	return;
    }
    
    /* We always do the change by recursing. */
    cbs->doit = False;
    
    if (quitting) {
	/* Don't allow any changes after the other end has hung up. */
	cbs->rc = XawPositionError;
	return;
    }
    
    if (cbs->start != cbs->end) {
	/* We are deleting. */
	XawTextPosition lastnl, lastpos, startdel;
	
	lastpos = XawTextSourceScan(textSrc, 0, XawstAll,
				    XawsdRight, 1, False);
	lastnl = XawTextSourceScan(textSrc, lastpos, XawstEOL,
				   XawsdLeft, 1, False);
	if (cbs->end != lastpos) {
	    /* Since we are in append mode, this shouldn't happen
	       but it does, probably because we confuse the Text widget. */
	    cbs->rc = XawPositionError;
	    return;
	}
	if (cbs->start < lastnl) {
	    /* Don't allow deletion past the last newline. */
	    cbs->rc = XawPositionError;
	    return;
	}
	/* Don't send  backspaces to the other end for the line header. */
	startdel = lastnl + sizeof(myname);
	if (cbs->start > startdel) startdel = cbs->start;
	{
	    /* Send backspaces to do the deletion. */
	    char buf[40];
	    int dellen;
	    (void)memset(buf, CTL_H, sizeof(buf));
	    dellen = cbs->end - startdel;
	    while (dellen > 0) {
		int thislen = dellen;
		if (thislen > sizeof(buf)) {
		    thislen = sizeof(buf);
		}
		if (write(sockt, buf, thislen) != thislen) {
		    p_error("Writing deletion");
		}
		dellen -= thislen;
	    }
	}
	if (cbs->start <= lastnl + sizeof(myname)) {
	    /* Remove the line header too. */
	    cbs->start = lastnl;
	    lastspeaker = NOBODY; /* Need a new header. */
	    lastsent = '\n'; /* But not an extra newline. */
	}
	
	/* Do the deletion by recursing here, in case we have some
	   insertions too. */
	textblock.firstPos = 0;
	textblock.length = 0;
	textblock.ptr = NULL;
	textblock.format = FMT8BIT;
	if (XawTextReplace(text, cbs->start, cbs->end, &textblock) !=
	    XawEditDone) {
	    fprintf(stderr, "Recursive delete failed.\n");
	}
	XawTextSetInsertionPoint(text, cbs->start);
    }
    if (cbs->text->length > 0) {
	/* We are inserting. */
	char *curp, *lastp, *endp;
	curp = lastp = cbs->text->ptr + cbs->text->firstPos;
	endp = lastp + cbs->text->length;
	
	if (lastspeaker != ME && lastsent != '\n' && *curp != '\n') {
	    /* Send a newline to the other end to make sure that the
	       recipient sees the switch. This doesn't work well if
	       both ends talk at once. */
	    if (write(sockt, "\n", 1) != 1) {
		p_error("Write of newline");
	    }
	}
	
	/* Send the insertion to the other end. */
	if (write(sockt, curp, cbs->text->length) != cbs->text->length) {
	    p_error("Writing data");
	}
	lastsent = *(endp - 1);
	
	for (; curp < endp; curp++) {
	    if (*curp == '\n') {
		/* Put out the delayed characters, including the newline. */
		mychars(lastp, curp - lastp + 1);
		lastp = curp + 1;
		/* We need a header after this. */
		lastspeaker = NOBODY;
	    }
	}
	if (lastp < curp) {
	    /* Put out the delayed characters. */
	    mychars(lastp, curp - lastp);
	}
    }
}

void mychars(buf, len)
    char *buf;
    int len;
{
    /* Recursively insert characters. */
    XawTextPosition lastpos;
    
    lastpos = XawTextSourceScan(textSrc, 0, XawstAll, XawsdRight, 1, False);
    textblock.firstPos = 0;
    textblock.format = FMT8BIT;
    if (lastspeaker != ME) {
	/* Label our output. */
	if (lastspeaker == HER) {
	    /* Put a newline in the header too. */
	    textblock.length = 1;
	    textblock.ptr = "\n";
	    if (XawTextReplace(text, lastpos, lastpos, &textblock)
		!= XawEditDone) {
		fprintf(stderr, "Recursive nl insert failed.\n");
	    }
	    lastpos ++;
	}
	textblock.length = sizeof(myname);
	textblock.ptr = myname;
	if (XawTextReplace(text, lastpos, lastpos, &textblock)
	    != XawEditDone) {
	    fprintf(stderr, "Recursive header insert failed.\n");
	}
	lastpos += sizeof(myname);
	lastspeaker = ME;
    }
    textblock.length = len;
    textblock.ptr = buf;
    if (XawTextReplace(text, lastpos, lastpos, &textblock)
	!= XawEditDone) {
	fprintf(stderr, "Recursive header insert failed.\n");
    }
    XawTextSetInsertionPoint(text, lastpos + len);
}

void p_error(str)
    char *str;
{
    /* p_error displays the system error message on the screen and
       then exits. */
    
    char *sys, fullstr[100];
    sys = "Unknown error";
    if (errno < sys_nerr) {
	sys = sys_errlist[errno];
    }
    sprintf(fullstr, "%.45s : (%d) %.45s", str, errno, sys);
    display_state(fullstr);
    slow_quit();
}

void display_state(str)
    char *str;
{
    /* Display the given character string as our current state. */
    
    Arg args[1];
    
    if (app_resources.trace) {
	fprintf(stderr, "display_state: %s\n", str);
    }
    XtSetArg(args[0], XtNlabel, str);
    XtSetValues(lab, args, 1);
    XFlush(XtDisplay(toplevel));
}

void slow_quit()
{
    /* Quit after a short pause (to read a message). */
    
    (void) XtAppAddTimeOut(app, 30 * 1000 /* thirty seconds */, quit, NULL);
    if (sockt_id) {
	XtRemoveInput(sockt_id);
    }
    if (ctl_sockt_id) {
	XtRemoveInput(ctl_sockt_id);
    }
    if (conn_id) {
	XtRemoveInput(conn_id);
    }
    if (ctl_resend_id) {
	XtRemoveTimeOut(ctl_resend_id);
    }
    if (ring_id) {
	XtRemoveTimeOut(ring_id);
    }
    XtSetSensitive(text, False);
    /* Do the main event loop until time expires. */
    XtAppMainLoop(app);
    /* NOTREACHED */
}

void quit()
{
    /* Quit the application. */
    if (my_invitation_waiting) {
	/* Clean up - we don't wait for any answers. */
	if (app_resources.trace) {
	    fprintf(stderr, "quit: Deleting my invitation.\n");
	}
	msg.type = DELETE;
	msg.id_num = htonl(remote_id);
	daemon_addr.sin_addr = his_machine_addr;
	if (sendto(ctl_sockt, (char *)&msg, sizeof(msg), 0,
		   (struct sockaddr *)&daemon_addr,
		   sizeof(daemon_addr)) != sizeof(msg)) {
	    perror("send delete (remote)");
	}
    }
    if (his_invitation_waiting) {
	/* Clean up - we don't wait for any answers. */
	if (app_resources.trace) {
	    fprintf(stderr, "quit: Deleting his invitation.\n");
	}
	msg.type = DELETE;
	msg.id_num = htonl(local_id);
	daemon_addr.sin_addr = my_machine_addr;
	if (sendto(ctl_sockt, (char *)&msg, sizeof(msg), 0,
		   (struct sockaddr *)&daemon_addr,
		   sizeof(daemon_addr)) != sizeof(msg)) {
	    perror("send delete (local)");
	}
    }
    exit(0);
}

void print_addr(addr)
    struct sockaddr_in addr;
{
    /* print_addr is a debug print routine */
    int i;
    
    printf("addr = %x, port = %o, family = %o zero = ",
	   addr.sin_addr, addr.sin_port, addr.sin_family);
    for (i = 0; i<8;i++) {
	printf("%o ", (int)addr.sin_zero[i]);
    }
    putchar('\n');
}
