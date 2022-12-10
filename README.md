# Esercizio Reti: "Assegnazione UDP-DNS - Conta vocali"
## Traccia

Di seguito è riportata la traccia di riferimento che possiamo ritrovare anche al questo [link](https://elearning.di.uniba.it/pluginfile.php/98160/mod_resource/content/1/Assegnazione%20UPD%202223%20Reti%20ITPS.pdf).

> Progettare e implementare (scrivendo codice portabile) un’applicazione UDP client/server
> conforme al seguente protocollo:
> 
> 1) Il client legge da tastiera il nome dell’host e il numero di porta da utilizzare per contattare il Server
> 2) Il client contatta il server (inviando un messaggio iniziale a piacere)
> 3) Il server visualizza sullo std output il messaggio ricevuto insieme al nome dell’host del client che l’ha appena contattato. (Esempio: “MSG ricevuto dal client con nomehost: nomepc”)
> 4) Il server invia al client la stringa "OK".
> 5) Il client riceve la stringa “OK” e la visualizza sullo std output.
> 6) Il client legge una stringa di caratteri alfanumerici dallo std input e conta il numero di vocali presenti nella stringa. A questo punto il client invia al server ogni vocale trovata mediante una "sendto" separata.
> 7) Il server per ogni vocale ricevuta invia, allo stesso client dal quale l’ha appena ricevuta, la stessa vocale ma in MAIUSCOLO.
> 8) Il client visualizza sullo std output ogni vocale maiuscola ricevuta dal server.
> 9) Il client Termina.
> 10) Il server rimane in attesa di ricevere altri pacchetti.
> 
> **NOTE**:
> per lo svolgimento dell’esercizio è necessario tra gli altri metodi dell’API delle socket l’utilizzo dei metodi
> - **gethostbyname()**
> - **gethostbyaddr()**

## Server
### Build

Per buildare e successivamente eseguire il server è necessario:
1. Aprire Eclipse
2. Andare su *File* -> *Open Projects from File System...*
3. Selezionare la cartella `Server` all'interno della repository e fare *Seleziona cartella*
4. Cliccare su *Finish*

Successivamente per buildare il server è necessario:
1. Aprire la cartella `src`
2. Aprire il file `serverMain.c`
3. Cliccare su *Build*

Il file eseguibile generato si troverà in `Binaries` e si chiamerà `Server.exe`

> Nota: il server è di default hostato sulla porta **5555**
## Client
### Build

Per buildare e successivamente eseguire il client è necessario:
1. Aprire Eclipse
2. Andare su *File* -> *Open Projects from File System...*
3. Selezionare la cartella `Client` all'interno della repository e fare *Seleziona cartella*
4. Cliccare su *Finish*

Successivamente per buildare il client è necessario:
1. Aprire la cartella `src`
2. Aprire il file `clientMain.c`
3. Cliccare su *Build*

Il file eseguibile generato si troverà in `Binaries` e si chiamerà `Client.exe`

> Nota: il client richiede all'inizio la porta del server. Se non si è modificata nel codice, la porta per potersi connettere al server è di default **5555**
