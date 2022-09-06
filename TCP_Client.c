/*
	Create a TCP socket
*/

#include<stdio.h>
#include<winsock2.h>
#include <iio.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#pragma warning(disable:4996)
//#define _WINSOCK_DEPRECATED_NO_WARNINGS





const char* get_reply(char* reply_ans) {
	 
	return reply_ans;
}

char* TCP_client_pluto(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char* massege, server_reply[2000];
	char reply_mes[1000];
	int recv_size;
	int n;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");


	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	//inet_ntop(AF_INET, L"127.0.0.1", &server.sin_addr.s_addr);
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected");


	for (;;) {

		//Receive a reply from the server
		if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR)
		{
			puts("recv failed");
		}

		puts("Reply received :	");

		server_reply[recv_size] = '\0';
		puts(server_reply);
		


		if ((strncmp(server_reply, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}


		if ((strncmp(server_reply, "Frequency", 9)) == 0) {

			double freq;
			sscanf(server_reply, "%*[^0123456789]%lf", &freq);

			tx_freq(freq);
		}




		// Sends a reply to the server

		//printf("Enter the string : ");
		//n = 0;

		//while ((reply_mes[n++] = getchar()) != '\n')
		//	;

		send(s, "OK", 2, 0);
	}

	return server_reply;
}
