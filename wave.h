#ifndef WAVE_H_
#define WAVE_H_

/*
 * About idea.
 *
 * Wave is composed of shape and repeat.
 * shape is one period of wave.
 * And period is also shape.
 * The fundamental shape is sine line etc.
 *
 */

#include "lifetime.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* It is like vector space.
 * w0 + w1 => w2
 * a w0 => w1 */

typedef struct {
  size_t count;
  size_t period;
  size_t freq;
  double (*func)(size_t i, size_t freq, size_t sampling_freq);
} BasicWave;

BasicWave bwave_init(size_t freq, double (*func)(size_t i, size_t freq,
                                                 size_t sampling_freq)) {
  BasicWave bw;
  bw.count = 0;
  bw.freq = freq;
  bw.func = func;
  return bw;
}

double bwave_next(BasicWave *bw, size_t sampling_freq) {
  bw->count %= sampling_freq;
  double r = bw->func(bw->count, bw->freq, sampling_freq);
  bw->count++;
  return r;
}

struct Wave;
typedef struct Wave Wave;

struct Wave {
  LifeTime lt;
  /* amp * ( *w[0] + *w[1] ) -> w
   * if w[1] == NULL, means w[1] = 0
   * if last == true, wave = period.
   */
  bool last;
  double amp;
  union {
    Wave *wave[2];
    //Period *period;
  };
};

double wave_next(Wave *w, size_t sampling_freq) {
  bool live = lifetime_is_live(&w->lt);
  lifetime_next(&w->lt);

  if (live == false) {
    return 0;
  }
  if (w->last == true) {
    // printf("period_next\n");
    //return period_next(w->period, sampling_freq);
	return 0;
  }
  if (w->wave[1] == NULL) {
    return w->amp * wave_next(w->wave[0], sampling_freq);
  }
  int16_t ret = 0;
  for (size_t i = 0; i < 2; i++) {
    ret += wave_next(w->wave[i], sampling_freq);
  }
  return ret * w->amp;
}

double p_sin(size_t i, size_t freq, size_t sampling_freq) {
  // printf("i=%u, freq=%u, sampling_freq=%u\n", i, freq, sampling_freq);
  // printf("i=%lu, freq=%lu, sampling_freq=%lu\n", i, freq, sampling_freq);
  // printf("%f\n", i / sampling_freq * 2 * M_PI * freq);
  return sin(2 * M_PI * freq * i / sampling_freq);
  double s = sin(2 * M_PI * freq * i / sampling_freq);
  return s;
}

int16_t p_line(size_t i, size_t freq, size_t sampling_freq) {
  /* -1 to 1 */
  return i / sampling_freq * freq / 2 - 1;
}

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
#endif
