#include<stdio.h>
#include"stdlib.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include"string.h"
#include<time.h>
#include<netdb.h>

#define ERROR -1
#define MAX_CLIENTS 2
#define MAX_DATA 1024
#define HOSTNAME "nunki.usc.edu"

char* getTime();
char* getDate();
char* getIpAddress();

int main(int argc, char** argv){

	//Server Socket and client socket
	int server_sock,client_sock;
	
	//Server address & Client Address
	struct sockaddr_in server,client;
	
	//Hostent
	struct hostent *hp = gethostbyname(HOSTNAME);

	//To store Length of structures
	socklen_t len;
	
	//Number of bytes sent
	int sent;
	
	//Data
	char data[MAX_DATA],*result;
	int data_len,i;

	//Operations
	char *sendData;
	int sendDataLen;

	char *ip=NULL;

	//Request counter
	int reqCount=1;	
	pid_t processid;

	//Create Socket
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server_sock == ERROR){
		perror("Socket: ");
		exit(1);
	}
	
	//Initialize Server connection variables for binding
	server.sin_family = AF_INET;
	server.sin_port = htons(22095);

	if (hp == 0) {
	      fprintf(stderr, "%s: unknown host",HOSTNAME);
	      exit(2);
	}
   	bcopy(hp->h_addr_list[0], &server.sin_addr, hp->h_length);

	//0-padding
//	bzero(&server.sin_zero,8);
	len = sizeof(struct sockaddr_in);
	
	//Bind Server to the port

	if(bind(server_sock,(struct sockaddr *) &server,len) == ERROR){
		perror("bind:");
		exit(1);
	}
	
	//Server Listening at port
	if(listen(server_sock,MAX_CLIENTS) == ERROR){
		perror("Listen:");
		exit(1);
	}


	//Keep listening
	while(reqCount<4){

		//If client request found then accept
		printf("Server has IP: %s and TCP port %d\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));
		
		printf("Server Listening...\n");
		client_sock = accept(server_sock,(struct sockaddr *) &client, &len);

		if(client_sock == ERROR){
			perror("Accept:");
			//close(client_sock);
			//close(server_sock);
			exit(1);
		}else{
			reqCount++;
			printf("Clients Remaining: %d\n\n",reqCount);
			switch(fork()){
			case -1://Error
				perror("Error Spawning Child:");		
				//close(client_sock);
				//close(server_sock);
				exit(1);
			case 0://In_child
				printf("\nIn child\n\n");
				//close(server_sock);//This is child process
				printf("\nParent Process ID:%d\n",getppid());
				printf("Child Process ID:%d\n\n",getpid());
				printf("Client connected has IP:%s & port %d\n\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

				//data_len = 999;
				//while(data_len==999){

				data_len = recv(client_sock,data,MAX_DATA,0);
				if(data_len != 0){
					data[data_len]='\0';
					printf("Query Received:%s\n\n",data);
					if(strncasecmp(data,"time",4) ==  0){
						sendData = getTime();
					}else if(strncasecmp(data,"date",4) == 0){
						sendData=getDate();
					}else{
						sendData = "Available Options[time,date]\n";
					}
					sendDataLen = strlen(sendData);
					sent = send(client_sock, sendData,sendDataLen,0);
					
					//data_len=999;
				}else{
					//data_len=0;
					//printf("No data received from server\n");
				}
				//}//inner_while
				//printf("Client Disconnected\n\n");	

				//Close client socket connection
			//	close(client_sock);
				//exit(0);
			default://In Server
				close(client_sock);
				//printf("Closing Server\n\n");		
				//close(server_sock);
				continue;				
			}
		}
	}//end-while
	printf("Closing Server\n\n");		
	close(server_sock);
	return 0;
}

char* getDate(){
	
	time_t timer;
	struct tm *tblock;           // Define a structure for time block
	char result[MAX_DATA];

	//Because struct tm_year calculates year since 1900
	int year=1900;
	char *month;

	// Get time of day
	timer = time(NULL);

	// Converts date/time to a structure
	tblock = localtime(&timer);
	
	year += tblock->tm_year;
	//month = getMonthName(tblock->tm_mon);
	month = tblock->tm_mon + 1;
	sprintf(result,"%d::%d::%d",year,month,tblock->tm_mday);
	printf("Server Date:%s\n",result);
	return result;
}

char* getTime(){
	
	time_t timer;

	struct tm *tblock;           // Define a structure for time block
	
	char result[MAX_DATA];
	// Get time of day
	timer = time(NULL);

	// Converts date/time to a structure
	tblock = localtime(&timer);
	
	sprintf(result,"%d::%d::%d",tblock->tm_hour,tblock->tm_min,tblock->tm_sec);

	printf("Server Time:%s\n",result);

	return result;
}

char* getIpAddress(){
	struct hostent *hp = gethostbyname(HOSTNAME);
	unsigned int i=0;
	char p[16];

	if (hp == NULL) {
		printf("gethostbyname() failed\n");
	} else {
		while ( hp -> h_addr_list[i] != NULL) {
			printf("IPAddr:%s\n");
			sprintf(p,"%s\n", inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[i])));
			i++;
		}
	}
	return p;
}
