#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>




#define BYTES_NR	64
#define	MSG_NR		512


char	buf[BYTES_NR];
char	buf2[BYTES_NR];

char	msg[MSG_NR][BYTES_NR];
char	answ[MSG_NR][BYTES_NR];

struct timeval xstime[MSG_NR];
struct timeval xftime[MSG_NR];



int main(int argc, char *argv[])
{
int	i,sock,rval,length;
unsigned long delay;
struct 	sockaddr_in 	server,client;
struct	hostent	*hp,*gethostbyname();

if(argc !=3)
  {
    fprintf(stderr,"Usage: %s servername serverport\n",argv[0]);
    exit(1);
  }



for(i=0;i<MSG_NR;i++)
	{
	sprintf(&msg[i][0],"%d",i);
	}



/* Create socket */
sock=	socket(AF_INET,SOCK_DGRAM,0);
if(sock<0)
	{
	perror("opening stream socket");
	exit(2);
	}


client.sin_family=	AF_INET;
client.sin_addr.s_addr = INADDR_ANY;
client.sin_port = htons(0);
 
if (bind(sock,(struct sockaddr *)&client,sizeof(client)) <0)
	{
	perror("binding error");
	exit(2);
	}

length= sizeof(client);

if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
	{
	perror("getting socket name");
	exit(3);
	}
printf("Socket port #%d\n",ntohs(client.sin_port));

hp = gethostbyname(argv[1]);
if (hp == 0)
	{
	fprintf(stderr,"%s :unknow host",argv[1]);
	exit(4);
	}

bcopy( (char *)hp ->h_addr, (char *)&server.sin_addr,hp ->h_length);
server.sin_family = AF_INET;
server.sin_port = htons(atoi(argv[2]));

for(i=0;i<MSG_NR;i++)
	{
	strcpy(buf,msg[i]);

	gettimeofday(&xstime[i],NULL);
	if(sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server,sizeof(server))<0)
		perror("sendto problem");

	if((rval = read(sock,buf2,sizeof(buf2)))<0)
			perror("reading stream message");

	strcpy(answ[i],buf2);

	gettimeofday(&xftime[i],NULL);
	}
close(sock);

for(i=0;i<MSG_NR;i++)
	{
	delay= (xftime[i].tv_sec-xstime[i].tv_sec)*1000000.+(xftime[i].tv_usec-xstime[i].tv_usec);
	printf("msg %d [%s]: %0.3f ms\n",i,answ[i],delay/1000.);
	}

exit(0);


}
