#include <sys/stat.h> //necessario per la systemcall fstat()
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> //necessario per la systemcall fstat()

// utilizzo della systemcall fstat()
//  int fstat(int filedes, struct stat *buf);

// TODO: controllare che la richiesta della consegna sia stata rispettata (non capisco la consegna appieno)

int main(int argc, char **argv)
{
  struct stat statbuf; // struttura per la systemcall fstat()
  int file;            // file descriptor del file di input

  // controllo il numero di argomenti
  if (argc != 2)
  {
    perror("Inserire il percorso del file come secondo argomento");
    exit(0);
  }

  /* apre il file passato come argomento */
  file = open(argv[1], O_RDONLY);
  // controllo di apertura del file di input
  if (file == -1)
  {
    perror("file opening error");
    exit(-1);
  }

  fstat(file, &statbuf); // ottiene informazioni sul file
  close(file);           // chiude il file di input

  /* mostra le informazioni ottenute */
  if (statbuf.st_mode & S_IFCHR)
    printf("Handle refers to a device.\n");
  if (statbuf.st_mode & S_IFREG)
    printf("Handle refers to an ordinary file.\n");
  if (statbuf.st_mode & S_IREAD)
    printf("User has read permission on file.\n");
  if (statbuf.st_mode & S_IWRITE)
    printf("User has write permission on file.\n");

  printf("Size of file in bytes: %ld\n", statbuf.st_size);
  printf("Time file last opened: %s\n", ctime(&statbuf.st_ctime));

  return 0;
}
