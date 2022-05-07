#include <stdio.h> 

int main(int argc, char **argv, char **envp)
{ 
  char *p;

  //ciclo di stampa per tutte le variabili ambiente di sistema
  while (p = *envp)
  {
    printf("%s.\n", p);
    envp += 1;
  }

  return 0;
}

