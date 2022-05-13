#include <unistd.h> // fork()
#include <stdlib.h>
#include <stdio.h>

// utilizzo della funzione fork()

int main()
{
    int pid; // pid del processo figlio

    // controllo della creazione del processo figlio
    if ((pid = fork()) < 0)
    {
        perror("Errore fork");
        exit(1);
    }

    /* Entrambi i processi eseguono la printf */
    printf("Processo PID=%d - dalla fork ho ottenuto %d\n", getpid(), pid);
}
