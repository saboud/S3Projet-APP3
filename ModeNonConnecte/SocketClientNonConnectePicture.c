#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h> 



#define BUFFER_LENGTH  1024

int main(argc, argv)  int argc; char *argv[];
{
     WSADATA              wsaData;
     int				  SendingSocket;
	 struct sockaddr_in   ReceiverAddr, SrcInfo;
	 struct hostent		  *hp;
     char                 *SendBuf = "Bonjour!Terre appelle Mars!";
	 char				  buf[BUFFER_LENGTH];
     int len;
     int TotalByteSent;
 
   
	 if (argc != 1)
	 {	fprintf(stderr, "%s has no command-line arguments\n", argv[0]);
	 exit(1);
	 }
	 WORD version = (1 << 8) + 1;  /* Version 1.1 de WINSOCK*/
	 /*  On fait appel à la fonction d'init */
	 if (WSAStartup(version, &wsaData) != 0)
	 {
		 perror("lors de l'initialisation du socket");
		 exit(1);
	 }
     
 
     // ?????????????????????????????????????????????
     SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	 if (SendingSocket < 0)
	 {
		 perror("lors de l'ouverture du socket");
		 exit(1);
	 }

	 // ?????????????????????????????????????????????
     ReceiverAddr.sin_family = AF_INET;
	 printf("Entrer le nom de la machine distante: ");
	 gets(buf);
	 hp = gethostbyname(buf);      /*?????????????????????*/
	 if (hp == 0)
	 {
		 fprintf(stderr, "%s: machine inconnue\n", buf);
		 exit(1);
	 }
	 memcpy((char *)&ReceiverAddr.sin_addr, (char *)hp->h_addr, hp->h_length);  /*?????????????????????*/
	 printf("Entrer le numero de port dans la machine distante: ");
	 gets(buf);
	 ReceiverAddr.sin_port = htons((short)atoi(buf)); /*?????????????????????*/
 
	 // ?????????????????????????????????????????????
     printf("Client: les donnees a envoyer : \"%s\"\n", SendBuf);
     printf("Client: entrain d'envoyer...\n");
	
     FILE *picture;
     int size, read_size, stat, packet_index;
	char FileName[30], send_buffer[BUFFER_LENGTH], read_buffer[256];
	packet_index = 1;
	
	printf("Enter file name:\n");
	gets(FileName);
	
	picture = fopen(FileName,"r");
	printf("Getting Picture Size\n");
	
	if(FileName == NULL)
	{
		 perror("Error while opening the file.\n");
		 exit(EXIT_FAILURE);
	}
	
	if(picture == NULL) {
        printf("Error Opening Image File"); 
    }
    
    fseek(picture, 0, SEEK_END);
   size = ftell(picture);
   fseek(picture, 0, SEEK_SET);
   printf("Total Picture size: %i\n",size);

   //Send Picture Size
   printf("Sending Picture Size\n");
   fwrite(socket, sizeof(socket),1, picture);

   //Send Picture as Byte Array
   printf("Sending Picture as Byte Array\n");
   
    do { //Read while we get errors that are due to signals.
      stat=fread(socket, sizeof(socket) , 1, picture);
      printf("Bytes read: %i\n",stat);
   } while (stat < 0);
   
    printf("Received data in socket\n");
   printf("Socket data: %s\n", read_buffer);
   

   while(!feof(picture)){
	  
   //Read from the file into our send buffer
      read_size = fread(send_buffer,sizeof(send_buffer), 1, picture);
      //Send data through our socket
      do{
        stat = fwrite(socket, sizeof(socket),1, picture);

      }while (stat < 0);
      
      printf("Packet Number: %i\n",packet_index);
      printf("Packet Size Sent: %i\n",read_size);     
      printf(" \n");
      printf(" \n");


      packet_index++;  

      }
	printf("\n\nEnd of Program\n");

	fclose(picture);
	
	 TotalByteSent = sendto(SendingSocket, SendBuf, BUFFER_LENGTH, 0,
		 (struct sockaddr *)&ReceiverAddr, sizeof(ReceiverAddr));
 
	 // ?????????????????????????????????????????????
     memset(&SrcInfo, 0, sizeof(SrcInfo));
     len = sizeof(SrcInfo);
 
	 getsockname(SendingSocket, (struct sockaddr *)&SrcInfo, &len);
     printf("Client: adresse  IP d'envoi: %s\n", inet_ntoa(SrcInfo.sin_addr));
     printf("Client: port utilise: %d\n", htons(SrcInfo.sin_port));
 
	 // ?????????????????????????????????????????????
	 getpeername(SendingSocket, (struct sockaddr *)&ReceiverAddr, (int *)sizeof(ReceiverAddr));
     printf("Client: adresse IP du serveur: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
     printf("Client: port du serveur: %d\n", htons(ReceiverAddr.sin_port));
     printf("Client: total de bytes envoyes: %d\n", TotalByteSent);
 
	 // ?????????????????????????????????????????????
	
	 closesocket(SendingSocket);
	// ?????????????????????????????????????????????
	WSACleanup();
	// Back to the system
	return 0;
}

