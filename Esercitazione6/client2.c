#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUF 8192

int main(int argc, char *argv[])
{
  int client_socket, fd, portno; // socket del client, file descriptor, numero della porta
  int retcode, letti;            // codice di ritorno, numero di byte letti
  struct sockaddr_in server;     // struttura per il server
  struct hostent *server_addr;   // struttura per l'indirizzo del server
  char message[MAXBUF];          // messaggio da inviare al server
  char *nomehost, *filename;     // nome del server e nome del file locale

  if (argc != 4) // controllo dei parametri
  {
    printf("Usage:\n%s <nomefile> <nomehost> <portno>\n", argv[0]);
    exit(1);
  }
  filename = argv[1]; // nome del file locale
  nomehost = argv[2]; // nome del server
  printf("Client (%d): fase di inizializzazione\n", getpid());
  client_socket = socket(AF_INET, SOCK_STREAM, 0); // creazione della socket del client
  if (client_socket == -1)                         // controllo dell'esito della creazione della socket
  {
    perror("aprendo la socket del cliente");
    exit(2);
  }

  portno = atoi(argv[3]);          // numero della porta dal terzo argomento di invocazione
  server.sin_family = AF_INET;     // tipologia di indirizzo
  server.sin_port = htons(portno); // assegnamento della porta

  server_addr = gethostbyname(argv[1]); // indirizzo del server
  if (server_addr == NULL)              // controllo dell'esito dell'indirizzo del server
  {
    fprintf(stderr, "ERRORE: host sconosciuto\n");
    exit(3);
  }

  memcpy((char *)&server.sin_addr, (char *)server_addr->h_addr, server_addr->h_length); // assegnamento dell'indirizzo del server alla struttura server (memcpy perchè h_addr è un puntatore)

  retcode = connect(client_socket,
                    (struct sockaddr *)&server_addr,
                    sizeof(server_addr)); // connessione al server
  if (retcode == -1)                      // controllo dell'esito della connessione
  {
    perror("Errore connect");
    exit(4);
  }

  fd = open(filename, O_RDONLY); // apertura del file locale
  if (fd == -1)                  // controllo dell'esito dell'apertura del file
  {
    perror("aprendo il file");
    exit(5);
  }
  do
  {
    letti = read(fd, message, MAXBUF); // lettura del file locale
    if (letti > 0)
    {                                                 /* solo se la lettura ha avuto buon fine */
      retcode = write(client_socket, message, letti); // scrittura del messaggio sul socket del server
      if (retcode == -1)                              // controllo dell'esito della scrittura sul socket del server
      {
        perror("scrivendo il messaggio");
        exit(6);
      }
    }
  } while (letti > 0);
  close(fd);            // chiusura del file locale
  close(client_socket); // chiusura della socket del client
  exit(0);
}
