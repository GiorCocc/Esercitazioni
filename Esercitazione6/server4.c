#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

// TODO: Si realizzi un client che si colleghi al server e riceva e visualizzi data e ora corrente

int main(int argc, char *argv[])
{
  char buf[26];
  time_t ticks;
  int sock, length, msgsock, rval, portno;
  struct sockaddr_in server;
  struct hostent *hp;

  if (argc != 2) // controllo degli argomenti di invocazione
  {
    printf("Usage: %s <port_number>\n", argv[0]);
    exit(1);
  }

  printf("\n*-----------------------------------------------*\n");
  printf("*                                               *\n");
  printf("*         SERVER data e ora correnti            *\n");
  printf("*                                               *\n");
  printf("* ^c per terminare!                             *\n");
  printf("*                                               *\n");
  printf("*-----------------------------------------------*\n\n");

  sock = socket(AF_INET, SOCK_STREAM, 0); /* Crea la  socket STREAM */
  if (sock < 0)                           // controllo della creazione della socket
  {
    perror("opening stream socket");
    exit(2);
  }

  portno = atoi(argv[1]); // conversione del numero di porta da ASCII a numero

  server.sin_family = AF_INET;                                   /* Utilizzo della wildcard per*/
  server.sin_addr.s_addr = INADDR_ANY;                           /* accettare le connessioni ricevute su ogni interfaccia di rete (ovvero qualunque indirizzo IP del nodo del server )*/
  server.sin_port = htons(portno);                               // numero di porta
  if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0) // controllo della bind
  {
    perror("binding stream socket");
    exit(3);
  }

  length = sizeof server;                                         // lunghezza della struttura SERVER
  if (getsockname(sock, (struct sockaddr *)&server, &length) < 0) // controllo della connessione prendendo il nome del socket
  {
    perror("getting socket name");
    exit(4);
  }

  printf("Socket port #%d\n\n", ntohs(server.sin_port)); // stampa del numero di porta del socket

  listen(sock, 2); /* Pronto ad accettare connessioni */

  do
  {
    /* Attesa di una connessione */
    msgsock = accept(sock, (struct sockaddr *)0, (int *)0);

    if (msgsock == -1) // controllo della connessione
      perror("accept");
    else
    {
      ticks = time(NULL);         /*calcola data e ora corrente*/
      strcpy(buf, ctime(&ticks)); /*la converte in stringa e la memorizza in buf[26]*/

      /* Invio dell'informazione */
      if ((rval = write(msgsock, buf, sizeof buf)) < 0) // controllo della scrittura del buffer nel socket del messaggio
        perror("writing on stream socket");
      printf("%d byte scritti\n", rval);
    }
    close(msgsock); // chiusura del socket del messaggio
  } while (1);

  exit(0);
}
