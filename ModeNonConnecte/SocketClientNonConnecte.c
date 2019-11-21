// Socket Client Non Connecte

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>
 
#define BUFFER_LENGTH  1024

int main(argc, argv)  
int argc; 
char *argv[];
{
	WSADATA	wsaData;
	int	SendingSocket;
	int 	len;
	int	TitleByteSent;
	int	MessageByteSent;
	int TotalByteSent=0;
	struct sockaddr_in		Receiver, SrcInfo;
	struct hostent			*hp;
	char	buf[BUFFER_LENGTH];
	char	input[BUFFER_LENGTH];
	char	FileName[30];
     
	FILE 	*file;

	if (argc != 1)
	{	
		fprintf(stderr, "%s has no command-line arguments\n", argv[0]);
		exit(1);
	} 
	WORD version = (1 << 8) + 1;  /* Version 1.1 de WINSOCK*/

	//-----------------------------------------------------------//
	// WSA Startup

	/* Initialisation de winsock: préparation de l'environnement à l'utilisation 
	de la librairie Ws2_32.lib */
	if (WSAStartup(version, &wsaData) != 0)
	{
		perror("lors de l'initialisation du socket");
		exit(1);
	}     
	
	// ------------------------------------------------------------//
	// socket
	
	/* Création d'un socket  d'envoi
	socket(int af, int type, int protocole);
	af : famille de protocoles unitlises par le socket AF_INET = TCP/IP
	type : type de service utilise par socket : SOCK_DGRAM = oriente non connexion*/
	SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendingSocket < 0)
	{
		perror("lors de l'ouverture du socket");
		exit(1);
	}

	/* Entrée des informations concernant le serveur auquel se connecter*/
	Receiver.sin_family = AF_INET;
	printf("Entrer le nom de la machine distante: ");
	gets(buf);
	hp = gethostbyname(buf);      /* Lecture de l'adresse IP_v4 de la machine*/
	if (hp == 0)
	{
		fprintf(stderr, "%s: machine inconnue\n", buf);
		exit(1);
	}
	memcpy((char *)&Receiver.sin_addr, (char *)hp->h_addr, hp->h_length);  /*Copie de Receiver.sin_addr dans h_address*/
	printf("Entrer le numero de port dans la machine distante: ");
	gets(buf);
	Receiver.sin_port = htons((short) atoi(buf)); /*Recuperation du port utilise par le serveur*/

	//-----------------------------------------------------------//
	// send
	
	// Choix du fichier a envoyer
	printf("Entrer le nom du fichier a transmettre a la machine distante : \n");
	gets(FileName);
	printf("Client : Un nom de fichier a ete entre \n");
	printf("Client: envoi du nom de fichier : \"%s\"\n", FileName);
	printf("Client: en cours d'envoi...\n");
	//Envoi du titre du fichier
	TitleByteSent = sendto(SendingSocket, FileName, BUFFER_LENGTH, 0,(struct sockaddr *)&Receiver, sizeof(Receiver));
	TotalByteSent = TotalByteSent + TitleByteSent;

	//Ouverture du fichier en lecture
	file = fopen(FileName,"r");
	if(FileName == NULL){
		 perror("Error while opening the file.\n");
		 exit(EXIT_FAILURE);
	}
	printf("Client : File opened in reading mode");
	// Envoi du fichier
	printf("Client : Getting Content from file %s \n", FileName);
	printf("Client: loading...\n");
	while(fgets(input, BUFFER_LENGTH, file))
	{
		printf("%s", input);
		MessageByteSent = sendto(SendingSocket, input, BUFFER_LENGTH, 0,(struct sockaddr *)&Receiver, sizeof(Receiver));
		TotalByteSent = TotalByteSent + MessageByteSent;
		printf("Client : Un packet envoye : %s \n" , input);
	}
	fclose(file);
	
	// Envoi d'un dernier message signifiant le fin de la transmission
	MessageByteSent = sendto(SendingSocket, '\0', BUFFER_LENGTH, 0,(struct sockaddr *)&Receiver, sizeof(Receiver));
	printf("Client : sent final character");
	TotalByteSent = TotalByteSent + MessageByteSent;
	
	// Recuperation de la taille du fichier / message transmis
	memset(&SrcInfo, 0, sizeof(SrcInfo));
	len = sizeof(SrcInfo);

	getsockname(SendingSocket, (struct sockaddr *)&SrcInfo, &len);
	printf("Client: adresse  IP d'envoi: %s\n", inet_ntoa(SrcInfo.sin_addr));
	printf("Client: port utilise: %d\n", htons(SrcInfo.sin_port));

	// Récupération et écriture en cmd des informations sur le serveur
	getpeername(SendingSocket, (struct sockaddr *)&Receiver, (int *)sizeof(Receiver));
	printf("Client: adresse IP du serveur: %s\n", inet_ntoa(Receiver.sin_addr));
	printf("Client: port du serveur: %d\n", htons(Receiver.sin_port));
	printf("Client: total de bytes envoyes: %d\n", MessageByteSent+TitleByteSent);
	
	gets(buf); // Attente pour pouvoir lire l'affichage
	
	//-----------------------------------------------------------//
	// WSA Wtartup
	closesocket(SendingSocket); // Fermeture du socket
	
	//-----------------------------------------------------------//
	// WSA Cleanup
	WSACleanup(); // Back to the system

	return 0;
}
