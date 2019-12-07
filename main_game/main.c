#include "../tetris_engine/tetris_engine.h"
#include "../interface/terminal_interface.h"
#include "singleplayer_mode.h"
#include <ncurses.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {

  Tinterface_in IO_in = getTerminalInterfaceIn ();
  Tinterface_out IO_out = getTerminalInterfaceOut ();

  playSingleplayer(IO_in, IO_out);

  return 0;
}
