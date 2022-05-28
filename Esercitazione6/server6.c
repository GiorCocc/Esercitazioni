#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int sock, length, portno;                                                       // socket, lunghezza, numero di porta
  struct sockaddr_in server, client;                                              // struttura per il server e client
  int pid, s, msgsock, rval, rval2, i;                                            // pid, socket per i messaggi, valore di ritorno1, valore di ritorno2, indice
  struct hostent *hp, *gethostbyname();                                           // struttura per l'host e funzione per ottenere l'host
  int pipecont[2], counter_value, value;                                          // pipe, contatore, valore
  char buffer[256], answer[256];                                                  // array buffer, array risposta
  char *hellomsg = "Immettere un incremento intero per il contatore condiviso\n"; // messaggio di benvenuto

  if (argc != 2) // controllo degli argomenti di invocazione
  {
    printf("Uso: %s <numero_porta>\n", argv[0]);
    exit(1);
  }

  /* Crea la  socket STREAM */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) // controllo creazione socket
  {
    perror("opening stream socket");
    exit(2);
  }

  portno = atoi(argv[1]); // numero di porta da argomento di invocazione

  /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
  server.sin_family = AF_INET;                                   // tipo di socket
  server.sin_addr.s_addr = INADDR_ANY;                           // indirisso IP del server
  server.sin_port = htons(portno);                               // numero di porta
  if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0) // controllo bind
  {
    perror("binding stream socket");
    exit(3);
  }

  length = sizeof(server);                                        // lunghezza della struttura SERVER
  if (getsockname(sock, (struct sockaddr *)&server, &length) < 0) // controllo connessione prendendo il nome del socket
  {
    perror("getting socket name");
    exit(4);
  }

  printf("Socket port #%d\n", ntohs(server.sin_port)); // stampa del numero di porta del socket

  /* Creazione di una pipe per ospitare il messaggio con il valore corrente del contatore */
  if (pipe(pipecont) < 0) // controllo creazione pipe
  {
    perror("creating pipe");
    exit(5);
  }
  // Valore iniziale del contatore
  counter_value = 0;
  write(pipecont[1], &counter_value, sizeof(counter_value)); // scrittura del valore del contatore in pipe

  /* Pronto ad accettare connessioni */

  listen(sock, 2); // metodo di ascolto del socket per la connessione

  do
  {
    /* Attesa di una connessione */

    msgsock = accept(sock, (struct sockaddr *)&client, (socklen_t *)&length); // metodo di accettazione del socket

    if (msgsock == -1) // controllo connessione
      perror("accept");
    else
    {
      printf("Connessione da %s, porta %d\n",
             inet_ntoa(client.sin_addr), ntohs(client.sin_port)); // stampa dell'indirizzo IP e del numero di porta del client

      if ((pid = fork()) == 0) // controllo fork
      {
        write(msgsock, hellomsg, strlen(hellomsg) + 1); // scrittura del messaggio di benvenuto nel socket di comunicazione con il client
        read(msgsock, &buffer, sizeof(buffer));         // lettura del messaggio dal socket di comunicazione con il client

        sscanf(buffer, "%d", &value); // lettura di una stringa formattata come un numero intero

        /* Uso della pipe per recuperare il valore corrente del contatore e per inserire il nuovo valore */
        read(pipecont[0], &counter_value, sizeof(counter_value));  // lettura del valore del contatore in pipe
        counter_value += value;                                    // aggiornamento del valore del contatore con il valore inserito nel client
        write(pipecont[1], &counter_value, sizeof(counter_value)); // scrittura del valore del nuovo contatore in pipe

        sprintf(answer, "Il contatore ora vale %d\n", counter_value); // stampa della stringa di risposta con il valore del contatore aggiornato

        write(msgsock, answer, strlen(answer) + 1); // scrittura della stringa di risposta nel socket di comunicazione con il client
        close(msgsock);                             // chiusura del socket di comunicazione con il client
        exit(0);
      }
      else
      {
        if (pid == -1) /* Errore nella fork */
        {
          perror("Error on fork");
          exit(6);
        }
        else
        {
          /* OK, il padre torna in accept */
          close(msgsock); // chiusura del socket di comunicazione con il client
        }
      }
    }

  } while (1); // ciclo infinito
  exit(0);
}
