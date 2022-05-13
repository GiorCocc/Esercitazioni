#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() 
{
  int status;
  pid_t pid;
  char *env[] = {"TERM=vt100", "PATH=/bin:/usr/bin", (char *) 0 };
  char *args[] = {"cat", "f1", "f2", (char *) 0};

  if ((pid=fork())==0) 
  {
    /* CODICE ESEGUITO DAL FIGLIO */
    execve("/bin/cat", args, env);
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
