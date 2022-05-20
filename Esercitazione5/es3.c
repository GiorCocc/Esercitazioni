#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ntimes = 0; /* variabile globale */

// funzione che gestisce il segnale SIGUSR1
void catcher(int signo)
{
  printf("Processo %d ricevuto #%d volte\n", getpid(), ++ntimes);
}

int main()
{
  int pid, ppid; // pid del processo figlio, pid del processo padre

  signal(SIGUSR1, catcher); /* il figlio la ereditera' */

  if ((pid = fork()) < 0) // creazione del processo figlio
  {
    perror("fork error");
    exit(1);
  }
  else if (pid == 0) // codice del figlio
  {
    ppid = getppid(); // recupero del pid del padre
    for (;;)
    {
      printf("figlio: mio padre e' %d\n", ppid); // stampa del pid del padre
      sleep(1);                                  // attesa di 1 secondo
      kill(ppid, SIGUSR1);                       // uccido il padre con il segnale SIGUSR1
      pause();                                   // attesa
    }
  }
  else // codice del padre
  {
    for (;;)
    {
      printf("padre: mio figlio e' %d\n", pid); // stampa del pid del figlio
      pause();                                  // attesa
      sleep(1);                                 // attesa di 1 secondo
      kill(pid, SIGUSR1);                       // uccido il figlio con il segnale SIGUSR1
    }
  }
}
