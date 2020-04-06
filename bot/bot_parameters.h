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
#define BOT_MAX_PREVIEWS 2 // Do not set lower than 1

#endif /* end of include guard: __BOT_PARAMETERS_H__ */
