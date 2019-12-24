#ifndef __TYPE_INTERFACE_H__
#define __TYPE_INTERFACE_H__

#include "../tetris_engine/tetris_engine.h"

// Type definitions for the interface

// Types for general display
typedef void (*TinitDisplay)();
typedef void (*TdisplaySkin)();
typedef void (*TresetScreen)();
typedef void (*TupdateScreen)();
typedef void (*TendDisplay)();
typedef void (*TshowBoard)(Tboard*);

typedef struct {
  TinitDisplay initDisplayFunc;
  TdisplaySkin displaySkinFunc;
  TresetScreen resetScreenFunc;
  TupdateScreen updateScreenFunc;
  TendDisplay endDisplayFunc;
  TshowBoard showBoardFunc;
} Tinterface_out;

// Types for the input functions
typedef void (*TinitInput)();
typedef Tmovement (*TgetInput)();
typedef void (*TendInput)();
typedef void (*TaddNewBag)(); // This is bot-specific, and used to give the new next pieces to the bot.

typedef struct {
  TinitInput initInputFunc;
  TgetInput getInputFunc;
  TendInput endInputFunc;
  TaddNewBag addNewBagFunc;
} Tinterface_in;

#endif
