#include <stdio.h>
#include <stdlib.h>

int main()
{
  char *path;

  /* ottieni e mostra il valore della variabile d'ambiente INCLUDE */
  path = getenv("INCLUDE");
  if (path != NULL) 
  {
    printf("INCLUDE=%s\n", path);
  }

  /* imposta un nuovo valore per INCLUDE */
  if (putenv("INCLUDE=/usr/local/src/include") != 0)  
  {
    printf("putenv() failed setting INCLUDE\n");
    return -1;
  }

  /* ottieni e mostra il valore della variabile d'ambiente INCLUDE */
  path = getenv("INCLUDE");
  if (path != NULL) 
  {
    printf("INCLUDE=%s\n", path);
  }
    
  return 0;
}