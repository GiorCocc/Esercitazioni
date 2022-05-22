#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ntimes = 0; /* variabile globale */

void catcher(int signo)
{
  printf("Processo %d ricevuto #%d volte\n", getpid(), ++ntimes);
}

int main()
{
  int pid, ppid;           // pid del processo e pid del processo padre
  sigset_t set, zeromask;  // set di segnali da bloccare e set vuoto
  struct sigaction action; // struttura per gestione segnali

  sigemptyset(&zeromask);
  /*  prepara una maschera di segnali vuota (da usare più avanti) ovvero azzera la maschera dei segnali */

  sigemptyset(&action.sa_mask);
  /* azzera tutti i flag della maschera di segnali sa_mask,
     presente all'interno della struttura action.
     Tale maschera corrisponde all'insieme di segnali
     che saranno bloccati durante l'esecuzione della procedura di
     gestione del segnale a cui action verrà associata */

  action.sa_handler = catcher; /* assegna la funzione da chiamare alla ricezione del segnale */
  action.sa_flags = 0;         /* azzera tutti i flag della struttura action */

  // PROTEGGO IL PROCESSO DALL'ARRIVO DI SEGNALI SIGUSR1
  // AL DI FUORI DEL MOMENTO IN CUI LO ATTENDO
  // Il "momento in cui lo attendo" corrisponde al punto in cui pongo la sigsuspend.
  sigemptyset(&set);                  /* azzera la maschera di segnali set... */
  sigaddset(&set, SIGUSR1);           /* ...e quindi ci aggiunge SIGUSR1 */
  sigprocmask(SIG_BLOCK, &set, NULL); /* aggiunge (in OR) all'insieme dei segnali mascherati dal processo corrente quelli contenuti in set */

  if (sigaction(SIGUSR1, &action, NULL) == -1)
    /* assegna action per la gestione di SIGUSR1 */

    perror("sigaction error");

  /* da questo punto in poi, alla ricezione di un segnale SIGUSR1
     il processo corrente (come anche tutti i suoi figli)
     risponderà con l'invocazione della funzione catcher().
     In questo momento però i segnali SIGUSR1 sono invisibili al
     processo grazie alla precedente invocazione della sigprocmask:
     grazie alla sigsuspend(&zeromask) sblocco i segnali nel momento
     in cui sono pronto a riceverli.
     */

  if ((pid = fork()) < 0) // creazione del processo figlio
  {
    perror("fork error");
    exit(1);
  }
  else if (pid == 0) /* siamo nel figlio */
  {
    ppid = getppid();                          // recupero il pid del padre
    printf("figlio: mio padre e' %d\n", ppid); // stampa il pid del padre
    for (;;)                                   /* ciclo infinito */
    {
      sleep(1);              /* aspetta un secondo */
      kill(ppid, SIGUSR1);   /* inviamo al padre un segnale SIGUSR1... */
      sigsuspend(&zeromask); /* ...e ci sospendiamo fino alla ricezione
                                di un segnale qualunque*/
    }
  }
  else /* siamo nel padre */
  {
    printf("padre: mio figlio e' %d\n", pid); // stampa il pid del figlio
    for (;;)                                  // ciclio infinito
    {
      sigsuspend(&zeromask); /* attendiamo la ricezione di un segnale... */
      sleep(1);              // ...attendiamo un secondo...
      kill(pid, SIGUSR1);    /* ...e quindi ne inviamo uno di tipo
                                SIGUSR1 al nostro figlio */
    }
  }
}
