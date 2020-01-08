// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"


#define PORT 1700
int protocol_v;
int data_type;
int token_a;
int token_b;

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


int main(int argc, char const *argv[]){
	user_input();
/*
	char status_report[mac_addr];
	for (i = 0; i < 5; i++) {
		scanf("%d", &mac_addr);
	}
*/

	//Creating the message to send to server (formatted in Json)
	//char *message = (char*)"{\"stat\":{\"time\":\"2020-01-07 15:30:01 GMT\",\"lati\":0.0,\"long\":0.0,\"alti\":10,\"rxnb\":0,\"rxok\":0,\"rxfw\":852,\"ackr\":0.0,\"dwnb\":0,\"txnb\":0,\"pfrm\":\"SC Gateway\",\"mail\":\"contact@whatever.com\",\"desc\":\"Dragino Single Channel Gateway on RPI\"}}";
	char *message = (char*)"{\"rxpk\":[{\"tmst\":141505199,\"freq\":868.0,\"chan\":0,\"rfch\":0,\"stat\":1,\"modu\":\"LORA\",\"datr\":\"SF7BW125\",\"codr\":\"4/5\",\"rssi\":-70,\"lsnr\":9.0,\"size\":10,\"data\":\"AQoSZGhlbGxvAA==\"}]}";

	int data_size=12+strlen(message);//total size = the message + the 12 byte header

	printf("size of message :%d\n",strlen(message));
	printf("Total Data size (Header+Message) : %d \n",data_size);

	//creating the 12byte header before sending content this header is composed of
	// [0] Protocol Version
	// [1] Random number
	// [2] Random number
	// [3] Type of packet here 0 equals data
	// [4]-[11] The Gateway EUI composed of Mac address + FF FF values in the middle
	char status_report[data_size];
	status_report[0] = protocol_v;//Protocol version 1
	status_report[1] = token_a;//Random number
	status_report[2] = token_b;//Random number
	status_report[3] = data_type;//Packet type 0 is for data
	status_report[4] = 0xB8;//First byte of mac address
	status_report[5] = 0x27;//Second byte of mac address
	status_report[6] = 0xEB;//Third byte of mac address
	status_report[7] = 0xFF;//Adding FF value for padding, Gateway EUI need to be 8 Byte long
	status_report[8] = 0xFF;//Same as above
	status_report[9] = 0x8F;//4th bute of mac address
	status_report[10] = 0x3F;//5th byte of mac address
	status_report[11] = 0x88;//6th byte of mac address

	//creating the content of the udp package

	memcpy(status_report+12, message, strlen(message));
	//memcpy(status_report2+12, message2, strlen(message2));

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
	if(inet_pton(AF_INET, "10.102.76.67", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	send(sock , status_report , sizeof(status_report) , 0 );
	usleep(10000);

	//send(sock , status_report2 , sizeof(status_report2) , 0 );

	printf("Gateway stat message sent\n");
	valread = read( sock , buffer, 1024);
	for(i=0;i<valread;i++) { printf("%02x ", buffer[i]); }
	printf("\n");
	return 0;
}
