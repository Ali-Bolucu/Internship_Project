#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "Pluto.h"

#define MAX 512		// Message size
#define PORT 32448	
#define SA struct sockaddr


// Function defining
void pluto_conn();
void client_conn();
void func();


int sockfd, connfd, len;
struct sockaddr_in servaddr, cli;

bool check = false;



void client_conn(){

	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n \n");
}



void pluto_conn(){

	printf("Will try again to start pluto in 5 sec...\n\n");
	sleep(5);

	check = pluto();

}



// Function designed for chat between client and server.
void func()
{
	int n;

	char buff[MAX] = "not empty";

	char mes0[MAX] = "NO MEANING";

	char mes1[MAX] = "OK";
	char mes2[MAX] = "!!NOT OK";

	char mes3[MAX] = "Frequency changed";
	char mes4[MAX] = "!!Frequency NOT changed";

	char mes5[MAX] = "Sin wave's frequency Changed";
	char mes6[MAX] = "!!Sin wave's frequency NOT Changed";

	char mes7[MAX] = "New sin wave added";
	char mes8[MAX] = "!!New sin wave NOT added";

	char mes9[MAX]  = "Returning single tone";
	char mes10[MAX] = "!!Couldn't return single tone";

	char mes11[MAX]  = "Shutting down Pluto and server";
	char mes12[MAX] = "!!Couldn't Shutting down Pluto but closing server";

	char mes13[MAX]  = "TX Hardware gain changed";
	char mes14[MAX] = "!!TX Hardware gain NOT changed";

	
	while(true){

		//buff bos degilse if"in icine giriyor
		if ((strcmp(buff, ""))) {
			
				bzero(buff, MAX);
				read(connfd, buff, sizeof(buff));
				printf("[client] %s \n", buff);
			

			
			if (strncmp("CHECK", buff, 5) == 0) {
				printf("Checking...\n");
				
				if (check){
				write(connfd, mes1, 512);
				printf("Pluto working...\n");
				}
				else{
				printf("!!Pluto NOT working...\n");
				pluto_conn();
				write(connfd, mes2, 512);
				}
			}
			
			// frequency for TX_LO 
			else if (strncmp("F:", buff, 2) == 0) {

				double freq;
				
				bool tx;
				sscanf(buff, "%*[^0123456789]%lf", &freq);
				tx = tx_lo_freq(freq);

				if(tx){
				write(connfd, mes3, 512);
				//printf("Frequency Changed to\n",);
				}
				else{
				write(connfd, mes4, 512);
				printf("!!Frequency NOT Changed...\n");
				}	
			}
			
			// frequency for TX_sampling
			else if (strncmp("sF:", buff, 3)== 0) {

				double freq_s;
				bool tx_s;

				sscanf(buff, "%*[^0123456789]%lf", &freq_s);
				tx_s = tx_sampling_freq(freq_s);

				if(tx_s){
				write(connfd, mes3, 512);
				//printf("Frequency Changed to\n",);
				}
				else{
				write(connfd, mes4, 512);
				printf("!!Frequency NOT Changed...\n");
				}	
			}

			// frequency for TX_bandwidth		
			else if (strncmp("bF:", buff, 3)== 0) {

				double freq;
				
				bool tx_b;
				sscanf(buff, "%*[^0123456789]%lf", &freq);
				tx_b = tx_bandwidth_freq(freq);

				if(tx_b){
				write(connfd, mes3, 512);
				//printf("Frequency Changed to\n",);
				}
				else{
				write(connfd, mes4, 512);
				printf("!!Frequency NOT Changed...\n");
				}	
			}
			
			// frequency for TX_port_select
			else if (strncmp("pF:", buff, 3) == 0) {

				char* port;
				
				bool tx_p;
				sscanf(buff, "%*[^0123456789]%lf", port);
				tx_p = tx_port(port);

				if(tx_p){
				write(connfd, mes3, 512);
				//printf("Frequency Changed to\n",);
				}
				else{
				write(connfd, mes4, 512);
				printf("!!Frequency NOT Changed...\n");
				}	
			}
				
			// Frequency of sin wave
			else if (strncmp("sinF:", buff, 5) == 0) {

				float freq;
				bool sinf;
				sscanf(buff, "%*[^0123456789]%f", &freq);

				sinf = sin_gen(freq);

				if(sinf){
				write(connfd, mes5, 512);
				printf("Sin wave's frequency Changed...\n");
				}
				else{
				write(connfd, mes6, 512);
				printf("!!Sin wave's frequency NOT Changed...\n");
				}
			}

			// for multitone
			else if (strncmp("multi:", buff, 6) == 0) {

				float freq;
				float num;
				bool multi;

				sscanf(buff, "%*[^0123456789]%f%*[^0123456789.]%f", &freq, &num);

				multi = Multi(freq, num);

				if(multi){
				write(connfd, mes7, 512);
				printf("New sin wave added...\n");
				}
				else{
				write(connfd, mes8, 512);
				printf("!!New sin wave NOT added...\n");
				}
			}

			// For change TX gain [-89 to 0]
			else if (strncmp("gain_tx:", buff, 7) == 0) {

				double gain;
				bool gain_bool;
				sscanf(buff, "%*[^-0123456789]%lf", &gain);

				gain_bool = gain_tx(gain);

				if(gain_bool){
				write(connfd, mes13, 512);
				printf("TX Hardware gain changed...\n");
				}
				else{
				write(connfd, mes14, 512);
				printf("!!TX Hardware gain NOT changed...\n");
				}
			}

			//going back to sinF frequency and single tone
			else if (strncmp("clear", buff, 5) == 0) {

				bool clc = clear();

				if(clc){
				write(connfd, mes9, 512);
				printf("Returning single tone...\n");
				}
				else{
				write(connfd, mes10, 512);
				printf("!!Couldnt return single tone...\n");
				}
			}
				
			// If client going to disconnect
			else if (strncmp("close", buff, 5) == 0) {

				printf("Connectin closed, waiting new ...");
				client_conn();
			}

			// For shutting down server
			else if (strncmp("exit", buff, 4) == 0) {

				bool exitt = shutdown_pluto();

				if(exitt){
				write(connfd, mes11, 512);
				printf("Shutting down Pluto and server...\n");
				}
				else{
				write(connfd, mes12, 512);
				printf("!!Couldn't Shutting down Pluto but closing server...\n");
				}
				break;
			}

			else{
				write(connfd, mes0, 512);
				}
				

		}
		else{
		printf("Connection lost...\n \t Waiting new connection \n\n");
		n = 0;
		client_conn();
		strcpy(buff, mes3);
		}
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

	// Starting Pluto
	check = pluto();
	
	// Accept the data packet from client and verification
	client_conn();


	// Function for chatting between client and server
	func();

	// After chatting close the socket
	close(sockfd);
	}

