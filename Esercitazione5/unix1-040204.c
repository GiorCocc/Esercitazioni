/****************************************************************************

Sistemi Operativi A

Esercizio UNIX-1 del 4/2/04


Il sistema consiste di due processi: un processo Ppadre che crea un
processo Pfiglio il quale inizialmente attende un segnale SIGUSR1
oppure SIGUSR2 dal processo Ppadre ;

Il processo Ppadre , dopo aver atteso un intervallo di durata casuale,
invia a Pfiflio un segnale tra SIGUSR1 e SIGUSR2 per poi inviare,
attraverso una pipe , il numero (1 o 2 ) del segnale inviato;

Il processo Pfiglio, ricevuto il segnale e letto il messaggio dalla
pipe, visualizza il segnale e il messaggio ricevuti.
 

Devono essere utilizzate le primitive per la gestione affidabile dei segnali.


****************************************************************************/



#include <signal.h>
#include <stdlib.h>



static int segnale_ricevuto;

void sigusr_handler(int signo)
{

  segnale_ricevuto = signo ;

}



main()
{
  int pid,piped[2], mesg_segnale, segnale_da_inviare;
  struct sigaction act;
  sigset_t sigmask, zeromask;
 

/* GESTIONE SEGNALI  */

sigemptyset( &zeromask);

sigemptyset( &sigmask);
sigaddset(&sigmask, SIGUSR1);
sigaddset(&sigmask, SIGUSR2);

/* Blocco dei segnali SIGUSR1/SIGUSR2 */
sigprocmask(SIG_BLOCK, &sigmask, NULL);

act.sa_handler= sigusr_handler; /* Lo stesso gestore per entrambi i segnali  */
sigemptyset( &act.sa_mask);
act.sa_flags= 0;

sigaction(SIGUSR1, &act, NULL);
sigaction(SIGUSR2, &act, NULL);

/* L'intera politica di gestione dei segnali verra' ereditata del figlio */

  

/* CREAZIONE PIPE  */
  if(pipe(piped)<0)
    {
      perror("Creazione pipe :");
      exit(-1);
    }

/* CREAZIONE FIGLIO  */

  if((pid=fork())<0)
     {
      perror("Creazione processo figlio (fork) :");
      exit(-2);
    }
  else 
    if(pid==0)	 /* Processo figlio */
      {
	sigsuspend(&zeromask);

	read(piped[0],&mesg_segnale,sizeof(int));

	printf("FIGLIO: Ricevuto il segnale %s e il messaggio %d\n",(segnale_ricevuto == SIGUSR1)? "SIGUSR1":"SIGUSR2", mesg_segnale);
	exit(0);
      }
    else   /* Processo padre */
      {

	srand(getpid());	/* Per inizializzare il generatore di numeri casuali */
	sleep(rand()%5+1);	/* Attesa di durata casuale tra 1 e 5 secondi */

	segnale_da_inviare = rand()%2 + 1 ; /* 1 o 2  */

	if(segnale_da_inviare == 1)
	  kill(pid,SIGUSR1);

	else
	   kill(pid,SIGUSR2);


	write(piped[1],&segnale_da_inviare,sizeof(int));

	printf("PADRE: Inviato il segnale %s e il messaggio %d\n",(segnale_da_inviare == 1)? "SIGUSR1":"SIGUSR2", segnale_da_inviare);

	wait(NULL);		/* Attesa della terminazione del figlio  */

	exit(0);
	
      }


}
