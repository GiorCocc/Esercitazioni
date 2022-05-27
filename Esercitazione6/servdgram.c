#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <string.h>

#define BYTES_NR	64
#define	MSG_NR		512


char	buf[BYTES_NR];




int main(int argc,char *argv[])
{
int	sock,length;
struct 	sockaddr_in 	server,client;
int	msgsock,rval,i;
struct	hostent	*hp,*gethostbyname();


if(argc !=2)
  {
    fprintf(stderr,"Usage: %s port\n",argv[0]);
    exit(-1);
  }

/* Create socket */
sock=	socket(AF_INET,SOCK_DGRAM,0);
if(sock<0)
	{
	perror("opening stream socket");
	exit(1);
	}

/* Name socket using wildcards */
server.sin_family = 	AF_INET;
server.sin_addr.s_addr= INADDR_ANY;
server.sin_port = htons(atoi(argv[1]));
if (bind(sock,(struct sockaddr *)&server,sizeof(server))<0)
	{
	perror("binding stream socket");
	exit(1);
	}

/* Find out assigned port and print out */
length= sizeof(server);
if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
	{
	perror("getting socket name");
	exit(1);
	}

printf("Socket port #%d\n",ntohs(server.sin_port));


while(1)
  {
    do
	{
	  bzero(buf,sizeof(buf));
	   if((rval = recvfrom(sock,buf,sizeof(buf), 0, (struct sockaddr *)&client, (socklen_t *)&length ))<0)
	  
			perror("reading stream message");
		i=0;
		if(rval==0)
			printf("Ending connection\n");
		else 
			{
			  printf("Message received: sending back\n");
			  strcat(buf,"*");
			  if(sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&client,sizeof(client))<0)
			    /* (write(msgsock,buf,sizeof(buf))<0) */
			  perror("writing on stream socket");
			}

	} while(rval !=0);

  }
exit(0);
}
