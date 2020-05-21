#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "wave.h"


int main(void) {

  FILE *fp = fopen("test.wav", "wb");
  if (fp == NULL) {
    perror("fp == NULL");
    return -1;
  }

  size_t length = 1000000;
  Wav* wav = malloc(sizeof(Wav)+sizeof(int16_t)*length);
  if (wav == NULL){
	  perror("wav == NULL");
	  return -1;
  }
  wav->bit_size = 16;
  wav->nchannel = 1;
  wav->sampling_freq = 80000;
  wav->length = length;

  test_sine(wav->signal, wav->length, 2000.0, 130, wav->sampling_freq);

  write_wav(fp, wav);

  fclose(fp);

  fp = fopen("test.wav", "rb");
  if (fp == NULL) {
    perror("fopen rb failed.");
    return -1;
  }

  read_wav(fp);
  fclose(fp);

  return 0;
}
