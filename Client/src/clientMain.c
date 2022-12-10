/*
 ============================================================================
 Name        : client.c
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
#include <string.h>
#define BUFFER_SIZE 1024  // Dimensione del buffer
#define DEFAULT_PORT 5555 // Numero di porta di default


// FUNZIONE PER GESTIONE ERRORI
void errorHandler(char *messaggioDiErrore)
{
	printf("%s", messaggioDiErrore);
	system("pause");
}

// FUNZIONE PER TERMINARE L'USO DI WINSOCK
void clearWinSock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

// FUNZIONE PER CHIUSURA CONNESSIONE
void closeConnection(int mySocket)
{
	closesocket(mySocket);
	clearWinSock();
}

int main(void) {

	#if defined WIN32

	//INIZIALIZZAZIONE DELLA WINSOCK
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (result != 0) {
		errorHandler("Errore! Funzione WSAStartup() fallita\n");
		return -1;
	}

	#endif
	
	// CREAZIONE DELLA SOCKET
	int clientSocket;
	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clientSocket < 0) {
		errorHandler("Errore! Creazione della socket fallita.\n");
		closeConnection(clientSocket);
		return -1;
	}

	// Inserimento da tastiera del numero di porta del server
	int port;
	printf("Inserisci il numero di porta del server (%d):", DEFAULT_PORT);
	scanf("%d", &port);

	// COSTRUZIONE DELL’INDIRIZZO DEL SERVER
	struct sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof(socketAddress));

	socketAddress.sin_family = AF_INET; //famiglia del protocollo
	socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del server
	socketAddress.sin_port = htons(port); // Numero di porta del server

	// CONNESSIONE AL SERVER
	if (connect(clientSocket, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0)
	{
		errorHandler("Errore! Connessione fallita.\n");
		closeConnection(clientSocket);
		return -1;
	}

	// GESTIONE DELLA CONNESSIONE COL SERVER
	// Ricezione della stringa "connnessione avvenuta"
	char buffer[BUFFER_SIZE]; // buffer per i dati dal server
	int bytesRicev = 0;

	if ((bytesRicev = recv(clientSocket, buffer, BUFFER_SIZE, 0)) <= 0) {
		errorHandler("Errore! Funzione recv() fallita.\n");
		closeConnection(clientSocket);
		return -1;
	}
	printf("%s\n", buffer);

	//Ciclo do-while fin quando non riceviamo la stringa bye
	do {
		char aString[BUFFER_SIZE] = ""; // Stringa A da inviare
		char bString[BUFFER_SIZE] = ""; // Stringa B da inviare


		// Inserimento stringhe A e B da tastiera
		printf("Inserisci stringa A:");
		scanf("%s", aString);

		printf("Inserisci stringa B:");
		scanf("%s", bString);

		// INVIO STRINGHE AL SERVER
		// Invio stringa A
		strcpy(buffer, aString);
		if (send(clientSocket, buffer, BUFFER_SIZE, 0) <= 0) {

			errorHandler("Errore! Funzione send() fallita.\n");
			closeConnection(clientSocket);
			return -1;
		}

		// Invio stringa B
		strcpy(buffer, bString);
		if (send(clientSocket, buffer, BUFFER_SIZE, 0) <= 0) {

			errorHandler("Errore! Funzione send() fallita.\n");
			closeConnection(clientSocket);
			return -1;
		}

		// RICEZIONE STRINGA DAL SERVER
		int bytesRcvd;
		printf("Ricevuto: ");

		if ((bytesRcvd = recv(clientSocket, buffer, BUFFER_SIZE, 0)) <= 0) {

			errorHandler("Errore! Funzione recv() fallita.\n");
			closeConnection(clientSocket);
			return -1;
		}

		printf("%s\n", buffer);
		printf("---------------\n");

	}while(strcmp(buffer,"bye") != 0);

	// CHIUSURA DELLA CONNESSIONE
	closeConnection(clientSocket);
	printf("\n");
	system("pause");
	return 0;
}
