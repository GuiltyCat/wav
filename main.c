#include "wav_format.h"
#include "wave.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int wave_test(void) {
  // Period p;
  // p.end = 10000;
  // p.counter = 0;
  // p.freq = 440;
  // p.func = p_sin;
  Wave w;
  size_t time = 1000;
  LifeTime lt = lifetime_init(10, 100);
  w.lt = lt;
  w.last = true;
  w.amp = 100;
  // w.period = &p;

  for (size_t i = 0; i < time; i++) {
    printf("%lu %f\n", i, wave_next(&w, 80000));
  }
  return 0;
}
/*
bool period_test(void) {
  size_t time = 1000;
  Period p;
  p.end = 1000;
  p.counter = 0;
  p.phase = 0;
  p.freq = 10;
  p.func = p_sin;
  for (size_t i = 0; i < time; i++) {
    printf("%lu %f\n", i, 100 * period_next(&p, 100));
  }
  return true;
}
*/

void p_sin_test(void) {
  for (size_t i = 0; i < 200; i++) {
    printf("%f\n", p_sin(i, 1, 100));
  }
}

void b_sin_test(void) {
  size_t sfreq = 100;
  size_t freq = 13;
  BasicWave b = bwave_init(freq, p_sin);

  for (size_t i = 0; i < 800; i++) {
    printf("%f %f\n", bwave_next(&b, sfreq), p_sin(i, freq, sfreq));
  }
}

int main(void) {
  b_sin_test();
  // p_sin_test();
  // period_test();
  // wav_test();
  // wave_test();
  // lifetime_test();
  return 0;
}
