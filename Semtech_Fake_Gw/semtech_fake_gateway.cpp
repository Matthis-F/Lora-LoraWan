// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "packet_builder.cpp"

#define PORT 1700

int protocol_v, data_type, token_a, token_b;
/*
void user_input(){
		srand(time(NULL));

		printf("Protocol value : ");
		scanf("%d", &protocol_v);

		token_a=rand()%255;
		token_b=rand()%255;

		printf("Data type value (0 for DATA, 1 for ACK) : ");
		scanf("%d", &data_type);

		printf("\ngerenated random tokens: \n[a]: %x [b]: %x \n",token_a,token_b);

}
*/

int main(int argc, char const *argv[]){

	struct param user_values = user_input();
	struct message msg = build_message(user_values);
	srand(time(NULL));
	user_values.token_a=rand()%255;
	user_values.token_b=rand()%255;

	printf("Protocol version : %d		Data type : %d",user_values.protocol_v,user_values.data_type);
	printf("\ngerenated random tokens: [token a]: %x	 [token b]: %x \n",user_values.token_a,user_values.token_b);
	printf("Message Payload:\n%s\n",msg.complete_msg+12);
	printf("Message size : %d\n",msg.size);
	//printf("Total Data size (Header+Message) : %d \n",sizeof(msg.complete_msg));


	struct sockaddr_in serv_addr;

	int sock = 0, valread, i;
	char buffer[1024] = {0};

	//Creating the UDP socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "52.169.76.203", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	send(sock,msg.complete_msg,msg.size,0);
	usleep(10000);
	printf("Gateway message sent\n");
	valread = read( sock , buffer, 1024);
	for(i=0;i<valread;i++) { printf("%02x ", buffer[i]); }
	printf("\n");
	return 0;
}
