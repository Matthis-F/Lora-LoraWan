#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define BINDPOINT 1700

int send_UDP(char *payload,int size){


//-------------------------------------------------------------//
//Creating the socket as followed socket(domain, type, protocol)
//  with domain AF_INET
//  with type SOCK_DGRAM -> UDP socket
//  with protocol : 0 -> (IP Protocol)
int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
//-------------------------------------------------------------//

//-------------------------------------------------------------//
//Testing if the socket has been correctly created
if (sock_fd < 0)
{
  printf("Error while creating the socket\n");
}
//-------------------------------------------------------------//

//-------------------------------------------------------------//
//Creating a structure holding the IP address and the port to bind
struct sockaddr_in server_address;
struct sockaddr_in ttn_address;

ttn_address.sin_family = AF_INET;
ttn_address.sin_port = htons(1700);
ttn_address.sin_addr.s_addr = inet_addr("52.169.76.203");

server_address.sin_family    = AF_INET; //IPv4
server_address.sin_addr.s_addr = INADDR_ANY; //Targetting all available interfaces on the machine
server_address.sin_port = htons(BINDPOINT);//The local port used to bind the socket
//-------------------------------------------------------------//

//-------------------------------------------------------------//
//Actually binding the Socket
int bnd = bind(sock_fd,(struct sockaddr*)&server_address,sizeof(server_address));
//-------------------------------------------------------------//

//-------------------------------------------------------------//
//Testing if the socket has been bound
if (bnd < 0)
{
  printf("Error while binding the socket\n");
}
//-------------------------------------------------------------//

//Now we need to receive data from the socket
//We need a place where our received data can be stored, so a buffer
unsigned char buffer[1024];
socklen_t ttn_len = sizeof(ttn_address);
int n;
sendto(sock_fd,payload,size,0,(struct sockaddr*)&ttn_address,sizeof(ttn_address));
//Receive data from client
while (1){

int data_len = recvfrom(sock_fd, (unsigned char *)buffer, 1024,  MSG_WAITALL, ( struct sockaddr *) &ttn_address,&ttn_len);
//Printing the data
printf("Data len :%d\n",data_len);
for (int i = 0; i < data_len; i++) {
  printf("%X",buffer[i]);
  fflush(stdout);
}
  n=n+1;
}
close(sock_fd);

  return 1;
}
