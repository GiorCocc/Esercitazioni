#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int pid, j, c; // pid del figlio, contatore, carattere
  int piped[2];  // array di file descriptor per la pipe, uno per la lettura (piped[0]) e uno per la scrittura (piped[1])

  /*
     Apre la pipe creando due file descriptor,
     uno per la lettura e l'altro per la scrittura
     (vengono memorizzati nell'array piped[])
  */

  if (pipe(piped) < 0)
    exit(1);

  if ((pid = fork()) < 0) // crea un processo figlio
    exit(2);
  else if (pid == 0) /* figlio, che ha una copia di piped[] */
  {
    close(piped[1]); // nel figlio il fd per la scrittura non serve, serve solo quella per la lettura (piped[0])

    for (j = 1; j <= 10; j++) // ciclo for per la lettura della pipe del padre
    {
      read(piped[0], &c, sizeof(int));                         // legge dalla pipe il numero j
      printf("Figlio: ho letto dalla pipe il numero %d\n", c); // stampa il numero letto
    }
    exit(0);
  }
  else /* padre */
  {
    close(piped[0]); // nel padre il fd per la lettura non serve, serve solo quello per la scrittura (piped[1])

    for (j = 1; j <= 10; j++) // scrive sulla pipe i numeri da 1 a 10
    {
      write(piped[1], &j, sizeof(int)); // scrive sulla pipe il numero j
      sleep(1);                         // aspetta 1 secondo tra una scrittura e l'altra
    }
    exit(0);
  }
}
