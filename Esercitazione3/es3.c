#include <stdio.h>
#include <sys/types.h>  //necessario per la system call open()
#include <sys/stat.h> //necessario per la system call open()
#include <unistd.h> //necessario per la system call close()
#include <fcntl.h>  //necessario per la system call open()
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define perm 0744
#define N 2048

//utilizzo della system call open() e close()
//int open(const char *pathname, int flags);
//int open(const char *pathname, int flags, mode_t mode);
//int close(int fd);

int main(int argc, char **argv)
{
  int infile, outfile, nread;
  char buffer[N];


  if(argc != 3) {
	fprintf(stderr,"Uso: %s nomefilesorgente nomefiledestinazione\n",argv[0]);
	exit(1);
	}

  /* Aggiungere i controlli di errore nell'invocazione delle seguenti system call */ 

  infile = open(argv[1], O_RDONLY);

  if(infile<0)
    perror("Errore nell'apertura!");
	
  outfile = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, perm);
  
  if(nread>=0)
    while ((nread = read(infile, buffer, N)) > 0)
    	write(outfile, buffer, nread);
  else perror("Errore sul valore di nread");
	
  

  printf("Copia del file %s eseguita con successo in %s\n",argv[1],argv[2]);

  close(infile);
  close(outfile);

  return 0;
}
