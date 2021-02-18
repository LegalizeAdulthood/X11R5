/*
 *
 *	$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/guardian/cmd/RCS/snptguar.c,v 1.13 1989/09/28 10:50:23 aw0g Exp $
 */


#include <pfiou.h>
#include <stdio.h>
#include <snap.h>
#include <snptguaru.h>

#ifndef TRUE
#define TRUE (0==0)
#define FALSE (0==1)
#endif

void rl_parerr();

jmp_buf guarcatch;

int SNAP_debugmask=0;

long time();

int SNAP_istcp()
{
  return TRUE;
}

/*
 *  (Null) Handler for notify packets
 *
 */

int NotifyHandler ()
{
    return SNAP_SUCCESS;
}

/*
  * Initialize the SNAP Guardian support layer
  *
  */

int SNAP_GuardianInit ()
{
    return (SNAP_SUCCESS);
}

/*
  * Terminate the SNAP Guardian support layer
  *
  */

int SNAP_GuardianTerm ()
{
    return (SNAP_SUCCESS);
}

static void set_no_block(fd)
int fd;
{
 int noblock=FNDELAY;
 int err;
 err=fcntl(fd,F_SETFL,&noblock,sizeof(noblock));
 err=fcntl(fd,F_SETFL,noblock);
}

static int tg_accept(socket)
int socket;
{
  struct sockaddr_in sin;
  int sinlen;
  int new_fd;
  sinlen=sizeof(sin);
  er_ucall((new_fd=accept(socket,&sin,&sinlen)),("accept client"));
  set_no_block(new_fd);
  set_socket_reusable(new_fd);
  return new_fd;
}

typedef struct sockaddr_in ADDRESS;	/* Our form of address */

void non_block_close(fd)
int fd;
{
  long timeout;
  int rc;
  timeout=time(0L)+10;
  do {
    rc=close(fd);
    if(rc==0)return;
    if(rc!=EWOULDBLOCK) {
      fprintf(stderr,"sntp:error closing connection");
      fflush(stderr);
      return;
    }
  }while(timeout>time(0L));
}

PFM_pt read_packet(fd,timeout)
int fd;
int timeout;
{
  static unsigned char inbuf[10000];
  static PFM inpar;
  int len;

  /*read the header*/
  pfio_with_timeout(FALSE,fd,inbuf,IHEAD_LEN,timeout);
  len=PFM_FETCH16(inbuf);
  if((len<4)||(len>sizeof(inbuf)))
    rl_error("header length curruped");
  pfio_with_timeout(FALSE,fd,inbuf+IHEAD_LEN,len-IHEAD_LEN,timeout);
  inpar.initial=inbuf;
  inpar.buf=inpar.initial+IHEAD_LEN;
  inpar.limit=inpar.buf+len;
  inpar.parerr=rl_parerr;
  return &inpar;
}

/*
 * allocate output space
 */
void PF_aloc(o,siz)
PFM_pt o;
long siz;
{
}

/*
 * drop off output buffer
 */
void PF_dropoff(o)
PFM_pt o;
{
  int olen;
  olen=BYTES_IN_BUF(*o);
  PFM_PUT16(o->initial,olen);
}

extern char *ghost();
extern double getla();
extern long Users;
/*
 * send a welcome message
 */
void welcome(fd)
int fd;
{
  char obuf[5000];
  long loadav=getla(2)*10;
  long reboot;
  sprintf(obuf,"Users:%ld Load:%ld Host:%s\n",Users,loadav,ghost());
  if((reboot=get_reboot_time())!=0)
    sprintf(obuf+strlen(obuf),"Server reboot scheduled for %s",ctime(&reboot));
  send_guar_news(fd,GUAR_news,FALSE,obuf);
}

/*
 * Handle a "Get authentication request" for the Guardian
 *
 */
void SNAP_GetAuthReq (socket, connparms, authenticator)
int socket;
SNAP_CPARMS *connparms;
int (*authenticator)();
{
  int key[SNAP_KEYLEN];
  ADDRESS server_na;
  ADDRESS source_na;
  char *userid;
  char *password;
  char *service;
  char *versinfo;
  int plen;
  int maj_vers;
  int min_vers;
  PFM_pt inp;
  int auth_result;
  char errbuf[1025];
  int serv_fd;
  int phys_len;

  errno=0;

  if(setjmp(SETJM guarcatch)!=0)
    goto auth_exit;

  serv_fd=tg_accept(socket);

  welcome(serv_fd);

  get_foreign_addr(serv_fd,&source_na);

  inp=read_packet(serv_fd,time(0L)+16);

  PFunpack(
	   inp,
	    LR_mread(LRh_int,LR_int16,&phys_len),
	   /* read list selector*/
	    LR_mread(LRh_int,LR_int16,&maj_vers),
	    LR_mread(LRh_int,LR_int16,&min_vers),
	   /* check the lengths here*/
	    LR_mread(LRh_str_pt,LR_str,&userid),
	    LR_mread(LRh_int,LR_strlen,&plen),
	    LR_mread(LRh_str_pt,LR_str,&password),
	    LR_mread(LRh_str_pt,LR_str,&service),
	    LR_mread(LRh_str_pt,LR_str,&versinfo),
	    LR_end);

  if(maj_vers!=GUAR_MAJ_VER) {
    char buf[1000];
    sprintf(buf,
	    "Your client program (MacMail, vui, etc.) is too %s to talk to this guardian. Your protocol version=%d, mine=%d",
	((maj_vers<GUAR_MAJ_VER)?"old":"new"),maj_vers,GUAR_MAJ_VER);
    fprintf(stderr,"%s\n",buf);
    send_guar_news(serv_fd,
	GUAR_fatal,TRUE,buf);
    goto auth_exit;
  }

  auth_result=(*authenticator) (userid,
		    password,
		    (int) plen+1,
		    (int) 0, /*ptype*/
		    service,
		    key,
		    &server_na,
		    &source_na,
		    errbuf,
		    versinfo,
		    serv_fd);

  if(auth_result!=0)
    send_guar_news(serv_fd,GUAR_fatal,TRUE,errbuf);
  else {
    char buf[300];
    long now=time(0L);
    sprintf(buf,"Guardian:%s started at %s",service,ctime(&now));
    send_guar_news(serv_fd,GUAR_news,TRUE,buf);
  }

auth_exit:
  if(serv_fd!=0)
    non_block_close(serv_fd);

  return;
}
