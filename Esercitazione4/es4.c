#include <unistd.h> // fork()
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  pid_t pid; // pid del processo figlio
  int N; //numero di volte in cui eseguire la fork()

  // controllo degli argomenti
  if (argc != 2)
  {
    perror("Inserire come argomento il numero di processi da generare");
    exit(-1);
  }

  N = atoi(argv[1]); //aggiornamento del numero di volte in cui eseguire la fork()

  for (int i = 0; i < N; i++)
  {
    if ((pid = fork()) < 0)
    {
      perror("fork error");
      exit(1);
    }
    printf("[PID %d - child of %d] hello pid=%d!\n", getpid(), getppid(), pid);
  }

  exit(0);
}
