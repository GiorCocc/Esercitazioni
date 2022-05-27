#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXBUF 8192

int main(int argc, char *argv[])
{
  int server_socket, connect_socket, portno;   // socket del server, socket del client, numero della porta
  unsigned int client_addr_len;                // lunghezza dell'indirizzo del client
  int retcode, fd;                             // codice di ritorno, file descriptor
  struct sockaddr_in client_addr, server_addr; // struttura per l'indirizzo del client e dell'indirizzo del server
  char line[MAXBUF];                           // linea di caratteri

  if (argc != 3) // controllo dei parametri
  {
    printf("Usage:\n%s port nomefilelocale\n", argv[0]);
    return (0);
  }
  printf("Server: fase di inizializzazione\n");
  server_socket = socket(AF_INET, SOCK_STREAM, 0); // creazione della socket del server
  if (server_socket == -1)                         // controllo dell'esito della creazione della socket
  {
    perror("aprendo il socket del server");
    return (-1);
  }

  portno = atoi(argv[1]);                   // numero della porta dal primo argomento di invocazione
  server_addr.sin_family = AF_INET;         // tipologia di indirizzo
  server_addr.sin_addr.s_addr = INADDR_ANY; // indirizzo
  server_addr.sin_port = htons(portno);     // assegnamento della porta

  retcode = bind(server_socket,
                 (struct sockaddr *)&server_addr,
                 sizeof(server_addr)); // associazione dell'indirizzo del server
  if (retcode == -1)                   // controllo dell'esito dell'associazione
  {
    perror("bind error");
    return (-1);
  }
  listen(server_socket, 1); // metodo di ascolto del server
  printf("Server: attendo connessione\n");
  client_addr_len = sizeof(client_addr); // lunghezza dell'indirizzo del client
  connect_socket = accept(server_socket,
                          (struct sockaddr *)&client_addr,
                          &client_addr_len); // metodo di accettazione del server

  // server non concorrente perché riceve una sola connessione che serve e poi termina
  printf("Server: accettata nuova connessione\nApro file locale %s", argv[2]);

  fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644); // apertura del file locale
  if (fd == -1)                                           // controllo dell'esito dell'apertura
  {
    perror("aprendo il file locale");
    return (-2);
  }
  do
  {
    retcode = read(connect_socket, line, MAXBUF); // lettura del file dal client
    if (retcode != -1)                            // controllo dell'esito della lettura
      write(fd, line, retcode);                   // scrittura del file locale
  } while (retcode > 0);                          // controllo dell'esito della lettura
  close(fd);                                      // chiusura del file locale
  printf("\nFine del messaggio, chiusura della connessione\n");
  close(connect_socket); // chiusura della connessione

  printf("Chiusura dei lavori ... \n");
  close(server_socket); // chiusura della socket del server
  return (0);
}
