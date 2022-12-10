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

#define ECHO_MAX 255
#define BUFFER_SIZE 1024  // Dimensione del buffer
#define DEFAULT_PORT 5555 // Numero di porta di default
#define DEFAULT_IP "127.0.0.1" // IP server di default


// Funzione per gestione errori
void errorHandler(char *messaggioDiErrore)
{
	printf("%s", messaggioDiErrore);
	system("pause");
}

// Funzione per terminare l'uso di winsock
void clearWinSock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

// Funzione per chiusura connessione
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
	
	int clientSocket;
	struct sockaddr_in servAddr;
	struct sockaddr_in fromAddr;
	unsigned int fromSize;
	char echoString[ECHO_MAX];
	char echoBuffer[ECHO_MAX];
	int echoStringLen;
	int respStringLen;

	// Input della stringa di echo
	printf("Inserisci la stringa echo da inviare al server\n");
	scanf("%s", echoString);
	if ((echoStringLen = strlen(echoString)) > ECHOMAX)
		ErrorHandler("echo word too long");

	// Creazione della socket
	clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clientSocket < 0) {
		errorHandler("Errore! Creazione della socket fallita.\n");
		closeConnection(clientSocket);
		return -1;
	}

	// Costruzione dell'indirizzo del server
	char* inputForServer = "";
	int inputIntForServer = 0;
	char* ipServer = DEFAULT_IP;
	int portServer = DEFAULT_PORT;

	printf("Inserisci IP del server da contattare (default: '%s'): ", DEFAULT_IP);
	scanf("%s", &ipServer);
	if(inputForServer == "\n")
		ipServer = inputForServer;

	printf("Inserisci porta del server da contattare (default: '%d'): ", DEFAULT_PORT);
	scanf("%d", &inputIntForServer);
	if(inputIntForServer == '\n')
		portServer = inputIntForServer;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_port = htons(portServer);
	servAddr.sin_addr.s_addr = inet_addr(ipServer);

	// Invio della stringa echo al server
	if (sendto(clientSocket, echoString, echoStringLen, 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) != echoStringLen) {
		ErrorHandler("sendto() sent different number of bytes than expected");
		exit(EXIT_FAILURE);
	}

	// Ritorno della stringa echo
	fromSize = sizeof(fromAddr);
	respStringLen = recvfrom(clientSocket, echoBuffer, ECHO_MAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

	if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
		fprintf(stderr, "Error: received a packet from unknown source.\n");
		exit(EXIT_FAILURE);
	}
	echoBuffer[respStringLen] = '\0';
	printf("Received: %s\n", echoBuffer);

	// Lettura stringa su cui contare le vocali
	char buffer[BUFFER_SIZE] = "";
	printf("Inserisci stringa su cui leggere le vocali:");
	scanf("%s", &buffer);

	// Conteggio e invio delle vocali
	int vowels = 0;
	int bufferLen = strlen(buffer);
	for(int i = 0; i < bufferLen; i++) {
		if(buffer[i] == 'A' || buffer[i] == 'a' || buffer[i] == 'E' || buffer[i] == 'e' || buffer[i] == 'I' || buffer[i] == 'i' || buffer[i] == 'O' || buffer[i] == 'o' || buffer[i] == 'U' || buffer[i] == 'u') {
			vowels++;
			if (sendto(clientSocket, buffer, bufferLen, 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) != bufferLen) {
				ErrorHandler("sendto() sent different number of bytes than expected");
				closeConnection(clientSocket);
			}
		}
	}

	// Ricezione delle vocali maiuscole
	while(vowels > 0) {
		vowels--;
		char bufferRecv[BUFFER_SIZE] = "";
		int respLen;
		respLen = recvfrom(clientSocket, bufferRecv, ECHO_MAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

		if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
		{
			fprintf(stderr, "Error: received a packet from unknown source.\n");
			exit(EXIT_FAILURE);
		}
		bufferRecv[respLen] = '\0';
		printf("Received: %s\n", bufferRecv);
	}

	closeConnection(clientSocket);
	system("pause");
	return EXIT_SUCCESS;
}
