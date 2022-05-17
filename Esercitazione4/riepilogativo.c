#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define N 5

int main(int argc, char *argv[])
{
  int i;                                                         // contatore
  int pid;                                                       // pid del processo figlio
  int status;                                                    // variabile per memorizzare lo stato di terminazione del processo
  char *args[N];                                                 // array di puntatori agli argomenti da passare al processo figlio
  char *env[] = {"TERM=vt100", "PATH=/bin:/usr/bin", (char *)0}; // variabili di ambiente da passare al processo figlio
  char com[90] = "/bin/";                                        // cartella in cui sono contenuti i comandi della shell

  // controllo in numero di argomenti passati al programma principale
  if (argc < 3)
  {
    printf("Usage: %s <command> [<arguments>]\n", argv[0]);
    exit(1);
  }

  // copio gli argomenti di invocazione nel array di puntatori args
  for (i = 0; i < N; i++)
    args[i] = argv[i + 1];
  args[i + 1] = (char *)0; // inserisco uno zero in coda all'array di puntatori args

  strcat(com, argv[1]); // concateno la cartella in cui sono contenuti i comandi della shell con il nome del comando da eseguire (argv[1])

  // controllo della creazione del processo figlio
  if ((pid = fork()) < 0)
  {
    perror("fork error");
    exit(-1);
  }
  else if (pid == 0)
  {                                                           /* CODICE ESEGUITO DAL FIGLIO */
    printf("Sono il processo figlio con PID=%d\n", getpid()); // stampo il PID del processo figlio
    int ret = execve(com, args, env);                         // cambio dell'ambiente di esecuzione del comando da eseguire (argv[1]) con argomenti di invocazione (args) e variabili di ambiente (env)

    // controllo dell'esecuzione del cambio di ambiente
    if (ret < 0)
    {
      perror("execve error");
      exit(1);
    }
  }
  else
  {                                                          /* CODICE ESEGUITO DAL PADRE */
    wait(&status);                                           // attendo la terminazione del processo figlio
    printf("Exit di %d con %d\n", pid, WEXITSTATUS(status)); // stampo il codice di terminazione del processo figlio
  }
}