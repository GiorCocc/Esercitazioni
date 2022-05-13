#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h> //necessario per l'utilizzo delle systemcall dup() e dup2()

#define PERM 0744

// utilizzo della systemcall dup() e dup2()
// int dup(int oldfd);
// int dup2(int oldfd, int newfd);

int main()
{
  int fd, newfd; // file descriptor del file di input e del file di output

  fd = open("file.txt", O_CREAT | O_WRONLY | O_TRUNC, PERM); // apre il file di input
  // controllo di apertura del file di input
  if (fd == -1)
  {
    perror("Errore open");
    exit(1);
  }
  close(1); // chiude lo standard output

  /* poi duplica fd nel descrittore libero piu' basso (1!) */
  if ((newfd = dup2(fd, 1)) < 0)
  {
    perror("Errore dup");
    exit(2);
  }

  /* infine scrive in 1, che ora e' una copia del descrittore del file */
  write(1, "Hello from write\n", 17);
  /* anche printf() scrive sul file */
  printf("Hello from printf\n");

  return 0;
}
