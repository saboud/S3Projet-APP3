////////////////////////////////////////////////////////////////////////////////
//
//		Fichier:  SocketServerConnecte.c
//
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>

#define BUFFER_LENGTH  1024
#define END_OF_STRING  '\0'

int main(argc, argv) 
int argc;
char *argv[];
{	
	WSADATA wsadata;
	int sock, msgsock;
	int length;
	int rval;
	char buf[BUFFER_LENGTH];
	struct sockaddr_in server;
	struct sockaddr_in client;
	struct hostent *cname;
	int port;

	WORD version = (1 << 8) + 1;  /* Version 1.1 de WINSOCK*/

	if (argc > 2)
	{	fprintf(stderr,"Usage: %s [portnumber]\n", argv[0]);
		exit(1);
	}


	//-----------------------------------------------------------//
	// WSA Wtartup
	 /* Initialisation : préparation de l'environnement à l'utilisation 
	 de la librairie Ws2_32.lib
	 Premier argument : version utilisee (Winsock 1.1)
	 Second arg : structure de donnees qui reçoit ap l'appel 
	 infos sur implementtion winsock utilisee.*/
	if (WSAStartup(version, &wsadata) != 0)    
	{	perror("lors de l'initialisation winsock");
		exit(1);
	}
	
	// ------------------------------------------------------------//
	// socket
	
	/* socket(int af, int type, int protocole);
        af : famille de protocoles unitialises par le socket AF_INET = TCP/IP
        type : type de service utilise par socket : SOCK_STREAM = oriente connexion
        protocole UDP utilisé :service de transport sans connexion */
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{	perror("lors de l'ouverture du socket");
		exit(1);
	}

	
	server.sin_family = AF_INET;   // Le socket serveur est pret a accepter des messages
	server.sin_addr.s_addr = INADDR_ANY; // recuperation de l'adresse ip de la machine serveur
	if (argc == 2)  /*si on a reussi a recuperer l'adresse ip*/
		port = htons((short) atoi(argv[1]));  /*on associe aussi un port*/
	else
		port = 0;   /*sinon on associe le port 0*/
	server.sin_port = port; /* definition du port exposé du serveur */
	
	// ------------------------------------------------------ //
	// bind
	
	/*La fonction bind permet de specifier les details du protocole 
	 * et de la communication pour le socket serveur*/
	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) 
	{	
		perror("lors de l'association au numero de port");
		exit(1);
	}

	// Interrogation du socket pour recuperer son adresse et son port.
	// Association du port au socket.

	length = sizeof(server);
	if (getsockname(sock, (struct sockaddr *)&server, &length) < 0)
	{	
		perror("lors de l'interrogation sur le socket");
		exit(1);
	}
	printf("Le port %d a ete associe au socket\n", ntohs(server.sin_port));

	// ------------------------------------------------------ //
	// listen
	// ON place le serveur en mode passif, a l'ecoute
	listen(sock, 5);

	// ------------------------------------------------------ //
	// accept
	// pour accepter la connexion du client
	while (1)  /*?????????????????????*/
	{	length = sizeof(client);
		msgsock = accept(sock, (struct sockaddr *) &client, &length); //on accepte la connexion du client
		cname = gethostbyaddr((char *) &client.sin_addr, 4, AF_INET); /* recuperation de l'adresse client*/
		if (cname == NULL)
			printf("Connection d'une machine inconnue (%s), ",
				inet_ntoa(client.sin_addr));
		else
			printf("Connection de la machine %s (%s), ", cname->h_name, /*enregistrement du nom de la machine client*/
				inet_ntoa(client.sin_addr));
		printf("au port %d\n", client.sin_port); /*impression du numero de port du client*/

		if (msgsock < 0)
		{	
			perror("lors de l'acceptation"); /*s'il y a un eerreur au cours du processus d'acceptation de la connexion*/
		}

		// ------------------------------------------------------ //
		// receive		
		else do  /*Si la connexion a bien ete accepte, on commence a recevoir des fichiers*/
		{	
			if ((rval = recv(msgsock, buf, sizeof(buf), 0)) < 0)
			{	
				perror("lors de la lecture du message");
			}
			if (rval == 0)
				printf("Serveur : Fin de connexion\n\n");
			else
			{	
				buf[rval] = END_OF_STRING;
				printf("Recu: '%s'\n", buf);
				printf("Entrer les caracteres+retour de chariot pour envoyer, ou juste retour de chariot pour terminer:\n ");
				gets(buf);
				length = strlen(buf);
				if (length == 0)  break;
				if (send(msgsock, buf, length, 0) < 0) /*?????????????????????*/
					{	
						perror("lors de l'ecriture dans le socket");
						exit(1);
					}
			}
		} while (rval != 0);

		// ------------------------------------------------------ //
		// closesocket : fermeture du socket de reception des messages ouvert par bind
		closesocket(msgsock);


	} 

	// ------------------------------------------------------ //
	// closesocket : fermeture du socket serveur initialise avec "socket"
	closesocket(sock);
	
	// ------------------------------------------------------ //
	// WSA Cleanup
	WSACleanup(); /*fin de l'utilisation des fonctions winsock*/

	return 0;
}  
