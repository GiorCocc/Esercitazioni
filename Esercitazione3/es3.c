#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define perm 0744
#define N 2048

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
	
  outfile = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, perm);
  
  while ((nread = read(infile, buffer, N)) > 0)
    	write(outfile, buffer, nread);
	
  

  printf("Copia del file %s eseguita con successo in %s\n",argv[1],argv[2]);

  close(infile);
  close(outfile);

  return 0;
}
