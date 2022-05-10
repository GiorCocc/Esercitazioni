#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h> //necessario per l'utilizzo delle systemcall dup() e dup2()

#define PERM 0744

//utilizzo della systemcall dup() e dup2()
// int dup(int oldfd);
// int dup2(int oldfd, int newfd);

int main()
{
  int fd, newfd;

  fd = open("file.txt", O_CREAT|O_WRONLY|O_TRUNC, PERM);
  if (fd == -1)
    {
      perror("Errore open");
      exit (1);
    }
  /* ora chiude lo standard output  */
  close(1);

  /* poi duplica fd nel descrittore libero piu' basso (1!) */
  if( (newfd=dup2(fd,1)) <0) {
	  perror("Errore dup");
	  exit(2);
	}
 
 
  /* infine scrive in 1, che ora e' una copia del descrittore del file */
  write(1, "Hello from write\n", 17);
  /* anche printf() scrive sul file */   
  printf("Hello from printf\n");
  
  return 0;       
}
