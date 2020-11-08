#ifndef __BOT_PARAMETERS_H__
#define __BOT_PARAMETERS_H__

// ------------------------------------------------------------------------
//                          BOT TYPES PARAMETERS
// ------------------------------------------------------------------------

// For bot custom board type
#define BOT_MATRIX_HEIGHT 45
#define BOT_MATRIX_WIDTH 10

#define MOVE_NODE_LIST_MAX_ITEMS 1000

// For move queue and bot struct
#define MAX_MOVES 80

// For tree search structure
#define MAX_MOVEMENTS 50
#define MAX_CHILDREN 200

// ------------------------------------------------------------------------
//                         GENERAL BOT PARAMETERS
// ------------------------------------------------------------------------

// Comment out to stop the bot from logging into a file
#define LOG_BOT_THINKING

// ------------------------------------------------------------------------
//                         PATHFINDER PARAMETERS
// ------------------------------------------------------------------------

#define BOT_MAX_MOVES 50
#define BOT_MAX_PREVIEWS 10 // Do not set lower than 1 or higher than MINIMUM_QUEUE_LENGTH in board_operations.c

// ------------------------------------------------------------------------
//                             BOT WEIGHTS
// ------------------------------------------------------------------------

#define WEIGHTS_FILE "weights.txt"
// Here we generate an enum, and a list of the correcponding strings
// This weird macro magic is made to make it easy to maintain
#define FOREACH_WEIGHT(WEIGHT) \
        WEIGHT(clear1                ,  -50)  \
        WEIGHT(clear2                ,  -50)  \
        WEIGHT(clear3                ,  -40)  \
        WEIGHT(clear4                ,  100)  \
        WEIGHT(tss                   ,  150)  \
        WEIGHT(tsd                   ,  300)  \
        WEIGHT(tst                   ,  450)  \
        WEIGHT(pc                    , 1000)  \
        WEIGHT(tsd_slot              ,   75)  \
        WEIGHT(tst_slot              ,  100)  \
        WEIGHT(bumpiness             ,  -.5)  \
        WEIGHT(bumpiness_sq          ,  -.1)  \
        WEIGHT(number_of_holes       ,  -80)  \
        WEIGHT(landing_height        ,   -3)  \
        WEIGHT(maximum_height        ,   -1)  \
        WEIGHT(average_height        ,   -3)  \
        WEIGHT(target_maximum_height ,    3)  \
        WEIGHT(target_average_height ,    5)  \
        WEIGHT(well0                 ,  -10)  \
        WEIGHT(well1                 ,  -10)  \
        WEIGHT(well2                 ,   -3)  \
        WEIGHT(well3                 ,    0)  \
        WEIGHT(well4                 ,    0)  \
        WEIGHT(well5                 ,    0)  \
        WEIGHT(well6                 ,    0)  \
        WEIGHT(well7                 ,   -3)  \
        WEIGHT(well8                 ,  -10)  \
        WEIGHT(well9                 ,  -10)  \
        WEIGHT(well_same_height      ,    3)  \

#define GENERATE_ENUM(ENUM, _) ENUM,
#define GENERATE_STRING(STRING, _) #STRING,
#define GENERATE_DEFAULTS(_, VAL) VAL,

// These count the amount of weights
#define COUNT_WEIGHTS(_1, _2) + 1
#define WEIGHT_COUNT (0 FOREACH_WEIGHT(COUNT_WEIGHTS))

// Finally generate the data structures
typedef enum {
  FOREACH_WEIGHT(GENERATE_ENUM)
} Tweights_enum;

static const char *weight_str[] = {
  FOREACH_WEIGHT(GENERATE_STRING)
};

// Function to create a file with the default weights
void createDefaultWeightsFile ();
void readWeights (float w[WEIGHT_COUNT]);

#endif /* end of include guard: __BOT_PARAMETERS_H__ */
