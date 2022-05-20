#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int stato=1;
int i=0;

void handler(int signo)
{
  stato=!stato;
}

int main()
{
  struct sigaction action,old_action;
 
  /* azzera tutti i flag della maschera sa_mask nella struttura action */
  sigemptyset(&action.sa_mask);
  action.sa_handler = handler;
  action.sa_flags = 0;

  /* assegna action per la gestione di SIGUSR1 */
  if (sigaction(SIGUSR1, &action, &old_action) == -1)
    perror("sigaction error");


  for(;;)
    {
      if (stato)
        printf("%d\n",i++);
      else
        printf("%d\n",i--);
      
      sleep(1);
    }
 
  exit(0);
}
