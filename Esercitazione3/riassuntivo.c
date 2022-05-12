#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define N 10
#define REC_LEN 22

// FIXME: inserire i dati letti da file nel vettore come elementi persona

typedef struct persona
{
  char nome[16];
  char eta[3];
} Persona;

int main(int argc, char **argv)
{
  Persona vettore[N];
  int strutture_fd; // file descriptor della struttura
  int iline;        // linea da scegliere
  int i = 0;
  char buffer[REC_LEN];
  long cur_pos;
  int length;

  // controllo l'inserimento della i-esima riga da leggere
  if (argc != 2 || atoi(argv[1]) > N)
  {
    perror("Inserire come argomento anche la i-esima riga da leggere con i<10");
    exit(-1);
  }
  iline = atoi(argv[1]); // i-esima riga da leggere

  // controllo la corretta apertura del file delle strutture
  if ((strutture_fd = open("strutture.txt", O_RDONLY)) < 0)
  {
    perror("Errore nell'apertura del file 'strutture.txt'");
    exit(1);
  }

  printf("This file contains %ld bytes\n", length = lseek(strutture_fd, 0, SEEK_END)); // stampo la dimensione del file

  cur_pos = lseek(strutture_fd, 0, SEEK_SET); // riposiziono all'inizio del file
  // stampo tutto il contenuto del file sullo stdout (console)
  while (cur_pos < length)
  {
    read(strutture_fd, buffer, length / N);     // leggo dal record corrente tutti i caratteri
    write(1, buffer, length / N);               // Visualizza il record corrente (N per stampare i primi N caratteri)
    cur_pos = lseek(strutture_fd, 0, SEEK_CUR); // avanzo alla riga successiva
  }

  cur_pos = lseek(strutture_fd, 0, SEEK_SET);                    // riposiziono all'inizio del file
  cur_pos = lseek(strutture_fd, (length / N) * iline, SEEK_SET); // riposiziono all'i-esima riga
  read(strutture_fd, buffer, length / N);                        // leggo la i-esima riga
  write(1, "Sulla i-esima riga ci sono i seguenti dati: ", 45);
  write(1, buffer, length / N); // stampo la i-esima riga su stdout (console)
}
