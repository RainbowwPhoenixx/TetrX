#include "singleplayer_mode.h"

static void initGame (Tinterface_in IO_in, Tinterface_out IO_out) {
  IO_out.initDisplayFunc();
  IO_out.displaySkinFunc();
  IO_in.initInputFunc();

  initRandom ();
}
static void endGame (Tinterface_in IO_in, Tinterface_out IO_out) {
  IO_in.endInputFunc();
  IO_out.endDisplayFunc ();
}

static void beginTurn (Tboard *b) {
  // Get new tetrimino from the queue

  popTetriminoFromQueue (b);
}
static void endTurn (Tboard *b) {
  // Perform tetrimino locking, line clearing, reset hold status, detect loss

  checkLoss (b);
  if (!getBoardHasLostStatus(b)) {
    lockActiveTetrimino (b);
    clearLines (b);
    setBoardHasHeldThisTurnStatus (b, false);
    setBoardShouldEndTurnStatus (b, false);
  }
}

void playSingleplayer (Tinterface_in IO_in, Tinterface_out IO_out) {

  initGame(IO_in, IO_out);
  Tboard b = createBoard ();

  while (!getBoardHasLostStatus (&b)) {
    beginTurn(&b);
    do {
      Tmovement mv = IO_in.getInputFunc();
      applyInput (&b, mv);
      IO_out.showBoardFunc (&b);
      IO_out.updateScreenFunc ();
    } while(!getBoardShouldEndTurnStatus (&b));

    endTurn(&b);
    usleep(16666); // ~60fps
  }

  endGame(IO_in, IO_out);
}
