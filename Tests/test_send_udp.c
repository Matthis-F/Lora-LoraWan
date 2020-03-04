#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <signal.h>
#include "../libraries/send_udp-thread.c"

#define BINDPOINT 1700
#define ENDPOINT 1700


int sock_fd; //This is the file descrispror that will be used a the socket
int i;//Just for test purpose, the thread will send the value of i on the network

//-------------------------------------------------------------//
//SIGINT HANDLER
//This function is used to handle potential ctrl+C by the user
void sig_handler(int signo)
{
  if (signo == SIGINT){
    printf("Received Ctrl+C \n exiting !\n");
    close(sock_fd);//Properly closing the web socket
    exit(1);//exiting the program (code 1)
  }
}
//-------------------------------------------------------------//



int main(void){
  //-------------------------------------------------------------//
  struct Downlink down;//This downlink structure is passed to a downlink handler
  //thread when it is created
  //Content of the Downlink struct:
  //int sock_fd : socket file descriptor used by the downlink handler to listen
  //for some downlink message sent by the LoraWan server
  //
  //struct sockaddr_in client_address : this is the local bindpoint (where we need to listen)
  //
  //char payload : the payload buffer, this is where our downlink handler writes
  //data when a downlink is received
  //-------------------------------------------------------------//


  //-------------------------------------------------------------//
  struct UpLink up;
  //This uplink structure is passed to an uplink handler
  //thread when it is created
  //Content of the Upnlik struct:
  //int sock_fd : socket file descriptor used by the downlink handler to write
  //data to the socket
  //
  //struct sockaddr_in server_address : this is the endpoint, the address of the
  //LoraWan server
  //
  //char payload : the payload buffer, this is where our uplink handler reads
  //data when an uplink is received by the radio handler
  //-------------------------------------------------------------//



  //-------------------------------------------------------------//
  //Creating the socket as followed socket(domain, type, protocol)
  //  with domain AF_INET
  //  with type SOCK_DGRAM -> UDP socket
  //  with protocol : 0 -> (IP Protocol)
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  //-------------------------------------------------------------//

  //Testing if the socket has been correctly created
  if (sock_fd < 0)
  {
    printf("Error while creating the socket\n");
  }



  //-------------------------------------------------------------//
  //This structure define the address and port of th remote LoraWan server
  struct sockaddr_in remote_address;
  remote_address.sin_family = AF_INET;
  remote_address.sin_port = htons(ENDPOINT);
  remote_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  //-------------------------------------------------------------//

  //-------------------------------------------------------------//
  //This structure is used to bind our socket
  struct sockaddr_in bind_address;
  bind_address.sin_family = AF_INET; //IPv4
  bind_address.sin_addr.s_addr = INADDR_ANY; //Targetting all available interfaces on the machine
  bind_address.sin_port = htons(BINDPOINT);//The local port used to bind the socket
  //-------------------------------------------------------------//




  //-------------------------------------------------------------//
  //Actually binding the Socket
  int bnd = bind(sock_fd,(struct sockaddr*)&bind_address,sizeof(bind_address));
  //-------------------------------------------------------------//

  //Testing if the socket has been bound
  if (bnd < 0)
  {
    printf("Error while binding the socket\n");
    close(sock_fd);
  }




  //-------------------------------------------------------------//
  //Arguments to pass to the DownlinkHandler
  down.sock_fd = sock_fd;//the socket file descriptor we just created
  down.client_address = bind_address;
  //-------------------------------------------------------------//


  //-------------------------------------------------------------//
  //Arguments to pass to the UplinkHandler
  up.sock_fd = sock_fd;//The socket file descriptor we just created
  up.server_address = remote_address;//The remote server address
  strcpy(up.payload,"Hello World");
  //-------------------------------------------------------------//


  //-------------------------------------------------------------//
  //Creating Threads for both Uplink and Downlink Handler
  pthread_t downlinkHandler_t;
  pthread_t uplinkHandler_t;

  pthread_create(&downlinkHandler_t,NULL,&downlinkHandler,&down);
  pthread_create(&uplinkHandler_t,NULL,&uplinkHandler,&up);
  //-------------------------------------------------------------//

  //Used to handle Ctrl+C signal
  signal(SIGINT, sig_handler);

  //-------------------------------------------------------------//
  //Sending a downlink each second
  while(1){
      sprintf(up.payload,"Test %d",i++);
      pthread_cond_signal(&uplink_request);//signaling an UpLink to the Handler
      sleep(1);
  }
  //-------------------------------------------------------------//
  pthread_join(downlinkHandler_t,NULL); //Waiting for the Downlink Handler thread
  //to end (so the main thread won't exit)
  close(sock_fd);

}
