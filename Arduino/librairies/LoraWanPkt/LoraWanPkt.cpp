#include <AES-128_V10.h>
#include <Wire.h>
#include <SPI.h>
#include "LoraWanPkt.h"
#include "LoraNodeParam.h"
#define DEBUG


int buildLoraWanPkt(uint8_t *data_in,uint8_t data_len,uint8_t dir,uint8_t FrameCount){


//--------------------------------------------------------------------------------------//

  #ifdef DEBUG
    Serial.print("[Raw data] ");
   for (uint8_t i = 0; i < data_len; i++) {
    Serial.print(data_in[i],HEX);
    Serial.print(" ");
   }
   Serial.println();
   #endif
//--------------------------------------------------------------------------------------//

     encodePacket(data_in,data_len,FrameCount,DevAddr,AppSKey,dir);

//--------------------------------------------------------------------------------------//
  #ifdef DEBUG
    Serial.print("[Encoded data] ");
   for (uint8_t i = 0; i < data_len; i++) {
    Serial.print(data_in[i],HEX);
    Serial.print(" ");
   }
   Serial.println();
   #endif
//--------------------------------------------------------------------------------------//

  buildHeader(data_in,data_len,DevAddr,FrameCount);

//--------------------------------------------------------------------------------------//
  #ifdef DEBUG
    Serial.print("[Header + Encoded data] ");
   for (uint8_t i = 0; i < data_len+9; i++) {
    Serial.print(data_in[i],HEX);
    Serial.print(" ");
   }
   Serial.println();
   #endif
//--------------------------------------------------------------------------------------//

  micPacket(data_in,data_len+9,FrameCount,NwkSKey,dir);


//--------------------------------------------------------------------------------------//
  #ifdef DEBUG
    Serial.print("[Header + Encoded data + MIC] ");
   for (uint8_t i = 0; i < data_len+13; i++) {
    Serial.print(data_in[i],HEX);
    Serial.print(" ");
   }
   Serial.println();
   #endif
//--------------------------------------------------------------------------------------//
return(data_len+13);
}

void buildHeader(uint8_t *Payload,uint8_t size ,uint8_t *DevAddr,uint16_t FrameCount){

    memmove(Payload+9,Payload,size);
    Payload[0]= 0x40;//Message Type : 0x40 Unconfirmed data UP use 0x80 for Confirmed data UP
    //for more information see LoraWan Speficication chapter 4.2.1

    //These 4 bytes represents the Dev Address
    Payload[1]=DevAddr[3];//LSB
    Payload[2]=DevAddr[2];
    Payload[3]=DevAddr[1];
    Payload[4]=DevAddr[0];

    Payload[5]=0x00;

    //Theses 4 bytes represents the Frae counter, we only use 2 of them (so 16 bits)
    Payload[6]=FrameCount&0x00FF; //LSB
    Payload[7]=(FrameCount>>8)&0x00FF;

    Payload[8]=0x01;//Fport value


    #ifdef DEBUG
      Serial.print("[Payload + Header [2]] ");
     for (uint8_t i = 0; i < size+9; i++) {
      Serial.print(Payload[i],HEX);
      Serial.print(" ");
     }
     Serial.println();
     Serial.print("size :");
     Serial.print(size);
     Serial.println();
     #endif


  }


// ----------------------------------------------------------------------------
// XOR()
// perform x-or function for buffer and key
// Since we do this ONLY for keys and X, Y we know that we need to XOR 16 bytes.
//
// ----------------------------------------------------------------------------
void mXor(uint8_t *buf, uint8_t *key)
{
  for (uint8_t i = 0; i < 16; ++i) buf[i] ^= key[i];
}


// ----------------------------------------------------------------------------
// SHIFT-LEFT
// Shift the buffer buf left one bit
// Parameters:
//  - buf: An array of uint8_t bytes
//  - len: Length of the array in bytes
// ----------------------------------------------------------------------------
void shift_left(uint8_t * buf, uint8_t len)
{
    while (len--) {
        uint8_t next = len ? buf[1] : 0;      // len 0 to 15

        uint8_t val = (*buf << 1);
        if (next & 0x80) val |= 0x01;
        *buf++ = val;
    }
}


