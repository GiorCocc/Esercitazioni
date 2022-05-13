#include <unistd.h> // necessario per l'utilizzo delle systemcall link() e unlink()
#include <stdio.h>

// utilizzo dela systemcall unlink()
// int unlink(char *alias_name);

int main()
{
  if ((unlink("test2.dat")) == -1) // unlink() cancella il link tra i due file
  {
    perror("link error");
    return 1;
  }
  return 0;
}
