#ifndef LIFETIME_H_
#define LIFETIME_H_
#include<stdio.h>
#include <stdbool.h>
#include <stddef.h>
typedef struct {
  size_t counter;
  size_t start;
  size_t end;
} LifeTime;

LifeTime lifetime_init(size_t start, size_t end) {
  LifeTime lt;
  lt.counter=0;
  lt.start=start;
  lt.end=end;
  return lt;
}

bool lifetime_is_live(LifeTime *lt) {
  return lt->start <= lt->counter && lt->counter < lt->end;
}

bool lifetime_is_dead(LifeTime *lt) {
  return lifetime_is_live(lt) == true ? false : true;
}

void lifetime_next(LifeTime *lt) { lt->counter++; }

bool lifetime_test(void) {
  LifeTime lt = lifetime_init(10, 20);
  bool ans[30] = {0};
  for (size_t i = 10; i < 20; i++) {
    ans[i] = true;
  }
  for (size_t i = 0; i < 30; i++) {
    if (ans[i] != lifetime_is_live(&lt)) {
      perror("lifetime test failed.");
      printf("i=%zu\n", i);
      return false;
    }
    lifetime_next(&lt);
  }
  return true;
}

#endif
