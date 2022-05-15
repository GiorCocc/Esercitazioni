#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int status;
  pid_t pid;
  char *env[] = {"TERM=vt100", "PATH=/bin:/usr/bin", (char *)0}; // variabili di ambiente da passare al processo figlio

  if(argc!=3)
  {
    printf("Usage: %s <nome file f1> <nome file f2>\n", argv[0]);
    exit(1);
  }

  char *args[] = {"cat", argv[1], argv[2], (char *)0}; // argomenti da passare al processo figlio (mostra i file argv[1] e argv[2])

  if ((pid = fork()) == 0)
  {
    /* CODICE ESEGUITO DAL FIGLIO */
    execve("/bin/cat", args, env); // cambio dell'ambiente di esecuzione "/bin/cat" con argomenti di invocazione "cat argv[1] argv[2]" contenuti in args e env
    /* Si torna solo in caso di errore */
    exit(1);
  }
  else
  {
    /* CODICE ESEGUITO DAL PADRE */
    wait(&status);
    printf("exit di %d con %d\n", pid, status);
  }

  exit(0);
}
