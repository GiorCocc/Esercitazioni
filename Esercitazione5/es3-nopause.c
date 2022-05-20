#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ntimes = 0; /* variabile globale */

void catcher(int signo)
{
  printf("Processo %d ricevuto #%d volte\n", getpid(), ++ntimes);
}

int main()
{
  int pid, ppid;

  signal(SIGUSR1, catcher); /* il figlio la ereditera' */

  if ((pid = fork()) < 0)
  {
    perror("fork error");
    exit(1);
  }
  else if (pid == 0)
  {
    ppid = getppid();
    for (;;)
    {
      printf("figlio: mio padre e' %d\n", ppid);
      sleep(1);
      kill(ppid, SIGUSR1);
      //          pause();
    }
  }
  else
  {
    for (;;)
    {
      printf("padre: mio figlio e' %d\n", pid);
      //          pause();
      sleep(1);
      kill(pid, SIGUSR1);
    }
  }
}
