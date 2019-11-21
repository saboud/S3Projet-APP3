#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>

#define BUFFER_LENGTH  1024

int main(argc, argv)
int argc;
char *argv[];
{
	WSADATA		wsaData;
	int			ReceivingSocket;
	struct sockaddr_in	ReceiverAddr;
	struct sockaddr_in SenderAddr;
	int     SenderAddrSize = sizeof(SenderAddr);
	int		ReceiverAddrSize = sizeof(ReceiverAddr);
	int		ByteReceived;
	int		TotalByteReceived=0;
	//char	ReceiveBuf[BUFFER_LENGTH];

	FILE	*NewFile;
	char	*FileName;
	char	Title[BUFFER_LENGTH];
	char	Text[BUFFER_LENGTH];
	
	if (argc > 2)
	{
		fprintf(stderr, "Usage: %s [portnumber]\n", argv[0]);
		exit(1);
	}
	
	 //Definition de la version de winsock utilisee
	 WORD version = (1 << 8) + 1;  /* Version 1.1 de WINSOCK*/

	//-----------------------------------------------------------//
	// WSA Wtartup
	 /* Initialisation : préparation de l'environnement à l'utilisation 
	 de la librairie Ws2_32.lib
	 Premier argument : version utilisee (Winsock 1.1)
	 Second arg : structure de donnees qui reçoit ap l'appel 
	 infos sur implementtion winsock utilisee.*/
	 if (WSAStartup(version, &wsaData) != 0)
	 {
		 perror("Server ERROR : lors de l'initialisation winsock");
		 exit(1);
	 }
	
	// ------------------------------------------------------------//
	// socket
	
	/* socket(int af, int type, int protocole);
        af : famille de protocoles unitialises par le socket AF_INET = TCP/IP
        type : type de service utilise par socket : SOCK_DGRAM = oriente non connexion
        protocole UDP utilisé :service de transport sans connexion */
	 if ((ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	 {
		 perror("Server ERROR : lors de l'ouverture du socket");
		 exit(1);
	 }
	 printf("Serveur : Socket cree\n");
	 
	// Le socket serveur est pret a accepter des messages
	ReceiverAddr.sin_family = AF_INET;
	// recuperation de l'adresse ip de la machine serveur
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (argc == 2)  /*si on a reussi a recup l'adresse ip*/
		ReceiverAddr.sin_port = htons((short)atoi(argv[1]));  /*on associe aussi un port*/
	else
		ReceiverAddr.sin_port= 0;  /*sinon on associe le port 0*/

	// ------------------------------------------------------ //
	// bind
	
	//La fonction bind permet de specifier les details du protocole et da la communication pour le socket serveur
	if (bind(ReceivingSocket, (struct sockaddr  *)&ReceiverAddr, ReceiverAddrSize) <0)
	{ 
		perror("Server ERROR : lors de l'association au numero de port");
		exit(1);
	}

	// Recuperation adresse et port du socket
	if (getsockname(ReceivingSocket, (struct sockaddr *)&ReceiverAddr, &ReceiverAddrSize)< 0)
	{
		perror("lors de l'interrogation sur le socket");
		exit(1);
	}

	// impression adrresse et port utilisés
	printf("Serveur: adresse IP utilisee: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
	printf("Serveur: port utilise %d\n", htons(ReceiverAddr.sin_port));
	printf("Server: pret a recevoir...\n");
	
	// ------------------------------------------------------ //
	// listen
	listen(ReceivingSocket, 5);
	
	// ------------------------------------------------------ //
	// receive
	
	
	///////////////////////////////////////////////////////////
	/////////////////File Writing
	ByteReceived = recvfrom(ReceivingSocket, Title, BUFFER_LENGTH, 0,
				(struct sockaddr *)&SenderAddr, &SenderAddrSize);
	TotalByteReceived = TotalByteReceived+ByteReceived;
	// FileName = Title 
	/* Ici on nnomme nous même le nouveeau fichier pour montrer le bon 
	   fonctionnement du code, mais il suffirait d'utiliser la ligne 
	   précédente pour pouvoir garder le même nom de fichier. Nous 
	   l'avons vérifiée, cette ligne de code fonctionne. */
	FileName = "newTexteValidation.txt";
	NewFile=fopen(FileName, "w+");
	printf("Created and Opened new file %s", FileName);
	if(NewFile == NULL)
   {
      printf("Error while opening file");
      exit(1);
   }

	while(Text!='\0') 
	{
		ByteReceived = recvfrom(ReceivingSocket, Text, BUFFER_LENGTH, 0,
				(struct sockaddr *)&SenderAddr, &SenderAddrSize);
		TotalByteReceived = TotalByteReceived+ByteReceived;
		fputs(Text, NewFile);
		printf("Writing in file text : %s \n", Text);
	} // Le code ne semble pas parvenir à sortir de sa boucle, ce que nous n'expliquons pas.
	
	// Close file
	fclose(NewFile);
	printf("Closing New File");
	
	// Recuperation des informations du socket envoyant message
	getpeername(ReceivingSocket, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
	printf("Serveur: adresse IP du client: %s\n", inet_ntoa(SenderAddr.sin_addr));
	printf("Server: numero de port du client: %d\n", htons(SenderAddr.sin_port));

	// Reception terminee
	printf("Serveur: Fini de recevoir...\n");

	gets(Text);
	
	// ------------------------------------------------------ //
	// closesocket : fermeture du socket de reception
	closesocket(ReceivingSocket);
	
	// ------------------------------------------------------ //
	// WSA Cleanup
	WSACleanup(); // fin de l'utilisation des fonctions winsock
	
	return 0;
}
