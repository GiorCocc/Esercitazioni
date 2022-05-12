#include <sys/types.h> //necessaria per la systemcall lseek()
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> //necessaria per la systemcall lseek()
#include <stdio.h>
#include <stdlib.h>

#define REC_LEN 19
#define N 4

// utilizzo della systemcall lseek()
// off_t lseek(int fildes, off_t offset, int whence);

int main()
{

  int f; // intero con il descrittore del file
  long int length, cur_pos;
  char buf[REC_LEN];

  // controllo che il file sia stato aperto correttamente
  if ((f = open("test.dat", O_RDWR)) < 0)
  {
    perror("Apertura di test.dat impossibile");
    exit(1);
  }
  printf("This file contains %ld bytes\n", length = lseek(f, 0, SEEK_END));

  /* Si riposiziona all'inizio */
  cur_pos = lseek(f, 0, SEEK_SET);

  /* Il file contiene dei record di REC_LEN Byte; vengono stampati i primi N byte di ogni record*/
  while (cur_pos < length)
  {
    // printf("\ncur_pos: %d\n",cur_pos);
    read(f, buf, REC_LEN); // leggo dal record corrente tutti i caratteri
    write(1, buf, N);      // Visualizza il record corrente (N per stampare i primi N caratteri)
    printf("\n");
    /* Si sposta avanti di 1 record */
    cur_pos = lseek(f, 0, SEEK_CUR);
  }
  close(f);

  return 0;
}
