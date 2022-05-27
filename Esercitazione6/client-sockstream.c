/**********************************************************************

              Semplice interazione su socket di tipo STREAM


    	                C   L  I  E  N  T


Utilizzo:            client nomeserver portaserver


**********************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>


#define BYTES_NR        8192
#define MSG_NR          64


main(argc,argv)
     int	argc;char *argv[];
{
  int	i,s,sock,rval,rval2;
  struct 	sockaddr_in 	server;
  struct	hostent	*hp,*gethostbyname();
  char	buf[BYTES_NR];



  if(argc != 3)
    {
      fprintf(stderr,"Uso: %s nomeserver portaserver\n\n",argv[0]);
      exit(-1);
    }

  /* Crea una  socket di tipo STREAM per il dominio TCP/IP */

  if((sock= socket(AF_INET,SOCK_STREAM,0)) <0)
    {
      perror("open su stream socket");
      exit(1);
    }


  /* Ottiene l'indirizzo IP del server */
  server.sin_family=	AF_INET;
  hp=	gethostbyname(argv[1]);

  if(hp==0)
    {
      fprintf(stderr,"%s: server sconosciuto",argv[1]);
      exit(2);
    }


  memcpy( (char *)&server.sin_addr, (char *)hp->h_addr ,hp->h_length);

  /* La porta e' sulla linea di comando */
  server.sin_port= htons(atoi(argv[2]));

  /* Tenta di realizzare la connessione */
  printf("Connessione in corso...\n");
  if(connect(sock,(struct sockaddr *)&server,sizeof server) <0)
    {
      perror("connect su stream socket");
      exit(1);
    }

  printf("Connesso.\n");

  rval=BYTES_NR;


  /* Invio/Ricezione di MSG_NR messaggi */

  for(i=0;i<MSG_NR;i++)
    {
      printf("About to write...\n");
      if((rval = write(sock,buf,sizeof buf))<0)
	perror("write su stream socket");

      printf("... w=%d byte written\n",rval);

      s=0 ;
      do
	{ /* Il messaggio puo` essere stato frammentato dal protocollo TCP */
	  if((rval = read(sock,&buf[s],sizeof buf))<0)
	    perror("reading stream message");
    
	  s+= rval;
	  printf("r=%d(%d) byte letti\n",rval,s);   
	}
      while((s!=BYTES_NR)&&rval !=0);
    }
 

  close(sock);


  exit(0);


}



