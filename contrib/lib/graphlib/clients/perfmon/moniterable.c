/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Eric Anderson 
 * Buttons and menus mostly by Jeffrey Hutzelman
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */


/* This is the file which handles getting all of the statistics out
   of the kernal.  Load from get_load.c Thanks go to
   Chris Siebenmann, CRSI, University of Toronto <cks@white.toronto.edu>
   for his xcpustate program, and to 
   Emanuel Jay Berkenbilt, MIT Project Athena, 
   and Thomas A. Baghli, PCS Computer Systeme GmbH, West Germany
   tom@meepmeep.pcs.com for their xperfmon program.
   Thanks also to Sun for the perfmeter program.
   Dec Ultrix 4.2 hacks from me.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/dk.h>
#include <sys/buf.h>
#include <nlist.h>
#include <stdio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <sys/vm.h>
#include <sys/time.h>
#include <sys/proc.h>
#ifdef Ultrix4
#include <machine/cpu.h>
#include <sys/cpudata.h>
#endif
#include "moniterable.h"

extern char *xmalloc(/* int nbytes */);

extern int open(), read();
extern long lseek();

int	kmem;			/* file descriptor of /dev/kmem. */

#ifdef Ultrix4
struct cpudata cp_time,cp_time_old;
struct nlist cp_time_nl[] = {
  {"_boot_cpudata"},
  {""},
};
#else
long	cp_time[CPUSTATES],cp_time_old[CPUSTATES];
struct nlist	cp_time_nl[] = {
	{ "_cp_time" },
	{ "" },
};
#endif

long    dk_bps[DK_NDRIVE],dk_bps_old[DK_NDRIVE];
struct nlist dk_bps_nl[] = {
  { "_dk_wds" },
  { "" },
};

long    if_pks[1],if_pks_old[1];
struct nlist if_pks_nl[] = {
  { "_ifnet" },
  { "" },
};

struct forkstat forkstat,oldforkstat;
struct nlist forkstat_nl[] = {
  { "_forkstat" },
  { "" },
};

struct vmmeter page;
struct nlist page_nl[] = {
  { "_rate" },
  { "" },
};

struct vmmeter swap;
struct nlist swap_nl[] = {
  { "_rate" },
  { "" },
};

#ifdef Ultrix4
struct cpudata interrupt,oldinterrupt;/* really should be an array */
struct nlist interrupt_nl[] = {
  { "_boot_cpudata" },
  { "" },
};

struct cpudata context,oldcontext;/* really should be an array */
struct nlist context_nl[] = {
  { "_boot_cpudata" },
  { "" },
};
#else
struct vmmeter interrupt;
struct nlist interrupt_nl[] = {
  { "_rate" },
  { "" },
};

struct vmmeter context;
struct nlist context_nl[] = {
  { "_rate" },
  { "" },
};
#endif

int    bufs;
struct nlist bufs_nl[] = {
  { "_buffalloc" },
  { "" },
};

struct vmtotal free_real_mem;
struct nlist free_real_mem_nl[] = {
  { "_total" },
  { "" },
};

/* 
 *  Called to initialize a something.
 */

void close_any()
{
  close(kmem);
}

void gen_init(anlist,dataplace,datasize)
struct nlist *anlist;
char *dataplace;
int datasize;
{
  int failed,loop;

  if ((kmem = open("/dev/kmem",0))<0) {
    perror("/dev/kmem");
    exit(-2);
  }
  failed =nlist("/vmunix",anlist);
  if (failed!=0) {
    fprintf(stderr,"Failed to get namelist out of kernal.\n");
    fprintf(stderr,"Failed on the following entries :\n");
    for (loop=0;strlen(anlist[loop].n_name)>0;loop++) {
      if (anlist[loop].n_type==N_UNDF)
	fprintf(stderr,"%s\n",anlist[loop].n_name);
    }
    exit(-1);
  }
  if (lseek(kmem,(long)anlist[0].n_value,0) !=
      (long)anlist[0].n_value)
    perror("lseek");
  if (read(kmem, dataplace,datasize)!=datasize)
    perror("read");
}

