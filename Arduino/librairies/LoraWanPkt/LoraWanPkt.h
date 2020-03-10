#include <stdint.h>
#include <string.h>


//Creating a structure containing all parameters needed to build the LoraWan packet
//This structure will be passed to the buildLoraWanPkt function
int buildLoraWanPkt(uint8_t *data_in,uint8_t data_len,uint8_t dir,uint8_t FrameCount);
void buildHeader(uint8_t *Payload,uint8_t size ,uint8_t *DevAddr,uint16_t FrameCount);
void mXor(uint8_t *buf, uint8_t *key);
void generate_subkey(uint8_t *key, uint8_t *k1, uint8_t *k2);
uint8_t micPacket(uint8_t *data, uint8_t len, uint16_t FrameCount, uint8_t * NwkSKey, uint8_t dir);
uint8_t encodePacket(uint8_t *Data, uint8_t DataLength, uint16_t FrameCount, uint8_t *DevAddr, uint8_t *AppSKey, uint8_t Direction);
