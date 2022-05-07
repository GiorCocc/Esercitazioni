#include <stdio.h>

int main()
{ 
  double m, n;

  m = 1234.5678;
  n = 9.999;
  
  m = n;
  printf("m = %f, n = %f\n", m, n);

  return 0;
}

