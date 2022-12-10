/*
 ============================================================================
 Name        : server.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PORT 5555		 // numero di porta di default
#define LENGTH 255		 // dimensione stringa

// FUNZIONE PER GESTIONE ERRORI
void errorHandler(char *messaggioDiErrore)
{
	printf("%s", messaggioDiErrore);
	system("pause");
}

// FUNZIONE PER TERMINARE L'USO DI WINSOCK
void clearWinSock()
{
	#if defined WIN32
	WSACleanup();
	#endif
}

//FUNZIONE PER CHIUSURA DELLA CONNESSIONE
void closeConnection(int mySocket)
{
	closesocket(mySocket);
	clearWinSock();
}

int main()
{
	//INIZIALIZZAZIONE WINSOCK
	#if defined WIN32

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2,2), &wsaData);

	if(result != 0)
	{
		errorHandler("Errore di WSAStartup!\n");
		return -1;
	}

	#endif

	int serverSocket;
	struct sockaddr_in echoServerAddress;
	struct sockaddr_in echoClientAddress;

	int clientAddressLength;
	char echo[LENGTH];
	int sizeOfRecived;


	//CREAZIONE SOCKET

	if((serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		errorHandler("Funzione socket() fallita!\n");
		return -1;
	}


	//COSTRUZIONE INDIRIZZO SERVER

	memeset(&echoServerAddress, 0 ,sizeof(echoServerAddress));
	echoServerAddress.sin_family= AF_INET;
	echoServerAddress.sin_port = htons(PORT);
	echoServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	//BIND SOCKET

	if((bind(serverSocket, (struct sockaddr*)&echoServerAddress, sizeof(echoServerAddress))) < 0)
	{
		errorHandler("Funzione bind() fallita!\n");
		return -1;
	}

	//RICEZIONE STRINGA DI ECHO DA PARTE DEL CLIENT

	while(true)
	{

		clientAddressLength = sizeof(echoClientAddress);

		sizeOfRecived = recvfrom(serverSocket, echo, LENGTH, 0,(struct sockaddr*)&echoClientAddress, &clientAddressLength);

		printf("Gestione client %s\n", inet_ntoa(echoClientAddress));

		printf("Ricevuto: %s\n", echo);

	}

	return 0;
}
