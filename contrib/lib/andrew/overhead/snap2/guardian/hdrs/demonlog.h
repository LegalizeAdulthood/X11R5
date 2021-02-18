/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
   facility for deamons to write accounting records
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/guardian/hdrs/RCS/demonlog.h,v 2.8 1991/09/12 20:30:53 bobg Exp $
*/

#ifndef _deamonlog_
#define _deamonlog_

extern char *deal_fixfield();
extern void deal_flush();
extern int deal_open();
extern void deal_close();
extern void deal_close_silently();
extern char *deal_get_log_name();
extern void deal_willfork();
extern void deal_log_your_self();


extern void deal_log();
#define DEALOG(xx_arg) {deal_log xx_arg ;}

/*convert an ip address into a comma seperated list of integers,
  suitable for use as a printf argument with the format string
  %d.%d.%d.%d
*/
#define low_eight_bits (0x000000ffl)
#define IP_ADDR_PARTS_INTERNAL(xx) \
  ((int)(((xx)>>24)&low_eight_bits)), \
  ((int)(((xx)>>16)&low_eight_bits)), \
  ((int)(((xx)>> 8)&low_eight_bits)), \
  ((int)((xx)&low_eight_bits))
#define IP_ADDR_PARTS(xx) IP_ADDR_PARTS_INTERNAL((htonl(xx)))
#endif /* _deamonlog_ */
