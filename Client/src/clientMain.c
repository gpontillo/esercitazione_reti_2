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
#include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ECHO_MAX 255
#define BUFFER_SIZE 255				 // Dimensione del buffer
#define DEFAULT_PORT 5555			 // Numero di porta di default
#define DEFAULT_HOSTNAME "localhost" // IP server di default

// Funzione per gestione errori
void errorHandler(char *messaggioDiErrore)
{
	printf("%s", messaggioDiErrore);
	system("pause");
}

// Funzione per terminare l'uso di winsock
void clearWinSock()
{
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

int main(void)
{

#if defined WIN32

	// INIZIALIZZAZIONE DELLA WINSOCK
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		errorHandler("Errore! Funzione WSAStartup() fallita\n");
		return -1;
	}

#endif
	int clientSocket;
	struct sockaddr_in servAddr;
	struct sockaddr_in fromAddr;
	unsigned int fromSize;
	char echoBuffer[ECHO_MAX];
	int respStringLen;
	int i = 0;

	// Creazione della socket
	clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clientSocket < 0)
	{
		errorHandler("Errore! Creazione della socket fallita.\n");
		closeConnection(clientSocket);
		return -1;
	}

	int echoStringLen;
	char echoString[ECHO_MAX];

	// Input della stringa di echo
	printf("Inserisci la stringa echo da inviare al server\n");
	scanf("%s", echoString);
	if ((echoStringLen = strlen(echoString)) > ECHO_MAX)
		errorHandler("Echo word too long");

	// Costruzione dell'indirizzo del server
	char hostnameServer[20] = DEFAULT_HOSTNAME;
	struct in_addr *ipServer;
	int portServer = DEFAULT_PORT;

	printf("Inserisci il nome dell'host del server da contattare (default: '%s'): ", DEFAULT_HOSTNAME);
	scanf("%s", hostnameServer);

	printf("Inserisci porta del server da contattare (default: '%d'): ", DEFAULT_PORT);
	scanf("%d", &portServer);

	// Recupero IP server a partire dal suo hostname
	struct hostent *host;
	host = gethostbyname(hostnameServer);
	if (host == NULL)
	{
		fprintf(stderr, "gethostbyname() failed.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		ipServer = (struct in_addr *)host->h_addr_list[0];
	}

	// Costruzione dell'indirizzo del server con ip e porta dell'utente
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_port = htons(portServer);
	servAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*ipServer));

	// Invio della stringa echo al server
	if (sendto(clientSocket, echoString, echoStringLen, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) != echoStringLen)
	{
		errorHandler("sendto() sent different number of bytes than expected");
		exit(EXIT_FAILURE);
	}

	// Ritorno della stringa echo
	fromSize = sizeof(fromAddr);
	respStringLen = recvfrom(clientSocket, echoBuffer, ECHO_MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);

	if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
		fprintf(stderr, "Error: received a packet from unknown source.\n");
		exit(EXIT_FAILURE);
	}
	echoBuffer[respStringLen] = '\0';
	printf("Received: %s\n", echoBuffer);

	// Lettura stringa su cui contare le vocali
	char stringVowels[BUFFER_SIZE] = "";
	printf("Inserisci stringa su cui leggere le vocali:");
	scanf("%s", &stringVowels);

	// Conteggio e invio delle vocali
	int stringVowelsLen = strlen(stringVowels);

	// Conteggio e invio delle vocali
	int vowels = 0;

	while (i < stringVowelsLen)
	{
		// Controllo se la i-esima lettera è una vocale
		if (stringVowels[i] == 'A' || stringVowels[i] == 'a' || stringVowels[i] == 'E' || stringVowels[i] == 'e' || stringVowels[i] == 'I' || stringVowels[i] == 'i' || stringVowels[i] == 'O' || stringVowels[i] == 'o' || stringVowels[i] == 'U' || stringVowels[i] == 'u')
		{

			// Vocale trovata
			vowels++;

			// Invio della vocale trovata
			printf("Sending server vowel: %c\n", stringVowels[i]);
			char vowelToSend[ECHO_MAX];
			vowelToSend[0] = stringVowels[i];
			vowelToSend[1] = '\0';
			if (sendto(clientSocket, vowelToSend, sizeof(vowelToSend), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) != sizeof(vowelToSend))
			{
				errorHandler("sendto() sent different number of bytes than expected\n");
				closeConnection(clientSocket);
			}

			// Ricezione della vocale maiuscola
			char bufferRecv[BUFFER_SIZE] = "";
			int respLen;
			respLen = recvfrom(clientSocket, bufferRecv, ECHO_MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);

			if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
			{
				fprintf(stderr, "Error: received a packet from unknown source.\n");
				exit(EXIT_FAILURE);
			}
			bufferRecv[respLen] = '\0';
			printf("Received vowel: %s\n---------\n", bufferRecv);
		}

		i++;
	}

	if (sendto(clientSocket, "end", sizeof("end"), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) != sizeof("end"))
	{
		errorHandler("sendto() sent different number of bytes than expected\n");
		closeConnection(clientSocket);
	}

	closeConnection(clientSocket);
	system("pause");
	return EXIT_SUCCESS;
}