void cpu_init()
{
#if Ultrix4
  gen_init(cp_time_nl,&cp_time_old,sizeof(cp_time_old));
#else
  gen_init(cp_time_nl,cp_time_old,sizeof(cp_time_old));
#endif
}

void disk_bps_init()
{
  gen_init(dk_bps_nl,dk_bps_old,sizeof(dk_bps_old));
}  

void forkstat_init()
{
  gen_init(forkstat_nl,&oldforkstat,sizeof(oldforkstat));
}

void page_init()
{
  gen_init(page_nl,&page,sizeof(page));
}

void swap_init()
{
  gen_init(swap_nl,&swap,sizeof(swap));
}

void interrupt_init()
{
  gen_init(interrupt_nl,&interrupt,sizeof(interrupt));
}

void context_init()
{
  gen_init(context_nl,&context,sizeof(context));
}

void load_init()
{
}

void bufs_init()
{
  gen_init(bufs_nl,&bufs,sizeof(bufs));
}

void free_real_mem_init()
{
  gen_init(free_real_mem_nl,&free_real_mem,sizeof(free_real_mem));
}

void get_if_packets(input,inerrors,output,outerrors,collisions)
long *input,*inerrors,*output,*outerrors,*collisions;
{
  struct ifnet ifnet;
  long ifaddr;

  *input = *inerrors = *output = *outerrors = *collisions = 0;

  if (lseek(kmem,(long)if_pks_nl[0].n_value,0) !=
      (long)if_pks_nl[0].n_value)
    perror("lseek");

  if (read(kmem,&ifaddr,sizeof(ifaddr))!=sizeof(ifaddr))
    perror("read");

  while(ifaddr) {
    if (lseek(kmem,ifaddr,0)!=ifaddr)
      perror("lseek");
    if (read(kmem,&ifnet,sizeof(ifnet))!= sizeof(ifnet))
      perror("read");
    *input+=ifnet.if_ipackets;
    *inerrors+=ifnet.if_ierrors;
    *output+=ifnet.if_opackets;
    *outerrors+=ifnet.if_oerrors;
    *collisions+=ifnet.if_collisions;
    ifaddr = (long)ifnet.if_next;
  }
}

void if_pks_init()
{
  int input,inerrors,output,outerrors,collisions;
  
  if ((kmem = open("/dev/kmem",0))<0) {
    perror("/dev/kmem");
    exit(-2);
  }
  (void)nlist("/vmunix",if_pks_nl);
  get_if_packets(&input,&inerrors,&output,&outerrors,&collisions);
  if_pks_old[0] = input+output;
}

void collisions_init()
{
  int input,inerrors,output,outerrors,collisions;
  
  if ((kmem = open("/dev/kmem",0))<0) {
    perror("/dev/kmem");
    exit(-2);
  }
  (void)nlist("/vmunix",if_pks_nl);
  get_if_packets(&input,&inerrors,&output,&outerrors,&collisions);
  if_pks_old[0] = collisions;
}

void errors_init()
{
  int input,inerrors,output,outerrors,collisions;
  
  if ((kmem = open("/dev/kmem",0))<0) {
    perror("/dev/kmem");
    exit(-2);
  }
  (void)nlist("/vmunix",if_pks_nl);
  get_if_packets(&input,&inerrors,&output,&outerrors,&collisions);
  if_pks_old[0] = inerrors+outerrors;
}
  
/******
 *
 * These routines are called to get the new values.
 * 
 ******/

void gen_readnew(anlist,dataplace,datasize)
struct nlist *anlist;
char *dataplace;
int datasize;
{
  if (lseek(kmem, (long) anlist[0].n_value,0) != anlist[0].n_value)
    perror("lseek");
  if (read(kmem, dataplace,datasize)!=datasize)
    perror("read");
}

