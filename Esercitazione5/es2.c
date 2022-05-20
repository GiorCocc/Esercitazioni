#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void catchint(int signo)
{
  printf("catchint: signo = %d\n", signo); // stampa il numero del segnale ricevuto
  exit(0);
}

int main()
{
  int pid; // pid del processo figlio

  if ((pid = fork()) < 0) // creazione del processo figlio
  {
    perror("fork error");
    exit(1);
  }
  else if (pid == 0)
  {
    /* il figlio dorme fino a quando riceve il segnale SIGKILL, poi muore*/
    for (;;)
    {
      printf("sono il figlio e sto ciclando all'infinito!\n");
      signal(SIGUSR1, catchint); // creazione del segnale SIGUSR1
      sleep(1);
    }

    printf("questo messaggio non dovrebbe mai essere visualizzato!\n");
    exit(0);
  }
  else
  {
    /* il padre invia un segnale SIGKILL al figlio */
    sleep(3);
    signal(SIGUSR1, catchint); // ricezione del segnale SIGUSR1
    kill(pid, SIGUSR1);        // uccisione del figlio
    printf("\nsono il padre e ho ucciso mio figlio!!\n");
    exit(0);
  }
}
