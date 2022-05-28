#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N_PROCESSI 2

int segnale_ricevuto; // variabile globale per il segnale

void handler(int signo) // gestisce il segnale SIGUSR1
{
  segnale_ricevuto = signo;                                    // memorizza il segnale ricevuto
  printf("Figlio %d riceve il segnale %d\n", getpid(), signo); // stampa il segnale ricevuto e il processo che lo riceve
}

int main(int argc, char *argv[])
{
  int pipedsc[2], pipedcs[2];        // pipe per i processi
  int n, numero, pid, tabellapid[2]; // numero massimo, numero generato, pid, tabella dei pid
  int messaggio1[2], messaggio2[2];  // messaggio per i processi

  struct sigaction sa;        // struttura per la gestione del segnale
  sigset_t sigmask, zeromask; // sigmask per la gestione del segnale, zeromask per la gestione del segnale

  // controllo degli argomenti di invocazione
  if (argc != 2)
  {
    printf("Uso: %s <numero>\n", argv[0]);
    exit(1);
  }

  n = atoi(argv[1]); // estremo del numero da generare

  // GESTIONE SEGNALI AFFIDABILI
  sigemptyset(&zeromask);                 // inizializzazione maschera a zero
  sigemptyset(&sigmask);                  // inizializzazione maschera dei segnali a zero
  sigaddset(&sigmask, SIGUSR1);           // aggiunta del segnale SIGUSR1 alla maschera
  sigprocmask(SIG_BLOCK, &sigmask, NULL); // blocco dei segnali SIGUSR1

  sa.sa_handler = handler;       // gestione del segnale SIGUSR1 con la funzione handler
  sigemptyset(&sa.sa_mask);      // inizializzazione maschera a zero
  sa.sa_flags = 0;               // inizializzazione flag a zero
  sigaction(SIGUSR1, &sa, NULL); // registrazione della gestione del segnale SIGUSR1

  // CREAZIONE DELLE PIPE DI COMUNICAZIONE
  if (pipe(pipedsc) == -1) // creazione della pipe per la comunicazione con il processo padre
  {
    perror("pipe");
    exit(1);
  }
  if (pipe(pipedcs) == -1) // creazione della pipe per la comunicazione con il processo figlio
  {
    perror("pipe");
    exit(1);
  }

  // CREAZIONE DEI PROCESSI FIGLI
  for (int i = 0; i < N_PROCESSI; i++)
  {
    if ((tabellapid[i] = fork()) < 0) // creazione dei processi figli e inserimento nella tabella dei pid
    {
      perror("fork");
      exit(1);
    }
    else if (tabellapid[i] == 0) // processo figlio
    {
      srand(getpid());         // inizializzazione della funzione rand
      numero = rand() % n + 1; // generazione del numero da 1 a n
      pid = getpid();          // recupero del pid del processo
      messaggio1[0] = pid;     // memorizzazione del pid del processo
      messaggio1[1] = numero;  // memorizzazione del numero generato

      printf("Figlio %d genera il numero %d\n", pid, numero); // stampa del numero generato e del processo che lo genera

      write(pipedcs[1], messaggio1, sizeof(messaggio1)); // scrittura del messaggio1 (pid,numero) sul canale di comunicazione
      read(pipedsc[0], &pid, sizeof(pid_t));             // lettura del pid del processo figlio

      if (pid == getpid())
      {
        sigsuspend(&zeromask);                                                                                                // sospensione del processo figlio
        printf("Figlio %d ha ricevuto il segnale %s\n", getpid(), (segnale_ricevuto == SIGUSR1) ? "SIGUSR1" : "segnale ?\n"); // stampa del segnale ricevuto dal processo figlio
      }
      else
      {
        kill(pid, SIGUSR1);                                                                                                   // invio del segnale SIGUSR1 al processo figlio
        printf("Figlio %d ha inviato il segnale SIGUSR1", getpid());                                                          // stampa a quale processo si è inviato il segnale SIGUSR1
        sigsuspend(&zeromask);                                                                                                // sospensione del processo figlio
        printf("Figlio %d ha ricevuto il segnale %s\n", getpid(), (segnale_ricevuto == SIGUSR1) ? "SIGUSR1" : "segnale ?\n"); // stampa del segnale ricevuto dal processo figlio
      }
      printf("Figlio %d termina\n", getpid()); // stampa del processo figlio che termina
      exit(0);
    }
  }

  // PROCESSO PADRE

  read(pipedcs[0], messaggio1, sizeof(messaggio1)); // lettura del messaggio1 (pid, numero) dal canale di comunicazione
  read(pipedcs[0], messaggio2, sizeof(messaggio2)); // lettura del messaggio2 (pid, numero) dal canale di comunicazione

  printf("Padre %d ha ricevuto %d (da figlio %d) e %d (da figlio %d)\n", getpid(), messaggio1[1], messaggio1[0], messaggio2[1], messaggio2[0]); // stampa del messaggio1 e del messaggio2 ricevuto dal padre

  if (messaggio2[1] <= messaggio1[1]) // se il numero generato dal processo figlio 2 è minore o uguale al numero generato dal processo figlio 1
  {
    printf("Padre %d ha inviato il segnale SIGUSR1 al figlio %d\n", getpid(), messaggio1[0]); // stampa del processo figlio a cui il padre invia il segnale SIGUSR1
    write(pipedsc[1], &messaggio1[0], sizeof(pid_t));                                         // scrittura del pid del processo figlio a cui il padre invia il segnale SIGUSR1 nella pipe di comunicazione
    write(pipedsc[1], &messaggio1[0], sizeof(pid_t));                                         // scrittura del pid del processo figlio a cui il padre invia il segnale SIGUSR1 nella pipe di comunicazione
  }
  else
  {
    printf("Padre %d ha inviato il segnale SIGUSR1 al figlio %d\n", getpid(), messaggio2[0]); // stampa del segnale SIGUSR1 inviato al processo figlio
    write(pipedsc[1], &messaggio2[0], sizeof(pid_t));                                         // scrittura del pid del processo figlio a cui il padre invia il segnale SIGUSR1 nella pipe di comunicazione
    write(pipedsc[1], &messaggio2[0], sizeof(pid_t));                                         // scrittura del pid del processo figlio a cui il padre invia il segnale SIGUSR1 nella pipe di comunicazione
  }
  wait(NULL); // attendo la terminazione ddi un processo figlio

  if (messaggio2[1] <= messaggio1[1]) // se il numero generato dal processo figlio 2 è minore o uguale al numero generato dal processo figlio 1
  {
    printf("Padre %d invia il segnale SIGUSR1 al figlio %d\n", getpid(), messaggio2[0]); // stampa del processo figlio a cui il padre invia il segnale SIGUSR1
    kill(messaggio2[0], SIGUSR1);                                                        // invio del segnale SIGUSR1 al processo figlio
  }
  else
  {
    printf("Padre %d invia il segnale SIGUSR1 al figlio %d\n", getpid(), messaggio1[0]); // stampa del processo figlio a cui il padre invia il segnale SIGUSR1
    kill(messaggio1[0], SIGUSR1);                                                        // invio del segnale SIGUSR1 al processo figlio
  }

  wait(NULL);                             // attendo la terminazione di un processo figlio
  printf("Padre %d termina\n", getpid()); // stampa del processo padre che termina
}