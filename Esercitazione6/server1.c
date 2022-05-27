#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void error(char *msg) // funzione per gestire errori
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;  // file descriptor per la socket, file descriptor per la socket di connessione, numero porta, dimensione della struttura per la connessione
    char buffer[256] = "";                  // buffer per la ricezione dei messaggi
    struct sockaddr_in serv_addr, cli_addr; // struttura con indirizzo del server, struttura con indirizzo del client
    int n;                                  // variabile per la lettura/scrittura sulla socket

    if (argc < 2) // controllo argomenti
    {
        fprintf(stderr, "ERRORE, nessuna porta specificata\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // creazione socket

    if (sockfd < 0) // controllo della creazione della socket
        error("ERRORE DI APERTURA DELLA SOCKET");

    portno = atoi(argv[1]);                 // conversione della porta da ASCII in numero
    serv_addr.sin_family = AF_INET;         // tipo di indirizzo
    serv_addr.sin_addr.s_addr = INADDR_ANY; // indirizzo IP del server (quello della macchina)
    serv_addr.sin_port = htons(portno);     // numero della porta del server

    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0) // controllo della bind della socket con l'indirizzo del server
        error("ERRORE DI BINDING");

    listen(sockfd, 5);         // metodo di ascolto della socket
    clilen = sizeof(cli_addr); // dimensione della struttura per la connessione
    newsockfd = accept(sockfd,
                       (struct sockaddr *)&cli_addr,
                       (socklen_t *)&clilen); // metodo di accettazione della connessione

    if (newsockfd < 0) // controllo della accept della socket
        error("ERRORE DI ACCEPT");

    n = read(newsockfd, buffer, 255); // lettura dei messaggi dalla socket
    if (n < 0)                        // controllo della lettura dalla socket
        error("ERRORE in lettura dalla socket");
    printf("(SERVER) Ecco il messaggio ricevuto dal client: %s\n", buffer);

    n = write(newsockfd, "MESSAGGIO RICEVUTO, FINE COMUNICAZIONE", 38); // scrittura dei messaggi sulla socket
    if (n < 0)                                                          // controllo della scrittura sulla socket
        error("ERRORE in scrittura sulla socket");

    close(sockfd);    // chiusura della socket
    close(newsockfd); // chiusura della socket di connessione

    return 0;
}
