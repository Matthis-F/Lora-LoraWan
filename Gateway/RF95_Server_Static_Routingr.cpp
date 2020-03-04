#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <RHReliableDatagram.h>
#include <RHRouter.h>
#include <RH_RF95.h>
#include "../libraries/send_udp-thread.c"

//----------------------------------------------------------------------------//
//Used for sending and receiving downlink
#define BINDPOINT 1700//This is our local port for incoming LoraWan downlink
#define ENDPOINT 1700//This is the port of the LoraWan Server
//----------------------------------------------------------------------------//

int sock_fd; //This is the file descrispror that will be used a the socket

//----------------------------------------------------------------------------//
	//Defining the pins used by our sheild
#define RF_CS_PIN  RPI_V2_GPIO_P1_22 // Slave Select on GPIO25 so P1 connector pin #22
#define RF_IRQ_PIN RPI_V2_GPIO_P1_07 // IRQ on GPIO4 so P1 connector pin #7
#define RF_RST_PIN RPI_V2_GPIO_P1_11 // Reset on GPIO17 so P1 connector pin #11
//----------------------------------------------------------------------------//


#define RF_FREQUENCY 868.1


//----------------------------------------------------------------------------//
	//Theses values will be used in the static routing table
#define CLIENT_ADDRESS 100 //This is the address of the client Node
#define ROUTER_ADDRESS 10 //This is the address of the router which will forward
//Packets to the Gateway (us)
#define GATEWAY_ADDRESS 1 //This is our address
// An example of a supported Lora infrastructure :
//Client <-> Router <-> This Gateway
//----------------------------------------------------------------------------//



RH_RF95 rf95(RF_CS_PIN, RF_IRQ_PIN); //Creating an instance of the RF95 Manager

RHRouter manager_routing(rf95, GATEWAY_ADDRESS);//Creating an instance of the
//Routing manager




//----------------------------------------------------------------------------//
volatile sig_atomic_t force_exit=false;//Used ti handle ctrl+C
//Sig_handler is used in the case a user is pressing ctrl+C to stop the gateway
void sig_handler(int sig){
	printf("Received Crtl+C \n exiting !\n");
	close(sock_fd);
	force_exit=true;

}
//----------------------------------------------------------------------------//




