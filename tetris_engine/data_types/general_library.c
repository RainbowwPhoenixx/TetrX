#include "general_library.h"
#include <stdlib.h>
#include <time.h>

void initRandom () {
  srand(time(0));
}

unsigned int getRandomInteger (unsigned int max) {
  // Returns a number in the range [0, max[
  return (rand() % max);
}
