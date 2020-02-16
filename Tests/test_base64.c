#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../libraries/Base64.h"
#include <math.h>

int main(void){
  char encodedData[1024];
  char data[128];
  scanf("%s",(char*)&data);
  int input_size = strlen(data);
  int base64size = ((input_size + ( (input_size % 3) ? (3 - (input_size % 3)) : 0) ) / 3) * 4;
  printf("Calculated length of the output before encoding : %d\n",base64size);
  printf("Size of raw data before encoding : %d\n",(int)strlen(data));
  base64_encode((char*)encodedData,data,strlen(data));
  printf("Size of data after encoding : %d\n",(int)strlen(encodedData));
  printf("\n%s\n",encodedData);
}
