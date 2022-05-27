#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

//  TODO: il processo che ha generato il numero casuale minore invia un segnale SIGUSR1 all'altro processo figlio, visualizza un messaggio con il proprio pid e si pone in attesa di un segnale

//  TODO: il padre, dopo che un figlio è terminato, invia un segnale SIGUSR1 all’altro figlio, e termina con un messaggio.

int n1, n2; // numeri casuali generati dai processi figli

void handler(int signo) // ricevo un segnale SIGUSR1, visualizzo il pid e termino
{
  printf("Sono il processo %d e ho ricevuto il segnale %d\n", getpid(), signo);
  exit(0);
}

int main(int argc, char *argv[])
{
  int pid1, pid2; // pid dei processi figli
  int N = 100;    // numero casuale

  // if (argc != 2)
  // {
  //   printf("Usage: %s <n>\n", argv[0]);
  //   exit(1);
  // }

  // N = atoi(argv[1]); // converte il parametro in numero intero

  // il processo padre genera due figli

  if ((pid1 = fork()) < 0)
  {
    printf("Errore nella creazione del processo figlio 1\n");
    exit(-1);
  }
  else if (pid1 == 0) // figlio 1
  {
    // il processo figlio 1 genera un numero casuale tra 0 e N compresi
    srand(time(NULL));
    n1 = rand() % N;

    printf("Sono il processo %d, primo figlio di %d,e il mio numero casuale è %d\n", getpid(), getppid(), n1);
  }
  else
  {
    sleep(1);                // aspetto che il processo figlio 1 abbia generato il numero casuale
    if ((pid2 = fork()) < 0) // figlio 2
    {
      printf("Errore nella creazione del processo figlio 2\n");
      exit(-2);
    }
    else if (pid2 == 0)
    {
      // il processo figlio 2 genera un numero casuale tra 0 e N compresi
      srand(time(NULL));
      n2 = rand() % N;

      printf("Sono il processo %d, secondo figlio di %d,e il mio numero casuale è %d\n", getpid(), getppid(), n2);
    }
    else
    {
      // il processo padre attende che i figli terminino
      printf("Sono il processo %d, padre di %d e %d\n", getpid(), pid1, pid2);
    }
  }
}