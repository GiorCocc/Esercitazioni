#include <stdio.h> 

int main(int argc, char **argv)
{ 
  int i;

  //stampa il nome del file (primo argomento) e il numero totale di argomenti inseriti
  printf("This program is %s and his number of arguments is %d.\n", argv[0], argc);
  
  //ciclo di stampa per tutti gli altri argomenti inseriti
  for (i = 1; i < argc; i++)
  {
    printf("The argument number %d is %s.\n", i, argv[i]);
  }
  return 0;
}
