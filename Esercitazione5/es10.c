#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


#define N 10

int pg[2];
int tabpid[N];
char arg1;

void handler(int signo)
{
  printf("Sono il processo %d e ho ricevuto il segnale %d\n",getpid(),signo);
}

void body_proc(int id)
{
}

int main (int argc, char* argv[])
{
  int i;

  if (pipe(pg)<0)
    {
      perror("pipe error");
      exit(-1);
    }
  arg1= argv[1][0]; /* primo carattere del secondo argomento */
  for (i=0;i<N;i++)
    {
      if ((tabpid[i]=fork())<0)
        {
          perror("fork error");
          exit(-1);
        } 
      else
        if (tabpid[i]==0) body_proc(i);
    }
  printf("Sono il padre e scrivo sulla pipe la tabella dei pid\n");
  write(pg[1],tabpid,sizeof tabpid);

  return 0;
}
