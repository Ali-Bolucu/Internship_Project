#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "Adalm_Pluto_TX.h"




#define MAX 512
#define PORT 32448
#define SA struct sockaddr


int sockfd, connfd, len;
struct sockaddr_in servaddr, cli;



void client_conn(){

	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n \n");




}



// Function designed for chat between client and server.
void func()
{

	int error = 0;
	socklen_t len = sizeof(error);
	int retval = 0;
	
	int n;

	char buff[MAX] = "not empty";
	
	char mes1[MAX] = "Shutting down";
	char mes2[MAX] = "It's working";
	char mes3[MAX] = "Frequency changed";
	char mes4[MAX] = "NO MEANING";

	while(true){
	
	//retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
	
	if ((strcmp(buff, ""))) {
		
		bzero(buff, MAX);
		read(connfd, buff, sizeof(buff));
		printf("[client] %s \n", buff);
		
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			write(connfd, mes1, 512);
			shutdown_pluto();
			break;
			}
		
		else if (strncmp("CHECK", buff, 4) == 0) {
			printf("Checking...\n");
			write(connfd, mes2, 512);
			}
		
		else if (strncmp("F:", buff, 2) == 0) {

			double freq;
			sscanf(buff, "%*[^0123456789]%lf", &freq);
			write(connfd, mes3, 512);		
			tx_freq(freq);
			}
			
		else if (strncmp("close", buff, 5) == 0) {

			client_conn();
			}
			
		else{
			write(connfd, mes4, 512);
			}
			

	}else{
	printf("Connection lost...\n \t Waiting new connection \n\n");
	n = 0;
	client_conn();
	strcpy(buff, mes3);
	}
}
			


// Driver function
int main()
{


	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr =inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	client_conn();

	// Starting Pluto
	pluto();


	// Function for chatting between client and server
	func();

	// After chatting close the socket
	close(sockfd);
}
