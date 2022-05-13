#include <unistd.h> // necessario per la system call write()
#include <string.h>
#include <stdio.h>

// utilizzo della systemcall write():
// ssize_t write(int fd, const void *buf, size_t count);

int main()
{
  char s[100]; // buffer
  size_t sl;   // dimensione della stringa

  strcpy(s, "This data will go to the standard output\n"); // inserie la stringa nel buffer
  sl = strlen(s);                                          // salvataggio della imensione della stringa

  // controllo della system call write():
  if ((write(1, s, sl)) == -1)
    perror("write error"); /* visualizza la stringa e un messaggio di errore */

  return 0;
}
