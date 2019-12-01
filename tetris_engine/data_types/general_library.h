#ifndef __GENERAL_LIBRARY_H__
#define __GENERAL_LIBRARY_H__

// Declaration of general purpose types
typedef unsigned char Tbyte;
typedef signed char Tcoordinate;    // Only 1 byte needed to represent a coordinate
typedef signed char Tcoordinate_diff; // Only 1 byte needed to represent a coordinate difference

// Declaration of constants
#define C_MATRIX_HEIGHT 45
#define C_MATRIX_WIDTH 10
#define C_QUEUE_LENGTH 20

// General purpose random function prototypes
void initRandom ();
unsigned int getRandomInteger (unsigned int max);


#endif
