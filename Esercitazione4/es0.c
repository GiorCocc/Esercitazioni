#include <unistd.h> // getpid() e getppid()
#include <stdio.h>

// utilizzo della funzione getpid() e getppid()

int main()
{
  printf("Sono il processo con PID=%d e sono figlio del processo con PID=%d\n", getpid(), getppid());
}