#include <stdio.h>
#include <signal.h>
#include <iroha/RK.h>
#include <iroha/jrkanji.h>

#define UI_CONNECTION

#define MAX_PROC 128

int proc_id[MAX_PROC];

main()
{
  int i, counter = 0;
  int pid, proc_error(), proc_cont();

  signal(SIGUSR1, proc_error);
  signal(SIGUSR2, proc_cont);

  for (i = 0 ; i < MAX_PROC ; i++) {
    proc_id[i] = 0;
  }

  for (i = 0 ; i < MAX_PROC ; i++) {
    pid = fork();
    if (pid == -1) { /* 失敗 */
      fprintf(stderr, "%d個目でforkが失敗しました。\n", i);
      exit_con(1);
    }
    else if (pid == 0) { /* 子プロセス */
      JrConnect(i);
      kill(getppid(), SIGUSR2);
      pause();
    }
    else { /* 親プロセス */
      proc_id[i] = pid;
      pause();
    }
  }
}

exit_con(val)
int val;
{
  int i;

  for (i = 0 ; i < MAX_PROC && proc_id[i] ; i++) {
    fprintf(stderr, "%d ", proc_id[i]);
  }
  fprintf(stderr, "\n");
  fflush(stderr);

  exit(val);
}

proc_error(sig)
int sig;
/* ARGSUSED */
{
  exit_con(1);
}


RkConnect(n)
int n;
{
  int res, resc1, resc2, resc3;
  res = RkInitialize();
  RkMountDic(res, "iroha", 0);
  resc1 = RkDuplicateContext(res);
  resc2 = RkDuplicateContext(res);
  resc3 = RkDuplicateContext(res);
  fprintf(stderr, "%2d: RkInitialize returns %d, 1: %d, 2: %d, 3: %d\n",
	  n, res, resc1, resc2, resc3);
  fflush(stderr);
  if (res == -1 || resc1 == -1 || resc2 == -1 || resc3 == -1) {
    kill(getppid(), SIGUSR1);
    exit(0);
  }
}


JrConnect(n)
int n;
{
  char aho[200];
  int res, resc1, resc2, resc3;
  res = jrKanjiControl(0, KC_INITIALIZE, 0);
  jrKanjiControl(0, KC_QUERYMODE, aho);
  fprintf(stderr, "%2d: KC_INITIALIZE returns %d\n", n, res);
  fflush(stderr);
  if (res == -1) {
    kill(getppid(), SIGUSR1);
    exit(0);
  }
}

proc_cont(sig)
int sig;
/* ARGSUSED */
{
  signal(SIGUSR2, proc_cont);
  return 0;
}
