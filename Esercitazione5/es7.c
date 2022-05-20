#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int pid, j, c;
  int piped[2];

  /*
     Apre la pipe creando due file descriptor,
     uno per la lettura e l'altro per la scrittura
     (vengono memorizzati nell'array piped[]) 
  */

  if (pipe(piped) < 0)
    exit(1);

  if ((pid = fork()) < 0)
    exit(2);
  else if (pid == 0)  /* figlio, che ha una copia di piped[] */
    {
      close(piped[1]); /* nel figlio il fd per la scrittura non serve */
      for (j = 1; j <= 10; j++)
        {
          read(piped[0],&c, sizeof (int));
          printf("Figlio: ho letto dalla pipe il numero %d\n", c);
        }
      exit(0);
    }
  else  /* padre */
    {
      close(piped[0]); /* nel padre il fd per la lettura non serve */
      for (j = 1; j <= 10; j++) 
        {
          write(piped[1], &j, sizeof (int));
          sleep(1);
        }
      exit(0);
    }
}
