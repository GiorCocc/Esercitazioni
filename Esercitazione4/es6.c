#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#define N 8

int main()
{
  int status, i; // variabile per memorizzare lo stato di terminazione del processo
  pid_t pid;     // pid del processo figlio
  int pids[N];   // array di pid dei figli

  /* IL PADRE CREA N PROCESSI FIGLI */
  for (i = 0; i < N; i++)
  {
    if ((pid = fork()) == 0) // controllo se sono il figlio
    {
      sleep(1);                                                               // aspetto 1 secondo
      printf("E' nato %d, figlio numero %d di %d\n", getpid(), i, getppid()); // stampo il pid del figlio, il numero del figlio e il pid del padre
      exit(10 + i);                                                           // esco dal processo figlio
    }
    else
    {
      pids[i] = pid; // salvo il pid del figlio nel vettore
    }
  }
  /* IL PADRE ATTENDE I FIGLI */
  i = 0;                                           // inizializzo i contatore dei figli
  while ((pid = waitpid(pids[i], &status, 0)) > 0) // attendo la termninazione dell'i-esimo figlio
  {
    if (pid == pids[i])      // controllo se è stato terminato l'i-esimo figlio in base all'ordine di arrivo nell'array
      if (WIFEXITED(status)) /* ritorna 1 se il figlio è terminato correttamente */
      {
        printf("Il figlio %d è terminato correttamente con exit status=%d\n", pid, WEXITSTATUS(status));
        i++; // aggiorno l'indice dell'array per passare al figlio successivo
      }
      else
        printf("Il figlio %d non è terminato correttamente\n", pid);
  }
  exit(0);
}
