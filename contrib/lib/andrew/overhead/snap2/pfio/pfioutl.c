/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *pfioutl_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/pfio/RCS/pfioutl.c,v 1.7 1991/09/12 17:20:53 bobg Exp $";

/*
 * portable packet packing/unpacking interface
 * unix io utility implementation
 * by Aaron Wohl
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/pfio/RCS/pfioutl.c,v 1.7 1991/09/12 17:20:53 bobg Exp $
 */
#include <pfiou.h>

void pfio_with_timeout(dowrite,fd,buf,len,timeout)
int dowrite;
int fd;
char *buf;
int len;
int timeout;
{
  register int rc;
  do {
    if(dowrite)
    	rc=write(fd,buf,len);	/*write what we can*/
    else
      rc=read(fd,buf,len);	/*read what we can*/
    if(rc != -1) {
      len-=rc;
      buf+=rc;
      if(len==0)return;
    }
    else if(errno!=EWOULDBLOCK)
	er_ucall(rc,("io"));	/*make sure its not an error code*/
 }while(timeout>time(0L));
 rl_error("io timed out, dowrite=%d",dowrite);
}

void rl_parerr();

/*
 * send an error message or an console message
 */
void send_guar_news(serv_port,kind,isfinal,news)
int serv_port;
int kind;
int isfinal;
char *news;
{
  PFM outpar;
  int newslen=strlen(news);
  char opack[5000];
  outpar.initial=TO_pfstr(opack+IHEAD_LEN);
  outpar.buf=outpar.initial;
  outpar.limit=TO_pfstr(opack+sizeof(opack)-1);
  outpar.parerr=rl_parerr;
  PFpack(
	 &outpar,
	  LS_aloc(newslen+30),	/*size of out buffer*/
	  /*opcode to send*/
	  LS_msend_num(LS_int8,kind), /*this is an error, (not news)*/
	  LS_msend_num(LS_int8,isfinal), /*end of connection*/
	  LS_msend_rstr(newslen,news),	/*string to print*/
	  LS_dropoff);
  outpar.initial=TO_pfstr(opack);
  PF_dropoff(&outpar);
  pfio_with_timeout(
		  TRUE,
		  serv_port,
		  opack,
		  BYTES_IN_BUF(outpar),
		  time(0L)+10);
}


void get_foreign_addr(fd,sin)
int fd;
register struct sockaddr_in *sin;
{
    int sin_len=sizeof(*sin);
    er_ucall(getpeername(fd,sin,&sin_len),("getpeername"));
}

void set_no_block(fd)
int fd;
{
 int noblock=FNDELAY;
#ifdef SOMEDAY
 er_ucall(fcntl(fd,F_SETFL,&noblock,sizeof(noblock)),("fcntl"));
#endif
 er_ucall(fcntl(fd,F_SETFL,noblock),("fcntl"));
}

int new_socket(port)
int port;
{
    struct sockaddr_in sin;
    int yes_reuse=1;
    register int result;
    er_ucall((result=socket(AF_INET,SOCK_STREAM,0)),("socket"));
    er_ucall(setsockopt(result,SOL_SOCKET,SO_REUSEADDR,&yes_reuse,sizeof(yes_reuse)),("setsockopt"));

    bzero(&sin,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    sin.sin_addr.s_addr=INADDR_ANY;
    er_ucall(bind(result,&sin,sizeof(sin)),("bind"));
    return result;
}

/*
 * Get internet address from kernel, zero on error.
 * code from ddp+
 */
long get_netaddr(sock)
    long sock;
{
    struct ifconf ifc;
#ifndef NETWORKS
#define NETWORKS (30)
#endif
    struct ifreq conf[NETWORKS];
    struct sockaddr_in *sa_struct;
    int i;
    int interfaces;             /* number of interfaces returned by ioctl */

    ifc.ifc_len = sizeof(conf);
    ifc.ifc_buf = (caddr_t) conf;
    if (ioctl(sock, SIOCGIFCONF, (char *)&ifc) < 0)
        return (0);

    interfaces = ifc.ifc_len / sizeof(struct ifreq);
    for (i = 0; i < interfaces; i++) {
        sa_struct = (struct sockaddr_in *) &conf[i].ifr_addr;
        if (sa_struct->sin_addr.s_addr != 0 &&
                    strcmp(conf[i].ifr_name, "lo0") != 0)
        return (sa_struct->sin_addr.s_addr);
    }
    return (0);
}

void get_local_addr(fd,sin)
register int fd;
register struct sockaddr_in *sin;
{
    int sock_len=sizeof(*sin);
    er_ucall(getsockname(fd,sin,&sock_len),("getsockname"));
}

