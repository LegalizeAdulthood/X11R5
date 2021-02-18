/* -*-C-*-
********************************************************************************
*
* File:         w_timeout.c
* RCS:          $Header: w_timeouts.c,v 1.4 91/03/14 03:14:17 mayer Exp $
* Description:  WINTERP interfaces to XtAppAddTimeOut() and XtRemoveTimeout().
* Author:       Niels Mayer, HPLabs; Bob Leichner, HPLabs
* Created:      Sat Aug 26 07:44:17 1989
* Modified:     Thu Oct  3 21:08:55 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_timeouts.c,v 1.4 91/03/14 03:14:17 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>		/* Xm/Xm.h only needed for "winterp.h"*/
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

static LVAL s_TIMEOUT_OBJ=NIL, s_TIMEOUT=NIL;


/******************************************************************************
 * This is called indirectly via XtAppAddTimeOut() in
 *   Wto_Prim_XtAddTimeOut().
 ******************************************************************************/
static void Winterp_TimeoutProc(client_data, id)
     XtPointer     client_data;
     XtIntervalId* id;
{
  extern LVAL          xlenv, xlfenv;
  LVAL                 oldenv, oldfenv, l_evalforms;
  LVAL                 timeout_obj = (LVAL) client_data;
  LVAL                 c_callback = get_timeout_closure(timeout_obj);

  /* the timeout-object is no longer active, so indicate that by nulling the timeout-id */
  set_timeout_id(timeout_obj, NULL);

  /* remove <timeout_obj> from v_savedobjs allowing it to be garbage collected */
  {
    extern LVAL v_savedobjs;
    int i = Wso_Hash(timeout_obj);
    LVAL l_hbucket = getelement(v_savedobjs, i);
    LVAL l_prev = NIL;

    while (l_hbucket && (car(l_hbucket) != timeout_obj)) {
      l_prev = l_hbucket;
      l_hbucket = cdr(l_hbucket);
    }
    if (!l_hbucket)
      xlerror("Internal error in Winterp_TimeoutProc -- couldn't remove <timeout-obj> from v_savedobjs. Hash error?",
	      timeout_obj);
    if (!l_prev)		/* first elt matched */
      setelement(v_savedobjs, i, cdr(l_hbucket));
    else
      rplacd(l_prev, cdr(l_hbucket));
  }

  /*
   * Now evaluate code associated with timeout -- 
   * Most of this procedure looks alot like xleval.c:evfun(), which is what
   * the evaluator calls when a functional form is to be evaluated. The
   * main difference is that instead of calling xlabind() to bind the
   * formal parameter symbols of a function to their values in the new
   * lexical environment frame returned by xlframe(getenv(fun)), we just
   * bind the symbol "TIMEOUT_OBJ" to the value of the <timeout-obj> that
   * caused this call.
   */
  
  /* protect some pointers */
  xlstkcheck(3);
  xlsave(oldenv);
  xlsave(oldfenv);
  xlsave(l_evalforms);

  /* create a new environment frame */
  oldenv = xlenv;
  oldfenv = xlfenv;
  xlenv = xlframe(getenvt(c_callback));	/* note: changed getenv()-->getenvt() due to name conflict with stdlib.h:getenv() */
  xlfenv = getfenv(c_callback);

  /* bind the <timeout-object> to locally referrable lexical var TIMEOUT_OBJ */
  xlpbind(s_TIMEOUT_OBJ, timeout_obj, xlenv);

  /* execute the block */
  for (l_evalforms = getbody(c_callback); consp(l_evalforms); 
       l_evalforms = cdr(l_evalforms))
    xleval(car(l_evalforms));
  
  /* restore the environment */
  xlenv = oldenv;
  xlfenv = oldfenv;
  
  /* restore the stack */
  xlpopn(3);
}


/*****************************************************************************
 * This primitive takes two forms of arguments:
 *      (XT_ADD_TIMEOUT <interval> <code>) or
 *      (XT_ADD_TIMEOUT <interval> <timeout-obj>)
 * returns: <timeout_object>
 *
 * <interval> of the timer in milliseconds.
 *
 * <code> is a list of lisp expressions that are evaluated when the timeout
 * occurs. During the timeout, the lexical environment that existed for
 * the call to Xt_Add_Timeout will be used for value and functional bindings.
 * Additionally, the symbol TIMEOUT_OBJ is bound to the <timeout-obj> that caused
 * the timeout.
 *
 * The form (XT_ADD_TIMEOUT <interval> <timeout-obj>) may be used to more
 * efficiently schedule recurrent timeouts. Instead of creating a new closure
 * around the same <code> each time a recurrent timeout is rescheduled,
 * this second form for XT_ADD_TIMEOUT allows you to take the <timeout-obj>
 * from a previously expired timeout and reschedule a new timeout using the 
 * closure setup by the initial call to (XT_ADD_TIMEOUT <interval> <code>).
 * During the execution of <code>, the symbol TIMEOUT_OBJ is bound to 
 * <timout-obj> so that you don't need to keep around a global variable
 * for each recurrent timeout.
 *
 * The returned <timeout-obj> may be passed into the  functions
 * (XT_REMOVE_TIMEOUT <timeout-obj>), or (XT_ADD_TIMEOUT <interval> <timeout-obj>).
 ****************************************************************************/
