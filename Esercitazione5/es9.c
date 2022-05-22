#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int join(char *com1[], char *com2[])
{
  int status, pid; // status è il valore di ritorno del processo figlio, pid è il pid del figlio
  int piped[2];    // array di 2 elementi, il primo per la pipe di scrittura, il secondo per la pipe di lettura

  switch (fork()) // creo un processo figlio
  {
  case -1: /* errore */
    return 1;
  case 0:  /* figlio */
    break; /* esce dal case */
  default: /* padre: attende il figlio */
    wait(&status);
    return status;
  }

  /* il figlio apre la pipe e poi crea un suo figlio (nipote del primo processo)*/
  if (pipe(piped) < 0)
    return 2;
  if ((pid = fork()) < 0)
    return 3;
  else if (pid == 0)
  {
    close(0);              // ora il fd 0 (stdin) è libero
    dup(piped[0]);         // l'fd 0 (stdin) diventa un duplicato di piped [0]
    close(piped[0]);       // chiudo il fd 0 (stdin)
    close(piped[1]);       // abbiamo liberato i due valori di fd salvati in piped, dato che non li useremo più
    execvp(com2[0], com2); // esegue il programma specificato in com2, utilizzando come input quello proveniente dallo stdin, ora associato all'uscita (lettura) dalla pipe
    return 4;
  }
  else
  {
    close(1);              // ora il fd 1 (stdout) è libero
    dup(piped[1]);         // l'fd 1 (stdout) diventa un duplicato di piped [1]
    close(piped[0]);       // chiudo il fd 0 (stdin)
    close(piped[1]);       // abbiamo liberato i due valori di fd salvati in piped, dato che non li useremo più
    execvp(com1[0], com1); // esegue il programma specificato in com1, inviando l'output allo stdout, ora associato all'ingresso (scrittura) nella pipe
    return 5;
  }
}

int main(int argc, char **argv)
{
  int integi, i, j;            // integi è usato per la join dei due comandi, i è un contatore, j è un contatore
  char *temp1[10], *temp2[10]; // temp1 e temp2 sono array di 10 elementi, che contengono i nomi dei comandi da eseguire

  /*
     si devono fornire nella linea di comando due comandi distinti,
     separati dal carattere ! (non usare | perch� questo viene direttamente
     interpretato dallo Shell come una pipe)
  */

  if (argc > 2)
  {
    for (i = 1; (i < argc) && ((strcmp(argv[i], "!")) != 0); i++) // i contatore parte da 1 perché il primo argomento è il nome del programma
      temp1[i - 1] = argv[i];                                     // copia i nomi dei comandi nell'array temp1
    temp1[i - 1] = (char *)0;                                     // inserisco un valore nullo per terminare l'array
    i++;                                                          // incremento i per saltare il carattere !
    for (j = 1; i < argc; i++, j++)                               // j contatore parte da 1 perché il primo argomento è il nome del programma
      temp2[j - 1] = argv[i];                                     // copia i nomi dei comandi nell'array temp2
    temp2[j - 1] = (char *)0;                                     // inserisco un valore nullo per terminare l'array
  }
  else
  {
    printf("errore nell'inserimento dei comandi"); // se non sono stati forniti due comandi, viene stampato un messaggio di errore
    exit(-2);
  }
  integi = join(temp1, temp2); // esegue la join dei due comandi
  exit(integi);                // esce dal processo con il valore di ritorno della join
}