long cpu_getvalue()
{
  long	idle,used,loop;
	
#if Ultrix4
  gen_readnew(cp_time_nl,&cp_time,sizeof(cp_time));
#else
  gen_readnew(cp_time_nl,cp_time,sizeof(cp_time));
#endif
  
#ifdef Ultrix4
#  define delta(cpustate) ((int) (cp_time.cpu_cptime[(cpustate)] - cp_time_old.cpu_cptime[(cpustate)]))
#else
#  define delta(cpustate) ((int) (cp_time[(cpustate)] - cp_time_old[(cpustate)]))
#endif

  used =delta(CP_USER)+delta(CP_NICE)+delta(CP_SYS);
  idle =delta(CP_IDLE);

#undef delta

  if ((used+idle)==0) {
    return (long)0;
  }
    
#ifdef Ultrix4
  memcpy(&cp_time_old,&cp_time,sizeof(cp_time));
#else
  memcpy(cp_time_old,cp_time,sizeof(cp_time));
#endif
  loop = (long)(100*used/(used+idle));
  return loop;
}

long disk_bps_getvalue()
{
  long	i,x;
	
  gen_readnew(dk_bps_nl,dk_bps,sizeof(dk_bps));

#define delta(cpustate) ((int) (dk_bps[(cpustate)] - dk_bps_old[(cpustate)]))

  x=0;
  for (i = 0; i < DK_NDRIVE; i ++) {
    x+=delta(i);
    dk_bps_old[i] = dk_bps[i];
  }

#undef delta

  return x/16;
}

long forkstat_getvalue()
{
  long difference;

  gen_readnew(forkstat_nl,&forkstat,sizeof(forkstat));

  difference = (forkstat.cntfork+forkstat.cntvfork) -
    (oldforkstat.cntfork+oldforkstat.cntvfork);

  oldforkstat = forkstat;
  return difference;
}

long page_getvalue()
{
  long difference;

  gen_readnew(page_nl,&page,sizeof(page));

  difference = page.v_pgpgin+page.v_pgpgout;

  return difference;
}

long swap_getvalue()
{
  long difference;

  gen_readnew(swap_nl,&swap,sizeof(swap));

  difference = swap.v_pswpin+swap.v_pswpout;

  return difference;
}

long interrupt_getvalue()
{
  long difference;

  gen_readnew(interrupt_nl,&interrupt,sizeof(interrupt));

#ifdef Ultrix4
  difference = interrupt.cpu_intr+interrupt.cpu_ip_intr
              -oldinterrupt.cpu_intr-oldinterrupt.cpu_ip_intr;
  oldinterrupt=interrupt;
#else
  difference = interrupt.v_intr;
#endif

  return difference;
}

long context_getvalue()
{
  long difference;

  gen_readnew(context_nl,&context,sizeof(context));

#ifdef Ultrix4
  difference = context.cpu_switch-oldcontext.cpu_switch;
  oldcontext=context;
#else
  difference = context.v_swtch;
#endif

  return difference;
}

long if_pks_getvalue()
{
  long input,output,inerrors,outerrors,collisions,difference;

  get_if_packets(&input,&inerrors,&output,&outerrors,&collisions);

  if_pks[0]=input+output;

  difference = if_pks[0]-if_pks_old[0];
  if_pks_old[0]=if_pks[0];
  return difference;
}

long collisions_getvalue()
{
  long input,output,inerrors,outerrors,collisions,difference;

  get_if_packets(&input,&inerrors,&output,&outerrors,&collisions);

  if_pks[0]=collisions;

  difference = if_pks[0]-if_pks_old[0];
  if_pks_old[0]=if_pks[0];
  return difference;
}

long errors_getvalue()
{
  long input,output,inerrors,outerrors,collisions,difference;

  get_if_packets(&input,&inerrors,&output,&outerrors,&collisions);

  if_pks[0]=inerrors+outerrors;

  difference = if_pks[0]-if_pks_old[0];
  if_pks_old[0]=if_pks[0];
  return difference;
}

long load_getvalue()
{
  double currentload;

  GetLoadPoint(&currentload);

  return (long)(currentload*LOAD_SCALE);
}

long bufs_getvalue()
{
  gen_readnew(bufs_nl,&bufs,sizeof(bufs));
  return bufs;
}

long free_real_mem_getvalue()
{
  gen_readnew(free_real_mem_nl,&free_real_mem,sizeof(free_real_mem));
/*  return (free_real_mem.t_rm-free_real_mem.t_arm);*/ 
  return (free_real_mem.t_free);

}

