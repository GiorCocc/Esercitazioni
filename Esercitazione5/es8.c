#include <stdio.h>
#include <stdlib.h> /* serve per rand() e srand() */
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int pdchf[2], pdfch1[2], pdfch2[2]; // pdchf[0] e pdchf[1] sono i file descriptor per la pipe che connette il padre al figlio, pdfch1[0] e pdfch1[1] sono i file descriptor per la pipe che connette il figlio al padre, pdfch2[0] e pdfch2[1] sono i file descriptor per la pipe che connette il figlio al figlio
  int pid1, pid2;                     // pid dei figli

  struct msg // struttura che contiene i dati da inviare tra i processi
  {
    int mpid; // pid del processo
    int n;    // numero da scrivere sulla pipe
  } m1, m2;   // m1 e m2 sono i messaggi da inviare tra i processi

  int n1, n2; // numeri da scrivere sulla pipe

  // controllo degli argomenti di intestazione
  if (argc != 3)
  {
    printf("Uso: %s <primo numero> <secondo numero>\n", argv[0]);
    exit(-1);
  }

  /* Apre la pipe per la comunicazione da figli a padre */
  if (pipe(pdchf) < 0)
    exit(1);

  /* Apre la pipe per la comunicazione da padre a figlio 1*/
  if (pipe(pdfch1) < 0)
    exit(1);

  /* Apre la pipe per la comunicazione da padre a figlio 2*/
  if (pipe(pdfch2) < 0)
    exit(1);

  if ((pid1 = fork()) < 0) // crea un processo figlio
    exit(2);
  else if (pid1 == 0) /* figlio 1 */
  {
    close(pdchf[0]);  // sulla pipe chf il figlio 1 deve solo scrivere, quindi chiudo il fd per la lettura
    close(pdfch1[1]); // sulla pipe fch1 il figlio 1 deve solo leggere, quindi chiudo il fd per la scrittura
    close(pdfch2[0]); // sulla pipe fch2 il figlio 1 non deve ne' leggere...
    close(pdfch2[1]); // ...ne' scrivere, quindi chiudo entrambi i fd per la scrittura e lettura

    m1.mpid = getpid();   // prende il pid del processo
    m1.n = atoi(argv[1]); // prende il numero da scrivere sulla pipe

    /* ora comunica al padre il proprio messaggio */
    write(pdchf[1], &m1, sizeof(m1));                      // scrive sul fd per la scrittura della pipe il messaggio m1
    printf("figlio 1: ho scritto al padre %d\n", m1.n);    // stampa il numero scritto sulla pipe
    read(pdfch1[0], &m1, sizeof(m1));                      // legge il messaggio del padre
    printf("figlio 1: il padre mi ha scritto %d\n", m1.n); // stampa il numero letto dalla pipe
    exit(0);
  }
  else /* padre */
  {
    if ((pid2 = fork()) < 0) // crea un processo figlio
      exit(2);
    else if (pid2 == 0) /* figlio 2 */
    {
      close(pdchf[0]);  // sulla pipe chf il figlio 2 deve solo scrivere, quindi chiudo il fd per la lettura
      close(pdfch2[1]); // sulla pipe fch2 il figlio 2 deve solo leggere, quindi chiudo il fd per la scrittura
      close(pdfch1[0]); // sulla pipe fch1 il figlio 2 non deve ne' leggere...
      close(pdfch1[1]); //...ne' scrivere, quindi chiudo entrambi i fd per la scrittura e lettura

      m2.mpid = getpid();   // prende il pid del processo
      m2.n = atoi(argv[2]); // prende il numero da scrivere sulla pipe

      /* ora comunica al padre il proprio messaggio */
      write(pdchf[1], &m2, sizeof(m2));                      // scrive sul fd per la scrittura della pipe il messaggio m2
      printf("figlio 2: ho scritto al padre %d\n", m2.n);    // stampa il numero scritto sulla pipe
      read(pdfch2[0], &m2, sizeof(m2));                      /* legge il messaggio del padre */
      printf("figlio 2: il padre mi ha scritto %d\n", m2.n); // stampa il numero letto dalla pipe
      exit(0);
    }
    else /* padre */
    {
      close(pdchf[1]);  // sulla pipe chf il padre deve solo leggere, quindi chiudo il fd per la scrittura
      close(pdfch1[0]); // sulla pipe fch1 il padre deve solo scrivere, quindi chiudo il fd per la lettura
      close(pdfch2[0]); // sulla pipe fch2 il padre deve solo scrivere, quindi chiudo il fd per la lettura

      read(pdchf[0], &m1, sizeof(m1)); // legge il messaggio del figlio 1
      read(pdchf[0], &m2, sizeof(m2)); // legge il messaggio del figlio 2

      /* ora genera un nuovo seme per la successiva sequenza di chiamate a rand() */
      srand(time(0));
      /* genera 2 numeri casuali compresi tra 1 e nt
         e li assegna rispettivamente a n1 e a n2    */
      n1 = 1 + (int)(m1.n * (rand() / (RAND_MAX + 1.0)));
      n2 = 1 + (int)(m2.n * (rand() / (RAND_MAX + 1.0)));

      if (m1.mpid == pid1)
      {
        m1.n = n1;                         // assegna a m1 il numero generato
        m2.n = n2;                         // assegna a m2 il numero generato
        write(pdfch1[1], &m1, sizeof(m1)); // scrive sul fd per la scrittura della pipe il messaggio m1
        write(pdfch2[1], &m2, sizeof(m2)); // scrive sul fd per la scrittura della pipe il messaggio m2
      }
      else
      {
        m1.n = n2;                         // assegna a m1 il numero generato
        m2.n = n1;                         // assegna a m2 il numero generato
        write(pdfch1[1], &m2, sizeof(m2)); // scrive sul fd per la scrittura della pipe il messaggio m2
        write(pdfch2[1], &m1, sizeof(m1)); // scrive sul fd per la scrittura della pipe il messaggio m1
      }
      sleep(1); // aspetta 1 secondo
      exit(0);
    }
  }
}
