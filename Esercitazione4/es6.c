#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>

#define N 8

int main()
{
  int status, i;
  pid_t pid;
  int vettore[N];

  /* IL PADRE CREA N PROCESSI FIGLI */
  for (i = 0; i < N; i++)
    if ((pid = fork()) == 0)
    {
      sleep(1);
      printf("[PID %d - child of %d] hello pid=%d!\n", getpid(), getppid(), pid);
      exit(10 + i);
    }

  /* IL PADRE ATTENDE I FIGLI */
  // while ((pid = waitpid(-1, &status, 0)) > 0)
  // {
  //   if (WIFEXITED(status)) /* ritorna 1 se il figlio ha terminato correttamente */
  //     printf("Il figlio %d e' terminato correttamente con exit status=%d\n",
  //            pid, WEXITSTATUS(status));
  //   else
  //     printf("Il figlio %d non e' terminato correttamente\n", pid);
  // }

  exit(0);
}
