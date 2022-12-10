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
#include <string.h>

#define PORT 5555		 // numero di porta di default
#define LENGTH 255		 // dimensione stringa


// Funzione per gestire eventuali errori
void errorHandler(char *messaggioDiErrore)
{
	printf("%s", messaggioDiErrore);
	system("pause");
}

// Funzione per terminare l'uso di Winsock
void clearWinSock()
{
	#if defined WIN32
	WSACleanup();
	#endif
}

// Funzione per chiudere la connessione
void closeConnection(int mySocket)
{
	closesocket(mySocket);
	clearWinSock();
}

int main()
{
	// Inizializzazione della Winsock
	#if defined WIN32

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2,2), &wsaData);

	if(result != 0)
	{
		errorHandler("Errore di WSAStartup!\n");
		return -1;
	}

	#endif

	// Inizializzazione variabile da utilizzare
	int serverSocket;
	struct sockaddr_in echoServerAddress;
	struct sockaddr_in echoClientAddress;

	int clientAddressLength;
	char echo[LENGTH];
	int sizeOfReceived;


	//CREAZIONE SOCKET

	if((serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		errorHandler("Funzione socket() fallita!\n");
		closeConnection(serverSocket);
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
		closeConnetction(serverSocket);
	}


	char stringOfOK = "Ok"; //stringa di OK
	int vowelReceived; //dimensine del messaggio ricevuto
	char echoVowel; //vocale da stampare
	char sendUpperVowel; //vocali convertire in maiuscolo da inviare

	while(true)
	{

		//Ricezione messaggio iniziale
		clientAddressLength = sizeof(echoClientAddress);

		sizeOfReceived = recvfrom(serverSocket, echo, LENGTH, 0,(struct sockaddr*)&echoClientAddress, &clientAddressLength);

		printf("%s ricevuto dal client con nome host: %s",echo);


		//Invio di messaggio ok

		if((sendto(serverSocket, stringOfOK, sizeof(stringOfOK), (struct sockaddr*) &echoClientAddress, sizeof(echoClientAddress))) != sizeof(stringOfOK))
		{
			errorHandler("sendto() ha inviato un numero di byte innaspettato");
			closeConnection(serverSocket);
		}

		//Ricevzione delle vocali;
		vowelReceived = recvfrom(serverSocket, echoVowel, LENGTH, 0,(struct sockaddr*)&echoClientAddress, &clientAddressLength);

		printf("Vocale ricevuta: %c", echoVowel);


		//Invio delle vocali convertite in maiuscolo
		sendUpperVowel = toupper(echoVowel);

		if((sendto(serverSocket, sendUpperVowel, sizeof(sendUpperVowel), (struct sockaddr*) &echoClientAddress, sizeof(echoClientAddress))) != sizeof(sendUpperVowel))
		{
			errorHandler("sendto() ha inviato un numero di byte innaspettato");
			closeConnection(serverSocket);
		}

	}

	return 0;
}
