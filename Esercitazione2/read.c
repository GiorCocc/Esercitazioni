#include<stdio.h>
#include<unistd.h>
#include<ctype.h>
#define N 256

int main(){
  char buffer[N];
  int nread;
  char carattere;
  
  nread=read(0,buffer,N);
  
  for(int i=0;i<nread;i++){
    if(isupper(buffer[i])){
       printf("Il carattere e\' %c\n", buffer[i]);
       carattere=buffer[i];
       buffer[i]=carattere+32;
    }
  }
  
  if (nread==-1)
    perror("read error");
  if ((write(1,buffer,nread))!=nread)
    perror("write error");
  
  return 0;
}
