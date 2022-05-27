#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define N 10

// variabili globali
int pg[2];     // array di 2 elementi, il primo per la pipe di scrittura, il secondo per la pipe di lettura
int tabpid[N]; // array di N elementi, contiene i pid dei processi figli
char arg1;     // arg1 è il carattere che viene passato come argomento al processo padre

void handler(int signo)
{
  printf("Sono il processo %d e ho ricevuto il segnale %d\n", getpid(), signo);
}

// TODO: sistemare la funzione e il suo comportamento. Ottengo come output: "Segnale 1 definito dall'utente"
void body_proc(int id, char arg1)
{
  // se argv1=='a' ogni processo figlio con id pari manda un segnale (SIGUSR1) al processo id+1
  if (arg1 == 'a')
  {
    while (1)
    {
      if (id % 2 == 0)
      {
        kill(tabpid[id + 1], SIGUSR1);
      }
      sleep(1);
    }
  }

  // se argv1=='b' ogni processo figlio con id<N/2 manda un segnale (SIGUSR1) al processo id+N/2
  if (arg1 == 'b')
  {
    while (1)
    {
      if (id < N / 2)
      {
        kill(tabpid[id + N / 2], SIGUSR1);
      }
      sleep(1);
    }
  }
}

int main(int argc, char *argv[])
{
  int i;

  if (pipe(pg) < 0) // crea la pipe
  {
    perror("pipe error");
    exit(-1);
  }

  if (argc!=2)
  {
    printf("Usage: %s <arg1>\n", argv[0]);
    exit(-1);
  }

  arg1 = argv[1][0]; // primo carattere del secondo argomento

  for (i = 0; i < N; i++) // crea N processi figli
  {
    if ((tabpid[i] = fork()) < 0) // crea un processo figlio
    {
      perror("fork error");
      exit(-1);
    }
    else if (tabpid[i] == 0) // figlio
      body_proc(i, arg1);    // esegue la funzione body_proc
  }
  printf("Sono il padre e scrivo sulla pipe la tabella dei pid\n");
  write(pg[1], tabpid, sizeof tabpid); // scrive sulla pipe la tabella dei pid

  return 0;
}
