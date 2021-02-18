/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  TTTTT PPPP                                %
%                             X X     T   P   P                               %
%                              X      T   PPPP                                %
%                             X X     T   P                                   %
%                            X   X    T   P                                   %
%                                                                             %
%                                                                             %
%                         File transfer program.                              %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1991                                      %
%                                                                             %
%                                                                             %
%  Copyright 1991 E. I. Dupont de Nemours & Company                           %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above copyright notice appear in all copies and that     %
%  both that copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. Dupont de Nemours     %
%  & Company not be used in advertising or publicity pertaining to            %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. Dupont de Nemours & Company makes no representations    %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. Dupont de Nemours & Company disclaims all warranties with regard     %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. Dupont de Nemours & Company be        %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortious action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Xtp is a utility for retrieving, listing, or printing files from a
%  remote network site.  Xtp performs most of the same functions as the
%  ftp program, but does not require any interactive commands.  You simply
%  specify the file transfer task on the command line and xtp performs the
%  transfer automatically.
%
%  This program was adapted from a similiar program written by Steve Singles,
%  University of Delaware.
%
%  Command syntax:
%
%  Usage: xtp [-options ...] <host/ip address> [ <home directory> ]
%
%  Where options include:
%    -binary                retrieve files as binary
%    -exclude expression    exclude files that match the expression
%    -directory expression  list file names that match the expression
%    -ident password        specifies password
%    -print expression      print files that match the expression
%    -retrieve expression   retrieve files that match the expression
%    -send expression       send files that match the expression
%    -timeout seconds       specifies maximum seconds to logon host
%    -user name             identify yourself to the remote FTP server
%
%
*/

/*
  Include declarations.
*/
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "regular.h"
/*
  Define declarations.
*/
#define False  0
#define True  1
/*
  Variable declarations.
*/
char
  *program_name,
  slave_tty[16],
  *Wait();

int
  master;

RegularExpression
  *directory_expression,
  *exclude_expression,
  *print_expression,
  *retrieve_expression;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i r e c t o r y R e q u e s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function DirectoryRequest lists a file name and its attributes.
