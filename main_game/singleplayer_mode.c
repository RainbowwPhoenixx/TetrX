#include "singleplayer_mode.h"
#include "../bot/bot.h"

Tbot bot;

static void initGame (Tinterface_in IO_in, Tinterface_out IO_out, Tboard b) {
  // Initialise the game
  IO_out.initDisplayFunc();
  IO_out.displaySkinFunc();
  IO_in.initInputFunc(&bot, b);
}
static void endGame (Tinterface_in IO_in, Tinterface_out IO_out) {
  // Function to run right before the game ends
  IO_in.endInputFunc(&bot);
  IO_out.endDisplayFunc ();
}

static void beginTurn (Tboard *b) {
  // Get new tetrimino from the queue

  popTetriminoFromQueue (b);
}
static void endTurn (Tboard *b) {
  // Perform tetrimino locking, line clearing, reset hold status, loss detection

  checkLoss (b);
  if (!getBoardHasLostStatus(b)) {
    lockActiveTetrimino (b);
    clearLines (b);
    setBoardHasHeldThisTurnStatus (b, false);
    setBoardShouldEndTurnStatus (b, false);
  }
}

void playSingleplayer (Tinterface_in IO_in, Tinterface_out IO_out) {
  // Play out a game of singleplayer tetris

  initRandom ();
  Tboard b = createBoard ();
  initGame(IO_in, IO_out, b);

  while (!getBoardHasLostStatus (&b)) {
    beginTurn(&b);
    do {
      Tmovement mv = IO_in.getInputFunc(&bot);
      applyInput (&b, mv);
      IO_out.showBoardFunc (&b);
      IO_out.updateScreenFunc ();
      usleep(16666); // ~60fps
    } while(!getBoardShouldEndTurnStatus (&b));

    endTurn(&b);
  }

  endGame(IO_in, IO_out);
}
