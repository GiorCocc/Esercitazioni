/**********************************************************************

              Semplice interazione su socket di tipo STREAM


    	             S    E   R   V  E   R


Utilizzo:             server [numeroporta]

**********************************************************************/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#define DEFAULTPORT 1520

#define BYTES_NR        8192
#define MSG_NR          64


main(int argc, char  *argv[])
{
  int	sock,length;
  struct 	sockaddr_in 	server;
  int	s,msgsock,rval;
  struct	hostent	*hp,*gethostbyname();
  char	buf[BYTES_NR];
  int myport;


  if(argc == 2)
    myport = atoi(argv[1]);
  else
    myport = DEFAULTPORT;
  

  if((myport < 1024  && myport != 0)|| myport > 65535)
    {
      fprintf(stderr, "Il numero di porta puo essere 0 (per ottenerne una libera)\noppure deve essere compresa tra 1024 e 65535\n");
      exit(-1);
    }


  /* Crea la  socket STREAM */
  sock=	socket(AF_INET,SOCK_STREAM,0);
  if(sock<0)
    {	perror("open su stream socket");
    exit(1);
    }


  server.sin_family = 	AF_INET;
  server.sin_addr.s_addr= INADDR_ANY;
  server.sin_port = htons(myport);


  if (bind(sock,(struct sockaddr *)&server,sizeof server)<0)
    {
      perror("bind su stream socket");
      exit(1);
    }


  /* Chiede conferma dell'indirizzo assegnato alla socket  */
  length= sizeof server;
  if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
    {
      perror("getsockname");
      exit(1);
    }

  printf("Porta (della socket) del server = #%d\n",ntohs(server.sin_port));

  /* Il server e' pronto ad accettare connessioni */

  listen(sock,2);

  do
    {
      /* Attesa di una connessione */	 
	  
	/* L'indirizzo della socket del mittente viene in questo caso ignorato (NULL) */
     

      if((msgsock= accept(sock,(struct sockaddr *)NULL,(int *)NULL)) < 0)
	{
	  perror("accept");
	  exit(2);
	}
      else // Server NON concorrente
	do
	  {
	    s = 0;

	    /* Ricezione del messaggio  */
	    do {

	      /* Il messaggio puo` essere stato frammentato dal protocollo TCP */

	      if((rval = read(msgsock,&buf[s],sizeof buf))<0)
		{
		perror("read su  stream message");
		exit(-3);
		}

	      s+= rval;
	      printf("r=%d(%d) byte letti\n",rval,s);   
	    }
	    while((s!=BYTES_NR)&&rval !=0);

	    if(rval==0)
	      printf("Termine della connessione\n");
	    else 
	      {
		/* Invio della risposta */

		if((rval = write(msgsock,buf,sizeof buf))<0)
		  {
		  perror("writing on stream socket");
		  exit(-4);
		  }

		printf("w=%d byte scritti\n",rval);
	      }

	  } while(rval !=0);
      close (msgsock);
    }
  while(1);

  exit(0);
}



