#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bot_parameters.h"

// ------------------------------------------------------------------------
//                             BOT WEIGHTS
// ------------------------------------------------------------------------

#define WEIGHT_FORMAT "%-25s %f\n"

static const float default_weights[WEIGHT_COUNT] = {FOREACH_WEIGHT(GENERATE_DEFAULTS)};

void createDefaultWeightsFile () {
  FILE *weights = fopen (WEIGHTS_FILE, "w");
  for (int i = 0; i < WEIGHT_COUNT; i++) {
    fprintf(weights, WEIGHT_FORMAT, weight_str[i], default_weights[i]);
  }
  fclose (weights);
}

void readWeights (float w[WEIGHT_COUNT]) {
  char name_buf[100];
  FILE *weights = fopen (WEIGHTS_FILE, "r");
  
  for (int i = 0; i < WEIGHT_COUNT; i++) {
    // Read the data
    fscanf(weights, "%s", name_buf);
    // If strings are different, weights file is invalid
    if (strcmp(name_buf, weight_str[i]) != 0) {
      printf("%s\n", name_buf);
      printf("Invalid weigth file\n");
      exit(-1);
    }
    
    fscanf(weights, "%f", &w[i]);
  }
  fclose (weights);
}

#undef WEIGHT_FORMAT
