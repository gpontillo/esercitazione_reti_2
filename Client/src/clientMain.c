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
	

	return 0;
}
