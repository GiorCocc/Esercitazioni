#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// variabili globali
int stato = 1;
int i = 0;

void handler(int signo)
{
  stato = !stato; // cambia lo stato
}

int main()
{
  struct sigaction action, old_action;

  /* azzera tutti i flag della maschera sa_mask nella struttura action */
  sigemptyset(&action.sa_mask);
  action.sa_handler = handler; // funzione che gestisce il segnale SIGUSR1
  action.sa_flags = 0;         // flag di default

  /* assegna action per la gestione di SIGUSR1 */
  if (sigaction(SIGUSR1, &action, &old_action) == -1)
    perror("sigaction error");

  // stampo il pid del processo così in una shell diversa eseguo il comando kill -USR1 <pid> per mandare il segnale e invertire il conteggio
  int pid = getpid();       // prende il pid del processo
  printf("PID: %d\n", pid); // stampa il pid del processo

  for (;;) // ciclo infinito
  {
    if (stato)             // se stato è 1
      printf("%d\n", i++); // stampo i++
    else                   // altrimenti
      printf("%d\n", i--); // stampo i--

    sleep(1); // attendo 1 secondo tra una stampa e l'altra
  }

  exit(0);
}
