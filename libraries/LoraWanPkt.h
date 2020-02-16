#include <stdint.h>
#include <string.h>


//Creating a structure containing all parameters needed to build the LoraWan packet
//This structure will be passed to the buildLoraWanPkt function


struct LoraWan {
	uint8_t DevAddr[4];
	uint16_t FrameCount;
	uint8_t NwkSKey[16];
	uint8_t AppSKey[16];
	uint8_t dir;
};

uint8_t buildLoraWanPkt(struct LoraWan param, uint8_t *data_in, uint8_t *data_out);
static void mXor(uint8_t *buf, uint8_t *key);
static void shift_left(uint8_t * buf, uint8_t len);
static void generate_subkey(uint8_t *key, uint8_t *k1, uint8_t *k2);
uint8_t micPacket(uint8_t *data, uint8_t len, uint16_t FrameCount, uint8_t * NwkSKey, uint8_t dir,char DevAddr[]);
uint8_t encodePacket(uint8_t *Data, uint8_t encryptedDataLength, uint16_t FrameCount, uint8_t *DevAddr, uint8_t *AppSKey, uint8_t Direction);
void buildHeader(uint8_t size,uint8_t *Payload, uint8_t encryptedDataLength,uint8_t *DevAddr,uint16_t FrameCount);
