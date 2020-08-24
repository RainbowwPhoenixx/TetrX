#include "../tetris_engine/tetris_engine.h"
#include "../interface/terminal_interface.h"
#include "../bot/bot.h"
#include "singleplayer_mode.h"
#include <stdio.h>
#include <string.h>

Tinterface_out IO_out;
Tinterface_in IO_in;

void displayHelpAndExit () {
  printf("Usage :\n$ ./TetrX <Player type>\n\n");
  printf("Where 'Player type' is either 'bot' or 'human'\n");
  exit(0);
}

int main(int argc, char const *argv[]) {
  
  if (argc != 2) {
    displayHelpAndExit ();
  }
  
  if (strcmp(argv[1], "bot") == 0) {
    IO_in = getBotInterface();
  } else if (strcmp(argv[1], "human") == 0) {
    IO_in = getTerminalInterfaceIn ();
  } else {
    displayHelpAndExit();
  }

  printf("Press any key to start ");
  getchar();
  IO_out = getTerminalInterfaceOut ();
  playSingleplayer();

  return 0;
}
