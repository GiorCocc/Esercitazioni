#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <signal.h> //signal
#include <unistd.h> //sleep

void catchint(int signo)
{
  printf("catchint: signo = %d\n", signo); // stampa il numero del segnale ricevuto
  if (signo == SIGTSTP)                    // controllo che il segnale ricevuto sia SIGSTP
  {
    printf("catchint (pressione ctrl^z): terminazione in corso\n");
    exit(0);
  }
}

int main()
{
  signal(SIGINT, catchint);  // gestione del segnale SIGINT (ctrl^c)
  signal(SIGTSTP, catchint); // gestione del segnale SIGTSTP (ctrl^z)
  for (;;)
  {
    printf("In attesa del segnale SIGINT (premere ctrl^c)\n");
    sleep(1); // attesa di 1 secondo
  }

  exit(0);
}
