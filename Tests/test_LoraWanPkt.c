#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../libraries/LoraWanPkt.h"

int main(void){
		uint8_t DevAddr1[4] = { 0x26, 0x01, 0x12, 0xE7 };
		uint8_t NwkSKey1[16] = { 0x10, 0x6B, 0x05, 0x99, 0xA0, 0xDB, 0xB2, 0x80, 0x94, 0xE5, 0xC4, 0x15, 0xE6, 0xB7, 0xE7, 0x77 };
		uint8_t AppSKey1[16] = { 0x25, 0xD8, 0x64, 0xE2, 0x57, 0x8F, 0xEB, 0x8D, 0x17, 0xEB, 0xBC, 0x59, 0xB0, 0x4C, 0x3C, 0xED };
		uint16_t FrameCount1 = 0x00;
		uint8_t dir1 = 0;
		uint8_t data[251];
		uint8_t pkt[300];
		scanf("%s",&data);

		for(int i=0 ; i<strlen(data); i++){
			printf("%X ",data[i]);
		}
		uint8_t len = strlen(data);
		struct UpFrame frm;


		memcpy(frm.NwkSKey,NwkSKey1,16);
		frm.FrameCount = FrameCount1;
		frm.dir = dir1;
		frm.len = len;
		memcpy(frm.DevAddr,DevAddr1,4);
		memcpy(frm.DevAddr,DevAddr1,4);
		memcpy(frm.AppSKey,AppSKey1,16);
		memcpy(frm.data,data,strlen(data));
		int complete_pkt_len = buildLoraWanPkt(frm,pkt);
		printf("%s\n",pkt);



	}