LVAL Wto_Prim_XtAddTimeOut()
{
  extern XtAppContext app_context; /* winterp.c */
  extern LVAL s_lambda, xlenv, xlfenv;
  LVAL arg, l_code, timeout_obj=NIL;
  long i;
  unsigned long interval;
  
  /* protect some pointers */
  xlsave1(timeout_obj);

  /* get interval */
  if ((i = getfixnum(xlgafixnum())) < 0L)
    xlfail("Timeout interval must be a positive integer.");
  else
    interval = (unsigned long) i;

  /* get <code> or <timeout_obj> */
  arg = xlgetarg();
  if (timeoutobj_p(arg)) {
    timeout_obj = arg;
    if (get_timeout_id(timeout_obj) != NULL)
      xlerror("Attempt to schedule a recurrent timeout before previous timeout's expiration.", timeout_obj);
  }
  else if (listp(arg))
    l_code = arg;
  else
    xlerror("Bad Argument Type, expected <timeout-code> or <timeout-object>.", arg);

  xllastarg();

  /* 
   * create the client_data to be sent to Winterp_TimeoutProc. 
   * That procedure takes the client_data==timeout_obj, extracts the closure,
   * and uses this to execute the timeout callback. We also need to retain
   * the timeout identifier for use in RemoveTimeout. For simplicity, we save
   * in lisp object timeout_obj.
   */

  /* if args gave <code>, need to create timeout-obj and closure around <code> */
  if (!timeout_obj) {		
    timeout_obj = new_timeoutobj();
    set_timeout_closure(timeout_obj,
			xlclose(s_TIMEOUT, s_lambda, NIL, l_code, xlenv, xlfenv));
  }

  set_timeout_id(timeout_obj,
		 XtAppAddTimeOut(app_context, interval, Winterp_TimeoutProc, (XtPointer) timeout_obj));  

  /*
   * Put timeout_obj in savedobjs so that it gets marked during gc. In that way, we
   * know that the timeout closure (i.e. the callback code, and it's lexical
   * environment) and timeout identifier won't get garbage collected while the
   * timeout-object is "referenced" inside Motif. The timeout_obj gets removed from
   * savedobjs each time the timeout expires, or when xt_remove_timeout gets called.
   */
  { 
    int  i = Wso_Hash(timeout_obj);
    LVAL l_hbucket;
    extern LVAL v_savedobjs;
    
    xlsave1(l_hbucket);
    l_hbucket = cons(timeout_obj, getelement(v_savedobjs, i));
    setelement(v_savedobjs, i, l_hbucket);
    xlpop();
  }

  /* resore the stack */
  xlpop();

  return (timeout_obj);
}


/******************************************************************************
 * extern void XtRemoveTimeout();
 *      XtIntervalId timer;      
 * 
 * Lisp:   (xt_remove_timeout <timeout_obj>)
 *         where <timeout_obj> is the value returned by xt_add_timeout.
 ******************************************************************************/
LVAL Wto_Prim_XtRemoveTimeout()
{
  extern LVAL true;
  LVAL timeout_obj;
  
  timeout_obj = xlga_timeoutobj();
  xllastarg();

  if (get_timeout_id(timeout_obj) == NULL)
    xlerror("Attempt to remove an expired/removed timeout.", timeout_obj);
  
  XtRemoveTimeOut(get_timeout_id(timeout_obj));

  /* the timeout-object is no longer active, so indicate that by nulling the timeout-id */
  set_timeout_id(timeout_obj, NULL);

  /* remove <timeout_obj> from v_savedobjs allowing it to be garbage collected */
  {
    extern LVAL v_savedobjs;
    int i = Wso_Hash(timeout_obj);
    LVAL l_hbucket = getelement(v_savedobjs, i);
    LVAL l_prev = NIL;

    while (l_hbucket && (car(l_hbucket) != timeout_obj)) {
      l_prev = l_hbucket;
      l_hbucket = cdr(l_hbucket);
    }
    if (!l_hbucket)
      xlerror("Internal error in Wto_Prim_XtRemoveTimeout -- couldn't remove <timeout-obj> from v_savedobjs. Hash error?",
	      timeout_obj);
    if (!l_prev)		/* first elt matched */
      setelement(v_savedobjs, i, cdr(l_hbucket));
    else
      rplacd(l_prev, cdr(l_hbucket));
  }

  return (true);
}


/******************************************************************************
 * (TIMEOUT_ACTIVE_P <timeout_obj>)
 * 	--> returns T if <timeout_obj> is still scheduled, returns
 *	    NIL if <timeout_obj> has expired or if the timeout was
 *	    inactivated by XT_REMOVE_TIMEOUT.
 *
 *  <timeout_obj> is the value returned by XT_ADD_TIMEOUT.
 ******************************************************************************/
LVAL Wto_Prim_TIMEOUT_ACTIVE_P()
{
  extern LVAL true;
  LVAL timeout_obj;
  
  timeout_obj = xlga_timeoutobj();
  xllastarg();
  
  return (get_timeout_id(timeout_obj) ? true : NIL);
}


/******************************************************************************
 *
 ******************************************************************************/
Wto_Init()
{
  s_TIMEOUT_OBJ = xlenter("TIMEOUT_OBJ");
  s_TIMEOUT     = xlenter("XT_TIMEOUT");
}