// ----------------------------------------------------------------------------
// generate_subkey
// RFC 4493, para 2.3
// -----------------------------------------------------------------------------
void generate_subkey(uint8_t *key, uint8_t *k1, uint8_t *k2)
{

  memset(k1, 0, 16);                // Fill subkey1 with 0x00

  // Step 1: Assume k1 is an all zero block
  AES_Encrypt(k1,key);

  // Step 2: Analyse outcome of Encrypt operation (in k1), generate k1
  if (k1[0] & 0x80) {
    shift_left(k1,16);
    k1[15] ^= 0x87;
  }
  else {
    shift_left(k1,16);
  }

  // Step 3: Generate k2
  for (int i=0; i<16; i++) k2[i]=k1[i];

  if (k1[0] & 0x80) {               // use k1(==k2) according rfc
    shift_left(k2,16);
    k2[15] ^= 0x87;
  }
  else {
    shift_left(k2,16);
  }

  // step 4: Done, return k1 and k2
  return;
}


// ----------------------------------------------------------------------------
// MICPACKET()
// Provide a valid MIC 4-byte code (par 2.4 of spec, RFC4493)
//    see also https://tools.ietf.org/html/rfc4493
//
// Although our own handler may choose not to interpret the last 4 (MIC) bytes
// of a PHYSPAYLOAD physical payload message of in internal sensor,
// The official TTN (and other) backends will interpret the complete message and
// conclude that the generated message is bogus.
// So we will really simulate internal messages coming from the -1ch gateway
// to come from a real sensor and append 4 MIC bytes to every message that are
// perfectly legimate
// Parameters:
//  - data:     uint8_t array of bytes = ( MHDR | FHDR | FPort | FRMPayload )
//  - len:      8=bit length of data, normally less than 64 bytes
//  - FrameCount: 16-bit framecounter
//  - dir:      0=up, 1=down
//
// B0 = ( 0x49 | 4 x 0x00 | Dir | 4 x DevAddr | 4 x FCnt |  0x00 | len )
// MIC is cmac [0:3] of ( aes128_cmac(NwkSKey, B0 | Data )
//
// ----------------------------------------------------------------------------
uint8_t micPacket(uint8_t *data, uint8_t len, uint16_t FrameCount, uint8_t * NwkSKey, uint8_t dir)
{


  //uint8_t NwkSKey[16] = _NWKSKEY;
  uint8_t Block_B[16];
  uint8_t X[16];
  uint8_t Y[16];

  // ------------------------------------
  // build the B block used by the MIC process
  Block_B[0]= 0x49;           // 1 byte MIC code

  Block_B[1]= 0x00;           // 4 byte 0x00
  Block_B[2]= 0x00;
  Block_B[3]= 0x00;
  Block_B[4]= 0x00;

  Block_B[5]= dir;            // 1 byte Direction

  Block_B[6]= DevAddr[3];         // 4 byte DevAddr
  Block_B[7]= DevAddr[2];
  Block_B[8]= DevAddr[1];
  Block_B[9]= DevAddr[0];

  Block_B[10]= (FrameCount & 0x00FF);   // 4 byte FCNT
  Block_B[11]= ((FrameCount >> 8) & 0x00FF);
  Block_B[12]= 0x00;            // Frame counter upper Bytes
  Block_B[13]= 0x00;            // These are not used so are 0

  Block_B[14]= 0x00;            // 1 byte 0x00

  Block_B[15]= len;           // 1 byte len

  // ------------------------------------
  // Step 1: Generate the subkeys
  //
  uint8_t k1[16];
  uint8_t k2[16];
  generate_subkey(NwkSKey, k1, k2);

  // ------------------------------------
  // Copy the data to a new buffer which is prepended with Block B0
  //
  uint8_t micBuf[len+16];         // B0 | data
  for (uint8_t i=0; i<16; i++) micBuf[i]=Block_B[i];
  for (uint8_t i=0; i<len; i++) micBuf[i+16]=data[i];

  // ------------------------------------
  // Step 2: Calculate the number of blocks for CMAC
  //
  uint8_t numBlocks = len/16 + 1;     // Compensate for B0 block
  if ((len % 16)!=0) numBlocks++;     // If we have only a part block, take it all

  // ------------------------------------
  // Step 3: Calculate padding is necessary
  //
  uint8_t restBits = len%16;        // if numBlocks is not a multiple of 16 bytes


  // ------------------------------------
  // Step 5: Make a buffer of zeros
  //
  memset(X, 0, 16);

  // ------------------------------------
  // Step 6: Do the actual encoding according to RFC
  //
  for(uint8_t i= 0x0; i < (numBlocks - 1); i++) {
    for (uint8_t j=0; j<16; j++) Y[j] = micBuf[(i*16)+j];
    mXor(Y, X);
    AES_Encrypt(Y, NwkSKey);
    for (uint8_t j=0; j<16; j++) X[j] = Y[j];
  }


  // ------------------------------------
  // Step 4: If there is a rest Block, padd it
  // Last block. We move step 4 to the end as we need Y
  // to compute the last block
  //
  if (restBits) {
    for (uint8_t i=0; i<16; i++) {
      if (i< restBits) Y[i] = micBuf[((numBlocks-1)*16)+i];
      if (i==restBits) Y[i] = 0x80;
      if (i> restBits) Y[i] = 0x00;
    }
    mXor(Y, k2);
  }
  else {
    for (uint8_t i=0; i<16; i++) {
      Y[i] = micBuf[((numBlocks-1)*16)+i];
    }
    mXor(Y, k1);
  }
  mXor(Y, X);
  AES_Encrypt(Y,NwkSKey);

  // ------------------------------------
  // Step 7: done, return the MIC size.
  // Only 4 bytes are returned (32 bits), which is less than the RFC recommends.
  // We return by appending 4 bytes to data, so there must be space in data array.
  //
  data[len+0]=Y[0];
  data[len+1]=Y[1];
  data[len+2]=Y[2];
  data[len+3]=Y[3];
                    // MMM to avoid crashes

  return 4;
}


