/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmon.h,v 2.8 1991/09/12 19:28:36 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmon.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidvmmonitor = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmon.h,v 2.8 1991/09/12 19:28:36 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#ifdef VMMON_DODECL
struct nlist   RawStatistics[] =
{
#endif /* VMMON_DODECL */
#define	X_CPTIME	0
#ifdef VMMON_DODECL
   {
      "_cp_time"
   },
#endif /* VMMON_DODECL */
#define	X_RATE		1
#ifdef VMMON_DODECL
   {
      "_rate"
   },
#endif /* VMMON_DODECL */
#define X_TOTAL		2
#ifdef VMMON_DODECL
   {
      "_total"
   },
#endif /* VMMON_DODECL */
#define	X_DEFICIT	3
#ifdef VMMON_DODECL
   {
      "_deficit"
   },
#endif /* VMMON_DODECL */
#define	X_BOOTIME	4
#ifdef VMMON_DODECL
   {
      "_boottime"
   },
#endif /* VMMON_DODECL */
#define	X_DKXFER	5
#ifdef VMMON_DODECL
   {
      "_dk_xfer"
   },
#endif /* VMMON_DODECL */
#define X_PGIN		6
#ifdef VMMON_DODECL
   {
      "_pgintime"
   },
#endif /* VMMON_DODECL */
#define X_NDSTAT	7
#ifdef VMMON_DODECL
   {
      "_ndstat"
   },
#endif /* VMMON_DODECL */
#define X_NSWAPMAP	8
#ifdef VMMON_DODECL
   {
       "_nswapmap"
   },
#endif /* VMMON_DODECL */
#define X_SWAPMAP	9
#ifdef VMMON_DODECL
   {
       "_swapmap"
   },
#endif /* VMMON_DODECL */
#define X_NSWAPBLKS	10
#ifdef VMMON_DODECL
   {
       "_nswap"
   },
#endif /* VMMON_DODECL */
#define X_DMMAX		11
#ifdef VMMON_DODECL
   {
       "_dmmax"
   },
#endif /* VMMON_DODECL */
#define	X_PROC		12
#ifdef VMMON_DODECL
   {
      "_proc"
   },
#endif /* VMMON_DODECL */
#define	X_NPROC		13
#ifdef VMMON_DODECL
   {
      "_nproc"
   },
#endif /* VMMON_DODECL */
#ifndef MAXUPRC
#define X_MAXUPRC	14
#ifdef VMMON_DODECL
   {
	"_maxuprc"
   },
#endif /* VMMON_DODECL */
#endif /* defined(vax) | defined(hpux) */
#ifdef VMMON_DODECL
   {
      0
   },
};
#endif /* VMMON_DODECL */
