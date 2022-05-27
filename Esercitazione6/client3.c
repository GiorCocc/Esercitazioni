#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _RICHIESTA_MSG // definizione della struttura di messaggio di richiesta
{
  int req;
} RICHIESTA_MSG;

typedef struct _RISPOSTA_MSG // definizione della struttura di messaggio di risposta
{
  int answ;
} RISPOSTA_MSG;

int main(int argc, char *argv[])
{
  int i, sock, rval, rval2, portno;     // indice del ciclo, socket, valore di ritorno, valore di ritorno 2, numero di porta
  struct sockaddr_in server;            // struttura server
  struct hostent *hp, *gethostbyname(); // host, funzione per ottenere l'indirizzo del server (puntatori a strutture)
  int tsum;                             // somma dei tempi di risposta
  float avg;                            // media dei tempi di risposta
  time_t nsec;                          // tempo in nanosecondi
  unsigned short nmil;                  // tempo in millisecondi
  RICHIESTA_MSG request;                // messaggio di richiesta
  RISPOSTA_MSG answer;                  // messaggio di risposta

  if (argc != 3) // controllo degli argomenti di invocazione
  {
    fprintf(stderr, "Uso: %s hostname portno\n\n", argv[0]);
    exit(-1);
  }

  srand(getpid()); // inizializzazione della funzione di generazione di numeri casuali a partire dal pid del processo

  /* Crea una  socket di tipo STREAM per il dominio TCP/IP */
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) // controllo della creazione della socket
  {
    perror("opening stream socket");
    exit(1);
  }

  /* Ottiene l'indirizzo del server */
  server.sin_family = AF_INET; // tipo di socket
  hp = gethostbyname(argv[1]); // ottieni l'indirizzo del server dagli argomenti di invocazione

  if (hp == 0) // controllo dell'indirizzo del server
  {
    fprintf(stderr, "%s: unknown host", argv[1]);
    exit(2);
  }

  memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length); // copia l'indirizzo del server nella struttura server (memcpy perchè hp->h_addr è un puntatore a char)

  /* La porta e' sulla linea di comando */
  portno = atoi(argv[2]);          // converte l'argomento da ASCII in numero
  server.sin_port = htons(portno); // converte la porta da numero a numero in notazione di rete

  /* Tenta di realizzare la connessione */
  printf("Connecting...\n");
  if (connect(sock, (struct sockaddr *)&server, sizeof server) < 0) // controllo della connessione
  {
    perror("connecting stream socket");
    exit(3);
  }

  printf("Connected.\n");

  request.req = rand() % 100;             // genera un numero casuale da 0 a 99
  write(sock, &request, sizeof(request)); // scrive il messaggio di richiesta sul socket

  read(sock, &answer, sizeof(answer)); // legge la risposta dal socket

  printf("Sent %d - server answer is : %d\n", request.req, answer.answ); // stampa il messaggio di richiesta e la risposta ricevuta dal server

  close(sock); // chiude il socket

  exit(0);
}
