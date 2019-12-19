#include "../tetris_engine/tetris_engine.h"
#include "../interface/terminal_interface.h"
#include "singleplayer_mode.h"
#include <stdio.h>

int getGamemode () {
  int choix = 0;

  while (choix != 1) {
    printf("Which gamemode ?\n");
    printf("1. Singleplayer\n");
    printf("2. 2-player (unavailable)\n");

    scanf("%d", &choix);
  }

  return choix;
}

int getPlayerType (char *question) {
  int choix = 0;

  while (choix != 1) {
    printf("%s\n", question);
    printf("1. Human\n");
    printf("2. Bot (unavailable)\n");

    scanf("%d", &choix);
  }

  return choix;
}

int main(int argc, char const *argv[]) {

  Tinterface_out IO_out = getTerminalInterfaceOut ();
  switch (getGamemode()) {
    case 1: ;
      Tinterface_in IO_in;
      switch (getPlayerType ("Please enter the player type")) {
        case 1: ;
          IO_in = getTerminalInterfaceIn ();
        break;
      }

      playSingleplayer(IO_in, IO_out);
    break;
  }

  return 0;
}
