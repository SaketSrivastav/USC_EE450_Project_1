#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<strings.h>
#include<arpa/inet.h>
#include<netdb.h>

#define ERROR	-1
#define BUFFER	1024
#define PORT	22095
#define HOSTNAME "nunki.usc.edu"

char* getIpAddress(){
	struct hostent *hp = gethostbyname(HOSTNAME);
	unsigned int i=0;
	char p[16];

	if (hp == NULL) {
		printf("gethostbyname() failed\n");
	} else {
		printf("1\n");
		while ( hp -> h_addr_list[i] != NULL) {
			printf("%s\n",inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[i])));
			sprintf(p,"%s\n", inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[i])));
		printf("2\n");
			i++;
		}
	}
	return p;
}

int main(int argc, char** argv){
	struct sockaddr_in remote_server,client_addr;

	//Hostent
	struct hostent *hp = gethostbyname(HOSTNAME);

	int sock,getsock_check;
	char input[BUFFER];
	char output[BUFFER];
	int len,addrlen;
	char *ip=NULL;
	socklen_t clientSockLength = sizeof(struct sockaddr_in);

	if(argc < 2){
		printf("USAGE: Client <date/time>\n\n");
		exit(0);
	}	

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		perror("Client Socket:");
		exit(-1);
	}
	
	

	//Initializing server variables
	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(PORT);
	
	//ip=getIpAddress();
if (hp == 0) {
	      fprintf(stderr, "%s: unknown host","nunki.usc.edu");
	      exit(2);
	}
   	bcopy(hp->h_addr_list[0], &remote_server.sin_addr, hp->h_length);

	//remote_server.sin_addr.s_addr = inet_addr(ip);
	//bzero(&remote_server.sin_zero, 8); 

	if((connect(sock, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in))) == ERROR)
	{
		perror("connect");
		exit(-1);
	}

	/*
	client address
	*/
	//Retrieve the locally-bound name of the specified socket and store it in the
	//sockaddr structure

	getsock_check=getsockname(sock,(struct sockaddr *) &client_addr,&clientSockLength);

	//Error checking
	if (getsock_check== -1) {
		perror("getsockname");
		exit(1);
	}

	printf("client IP:%s & port: %d\n\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

	strcpy(input,argv[1]);

	send(sock, input, strlen(input), 0);
	
	len = recv(sock, output, BUFFER, 0);
	output[len] = '\0';
	printf("%s : %s\n\n", argv[1],output);
	
	close(sock);
}

