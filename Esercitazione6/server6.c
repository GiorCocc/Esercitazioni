#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char* argv[])
{
  int	sock,length,portno;
  struct  sockaddr_in 	server,client;
  int	pid,s,msgsock,rval,rval2,i;
  struct	hostent	*hp,*gethostbyname();
  int pipecont[2],counter_value,value;
  char buffer[256],answer[256];
  char *hellomsg = "Immettere un incremento intero per il contatore condiviso\n";
  
  if (argc !=2) {
    printf("Uso: %s <numero_porta>\n", argv[0]);
    exit(1);
  }

  /* Crea la  socket STREAM */
  sock=	socket(AF_INET,SOCK_STREAM,0);
  if(sock<0)
  {	
    perror("opening stream socket");
    exit(2);
  }
  
  portno = atoi(argv[1]);
  
  /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
  server.sin_family = 	AF_INET;
  server.sin_addr.s_addr= INADDR_ANY;
  server.sin_port = htons(portno);
  if (bind(sock,(struct sockaddr *)&server,sizeof server)<0)
  {
    perror("binding stream socket");
    exit(3);
  }
  
  
  length= sizeof(server);
  if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
  {
    perror("getting socket name");
    exit(4);
  }
  
  printf("Socket port #%d\n",ntohs(server.sin_port));
  
  /* Creazione di una pipe per ospitare il messaggio con il valore corrente del contatore */
  if (pipe(pipecont)<0) {
        perror("creating pipe");
        exit(5);
    }
  // Valore iniziale del contatore
  counter_value = 0;
  write(pipecont[1],&counter_value,sizeof(counter_value));
  
  
  /* Pronto ad accettare connessioni */
  
  listen(sock,2);
  
  do {
    /* Attesa di una connessione */	 
      
    msgsock= accept(sock,(struct sockaddr *)&client,(socklen_t *)&length);
      
    if(msgsock ==-1)
      perror("accept");
    else
    { 
      printf("Connessione da %s, porta %d\n", 
             inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	 

      if((pid = fork())== 0)
      {
        write(msgsock,hellomsg,strlen(hellomsg)+1);
        read(msgsock,&buffer,sizeof(buffer));

        sscanf(buffer,"%d",&value);
        
        /* Uso della pipe per recuperare il valore corrente del contatore e per inserire il nuovo valore */
        read(pipecont[0],&counter_value,sizeof(counter_value));
        counter_value  += value;
        write(pipecont[1],&counter_value,sizeof(counter_value));

        sprintf(answer,"Il contatore ora vale %d\n",counter_value);

        write(msgsock,answer,strlen(answer)+1);
        close(msgsock);
        exit(0);     
      }
      else
      {
        if(pid == -1)	/* Errore nella fork */
        {
          perror("Error on fork");
          exit(6);
        }
        else
	{	
          /* OK, il padre torna in accept */
          close(msgsock);
        }
      }	  
    }
      
  }
  while(1);
  exit(0);
}
