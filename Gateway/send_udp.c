#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define PORT 80

void sendUDP(char *udp_packet,int len){

	struct sockaddr_in serv_addr;

	int sock = 0, valread, i;
	char buffer[1024] = {0};

	//Creating the UDP socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// TTN 52.169.76.203
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "52.169.76.203", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
	}
	send(sock,udp_packet,len,0);
	usleep(10000);
	printf("Gateway message sent\n");
	while(1){
	valread = read( sock , buffer, 1024);
	for(i=0;i<valread;i++) { printf("%02x ", buffer[i]); }
	printf("\n");
}
}