//Main function of our program
int main(int argc, const char* argv[]){

	//Signal is used in case of a ctrl+C
	signal(SIGINT, sig_handler);




//----------------------------------------------------------------------------//
	//Initialising the gpio
	if (!bcm2835_init()){
		fprintf(stderr, "[Init] bcm2835_init() failed \n\n", __BASEFILE__);
		exit(-1);
	}

//----------------------------------------------------------------------------//





//----------------------------------------------------------------------------//
	//Initialising The radio module
	#ifdef RF_IRQ_PIN
		printf(", IRQ=GPIO %d\n", RF_IRQ_PIN);
		pinMode(RF_IRQ_PIN, INPUT);
		bcm2835_gpio_set_pud(RF_IRQ_PIN, BCM2835_GPIO_PUD_DOWN);
		bcm2835_gpio_ren(RF_IRQ_PIN);
	#endif
	#ifdef RF_RST_PIN
		pinMode(RF_RST_PIN, OUTPUT);
		printf("[Init] BCM2835 GPIO : OK \t  CS = GPIO %d \t IRQ = GPIO %d \t RST = GPIO %d", RF_CS_PIN,RF_IRQ_PIN,RF_RST_PIN);
		digitalWrite(RF_RST_PIN, LOW);
		bcm2835_delay(150);
		digitalWrite(RF_RST_PIN, HIGH);
		bcm2835_delay(150);
	#endif


	if (!rf95.init()){
		printf("\n");
		fprintf(stderr, "[Init] RF95 module init failed, please verify wiring module\n");
		exit(-1);
	}
//----------------------------------------------------------------------------//




//----------------------------------------------------------------------------//
	//Configuring radio module settings
		rf95.setTxPower(14, false);
		rf95.setFrequency(RF_FREQUENCY);
		rf95.setThisAddress(GATEWAY_ADDRESS);
		rf95.setHeaderFrom(GATEWAY_ADDRESS);
		rf95.setModeRx();
//----------------------------------------------------------------------------//





//----------------------------------------------------------------------------//
	//Setting the sattic routing table The routing table will be used if a
	//downlink is requested, but our gateway is mainly listening
		manager_routing.addRouteTo(CLIENT_ADDRESS, ROUTER_ADDRESS);
		manager_routing.addRouteTo(ROUTER_ADDRESS, ROUTER_ADDRESS);
//----------------------------------------------------------------------------//

		printf("[Init] RF95 Module : OK \t Node ID=%d @ %3.2f MHz \n", GATEWAY_ADDRESS, RF_FREQUENCY);



//----------------------------------------------------------------------------//
		struct Downlink down;//This downlink structure is passed to a downlink handler
		//thread when it is created
		//Content of the Downlink struct:
		//int sock_fd : socket file descriptor used by the downlink handler to listen
		//for some downlink message sent by the LoraWan server
		//
		//struct sockaddr_in client_address : this is the local bindpoint (where we need to listen)
		//
		//char payload : the payload buffer, this is where our downlink handler writes
		//data when a downlink is //----------------------------------------------------------------------------//



//----------------------------------------------------------------------------//
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
//----------------------------------------------------------------------------//




//----------------------------------------------------------------------------//
		//Creating the socket as followed socket(domain, type, protocol)
		//  with domain AF_INET
		//  with type SOCK_DGRAM -> UDP socket
		//  with protocol : 0 -> (IP Protocol)
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
//----------------------------------------------------------------------------//
		//Testing if the socket has been correctly created
		if (sock_fd < 0)
		{
			printf("[Init] Error while creating the socket\n");
			exit(-1);
		}




//----------------------------------------------------------------------------//
		//This structure define the address and port of th remote LoraWan server
		struct sockaddr_in remote_address;
		remote_address.sin_family = AF_INET;
		remote_address.sin_port = htons(ENDPOINT);
		remote_address.sin_addr.s_addr = inet_addr("52.169.76.203");
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
		//This structure is used to bind our socket
		struct sockaddr_in bind_address;
		bind_address.sin_family = AF_INET; //IPv4
		bind_address.sin_addr.s_addr = INADDR_ANY; //Targetting all available interfaces on the machine
		bind_address.sin_port = htons(BINDPOINT);//The local port used to bind the socket
//----------------------------------------------------------------------------//




//----------------------------------------------------------------------------//
		//Actually binding the Socket
		int bnd = bind(sock_fd,(struct sockaddr*)&bind_address,sizeof(bind_address));
//----------------------------------------------------------------------------//
		//Testing if the socket has been bound
		if (bnd < 0)
		{
			printf("[Init] Error while binding the socket\n");
			exit(-1);
			close(sock_fd);
		}




//----------------------------------------------------------------------------//
		//Arguments to pass to the DownlinkHandler
		down.sock_fd = sock_fd;//the socket file descriptor we just created
		down.client_address = bind_address;
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
		//Arguments to pass to the UplinkHandler
		up.sock_fd = sock_fd;//The socket file descriptor we just created
		up.server_address = remote_address;//The remote server address
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
		//Creating Threads for both Uplink and Downlink Handler
		pthread_t downlinkHandler_t;
		pthread_t uplinkHandler_t;

		pthread_create(&downlinkHandler_t,NULL,&downlinkHandler,&down);
		pthread_create(&uplinkHandler_t,NULL,&uplinkHandler,&up);
//----------------------------------------------------------------------------//
		printf("[Init]: Packet Forwarder : OK\n", );//Network configuration Done




//----------------------------------------------------------------------------//
	//At this point we're starting to listen for some packet
		while(!force_exit){

			//Detecting if the IRQ(Interrupt Request) Pin is High
			if (bcm2835_gpio_eds(RF_IRQ_PIN)){
				//if the IQR Pin is high it means a packet has been received by the
				//radio module.
				//From bcm2835 documentation : bcm2835_gpio_eds():
				//Event Detect Status. Tests whether the specified pin has detected
				// a level or edge.

				bcm2835_gpio_set_eds(RF_IRQ_PIN);
				//From bcm2835 documentation : bcm2835_gpio_set_eds():
				//Sets the Event Detect Status register for a given pin to 1, which has
				//the effect of clearing the flag. Use this afer seeing an
				//Event Detect Status on the pin.


				if (rf95.available()){

					uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

					//Reading values from the received packet
					uint8_t len = sizeof(buf);
					uint8_t from = manager_routing.headerFrom();
					uint8_t to = manager_routing.headerTo();
					uint8_t id = manager_routing.headerId();
					uint8_t flags = manager_routing.headerFlags();

					int8_t rssi = rf95.lastRssi();//this one is a signed int because rssi
					//value is often negative

					if (manager_routing.recvfromAck(buf, &len, &from, &to)){

						printf("Packet received [%d] --> [%d]\t%d dB\n",from, to, rssi);
						printf("Message : %s\n\n",buf);
						strcpy(up.payload,buf);
			      pthread_cond_signal(&uplink_request);//signaling an UpLink to the Handler
					}
				else{
					Serial.print("Reception failed\n");
				}
			}
		}
	}
//----------------------------------------------------------------------------//



	printf("Ending\n", __BASEFILE__);
	bcm2835_close();
	close(sock_fd);
	return 0;
}
