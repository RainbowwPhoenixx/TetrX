#include "singleplayer_mode.h"
#include "../bot/bot.h"

Tbot bot;
extern Tinterface_in IO_in;
extern Tinterface_out IO_out;

static void addClearToStats (Tline_clear clear, Tstatistics *stats) {
  switch (clear.attack_kind) {
    case NONE: break;
    case NORMAL:
      switch (clear.nb_of_lines) {
        case 1: stats->clear_single++; break;
        case 2: stats->clear_double++; break;
        case 3: stats->clear_triple++; break;
        case 4: stats->clear_tetris++; break;
        default :return;
      }
      break;
    case TSPIN:
      switch (clear.nb_of_lines) {
        case 1:stats->clear_tss++; break;
        case 2:stats->clear_tsd++; break;
        case 3:stats->clear_tst++; break;
      }
      break;
    case PC: stats->clear_pc++;
  }
  stats->nb_of_pieces++;
}

static void initGame (Tboard b) {
  // Initialise the game
  IO_out.initDisplayFunc();
  IO_out.displaySkinFunc();
  IO_in.initInputFunc(&bot, b);
}
static void endGame () {
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
    Tline_clear clear = clearLines (b);
    setBoardHasHeldThisTurnStatus (b, false);
    setBoardShouldEndTurnStatus (b, false);
    
    addClearToStats (clear, getBoardStats (b));
  }
}

void playSingleplayer () {
  // Play out a game of singleplayer tetris

  initRandom ();
  Tboard b = createBoard ();
  initGame(b);

  while (!getBoardHasLostStatus (&b)) {
    beginTurn(&b);
    do {
      Tmovement mv = IO_in.getInputFunc(&bot);
      applyInput (&b, mv);
      
      IO_out.showBoardFunc (&b);
      IO_out.updateScreenFunc ();
    } while(!getBoardShouldEndTurnStatus (&b));

    endTurn(&b);
  }

  endGame();
}
