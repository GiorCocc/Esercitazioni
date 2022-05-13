#include <unistd.h> // contiene la systemcall read()
#include <stdio.h>
#include <ctype.h> // include isupper() e tolower()

#define N 256 // dimensione massima di elementi leggibili

// utilizzo della systemcall read()
// ssize_t read(int fd, void *buf, size_t count);

int main()
{
  char buffer[N]; // buffer
  int nread;      // numero di caratteri letti

  nread = read(0, buffer, N); // leggo dalla standard input (console)

  // impossibile leggere dal buffer
  if (nread == -1)
    perror("read error");

  // lettura del buffer e conversione da MAIUSCOLO a minuscolo
  for (int i = 0; i < nread; i++)
  {
    if (isupper(buffer[i])) // controllo se il carattere è maiuscolo
    {
      buffer[i] = tolower(buffer[i]); // converto in minuscolo e salvo nel buffer
    }
  }

  // impossibile scrivere nel buffer
  if ((write(1, buffer, nread)) != nread)
    perror("write error");

  return 0;
}
