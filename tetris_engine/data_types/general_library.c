#include "general_library.h"
#include <stdlib.h>
#include <time.h>

void initRandom () {
  // Initializes the seed
  srand(time(0));
}

unsigned int getRandomInteger (unsigned int max) {
  // Returns a number in the range [0, max[
  return (rand() % max);
}


// --------------------------------------------------------------------------
//                         LINE CLEAR TYPES
// --------------------------------------------------------------------------

Tline_clear createLineClear (Tbyte nb_lines, Tattack_kind attack) {
  Tline_clear res = {.nb_of_lines = nb_lines, .attack_kind = attack};
  return res;
}
