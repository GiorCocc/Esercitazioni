#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> // fork() e getpid() e sleep()
#include <stdlib.h>
#include <stdio.h>

int main()
{
  int pid, status; // pid del processo figlio e stato del processo figlio
  int myvar;       // variabile di appoggio

  myvar = 1; // inizializzazione della variabile di appoggio (rimane inalterata nel processo padre)

  // controllo della creazione del processo figlio
  if ((pid = fork()) < 0)
  {
    perror("fork error");
    exit(1);
  }
  else if (pid == 0) // codice eseguito dal figlio
  {
    sleep(2);  // attendo 2 secondi
    myvar = 2; // modifico la variabile di appoggio nello spazio di memoria del figlio
    printf("Figlio: sono il processo %d e sono figlio di %d \n",
           getpid(), getppid());          // stampo i PID del figlio e del padre
    printf("Figlio: myvar=%d \n", myvar); // stampo la variabile di appoggio contenuta nello spazio di memoria del figlio (2)
    exit(0);
  }
  else // codice eseguito dal padre
  {
    wait(&status); // attendo la terminazione del figlio
    printf("Padre: sono il processo %d e sono figlio di %d \n",
           getpid(), getppid());             // stampo il pid del padre e del processo figlio
    printf("Padre: status = %d \n", status); // stampo lo stato del figlio
    printf("Padre: myvar=%d \n", myvar);     // stampo la variabile di appoggio contenuta nello spazio di memoria del padre (1)
  }
}
