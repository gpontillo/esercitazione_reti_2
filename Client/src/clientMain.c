/*
 ============================================================================
 Name        : clientMain.c
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

#define ECHO_MAX 255				 // Dimensione della stringa di echo
#define SIZE 255				 // Dimensione del stringa per le vocali
#define DEFAULT_PORT 5555			 // Numero di porta di default
#define DEFAULT_HOSTNAME "localhost" // Hostname di default

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

int main(void)
{

	#if defined WIN32

	// Inizializzazione della Winsock
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		errorHandler("Errore! Funzione WSAStartup() fallita\n");
		return -1;
	}

	#endif

	// Inizializzazione variabili da utilizzare
	int clientSocket;
	struct sockaddr_in serverAddress;
	struct sockaddr_in fromAddress;
	int fromSize;
	char echo[ECHO_MAX];
	int responseStringLength;

	// Creazione della socket
	clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (clientSocket < 0)
	{
		errorHandler("Errore! Creazione della socket fallita.\n");
		return -1;
	}

	int echoStringLen;
	char echoString[ECHO_MAX];

	// Input della stringa di echo

	printf("Inserisci la stringa di echo da inviare al server:");
	scanf("%s", echoString);
	if ((echoStringLen = strlen(echoString)) > ECHO_MAX)
		errorHandler("Messaggio troppo lungo!\n");

	printf("\n");

	// Costruzione dell'indirizzo del server
	char hostNameServer[20] = DEFAULT_HOSTNAME;
	struct in_addr *ipServer;
	int portServer = DEFAULT_PORT;

	printf("Inserisci il nome dell'host del server da contattare (default: '%s'): ", DEFAULT_HOSTNAME);
	scanf("%s", hostNameServer);
	printf("\n");

	printf("Inserisci porta del server da contattare (default: '%d'): ", DEFAULT_PORT);
	scanf("%d", &portServer);
	printf("\n");

	// Recupero IP server a partire dal suo hostname mediante gethostbyname()
	struct hostent *host; //struttura necessaria per utilizzare gethostbyname()
	host = gethostbyname(hostNameServer);

	if (host == NULL)
	{
		errorHandler("funzione gethostbyname() fallita!\n");
		return -1;
	}
	else
	{
		ipServer = (struct in_addr *)host->h_addr_list[0];
	}

	// Costruzione dell'indirizzo del server con ip e porta dell'utente
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET; //famiglia di protocolli
	serverAddress.sin_port = htons(portServer); //numero di porta del server
	serverAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*ipServer)); //indirizzo IP del server

	// Invio della stringa echo al server
	if (sendto(clientSocket, echoString, echoStringLen, 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != echoStringLen)
	{
		errorHandler("sendto() ha inviato un numero di bytes inaspettato!\n");
		return -1;
	}

	// Ritorno della stringa echo
	fromSize = sizeof(fromAddress);
	responseStringLength = recvfrom(clientSocket, echo, ECHO_MAX, 0, (struct sockaddr *)&fromAddress, &fromSize);

	//Controllo per vedere se gli indirizzi di server e client sono uguali
	if (serverAddress.sin_addr.s_addr != fromAddress.sin_addr.s_addr)
	{
		errorHandler("Errore! E' stato ricevuto un pacchetto da una fonte sconosciuta.\n");
		return -1;
	}

	echo[responseStringLength] = '\0';
	printf("Ricevuto: %s\n", echo);

	// Lettura stringa su cui contare le vocali
	char stringVowels[SIZE] = "";
	printf("Inserisci stringa su cui leggere le vocali:");
	scanf("%s", stringVowels);
	printf("\n");

	int stringVowelsLen = strlen(stringVowels);
	int i = 0;

	// ciclo per scorrere la stringa con le vocali
	while (i < stringVowelsLen)
	{
		// Controllo se la i-esima lettera è una vocale
		if (stringVowels[i] == 'A' || stringVowels[i] == 'a' || stringVowels[i] == 'E' || stringVowels[i] == 'e' || stringVowels[i] == 'I' || stringVowels[i] == 'i' || stringVowels[i] == 'O' || stringVowels[i] == 'o' || stringVowels[i] == 'U' || stringVowels[i] == 'u')
		{

			// Invio della vocale trovata
			printf("Invio al server della vocale: %c\n", stringVowels[i]);
			char vowelToSend[ECHO_MAX];
			vowelToSend[0] = stringVowels[i];
			vowelToSend[1] = '\0';

			if (sendto(clientSocket, vowelToSend, sizeof(vowelToSend), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != sizeof(vowelToSend))
			{
				errorHandler("sendto() ha inviato un numero di bytes non desiderato\n");
				return -1;
			}

			// Ricezione della vocale maiuscola
			char bufferRecv[SIZE] = "";
			int respLen;
			respLen = recvfrom(clientSocket, bufferRecv, ECHO_MAX, 0, (struct sockaddr *)&fromAddress, &fromSize);


			// Controllo per vedere se gli indirizzi di server e client sono uguali
			if (serverAddress.sin_addr.s_addr != fromAddress.sin_addr.s_addr)
			{
				errorHandler("Errore! E' stato ricevuto un pacchetto da una fonte sconosciuta\n");
				return -1;
			}

			bufferRecv[respLen] = '\0';
			printf("Vocale ricevuta: %s\n------------------\n", bufferRecv);
		}

		i++;
	}

	// Invio di un pacchetto per dire al server che il client ha finito
	if (sendto(clientSocket, "end", sizeof("end"), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != sizeof("end"))
	{
		errorHandler("sendto() ha inviato un numero di bytes inaspettato.\n");
		return -1;
	}

	// Chiusura connessione
	closesocket(clientSocket);
	clearWinSock();
	system("pause");
	return 0;
}
