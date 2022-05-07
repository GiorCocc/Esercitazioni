#include <unistd.h>
#include <stdio.h>


int main()
{
 if ((link("test.dat", "test2.dat")) == -1)
 {
  perror("link error");
  return 1;       
 }
 return 0;
}