%
%  The format of the DirectoryRequest routine is:
%
%    DirectoryRequest(error)
%
%
*/
void DirectoryRequest(fileinfo,filename)
char
  *fileinfo,
  *filename;
{
  if (*fileinfo == (char) NULL)
    (void) fprintf(stdout,"%s\n",filename);
  else
    (void) fprintf(stdout,"%s %s\n",fileinfo,filename);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E r r o r                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Error prints an error message then terminates the program.
%
%  The format of the Error routine is:
%
%    Error(error)
%
%  A description of each parameter follows:
%
%    o error:  Specifies a pointer to a character array that contains the
%      text of the error message.
%
%
*/
void Error(error)
char
  *error;
{
  char
    message[80];

  (void) sprintf(message,"%s: %s",program_name,error);
  perror(message);
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x e c u t e F t p                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ExecuteFtp executes the FTP program as a child process.
%
%  The format of the ExecuteFtp routine is:
%
%    ExecuteFtp(hostname)
%
%  A description of each parameter follows:
%
%    o hostname:  Specifies a pointer to a character array that contains the
%      name of the host to establish a connection to a FTP server.
%
%
*/
void ExecuteFtp(hostname)
char
  *hostname;
{
#include <sys/ioctl.h>

  int
    slave;

  struct ltchars
    lc;

  struct sgttyb
    b;

  struct tchars
    tc;

  (void) signal(SIGTSTP,SIG_IGN);
  if (isatty(0))
    {
      int
        tty;

      /*
        Disable controlling terminal.
      */
      tty=open("/dev/tty",O_RDWR);
      if (tty >= 0)
        {
          (void) ioctl(tty,TIOCNOTTY,(char *) 0);
          (void) close(tty);
        }
    }
  slave=open(slave_tty,O_RDWR);
  if (slave < 0)
    Error(slave_tty);
  /*
    Fix tty line.
  */
  (void) ioctl(slave,TIOCGETP,&b);
  b.sg_flags&=~(ECHO | CRMOD);
  b.sg_erase=(-1);
  b.sg_kill=(-1);
  (void) ioctl(slave,TIOCSETP,&b);
  tc.t_intrc=(-1);
  tc.t_quitc=(-1);
  tc.t_startc=(-1);
  tc.t_stopc=(-1);
  tc.t_eofc=(-1);
  tc.t_brkc=(-1);
  (void) ioctl(slave,TIOCSETC,&tc);
  lc.t_suspc=(-1);
  lc.t_dsuspc=(-1);
  lc.t_rprntc=(-1);
  lc.t_flushc=(-1);
  lc.t_werasc=(-1);
  lc.t_lnextc=(-1);
  (void) ioctl(slave,TIOCSLTC,&lc);
  /*
    Execute FTP program as a child process.
  */
  (void) close(master);
  (void) dup2(slave,0);
  (void) dup2(slave,1);
  (void) dup2(slave,2);
  (void) close(slave);
  (void) execlp("ftp","ftp","-n","-i","-g",hostname,(char *) 0);
  perror("/usr/ucb/ftp");
  (void) kill(0,SIGTERM);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t H o s t I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function GetHostInfo prints the hostname and IP address.
%
%  The format of the GetHostInfo routine is:
%
%    GetHostInfo(host)
%
%  A description of each parameter follows:
%
%    o host:  Specifies a pointer to a character array that contains either
%      a name of a host or an IP address.
%
%
*/
void GetHostInfo(host)
char
  *host;
{
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

  char
    *address,
    *inet_ntoa(),
    *p;

  struct in_addr
    in;

  struct hostent
    *hp;

  if (isdigit(*host))
    {
      /*
        Internet address to name.
      */
      in.s_addr=inet_addr(host);
      hp=gethostbyaddr((char *) &in.s_addr,sizeof(in.s_addr),AF_INET);
      if (hp != (struct hostent *) NULL)
        {
          hp=gethostbyname(hp->h_name);
          if (hp != (struct hostent *) NULL)
            {
              in.s_addr= *(int *) hp->h_addr;
              address=inet_ntoa(in);
            }
        }
    }
  else
    {
      /*
        Internet name to address.
      */
      hp=gethostbyname(host);
      if (hp != (struct hostent *) NULL)
        {
          in.s_addr= *(int *) hp->h_addr;
          address=inet_ntoa(in);
          hp=gethostbyaddr((char *) &in.s_addr,sizeof(in.s_addr),AF_INET);
        }
    }
  if (hp == (struct hostent *) NULL)
    (void) fprintf(stdout,"%s: ",host);
  else
    {
      /*
        Convert hostname to lower-case characters and print.
      */
      p=hp->h_name;
      while (*p)
      {
        if (isupper(*p))
          *p=tolower(*p);
        p++;
      }
      (void) fprintf(stdout,"%s [%s]: ",hp->h_name,address);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P s e u d o T e r m i n a l                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function GetPseudoTerminal returns a master/slave pair of pseudo-terminals.
%
%  The format of the GetPseudoTerminal routine is:
%
%    GetPseudoTerminal()
%
%
*/
void GetPseudoTerminal()
{
  char
    master_tty[16];

  register char
    *bank,
    *cp;

  struct stat
    info;

  for (bank="pqrs"; *bank; bank++)
  {
    (void) sprintf(master_tty,"/dev/pty%c0",*bank);
    if (stat(master_tty,&info) < 0)
      break;
    for (cp="0123456789abcdef"; *cp; cp++)
    {
      (void) sprintf((char *) master_tty,"/dev/pty%c%c",*bank,*cp);
      master=open(master_tty,O_RDWR);
      if (master >= 0)
        {
          /*
            Verify slave side is usable.
          */
          (void) sprintf(slave_tty,"/dev/tty%c%c",*bank,*cp);
          if (access(slave_tty,R_OK | W_OK) == 0)
            return;
          (void) close(master);
        }
    }
  }
  Error("All network ports in use.\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a k e D i r e c t o r y                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MakeDirectory checks each component of a directory path and if it
%  does not exist, creates it.
%
%  The format of the MakeDirectory routine is:
%
%    MakeDirectory(directory)
%
%  A description of each parameter follows:
%
%    o directory:  Specifies a pointer to a character array that contains
%      the name of the directory to create.
%
%
*/
int MakeDirectory(directory)
char
  *directory;
{
#define IsDirectory(mode) (((mode) & S_IFMT) == S_IFDIR)

  register char
    *p;

  struct stat
    info;

  /*
    Determine first component of the directory.
  */
  p=strrchr(directory,'/');
  if ((p == (char *) NULL) || (p == directory))
    return(False);
  *p=(char) NULL;
  if (lstat(directory,&info) < 0)
    {
      /*
        Path component does not exist;  create it.
      */
      if (MakeDirectory(directory) == 0)
        if (mkdir(directory,0777) >= 0)
          {
            *p='/';
            return(False);
          }
    }
  else
    if (IsDirectory(info.st_mode))
      {
        /*
          Path component already exists.
        */
        *p='/';
        return(False);
      }
  /*
    Path component is a file not a directory.
  */
  *p='/';
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r i n t R e q u e s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function PrintRequest prints a file on the remote FTP server.
%
%  The format of the PrintRequest routine is:
%
%    PrintRequest(filename)
%
%  A description of each parameter follows:
%
%    o filename:  Specifies a pointer to a character array that contains
%      the name of the file to print.
%
%
*/
void PrintRequest(filename)
char
  *filename;
{
  char
    command[256],
    *response;

  /*
    get remote-file [ - | <| zcat> ].
  */
  (void) sprintf(command,"get %s",filename);
  (void) write(master,command,(int) strlen(command));
  if (strcmp(filename+strlen(filename)-2,".Z"))
    (void) sprintf(command," -\n");
  else
    (void) sprintf(command," | zcat\n");
  (void) write(master,command,(int) strlen(command));
  (void) fprintf(stdout,"%s:\n",filename);
  while (response=Wait())
    (void) fprintf(stdout,"%s\n",response);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r o c e s s R e q u e s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ProcessRequest first records any file in the current directory
%  of the remote FTP server or any of its subdirectories.  Next each filename
%  is either accepted or rejected based on a user specified regular
%  expresssion.  If any files match the regular expression, its filename is
%  listed, it is printed, or it is retrieved as specified on the command line.
%
%  The format of the ProcessRequest routine is:
%
%    ProcessRequest()
%
%
*/
void ProcessRequest()
{
  typedef struct _DirectoryNode
  {
    char
      *info,
      *name;

    struct _DirectoryNode
      *next;
  } DirectoryNode;

  char
    command[256],
    directory[1024],
    *info,
    *name,
    *response;

  DirectoryNode
    *next,
    *root;

  register char
    *p;

  register DirectoryNode
    **last,
    *node;

  RegularExpression
    *date_expression,
    *mode_expression;

  unsigned int
    unix_filesystem;

  void
    DirectoryRequest(),
    PrintRequest(),
    RetrieveRequest();

  /*
    Initialize function variables.
  */
  root=(DirectoryNode *) NULL;
  last=(&root);
  *directory=(char) NULL;
  /*
    Unix-style filesystem if the first few characters is in mode format.
  */
  mode_expression=CompileRegularExpression("^[dbclps-][rwx-][rwx-][rwx-]");
  unix_filesystem=False;
  (void) strcpy(command,"dir\n");
  (void) write(master,command,(int) strlen(command));
  response=Wait();
  if (response == (char *) NULL)
    return;
  while (response=Wait())
    if (*response != (char) NULL)
      if (!unix_filesystem)
        unix_filesystem=ExecuteRegularExpression(mode_expression,response);
  (void) free((void *) mode_expression);
  /*
    Issue directory command to remote FTP server.
  */
  if (unix_filesystem)
    (void) strcpy(command,"dir -R\n");
  else
    (void) strcpy(command,"ls [...]\n");
  (void) write(master,command,(int) strlen(command));
  response=Wait();
  if (response == (char *) NULL)
    return;
  response=Wait();
  if (response == (char *) NULL)
    {
      /*
        Directory command has limited functionality.
      */
      (void) strcpy(command,"dir\n");
      (void) write(master,command,(int) strlen(command));
      response=Wait();
      if (response == (char *) NULL)
        return;
    }
  if (!unix_filesystem)
    do 
    {
      /*
        Link non unix-style file into file list.
      */
      if (*response == (char) NULL)
        continue;
      while (*response == ' ')
        response++;
      /*
        Extract file name & info.
      */
      name=response;
      info=response;
      while ((*info != ' ') && *info)
        info++;
      *info=(char) NULL;
      if (exclude_expression)
        if (ExecuteRegularExpression(exclude_expression,name))
          continue;
      node=(DirectoryNode *) malloc(sizeof(DirectoryNode));
      if (node == (DirectoryNode *) NULL)
        Error("unable to allocate memory");
      node->name=(char *) malloc((unsigned int) (strlen(name)+1));
      node->info=(char *) malloc((unsigned int) (strlen(info)+1));
      if ((node->name == (char *) NULL) || (node->info == (char *) NULL))
        Error("unable to allocate memory");
      (void) strcpy(node->name,name);
      (void) strcpy(node->info,info);
      node->next=(DirectoryNode *) NULL;
      *last=node;
      last=(&node->next);
    }
    while (response=Wait());
  else
    {
      RegularExpression
        *access_expression;

      access_expression=
        CompileRegularExpression("Permission denied|not found|cannot access");
      date_expression=
        CompileRegularExpression(" [0-9][0-9][0-9][0-9]|[0-9][0-9]:[0-9][0-9]");
      do
      {
        /*
           Link unix-style file into file list.
        */
        if (*response == (char) NULL)
          continue;
        while (*response == ' ')
          response++;
        p=response+strlen(response)-1;
        if (*response == '-')
          {
            if (ExecuteRegularExpression(access_expression,response))
              continue;
            /*
              Extract file info & name.
            */
            while (p-- > (response+5))
            if (*p == ' ')
              if (!ExecuteRegularExpression(date_expression,p-5))
                *p='_';
              else
                break;
            *p++=(char) NULL;
            while (*p == ' ')
              p++;
            name=p;
            if (exclude_expression)
              if (ExecuteRegularExpression(exclude_expression,name))
                continue;
            info=response;
            node=(DirectoryNode *) malloc(sizeof(DirectoryNode));
            if (node == (DirectoryNode *) NULL)
              Error("unable to allocate memory");
            node->name=(char *)
              malloc((unsigned int) (strlen(directory)+strlen(name)+1));
            node->info=(char *) malloc((unsigned int) (strlen(info)+1));
            if ((node->name == (char *) NULL) || (node->info == (char *) NULL))
              Error("unable to allocate memory");
            (void) strcpy(node->name,directory);
            (void) strcat(node->name,name);
            (void) strcpy(node->info,info);
            node->next=(DirectoryNode *) NULL;
            *last=node;
            last=(&node->next);
          }
        else
          if (*p == ':')
            {
              /*
                File is a directory.
              */
              do { p--; } while (*p == ' ');
              *(++p)=(char) NULL;
              (void) strcpy(directory,response);
              (void) strcat(directory,"/");
            }
      }
      while (response=Wait());
      (void) free((void *) access_expression);
      (void) free((void *) date_expression);
    }
  /*
    Traverse the file list and act on a filename if it matches the regular
    expression.
  */
  node=root;
  while (node)
  {
    if (directory_expression)
      if (ExecuteRegularExpression(directory_expression,node->name))
        (void) DirectoryRequest(node->info,node->name);
    if (retrieve_expression)
      if (ExecuteRegularExpression(retrieve_expression,node->name))
        (void) RetrieveRequest(node->name);
    if (print_expression)
      if (ExecuteRegularExpression(print_expression,node->name))
        (void) PrintRequest(node->name);
    /*
      Free allocated memory for this node.
    */
    (void) free(node->info);
    (void) free(node->name);
    next=node->next;
    (void) free((void *) node);
    node=next;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e t r i e v e R e q u e s t                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function RetrieveRequest retrieves a file from the remote FTP server.
%
%  The format of the RetrieveRequest routine is:
%
%    RetrieveRequest(filename)
%
%  A description of each parameter follows:
%
%    o filename:  Specifies a pointer to a character array that contains
%      the name of the file to retrieve.
%
%
*/
void RetrieveRequest(filename)
char
  *filename;
{
  char
    command[256],
    *response;

  /*
    get remote-file
  */
  (void) MakeDirectory(filename);
  (void) sprintf(command,"get %s\n",filename);
  (void) write(master,command,(int) strlen(command));
  (void) fprintf(stdout,"Retrieving %s...\n",filename);
  while (response=Wait())
    (void) fprintf(stdout,"%s\n",response);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S i g n a l C h i l d                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function SignalChild is called if the status of the child process changes.
%
%  The format of the SignalChild routine is:
%
%    SignalChild()
%
%
*/
void SignalChild()
{
  char
    message[256];

  union wait
    status;

  while (wait3(&status,WNOHANG,(struct rusage *) NULL) > 0);
  (void) sprintf(message,"child died, status %x",wait(&status));
  Error(message);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Usage displays the program usage;
%
%  The format of the Usage routine is:
%
%      Usage(message)
%
%  A description of each parameter follows:
%
%    o message:  Specifies a specific message to display to the user.
%
%
*/
void Usage(message)
char
  *message;
{
  char
    **p;

  static char
    *options[]=
    {
      "-binary                retrieve files as binary",
      "-exclude expression    exclude files that match the expression",
      "-directory expression  list file names that match the expression",
      "-ident password        specifies password",
      "-print expression      print files that match the expression",
      "-retrieve expression   retrieve files that match the expression",
      "-send expression       send files that match the expression",
      "-timeout seconds       specifies maximum seconds to logon host",
      "-user name             identify yourself to the remote FTP server",
      "-verbose               show all responses from the remote server",
      NULL
    };
  if (message)
    (void) fprintf(stderr,"Can't continue, %s\n\n",message);
  (void) fprintf(stderr,
    "Usage: %s [-options ...] <host/ip address> [ <home directory> ]\n",
    program_name);
  (void) fprintf(stderr,"\nWhere options include:\n");
  for (p=options; *p; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W a i t                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Wait reads a line of output from the remote FTP server.
%
%  The format of the Wait() routine is:
%
%    response=Wait()
%
%  A description of each parameter follows:
%
%    o response:  Function Wait returns this pointer to the output obtained
%      from the remote FTP server.
%
%
*/
char *Wait()
{
  register char
    *p;

  static char
    buffer[1024],
    *q;

  static char
    line[1024];

  static int
    count=0;

  p=line;
  do
  {
    if (count <= 0)
      {
        /*
          The buffer is empty;  read output from the remote FTP server.
        */
        count=read(master,buffer,sizeof(buffer));
        q=buffer;
        if (count <= 0)
          {
            if (p == line)
              return((char *) NULL);
            break;
          }
      }
    count--;
    *p=(*q++);
    if (*p == '\n')
      break;
    p++;
    if ((p-line) >= 5)
      if (!strncmp(p-5,"ftp> ",5))
        if (count == 0)
          return((char *) NULL);
  } while (p < (line+sizeof(line)));
  *p=(char) NULL;
  return(line);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   m a i n                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
main(argc,argv)
int
  argc;

register char
  **argv;
{
#include <pwd.h>

  char
    command[256],
    *ident,
    *home_directory,
    *hostname,
    *send_expression,
    *user;

  extern char
    *getpass();

  int
    binary,
    child,
    status;

  register char
    *p,
    *response;

  unsigned int
    timeout,
    verbose;

  /*
    Initialize program variables.
  */
  binary=False;
  directory_expression=(RegularExpression *) NULL;
  exclude_expression=(RegularExpression *) NULL;
  ident=(char *) NULL;
  print_expression=(RegularExpression *) NULL;
  retrieve_expression=(RegularExpression *) NULL;
  send_expression=(char *) NULL;
  timeout=0;
  user=(char *) NULL;
  program_name=argv[0];
  verbose=False;
  /*
    Parse command line arguments.
  */
  for (p=(*argv++); *argv && (**argv == '-'); argv++)
    switch (argv[0][1])
    {
      case 'b':
      {
        binary=True;
        break;
      }
      case 'd':
      {
        directory_expression=CompileRegularExpression(*++argv);
        if (!directory_expression)
          exit(1);
        break;
      }
      case 'e':
      {
        exclude_expression=CompileRegularExpression(*++argv);
        if (!exclude_expression)
          exit(1);
        break;
      }
      case 'i':
      {
        ident=(*++argv);
        break;
      }
      case 'p':
      {
        print_expression=CompileRegularExpression(*++argv);
        if (!print_expression)
          exit(1);
        break;
      }
      case 'r':
      {
        retrieve_expression=CompileRegularExpression(*++argv);
        if (!retrieve_expression)
          exit(1);
        break;
      }
      case 's':
      {
        send_expression=(*++argv);
        break;
      }
      case 't':
      {
        timeout=atoi(*++argv);
        break;
      }
      case 'u':
      {
        user=(*++argv);
        break;
      }
      case 'v':
      {
        verbose=True;
        break;
      }
      default:
      {
        Usage((char *) NULL);
        break;
      }
    }
  if ((argc < 2) || (*argv == (char *) NULL))
    Usage((char *) NULL);
  hostname=argv[0];
  home_directory=argv[1];
  if ((directory_expression == (RegularExpression *) NULL) &&
      (print_expression == (RegularExpression *) NULL) &&
      (retrieve_expression == (RegularExpression *) NULL) &&
      (send_expression == (char *) NULL))
    directory_expression=CompileRegularExpression("");
  if ((ident == (char *) NULL) && (user == (char *) NULL))
    {
      int
        uid;

      static char
        name[256];

      struct passwd
        *user_info;

      /*
        Identify user as user@host.domain.
      */
      uid=geteuid();
      user_info=getpwuid(uid);
      if (user_info == (struct passwd *) NULL)
        (void) strcpy(name,"anonymous");
      else
        (void) strcpy(name,user_info->pw_name);
      p=name+strlen(name);
      *p++='@';
      (void) gethostname(p,64);
      while (*p)
        p++;
      *p++='.';
      (void) getdomainname(p,64);
      user="anonymous";
      ident=name;
    }
  else
    if (ident == (char *) NULL)
      ident=(char *) getpass("Password: ");
    else
      if (user == (char *) NULL)
        user="anonymous";
  (void) GetHostInfo(hostname);
  if (!home_directory)
    (void) fprintf(stdout,"\n");
  else
    (void) fprintf(stdout,"%s\n",home_directory);
  (void) GetPseudoTerminal();
  /*
    Connect and logon to host.
  */
  (void) signal(SIGCHLD,SignalChild);
  if (timeout > 0)
    (void) alarm(timeout);  /* enable timer. */
  child=fork();
  if (child < 0)
    Error("fork");
  if (child == 0)
    ExecuteFtp(hostname);
  while (response=Wait())
    (void) fprintf(stderr,"%s\n",response);
  (void) sprintf(command,"user %s %s\n",user,ident);
  (void) write(master,command,(int) strlen(command));
  while (response=Wait())
    (void) fprintf(stderr,"%s\n",response);
  if (timeout > 0)
    (void) alarm(timeout*100);
  (void) fprintf(stderr,"\n");
  if (!verbose)
    {
      (void) strcpy(command,"verbose off\n");
      (void) write(master,command,(int) strlen(command));
      while (response=Wait());
    }
  if (home_directory)
    {
      /*
        Change remote working directory.
      */
      (void) sprintf(command,"cd %s\n",home_directory);
      (void) write(master,command,(int) strlen(command));
      while (response=Wait());
      (void) strcpy(command,"pwd\n");
      (void) write(master,command,(int) strlen(command));
      while (response=Wait())
        (void) fprintf(stderr,"%s\n",response);
    }
  if (binary)
    {
      /*
        Set file transfer type.
      */
      (void) strcpy(command,"binary\n");
      (void) write(master,command,(int) strlen(command));
      while (Wait());
      (void) strcpy(command,"type\n");
      (void) write(master,command,(int) strlen(command));
      while (response=Wait())
        (void) fprintf(stderr,"%s\n",response);
    }
  (void) strcpy(command,"runique\n");
  (void) write(master,command,(int) strlen(command));
  while (Wait());
  if (send_expression == (char *) NULL)
    ProcessRequest();
  else
    {
      /*
        Process send request.
      */
      (void) strcpy(command,"glob on\n");
      (void) write(master,command,(int) strlen(command));
      while (Wait());
      (void) sprintf(command,"mput %s\n",send_expression);
      (void) write(master,command,(int) strlen(command));
      while (response=Wait())
        (void) fprintf(stderr,"%s\n",response);
    }
  (void) strcpy(command,"quit\n");
  (void) write(master,command,(int) strlen(command));
  /*
    Wait for child to finish.
  */
  (void) signal(SIGCHLD,SIG_DFL);
  while (child != wait((union wait *) &status));
  (void) close(master);
  (void) free((void *) directory_expression);
  (void) free((void *) exclude_expression);
  (void) free((void *) print_expression);
  (void) free((void *) retrieve_expression);
  return(False);
}
