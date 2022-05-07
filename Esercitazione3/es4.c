#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define REC_LEN 19

int main() {

  int f;
  long int length, cur_pos;
  char buf[REC_LEN];
  
  f = open("test.dat", O_RDWR);
  printf("This file contains %ld bytes\n", length = lseek(f, 0, SEEK_END));
  /* Si riposiziona all'inizio */
  cur_pos = lseek(f, 0, SEEK_SET); 
  /* Il file contiene dei record di REC_LEN Byte; ne viene letto uno ogni 6 */
  while (cur_pos < length) {
    read(f, buf, REC_LEN);
    write(1, buf, REC_LEN); // Visualizza il record corrente
    /* Si sposta avanti di 5 record */
    cur_pos = lseek(f, 5*REC_LEN, SEEK_CUR);
  }
  close(f);
  
  return 0;
}
