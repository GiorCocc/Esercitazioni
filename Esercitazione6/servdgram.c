#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <string.h>

#define BYTES_NR 64 // dimensione del buffer
#define MSG_NR 512	// numero di messaggi

char buf[BYTES_NR]; // variabile globale per il buffer

int main(int argc, char *argv[])
{
	int sock, length;											// socket e lunghezza
	struct sockaddr_in server, client;		// struttura per il server e client
	int msgsock, rval, i;									// socket per i messaggi, valore di ritorno, indice
	struct hostent *hp, *gethostbyname(); // struttura per l'host e funzione per ottenere l'host

	if (argc != 2) // controllo argomenti di invocazione
	{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(-1);
	}

	/* Create socket */
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) // controllo creazione socket
	{
		perror("opening stream socket");
		exit(1);
	}

	/* Name socket using wildcards */
	server.sin_family = AF_INET;																		// tipo di socket
	server.sin_addr.s_addr = INADDR_ANY;														// indirizzo IP della socket
	server.sin_port = htons(atoi(argv[1]));													// porta della socket
	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) // controllo bind
	{
		perror("binding stream socket");
		exit(1);
	}

	/* Find out assigned port and print out */
	length = sizeof(server);																				// lunghezza della struttura server
	if (getsockname(sock, (struct sockaddr *)&server, &length) < 0) // controllo del prelievo del nome del socket
	{
		perror("getting socket name");
		exit(1);
	}

	printf("Socket port #%d\n", ntohs(server.sin_port)); // stampa porta socket

	while (1) // ciclo infinito
	{
		do
		{
			bzero(buf, sizeof(buf));																																								// azzero il buffer
			if ((rval = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, (socklen_t *)&length)) < 0) // controllo la lettura del contenuto della socket

				perror("reading stream message");
			i = 0;				 // azzero l'indice
			if (rval == 0) // controllo ricezione messaggio
				printf("Ending connection\n");
			else
			{
				printf("Message received: sending back\n");
				strcat(buf, "*");																																			 // aggiungo il carattere di fine stringa
				if (sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, sizeof(client)) < 0) // controllo invio messaggio a client
					/* (write(msgsock,buf,sizeof(buf))<0) */
					perror("writing on stream socket");
			}

		} while (rval != 0); // controllo fine connessione
	}
	exit(0);
}
