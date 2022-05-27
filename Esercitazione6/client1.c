#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

void error(char *msg) // funzione per gestire errori
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;        // file descriptor per la socket, numero porta, variabile per la lettura/scrittura sulla socket
    struct sockaddr_in serv_addr; // struttura con indirizzo del server
    struct hostent *server;       // struttura con informazioni sul server

    char send_buffer[256];     // buffer per la scrittura dei messaggi
    char rec_buffer[256] = ""; // buffer per la lettura dei messaggi

    if (argc < 3) // controllo argomenti
    {
        fprintf(stderr, "uso %s nomehost porta\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[2]);                   // conversione della porta da ASCII in numero
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // creazione socket
    if (sockfd < 0)                           // controllo della creazione della socket
        error("ERRORE in apertura");
    server = gethostbyname(argv[1]); // recupero informazioni sul server dal nome passato come argomento di invocazione
    if (server == NULL)              // controllo dell'esistenza del server
    {
        fprintf(stderr, "ERRORE: host sconosciuto\n");
        exit(2);
    }

    serv_addr.sin_family = AF_INET;                                                // tipo di indirizzo
    memcpy((char *)&serv_addr.sin_addr, (char *)server->h_addr, server->h_length); // copia dell'indirizzo IP del server

    serv_addr.sin_port = htons(portno);                                        // numero della porta del server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // controllo della connessione al server
        error("ERRORE di connessione");
    printf("(CLIENT) Scrivere un messaggio: ");

    fgets(send_buffer, 255, stdin);                      // lettura del messaggio da inviare
    n = write(sockfd, send_buffer, strlen(send_buffer)); // scrittura del messaggio sulla socket
    if (n < 0)                                           // controllo della scrittura del messaggio
    {
        error("ERRORE in scrittura sulla socket");
        exit(3);
    }

    n = read(sockfd, rec_buffer, 255); // lettura del messaggio ricevuto dal server
    if (n < 0)                         // controllo della lettura del messaggio
    {
        error("ERRORE in lettura sulla socket");
        exit(4);
    }
    printf("(CLIENT) Ecco il messaggio ricevuto dal server: %s\n", rec_buffer);

    close(sockfd); // chiusura della socket

    return 0;
}
