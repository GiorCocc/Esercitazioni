#include <unistd.h> //necessario per l'utilizzo delle systemcall link() e unlink()
#include <stdio.h>

// utilizzo dela systemcall link()

int main()
{
  if ((link("test.dat", "test2.dat")) == -1) // link() crea un link simbolico tra i due file
  {
    perror("link error"); // segnala l'errore
    return 1;
  }
  return 0;
}
