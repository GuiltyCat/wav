#include <stdint.h>
#include <stdio.h>
typedef struct {
  int bit_size;
  int nchannel;
  int32_t sampling_freq;
  size_t length;
  int16_t signal[];
} Wav;

int read_wav(FILE *fp);
int write_wav(FILE *fp, Wav *wav);
int write_sine(int16_t *wave, size_t length, double amp, double freq, int32_t sampling_freq);
