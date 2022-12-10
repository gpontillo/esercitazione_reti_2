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
#define MAX_CONNECTIONS 6 // numero massimo di connessioni accettate dal Server
#define BUFFER_SIZE 1024 // dimensione del buffer


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

	#if defined WIN32

	//INIZIALIZZAZIONE DELLA WINSOCK
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		errorHandler("Errore! Funzione WSAStartup() fallita.\n");
		return 0;
	}

	#endif

	// CREAZIONE DELLA SOCKET
	int serverSocket;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSocket < 0)
	{
		errorHandler("Errore! Creazione della socket fallita.\n");
		clearWinSock();
		return -1;
	}

	// ASSEGNAZIONE INDIRIZZO PER LA SOCKET
	struct sockaddr_in socketAddress;

	memset(&socketAddress, 0, sizeof(socketAddress));

	socketAddress.sin_family = AF_INET; //famiglia protocolli
	socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //indirizzo IP Server
	socketAddress.sin_port = htons(PORT); //numero di porta del Server

	// ASSOCIAMENTO DI UN INDIRIZZO ALLA SOCKET
	if (bind(serverSocket, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0)
	{
		errorHandler("Errore! Funzione bind() fallita.\n");
		closesocket(serverSocket);
		clearWinSock();
		return -1;
	}

	// ASCOLTO DI EVENTUALI CONNESSIONI
	if (listen(serverSocket, MAX_CONNECTIONS) < 0)
	{
		errorHandler("Errore! Funzione listen() fallita.\n");
		closesocket(serverSocket);
		clearWinSock();
		return -1;
	}

	struct sockaddr_in clientAddress; // struttura per l'indirizzo del Client
	int clientSocket;				  // descrittore per la socket Client
	int clientLen;					  // dimensione dell'indirizzo del Client

	while(true)
	{
		printf("In attesa di connessione da parte di un client...\n");


		clientLen = sizeof(clientAddress); // dimensione dell'indirizzo del Client

		//ACCETTAZIONE DI NUOVE CONNESSIONI
		clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLen);

		if (clientSocket < 0)
		{
			errorHandler("Errore! Funzione accept() fallita.\n");
			closeConnection(serverSocket);
			return 0;
		}

		// VISUALIZZAZIONE IP DEL CLIENT
		printf("Connessione avvenuta con il client: %s\n\n", inet_ntoa(clientAddress.sin_addr));

		// GESTIONE DELLA CONNESSIONE COL SERVER

		char buffer[BUFFER_SIZE];

		// Invio della stringa di connessione
		strcpy(buffer, "Connessione avvenuta\n");
		if (send(clientSocket, buffer, BUFFER_SIZE, 0) <= 0)
		{
			errorHandler("Errore! funzione send() fallita.\n");
			closeConnection(serverSocket);
			return -1;
		}

		int fine = 0;

		// Ciclo do-while fin quando non riceviamo una stringa 'quit'
		do
		{

			char stringA[BUFFER_SIZE] = "";
			char stringB[BUFFER_SIZE] = "";
			int bytesRcvd;

			// Ricezione della stringa A
			if ((bytesRcvd = recv(clientSocket, buffer, BUFFER_SIZE, 0)) <= 0)
			{
				errorHandler("Errore! Funzione recv() fallita.\n");
				closesocket(clientSocket);
				return -1;
			}

			strcpy(stringA, buffer);
			printf("Stringa A: %s\n", stringA);

			// Ricezione della stringa B
			if ((bytesRcvd = recv(clientSocket, buffer, BUFFER_SIZE, 0)) <= 0)
			{
				errorHandler("Errore! Funzione recv() fallita.\n");
				closeConnection(serverSocket);
				return -1;
			}


			strcpy(stringB, buffer);
			printf("Stringa B: %s\n", stringB);

			//Se una delle due stringhe è uguale a 'quit'

			if (strcmp(stringA, "quit") == 0 || strcmp(stringB, "quit") == 0)
			{
				strcpy(buffer, "bye");
				fine = 1;

				printf("Stringa con 'quit' individuata. Invio di 'bye' al client.\n");

			}
			else
			{
				//concatena le due stringhe
				char stringConcat[BUFFER_SIZE] = "";
				strcpy(stringConcat, stringA);
				strcat(stringConcat, stringB);
				strcpy(buffer, stringConcat);
				printf("A+B: %s\n", stringConcat);
			}

			if (send(clientSocket, buffer, BUFFER_SIZE, 0) <= 0)
			{
				errorHandler("Errore! Funzione send() fallita.\n");
				closeConnection(serverSocket);
				return -1;
			}
			printf("---------------\n");

		} while (!fine);

	}
	return 0;
}