// ----------------------------------------------------------------------------
// ENCODEPACKET
// In Sensor mode, we have to encode the user payload before sending.
// The same applies to decoding packages in the payload for _LOCALSERVER.
// The library files for AES are added to the library directory in AES.
// For the moment we use the AES library made by ideetron as this library
// is also used in the LMIC stack and is small in size.
//
// The function below follows the LoRa spec exactly.
//
// The resulting mumber of Bytes is returned by the functions. This means
// 16 bytes per block, and as we add to the last block we also return 16
// bytes for the last block.
//
// The LMIC code does not do this, so maybe we shorten the last block to only
// the meaningful bytes in the last block. This means that encoded buffer
// is exactly as big as the original message.
//
// NOTE:: Be aware that the LICENSE of the used AES library files
//  that we call with AES_Encrypt() is GPL3. It is used as-is,
//  but not part of this code.
//
// cmac = aes128_encrypt(K, Block_A[i])
// ----------------------------------------------------------------------------
uint8_t encodePacket(uint8_t *Data, uint8_t DataLength, uint16_t FrameCount, uint8_t *DevAddr, uint8_t *AppSKey, uint8_t Direction)
{



  //unsigned char AppSKey[16] = _APPSKEY ;  // see configGway.h
  uint8_t i, j;
  uint8_t Block_A[16];
  uint8_t bLen=16;            // Block length is 16 except for last block in message

  uint8_t restLength = DataLength % 16; // We work in blocks of 16 bytes, this is the rest
  uint8_t numBlocks  = DataLength / 16; // Number of whole blocks to encrypt
  if (restLength>0) numBlocks++;      // And add block for the rest if any

  for(i = 1; i <= numBlocks; i++) {
    Block_A[0] = 0x01;

    Block_A[1] = 0x00;
    Block_A[2] = 0x00;
    Block_A[3] = 0x00;
    Block_A[4] = 0x00;

    Block_A[5] = Direction;       // 0 is uplink

    Block_A[6] = DevAddr[3];      // Only works for and with ABP
    Block_A[7] = DevAddr[2];
    Block_A[8] = DevAddr[1];
    Block_A[9] = DevAddr[0];

    Block_A[10] = (FrameCount & 0x00FF);
    Block_A[11] = ((FrameCount >> 8) & 0x00FF);
    Block_A[12] = 0x00;         // Frame counter upper Bytes
    Block_A[13] = 0x00;         // These are not used so are 0

    Block_A[14] = 0x00;

    Block_A[15] = i;

    // Encrypt and calculate the S
    AES_Encrypt(Block_A, AppSKey);

    // Last block? set bLen to rest
    if ((i == numBlocks) && (restLength>0)) bLen = restLength;

    for(j = 0; j < bLen; j++) {
      *Data = *Data ^ Block_A[j];
      Data++;
    }
  }
  //return(numBlocks*16);     // Do we really want to return all 16 bytes in lastblock
  return(DataLength);       // or only 16*(numBlocks-1)+bLen;
}
