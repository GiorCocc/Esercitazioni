#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUF 8192 // massima dimensione per il buffer

int main(int argc, char *argv[])
{
  int client_socket, fd, portno; // socket del client, file descriptor, numero della porta
  int retcode, letti;            // codice di ritorno, byte letti
  struct sockaddr_in server;     // struttura server
  struct hostent *server_addr;   // indirizzo server
  char message[MAXBUF];          // array del messaggio
  char *nomehost, *filename;     // puntatore al nome dellhost e del nome del file

  if (argc != 4) // controllo del numero degli argomenti d'invocazione
  {
    printf("Usage:\n%s nomefile nomehost portno\n", argv[0]);
    exit(1);
  }

  filename = argv[1]; // nome del file dal primo argomento d'invocazione
  nomehost = argv[2]; // nome dell'host dal secondo argomento d'invocazione

  printf("Client (%d): fase di inizializzazione\n", getpid());

  client_socket = socket(AF_INET, SOCK_STREAM, 0); // creazione del socket

  if (client_socket == -1) // controllo dell'esito di creazione del socket
  {
    perror("aprendo la socket del cliente");
    exit(2);
  }

  portno = atoi(argv[3]); // conversione della porta da ASCII a int dal terzo argomento d'invocazione

  server.sin_family = AF_INET;          // impostazione del tipo di indirizzo
  server.sin_port = htons(portno);      // impostazione della porta
  server_addr = gethostbyname(argv[2]); // conversione del nome dell'host in indirizzo

  if (server_addr == NULL) // controllo della conversione del nome dell'host
  {
    fprintf(stderr, "ERRORE: host sconosciuto\n");
    exit(3);
  }

  memcpy((char *)&server.sin_addr, (char *)server_addr->h_addr, server_addr->h_length); // copia dell'indirizzo dell'host nel socket del client

  retcode = connect(client_socket,
                    (struct sockaddr *)&server,
                    sizeof(server)); // connessione al server

  if (retcode == -1) // controllo l'esito della connessione al server
  {
    perror("Errore connect");
    exit(4);
  }

  fd = open(filename, O_RDONLY); // apertura del file in sola lettura

  if (fd == -1) // controllo l'esito dell'apertura del file
  {
    perror("aprendo il file");
    exit(5);
  }
  do
  {
    letti = read(fd, message, MAXBUF); // lettura del file e copia del messaggio nel buffer

    if (letti > 0)                                    // controllo il numero di byte letti
    {                                                 /* solo se la lettura ha avuto buon fine */
      retcode = write(client_socket, message, letti); // invio del messaggio al server
      if (retcode == -1)                              // controllo l'esito dell'invio
      {
        perror("scrivendo il messaggio");
        exit(6);
      }
    }
  } while (letti > 0);  // finchè il numero di byte letti è maggiore di 0
  close(fd);            // chiusura del file
  close(client_socket); // chiusura del socket del client
  exit(0);
}