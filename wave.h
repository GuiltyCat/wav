#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {
  unsigned bit_size;
  unsigned nchannel;
  uint32_t sampling_freq;
  size_t length;
  int16_t signal[];
} Wav;

Wav *wav_init(size_t len) {
  Wav *wav = malloc(sizeof(Wav) + sizeof(int16_t) * len);
  if (wav == NULL) {
    return NULL;
  }
  wav->bit_size = 16;
  wav->nchannel = 1;
  wav->sampling_freq = 80000;
  wav->length = len;
  return wav;
}

int read_wav(FILE *fp) {
  uint8_t header[44] = {0};
  size_t ret = fread(header, sizeof(uint8_t), 44, fp);
  if (ret != 44) {
    perror("header read failed.");
    return -1;
  }
  const char RIFF[] = "RIFF";
  for (int i = 0; i < 4; i++) {
    if (header[i] != RIFF[i]) {
      perror("RIFF not match.");
      return -1;
    }
  }
  int32_t chunk_size = 0;
  for (int i = 0; i < 4; i++) {
    chunk_size += header[4 + i] << i * 8;
  }
  printf("chunk size = %d.\n", chunk_size);
  const char format[] = "WAVE";
  for (int i = 0; i < 4; i++) {
    if (header[8 + i] != format[i]) {
      perror("WAVE match failed.");
      return -1;
    }
  }

  int32_t length = chunk_size - 36;
  uint8_t *signal = malloc(sizeof(uint8_t) * length);
  ret = fread(signal, sizeof(uint8_t), length, fp);
  if (ret != (uint32_t)length) {
    perror("length is not correct.");
    free(signal);
    return -1;
  }
  printf("length = %d.\n", length);

  return 0;
}

int write_wav(FILE *fp, Wav *wav) {
  if (fp == NULL) {
    return -1;
  }

  uint8_t header[44] = {0};

  /* ChunkID */
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  /* ChunkSize */
  int32_t chunk_size = 4 + 24 + 8 + wav->length * (wav->bit_size / 8);
  for (int i = 0; i < 4; i++) {
    header[4 + i] = (chunk_size >> i * 8) & 0xFF;
  }
  // printf("header_size=%d\n", 4 + 24 + 8);
  // printf("data size = %lu\n", wav->length * (wav->bit_size / 8));
  // printf("chunk_size=%d\n", chunk_size);

  /* Format */
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';

  /* Subchunk1ID */
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';

  /* Subchunk1Size */
  uint32_t subchunk1size = 16;
  for (int i = 0; i < 4; i++) {
    header[16 + i] = subchunk1size >> i * 8 & 0xFF;
  }

  /* AudioFormat. PCM=1 */
  header[20] = 1;
  header[21] = 0;

  /* NumChannels. Moono=1, Stereo=2 */
  header[22] = wav->nchannel & 0xFF;
  header[23] = 0;

  /* Sample Rate */
  for (int i = 0; i < 4; i++) {
    header[24 + i] = wav->sampling_freq >> i * 8 & 0xFF;
  }

  /* ByteRate */
  uint32_t byte_rate = wav->sampling_freq * wav->nchannel * wav->bit_size / 8;
  for (int i = 0; i < 4; i++) {
    header[28 + i] = byte_rate >> i * 8 & 0xFF;
  }
  /* BlockAlign */
  uint16_t block_align = wav->nchannel * wav->bit_size / 8;
  for (int i = 0; i < 2; i++) {
    header[32 + i] = block_align >> i * 8 & 0xFF;
  }
  /* BitsPerSample */
  uint16_t bits_per_sample = wav->bit_size;
  if (bits_per_sample != 16) {
    perror("currently bits_per_sample can be only 16.");
  }
  for (int i = 0; i < 2; i++) {
    header[34 + i] = bits_per_sample >> i * 8 & 0xFF;
  }

  /* Subchunk2ID */
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';

  /* Subchunk2Size */
  uint32_t subchunk2size = wav->length * wav->bit_size / 8;
  for (int i = 0; i < 4; i++) {
    header[40 + i] = (uint8_t)(subchunk2size >> i * 8) & 0xFF;
  }
  // printf("subchunk2size = %d\n", subchunk2size);

  size_t ret;
  ret = fwrite(header, sizeof(uint8_t), sizeof(header) / sizeof(uint8_t), fp);
  if (ret != sizeof(header) / sizeof(uint8_t)) {
    perror("fwrite header size is not equal.");
    return -1;
  }

  size_t nmemb = wav->length * wav->bit_size / 8;
  ret = fwrite(wav->signal, sizeof(uint8_t), nmemb, fp);
  // printf("nmemb = %lu.\n", nmemb);
  if (ret != nmemb) {
    perror("bit_size == 8. fwrite failed.");
    return -1;
  }

  return 0;
}

/* do not use. only for simple test. */

int16_t *test_sine(int16_t *wave, size_t len, double amp, double freq,
                   uint32_t sampling_freq) {
  for (size_t i = 0; i < len; i++) {
    double s = amp * sin(2 * M_PI * i * freq / sampling_freq);
    wave[i] = (int16_t)s;
  }
  return wave;
}

int wav_test(void){
  FILE *fp = fopen("test.wav", "wb");
  if (fp == NULL) {
    perror("fp == NULL");
    return -1;
  }

  size_t length = 1000000;
  Wav* wav = wav_init(length);
  if (wav == NULL){
	  perror("wav == NULL");
	  return -1;
  }

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

/********************************************
 * Design Wave
 ********************************************/

int16_t *fill(int16_t *wave, size_t len, int16_t v) {
  for (size_t i = 0; i < len; i++) {
    wave[i] = v;
  }
  return wave;
}

int16_t *add_wave(int16_t *to, int16_t *from, size_t len) {
  for (size_t i = 0; i < len; i++) {
    to[i] += from[i];
  }
  return to;
}

int16_t *multi_wave(int16_t *wave, size_t len, int16_t num, int16_t den) {
  for (size_t i = 0; i < len; i++) {
    wave[i] *= num / den;
  }
  return wave;
}

int16_t *prod_wave(int16_t *to, int16_t *from, size_t len) {
  for (size_t i = 0; i < len; i++) {
    to[i] *= from[i];
  }
  return to;
}

int16_t *div_wave(int16_t *to, int16_t *from, size_t len) {
  for (size_t i = 0; i < len; i++) {
    to[i] /= from[i];
  }
  return to;
}

int16_t *line(int16_t *wave, size_t len, int16_t start, int16_t end) {
  size_t tlen = len - 1;
  for (size_t i = 0; i < len; i++) {
    wave[i] = start * (tlen - i) / tlen + end * i / tlen;
  }
  return wave;
}

/* phase shift can be done by shifting starting point */
// int16_t *shift_phase(int16_t* wave){return wave;}
int16_t *sine(int16_t *wave, size_t len) {
  /* create one wave in range of len */
  for (size_t i = 0; i < len; i++) {
    wave[i] = sin(i * 2 * M_PI / len);
  }
  return wave;
}

size_t sec2len(double sec, uint32_t sampling_freq) {
  return sec * sampling_freq;
}

size_t freq2len(double freq, uint32_t sampling_freq) {
  /* return length of one wave */
  return sampling_freq / freq;
}

int16_t *repeat(int16_t *wave, size_t len, int16_t *period, size_t plen) {
  for (size_t i = 0; i < len; i++) {
    wave[i] = period[i % plen];
  }
  return wave;
}
