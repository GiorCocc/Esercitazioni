#include <sys/types.h>
#include <sys/wait.h>


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main()  
{
  int status;
  pid_t pid;
  char* arg[]= {"ls", "-l", "/usr/include",(char *)0};

  if ((pid=fork())==0) 
  {
    /* CODICE ESEGUITO DAL FIGLIO */
    execv("/bin/ls", arg);
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
