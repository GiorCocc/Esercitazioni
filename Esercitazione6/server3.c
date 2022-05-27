#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
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
  int sock, length, portno;             // socket, lunghezza, numero di porta
  struct sockaddr_in server, client;    // server e client
  int pid, s, msgsock, rval, rval2, i;  // pid, socket, socket di messaggio, valore di ritorno, valore di ritorno 2, indice del ciclo
  struct hostent *hp, *gethostbyname(); // host, funzione per ottenere l'indirizzo del server (puntatori a strutture)
  RICHIESTA_MSG request;                // messaggio di richiesta
  RISPOSTA_MSG answer;                  // messaggio di risposta

  if (argc != 2) // controllo degli argomenti di invocazione
  {
    printf("Usage: %s <port_number>\n", argv[0]);
    exit(1);
  }

  /* Crea la  socket STREAM */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) // controllo della creazione della socket
  {
    perror("opening stream socket");
    exit(2);
  }

  portno = atoi(argv[1]); // conversione del numero di porta da ASCII a numero

  /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
  server.sin_family = AF_INET;         // tipo di socket
  server.sin_addr.s_addr = INADDR_ANY; // wildcard
  server.sin_port = htons(portno);     // numero di porta

  if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0) // controllo della connessione
  {
    perror("binding stream socket");
    exit(3);
  }

  length = sizeof(server);                                        // lunghezza della struttura SERVER
  if (getsockname(sock, (struct sockaddr *)&server, &length) < 0) // controllo della connessione prendendo il nome del socket
  {
    perror("getting socket name");
    exit(4);
  }

  printf("Socket port #%d\n", ntohs(server.sin_port));

  /* Pronto ad accettare connessioni */

  listen(sock, 2); // attende eventuali connessioni

  do
  {
    /* Attesa di una connessione */

    msgsock = accept(sock, (struct sockaddr *)&client, (socklen_t *)&length); // accetta connessioni

    if (msgsock == -1) // controllo dell'accettazione della connessione
      perror("accept");
    else
    {
      printf("Connection from %s, port %d\n",
             inet_ntoa(client.sin_addr), ntohs(client.sin_port));

      // Server concorrente: crea un nuovo figlio per il servizio del client che si è connesso
      if ((pid = fork()) == 0)
      {

        read(msgsock, &request, sizeof(request)); // leggi il messaggio di richiesta dal client e lo salva in una struttura

        /* Esecuzione del servizio */
        answer.answ = request.req + (rand() % 100); // calcolo della risposta: somma della richiesta con un numero casuale tra 0 e 100
        write(msgsock, &answer, sizeof(answer));    // scrivi la risposta sul socket di messaggio
        close(msgsock);                             // chiusura della socket per il messaggio
        exit(0);
      }
      else
      {
        if (pid == -1) /* Errore nella fork */
        {
          perror("Error on fork");
          exit(5);
        }
        else
        {
          /* OK, il padre torna in accept */
          close(msgsock);
        }
      }
    }

  } while (1);
  exit(0);
}
