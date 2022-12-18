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
#include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PORT 5555  // numero di porta di default
#define LENGTH 255 // dimensione stringa

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

int main()
{
	// Inizializzazione della Winsock
	#if defined WIN32

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		errorHandler("Errore di WSAStartup!\n");
		return -1;
	}

	#endif

	// Inizializzazione variabile da utilizzare
	int serverSocket;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;

	int clientAddressLength;
	char echo[LENGTH];
	int sizeOfReceived;

	// Creazione socket
	if ((serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		errorHandler("Funzione socket() fallita!\n");
		return -1;
	}

	// Costruzione indririzzo del server
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET; // famiglia di protocolli
	serverAddress.sin_port = htons(PORT); // numeri di porta di default
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //indirizzo IP del server

	// Binding della socket
	if ((bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0)
	{
		errorHandler("Funzione bind() fallita!\n");
		return -1;
	}

	char echoVowel[LENGTH];	 // vocale da stampare
	char upperVowel[LENGTH]; // vocali convertite in maiuscolo da inviare
	char *hostNameClient;    // hostname del client
	struct hostent *host;	//struttura necessaria per utilizzare gethostbyaddr()

	while (true)
	{

		printf("In attesa di pacchetti da ricevere...\n\n");

		// Ricezione messaggio di echo iniziale
		clientAddressLength = sizeof(clientAddress);

		sizeOfReceived = recvfrom(serverSocket, echo, LENGTH, 0, (struct sockaddr *)&clientAddress, &clientAddressLength);

		// Ottenimento del hostname mediante gethostbyaddr()
		host = gethostbyaddr((char *)&clientAddress.sin_addr, 4, AF_INET);
		hostNameClient = host->h_name;

		printf("\"%s\" ricevuto dal client con hostname: %s\n\n", echo, hostNameClient);

		// Invio di messaggio ok
		if ((sendto(serverSocket, "Ok", sizeof("Ok"), 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress))) != sizeof("Ok"))
		{
			errorHandler("sendto() ha inviato un numero di byte innaspettato\n");
			return -1;
		}

		while (true)
		{
			struct sockaddr_in vocalClientAddress;
			int vocalClientAddressLength = sizeof(vocalClientAddress);

			// Ricezione delle vocali;
			sizeOfReceived = recvfrom(serverSocket, echoVowel, sizeof(echoVowel), 0, (struct sockaddr *)&vocalClientAddress, &vocalClientAddressLength);

			//nel caso in cui si riceve una stringa che indica la fine delle operazione da parte del client
			//il server ritorna in attesa di altri pacchetti di altri client
			if (strcmp(echoVowel, "end") == 0)
			{
				break;
			}
			else
			{
				printf("Vocale ricevuta: %s\n------------------\n", echoVowel);

				// Controllo per vedere se gli indirizzi di server e client sono uguali
				if (clientAddress.sin_addr.s_addr != vocalClientAddress.sin_addr.s_addr)
				{
					errorHandler("Errore: è stato ricevuto un pacchetto da una fonte sconosciuta.\n");
					return -1;
				}

				// Conversione in maiuscolo e invio delle vocali
				upperVowel[0] = toupper(echoVowel[0]);
				upperVowel[1] = '\0';

				if ((sendto(serverSocket, upperVowel, sizeof(upperVowel), 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress))) != sizeof(upperVowel))
				{
					errorHandler("sendto() ha inviato un numero di byte innaspettato\n");
					return -1;
				}
			}
		}
	}

	return 0;
}
