#include <stdio.h>
#include <stdlib.h>  /* serve per rand() e srand() */
#include <time.h> 
#include <unistd.h>

int main(int argc , char* argv[])
{
  int pdchf[2], pdfch1[2], pdfch2[2];
  int pid1, pid2;

  struct msg {
    int mpid;
    int n;
  } m1, m2;

  int n1, n2;


  /* Apre la pipe per la comunicazione da figli a padre */
  if (pipe(pdchf) < 0)
    exit(1);

  /* Apre la pipe per la comunicazione da padre a figlio 1*/
  if (pipe(pdfch1) < 0)
    exit(1);

  /* Apre la pipe per la comunicazione da padre a figlio 2*/
  if (pipe(pdfch2) < 0)
    exit(1);

  if ((pid1 = fork()) < 0)
    exit(2);
  else if (pid1 == 0)  /* figlio 1 */
    {
      close(pdchf[0]);   /* sulla pipe chf il figlio 1 deve solo scrivere */
      close(pdfch1[1]);  /* sulla pipe fch1 il figlio 1 deve solo leggere */
      close(pdfch2[0]);  /* sulla pipe fch2 il figlio 1 non deve ne' leggere */ 
      close(pdfch2[1]);  /* ne' scrivere */
      m1.mpid = getpid();
      m1.n = atoi(argv[1]);  
      /* ora comunica al padre il proprio messaggio */     
      write(pdchf[1], &m1, sizeof(m1));  
      printf("figlio 1: ho scritto al padre %d\n", m1.n);
      read(pdfch1[0], &m1, sizeof(m1));  /* legge il messaggio del padre */
      printf("figlio 1: il padre mi ha scritto %d\n", m1.n);
      exit(0);
    }
  else  /* padre */
    {
      if ((pid2 = fork()) < 0)
        exit(2);
      else if (pid2 == 0)  /* figlio 2 */
        {
          close(pdchf[0]);   /* sulla pipe chf il figlio 2 deve solo scrivere */
          close(pdfch2[1]);  /* sulla pipe fch2 il figlio 2 deve solo leggere */
          close(pdfch1[0]);  /* sulla pipe fch1 il figlio 2 non deve ne' leggere */ 
          close(pdfch1[1]);  /* ne' scrivere */
          m2.mpid = getpid();
          m2.n = atoi(argv[2]);
          /* ora comunica al padre il proprio messaggio */       
          write(pdchf[1], &m2, sizeof(m2));  
          printf("figlio 2: ho scritto al padre %d\n", m2.n);
          read(pdfch2[0], &m2, sizeof(m2));  /* legge il messaggio del padre */
          printf("figlio 2: il padre mi ha scritto %d\n", m2.n);
          exit(0);
        }
      else  /* padre */
        {
          close(pdchf[1]);   /* sulla pipe chf il padre deve solo leggere */
          close(pdfch1[0]);  /* sulla pipe fch1 il padre deve solo scrivere */
          close(pdfch2[0]);  /* sulla pipe fch2 il padre deve solo scrivere */ 

          read(pdchf[0], &m1, sizeof(m1));
          read(pdchf[0], &m2, sizeof(m2));

          /* ora genera un nuovo seme per la successiva sequenza di chiamate a rand() */ 
          srand(time(0)); 
          /* genera 2 numeri casuali compresi tra 1 e nt 
             e li assegna rispettivamente a n1 e a n2    */   
          n1 = 1+(int) (m1.n * (rand()/(RAND_MAX+1.0)));
          n2 = 1+(int) (m2.n * (rand()/(RAND_MAX+1.0)));

          if (m1.mpid == pid1)
            { 
              m1.n = n1;
              m2.n = n2; 
              write(pdfch1[1], &m1, sizeof(m1));
              write(pdfch2[1], &m2, sizeof(m2));
            }
          else
            {
              m1.n = n2;
              m2.n = n1;
              write(pdfch1[1], &m2, sizeof(m2));
              write(pdfch2[1], &m1, sizeof(m1));
            }
          sleep(1);
          exit(0);
        }
    }
}
