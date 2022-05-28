#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define BYTES_NR 64 // dimensione del buffer
#define MSG_NR 512	// numero di messaggi

char buf[BYTES_NR];	 // variabile globale per il buffer
char buf2[BYTES_NR]; // variabile globale per il buffer

char msg[MSG_NR][BYTES_NR];	 // array globale di MG_NR messaggi letti di dimensione BYTES_NR
char answ[MSG_NR][BYTES_NR]; // array globale di MG_NR risposte di dimensione BYTES_NR

struct timeval xstime[MSG_NR];
struct timeval xftime[MSG_NR];

int main(int argc, char *argv[])
{
	int i, sock, rval, length;						// indice, socket, valore di ritorno, lunghezza
	unsigned long delay;									// ritardo
	struct sockaddr_in server, client;		// struttura per il server e client
	struct hostent *hp, *gethostbyname(); // struttura per l'host e funzione per ottenere l'host

	if (argc != 3) // controllo degli argomenti di invocazione
	{
		fprintf(stderr, "Usage: %s servername serverport\n", argv[0]);
		exit(1);
	}

	for (i = 0; i < MSG_NR; i++)
	{
		sprintf(&msg[i][0], "%d", i); // stampo il numero del messaggio
	}

	/* Create socket */
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) // controllo creazione socket
	{
		perror("opening stream socket");
		exit(2);
	}

	client.sin_family = AF_INET;				 // tipo di socket
	client.sin_addr.s_addr = INADDR_ANY; // indirizzo IP della socket
	client.sin_port = htons(0);					 // porta della socket

	if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0) // controllo bind
	{
		perror("binding error");
		exit(2);
	}

	length = sizeof(client); // lunghezza della struttura client

	if (getsockname(sock, (struct sockaddr *)&server, &length) < 0) // controllo connessione prendendo il nome del socket
	{
		perror("getting socket name");
		exit(3);
	}
	printf("Socket port #%d\n", ntohs(client.sin_port)); // stampo la porta della socket (0)

	hp = gethostbyname(argv[1]); // ottenimento dell'host dal primo argomento d'invocazione
	if (hp == 0)								 // controllo ottenimento host
	{
		fprintf(stderr, "%s :unknow host", argv[1]);
		exit(4);
	}

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length); // copia dell'indirizzo IP dell'host
	server.sin_family = AF_INET;																			 // tipo di socket
	server.sin_port = htons(atoi(argv[2]));														 // porta della socket (secondo argomento d'invocazione)

	for (i = 0; i < MSG_NR; i++)
	{
		strcpy(buf, msg[i]); // copia il messaggio nell'array globale buf

		gettimeofday(&xstime[i], NULL);																												 // prendo il tempo di inizio
		if (sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&server, sizeof(server)) < 0) // controllo invio messaggio
			perror("sendto problem");

		if ((rval = read(sock, buf2, sizeof(buf2))) < 0) // controllo ricezione messaggio
			perror("reading stream message");

		strcpy(answ[i], buf2); // copia il messaggio contenuto nell'array delle risposte nell'array globale buf2

		gettimeofday(&xftime[i], NULL); // prendo il tempo di fine
	}
	close(sock); // chiudo la socket

	for (i = 0; i < MSG_NR; i++)
	{
		delay = (xftime[i].tv_sec - xstime[i].tv_sec) * 1000000. + (xftime[i].tv_usec - xstime[i].tv_usec); // calcolo il ritardo
		printf("msg %d [%s]: %0.3f ms\n", i, answ[i], delay / 1000.);																				// stampo di messaggio, risposta e ritardo in ms con una precisione di 3 cifre decimali
	}

	exit(0);
}
