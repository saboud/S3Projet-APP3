////////////////////////////////////////////////////////////////////////////////
//
//		Fichier:  SocketClientConnecte.c
//
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>

#define BUFFER_LENGTH  1025
#define TEXT_LENGH	   1025
#define END_OF_STRING  '\0'

int main(argc, argv)  
int argc; 
char *argv[];
{	
	WSADATA	wsaData;
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	char buf[BUFFER_LENGTH];
	char FileName[BUFFER_LENGTH];
	char input[TEXT_LENGH];
	FILE *file;

	WORD version = (1 << 8) + 1;  /* Version 1.1 */


	if (argc != 1)
	{	fprintf(stderr,"%s manque d'arguments\n", argv[0]);
		exit(1);
	}

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
	type : type de service utilise par socket : SOCK_STREAM = oriente connexion*/
	sock = socket(AF_INET, SOCK_STREAM, 0 ); // IPPROTO_TCP ? 
	if (sock < 0)
	{
		perror("lors de l'ouverture du socket");
		exit(1);
	}
	
	server.sin_family = AF_INET;  
	
	// Entree des information sur le serveur auquel se connecter
	printf("Entrer le nom de la machine distante: "); 
	gets(buf);    
	hp = gethostbyname(buf);      
	if (hp == 0)
	{	fprintf(stderr, "%s: machine inconnue\n", buf);
		exit(1);
	}
	memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);  
	printf("Entrer le numero de port dans la machine distante: ");   
	gets(buf);
	server.sin_port = htons((short) atoi(buf)); 
	
	//-----------------------------------------------------------//
	// connect
	printf("Client: connexion...\n");
	connect (sock,(struct sockaddr*) &server,sizeof(server));
	
	//-----------------------------------------------------------//
	// send
	
	// Choix du fichier a transmettre
	printf("Entrer le nom du fichier a transmettre a la machine distante : \n");
	gets(FileName);
	printf("Client : Un nom de fichier a ete entre \n");
	printf("Client: envoi du nom de fichier : \"%s\"\n", FileName);
	printf("Client: en cours d'envoi...\n");
	
	//Envoi du titre du fichier
	if(send(sock, FileName, strlen(FileName), 0)<0)
	{
		perror("Client ERROR : lors de l'ecriture du titre dans le socket");
		exit(1);
	}
	
	//Ouverture du fichier en lecture
	file = fopen(FileName,"r");
	if(FileName == NULL){
		 perror("Client ERROR : while opening the file.\n");
		 exit(EXIT_FAILURE);
	}
	printf("Client : File %s opened in reading mode", FileName);
	
	// Envoi du fichier
	printf("Client : Getting Content from file %s \n", FileName);
	printf("Client: loading...\n");
	while(fgets(input, BUFFER_LENGTH, file))
	{
		printf("%s", input);
		if(send(sock, input, BUFFER_LENGTH, 0) <0 )
		{
			perror("Client ERROR : lors de l'ecriture dans le socket");
			exit(1);
		}
		printf("Client : Un packet envoye : %s \n" , input);
	}
	fclose(file);

	gets(buf); // Attente pour pouvoir lire l'affichage
	
	//-----------------------------------------------------------//
	// WSA Wtartup
	closesocket(sock); // Fermeture du socket
	
	//-----------------------------------------------------------//
	// WSA Cleanup
	WSACleanup(); // Back to the system

	return 0;
}  
