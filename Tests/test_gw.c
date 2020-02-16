#include <stdio.h>
#include <stdint.h>
#include "../libraries/udp_packet_builder.c"
#include "../libraries/LoraWanPkt.h"
#include "../Gateway/send_udp.c"

int main(void){
  //Setting up virtual node parameters
  uint8_t DevAddr1[4] = { 0x26, 0x01, 0x12, 0xE7 };
  uint8_t NwkSKey1[16] = { 0x10, 0x6B, 0x05, 0x99, 0xA0, 0xDB, 0xB2, 0x80, 0x94, 0xE5, 0xC4, 0x15, 0xE6, 0xB7, 0xE7, 0x77 };
  uint8_t AppSKey1[16] = { 0x25, 0xD8, 0x64, 0xE2, 0x57, 0x8F, 0xEB, 0x8D, 0x17, 0xEB, 0xBC, 0x59, 0xB0, 0x4C, 0x3C, 0xED };
  uint16_t FrameCount1 = 0x0000;
  uint8_t dir1 = 0;
  char data[1024];
  //Asking the user to enter the payload content
  scanf("%s",(char*)&data);


  struct LoraWan param;


//Putting parameters into a frame struct
  memcpy(param.NwkSKey,NwkSKey1,16);
  param.FrameCount = FrameCount1;
  param.dir = dir1;
  memcpy(param.DevAddr,DevAddr1,4);
  memcpy(param.AppSKey,AppSKey1,16);


//building the packet by giving the buildLoraWanPkt a frame struct containing
//informations about the virtual end-node
//the function return a buffer (pkt) with the complete packet (base64 format)
printf("size of data %ld\n",(((strlen(data)+13) + ( ((strlen(data)+13) % 3) ? (3 - ((strlen(data)+13) % 3)) : 0) ) / 3) * 4 );
uint8_t pkt[256];
int complete_pkt_len = buildLoraWanPkt(param,data,pkt);
	printf("[test_LoraWanPkt]:Final Packet %s\n",pkt);
  printf("actual size of data %d \n",(int)strlen((char*)pkt));
  struct message msg = build_udp_packet(0,pkt,strlen((char*)pkt));

  sendUDP((uint8_t*)msg.complete_msg,msg.size);
}
