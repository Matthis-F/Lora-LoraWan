#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

//#define DEBUG //Please uncomment for debugging functions


struct Downlink{
  int sock_fd;
  struct sockaddr_in client_address;
  char payload[1024];
};

struct UpLink{
  int sock_fd;
  struct sockaddr_in server_address;
  char payload[1024];
};

pthread_mutex_t socket_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t uplink_work_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t downlink_work_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t uplink_request = PTHREAD_COND_INITIALIZER;
pthread_cond_t downlink_request = PTHREAD_COND_INITIALIZER;


void * downlinkHandler(void * arg){
  //Threads function only accepts void pointers as argument, a void pointer can
  //point to anything we need to tell our compiler to what type of data out pointer
  //is pointing, here we are pointing to a structure, so we "cast our arg to a struct"

  struct Downlink *downlink = (struct Downlink*) arg;

  //getting the parameters given to the function
  struct sockaddr_in client_address = downlink->client_address;
  socklen_t client_len = sizeof(client_address);
  int sock_fd = downlink->sock_fd;


  char buffer[1024];
  while(1){
    int data_len = recvfrom(sock_fd, (unsigned char *)buffer, 1024,  MSG_WAITALL, ( struct sockaddr *) &client_address,&client_len);
    if(data_len < 0){
      printf("Error while receiving data %d\n",data_len);
    }

    #ifdef DEBUG
    //Printing the data
    for (int i = 0; i < data_len; i++) {
      printf("%X ",buffer[i]);
      fflush(stdout);
    }
    printf("\n");
    #endif


    pthread_mutex_lock(&downlink_work_lock);

    memcpy(downlink->payload,buffer,data_len);
    pthread_cond_signal(&downlink_request);

    pthread_mutex_unlock(&downlink_work_lock);
  }

}


void * uplinkHandler(void *arg){
  struct UpLink *up = (struct UpLink*) arg;

  #ifdef DEBUG
    printf("Successfully created Thread\n");
  #endif

  //getting this into local variables
  int sock_fd = up->sock_fd;
  struct sockaddr_in server_address = up->server_address;


  while(1){
    //-------------------------------------------------------------//
    //Blocking until signal is received
    pthread_mutex_lock(&socket_lock);
    pthread_cond_wait(&uplink_request,&socket_lock);
    //-------------------------------------------------------------//

    #ifdef DEBUG
      printf("Received uplink signal\n");
    #endif
    //-------------------------------------------------------------//
    //mutexes are used to to avoid multiple threads reading or writing to the same
    //variable at the same time --> aka Thread Safety

    pthread_mutex_lock(&uplink_work_lock);
    //-------------------------------------------------------------//



    //-------------------------------------------------------------//
    //Step 1:
    //Getting th payload from the buffer
    char *payload = up->payload;
    //-------------------------------------------------------------//



    //-------------------------------------------------------------//
    //Relasing the mutex for the payload buffer
    pthread_mutex_unlock(&uplink_work_lock);
    //-------------------------------------------------------------//


    //-------------------------------------------------------------//
    //Step 2:
    //Sending the data to the Server
    int send = sendto(sock_fd,payload,strlen(payload),0,(struct sockaddr*)&server_address,sizeof(server_address));
    //-------------------------------------------------------------//



    printf("%s\n",up->payload);


    //-------------------------------------------------------------//
    //Testing if the data has been correctly sent
    if(send < 0){
        printf("[UDP Packet Forwarder] An error occured while sending uplink: %d\n",send);
    }
    //-------------------------------------------------------------//
    pthread_mutex_unlock(&socket_lock);
  }
}
