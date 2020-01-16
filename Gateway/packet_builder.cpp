#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct param{
	int protocol_v;
	int data_type;
	int token_a;
	int token_b;
	int msg_type;
	char data[2048];
	int payload_size;

};

struct message{
	char complete_msg[2048];
	int size;
};

struct time{
  time_t timestamp;
  char time_formatted[100];
};




struct time get_time(){
    struct time t;
    time_t now = time (NULL); //This put the value of th unix timestamp into the variable now
    strftime (t.time_formatted, 100, "%Y-%m-%dT%H:%M:%S.528000Z", localtime (&now));//This format the value of the UNIX
    //timestamp and put it in the variable time_formated, here we use the pointer to the variable now
    return t;
}

struct param user_input(){

		struct param choices;

		printf("Protocol value (Possible values 1,2) : ");
		scanf("%d", &choices.protocol_v);

		printf("Data type value (0 for DATA, 1 for ACK) : ");
		scanf("%d", &choices.data_type);

		printf("Message type (0 for rxpk, 1 for stat update) : ");
		scanf("%d", &choices.msg_type);

		return choices;
}




struct message build_message(struct param user_values){

	char payload[1024];

	srand(time(NULL));
	user_values.token_a=rand()%255;
	user_values.token_b=rand()%255;

	struct message msg;
	struct time t = get_time();

	//////////////////		CREATING THE PAYLOAD OF THE MESSAGE ///////////////////////////
	//It can be either a stat update (just sending some stats about the GW) or an rxpk (an uplink message forwarding DATA from one node)//
  if (user_values.msg_type == 0){

    sprintf(payload,"{\"rxpk\": [{\"stat\": 1, \"chan\": 0, \"datr\": \"SF12BW125\", \"tmst\": %d, \"codr\": \"4/5\", \"lsnr\": 8, \"freq\": 868.1, \"data\": \"QOcSASYAAAABKNn0aqptT71PyEHfU2PjSAooVnbHiKCQ\", \"size\": 5, \"modu\": \"LORA\", \"rfch\": 0, \"time\": \"%s\", \"rssi\": -45}]}",(int)time(NULL),t.time_formatted);
		}
  else if (user_values.msg_type == 1){
    strcpy(payload,"{\"stat\":{\"time\":\"2020-01-07 15:30:01 GMT\",\"lati\":0.0,\"long\":0.0,\"alti\":10,\"rxnb\":0,\"rxok\":0,\"rxfw\":852,\"ackr\":0.0,\"dwnb\":0,\"txnb\":0,\"pfrm\":\"SC Gateway\",\"mail\":\"contact@whatever.com\",\"desc\":\"Single Channel GW\"}}");
	}
	////////////////////////////////////////////////////////////////////////////////////

  int data_size=12+strlen(payload); //The length of ther msg_header 12Bytes + The length of the payload (dynamic)

  char msg_header[data_size];//Creating a char array fot the msg_header
  msg_header[0] = user_values.protocol_v;//Protocol version 1
	msg_header[1] = user_values.token_a;//Random number
  msg_header[2] = user_values.token_b;//Random number
  msg_header[3] = user_values.data_type;//Packet type 0 is for data
  msg_header[4] = 0xB8;//First byte of mac address
  msg_header[5] = 0x27;//Second byte of mac address
  msg_header[6] = 0xEB;//Third byte of mac address
  msg_header[7] = 0xFF;//Adding FF value for padding, Gateway EUI need to be 8 Byte long
  msg_header[8] = 0xFF;//Same as above
  msg_header[9] = 0x8F;//4th bute of mac address
  msg_header[10] = 0x3F;//5th byte of mac address
  msg_header[11] = 0x88;//6th byte of mac address

	msg.size = strlen(payload)+12; //The message size is used to know how much bytes must be send on the network by the socket

	/////////////////////////////////////////////////
	//Creating the complete message
	memcpy(msg.complete_msg, msg_header, sizeof(msg_header));
	memcpy(msg.complete_msg+12, payload, strlen(payload));
	/////////////////////////////////////////////////


	return msg;

}
