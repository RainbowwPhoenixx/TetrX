#include <ncurses.h>
#include "terminal_interface.h"
#include "../tetris_engine/tetris_engine.h"
#include <unistd.h>

// Definition of the color IDs for each of the shapes
#define COL_L 1
#define COL_J 2
#define COL_S 3
#define COL_Z 4
#define COL_O 5
#define COL_I 6
#define COL_T 7
#define COL_EMPTY 8

#define MAX_CHAR_SKIN_WIDTH 85

// Definition of the different window sizes for each of the interface regions
#define MATRIX_WINDOW_X 12
#define MATRIX_WINDOW_Y 1
#define MATRIX_WINDOW_HEIGHT 21
#define MATRIX_WINDOW_WIDTH 20

#define MATRIX_DISPLAY_HEIGHT 21
#define MATRIX_DISPLAY_WIDTH 10

#define NEXT_WINDOW_X 33
#define NEXT_WINDOW_Y 1
#define NEXT_WINDOW_HEIGHT 14
#define NEXT_WINDOW_WIDTH 10

#define NEXT_DISPLAY_AMOUNT 5
#define NEXT_DISPLAY_SPACING 3

#define HOLD_WINDOW_X 1
#define HOLD_WINDOW_Y 1
#define HOLD_WINDOW_HEIGHT 2
#define HOLD_WINDOW_WIDTH 10

#define LINES_WINDOW_X 1
#define LINES_WINDOW_Y 10
#define LINES_WINDOW_HEIGHT 1
#define LINES_WINDOW_WIDTH 10

// Definition of the duration of a frame (in microseconds)
#define FRAME_DURATION 16666
// Definition of the different delays (in # of frames)
#define LINE_CLEAR_DELAY 0
#define ENTRY_DELAY 0

const char mino_skin[] = "  ";

// Declare all of the windows that define the different interface areas
static WINDOW *win_matrix;
static WINDOW *win_hold;
static WINDOW *win_next;
static WINDOW *win_score;
static WINDOW *win_lines;

// Output

static void initDisplay () {
  // Initialises everything that is necessary for the display
  initscr(); // Init ncurses
  start_color(); // Inits the colors and color pairs macros
  use_default_colors(); // Make the terminal not look like shit
  curs_set(FALSE); // Turn off cursor

  // Define all the colors
  init_color (COL_L    , 1000,  500,    0);
  init_color (COL_J    ,    0,    0, 1000);
  init_color (COL_S    ,    0, 1000,    0);
  init_color (COL_Z    , 1000,    0,    0);
  init_color (COL_O    , 1000, 1000,    0);
  init_color (COL_I    ,    0, 1000, 1000);
  init_color (COL_T    , 1000,    0, 1000);
  init_pair (COL_L    , COLOR_WHITE, COL_L    );
  init_pair (COL_J    , COLOR_WHITE, COL_J    );
  init_pair (COL_S    , COLOR_WHITE, COL_S    );
  init_pair (COL_Z    , COLOR_WHITE, COL_Z    );
  init_pair (COL_O    , COLOR_WHITE, COL_O    );
  init_pair (COL_I    , COLOR_WHITE, COL_I    );
  init_pair (COL_T    , COLOR_WHITE, COL_T    );
  init_pair (COL_EMPTY, COLOR_WHITE, -1       );

  // Init the windows
  win_matrix = newwin (MATRIX_WINDOW_HEIGHT, MATRIX_WINDOW_WIDTH, MATRIX_WINDOW_Y, MATRIX_WINDOW_X);
  win_next = newwin (NEXT_WINDOW_HEIGHT, NEXT_WINDOW_WIDTH, NEXT_WINDOW_Y, NEXT_WINDOW_X);
  win_hold = newwin (HOLD_WINDOW_HEIGHT, HOLD_WINDOW_WIDTH, HOLD_WINDOW_Y, HOLD_WINDOW_X);
  win_lines = newwin (LINES_WINDOW_HEIGHT, LINES_WINDOW_WIDTH, LINES_WINDOW_Y, LINES_WINDOW_X);
}
static void resetScreen () {
  // Clears the whole terminal
  clear();
}
static void updateScreen() {
  // Refreshes all of the surfaces
  wnoutrefresh (stdscr);
  wnoutrefresh (win_matrix);
  wnoutrefresh (win_next);
  wnoutrefresh (win_hold);
  wnoutrefresh (win_lines);
  doupdate();
  usleep(FRAME_DURATION);
}
static void endDisplay () {
  // Does all of the necessary operations to close the display
  delwin(win_matrix);
  delwin(win_hold);
  delwin(win_next);
  delwin(win_score);
  delwin(win_lines);
  endwin();
}
static void displaySkin () {
  // Get the skin from the skin file and display it on stdscr
  FILE *fp = fopen("res/terminal_skin", "r");
  char line[MAX_CHAR_SKIN_WIDTH];
  Tbyte i = 0;
  while (fgets(line, MAX_CHAR_SKIN_WIDTH, fp) != NULL) {
    mvaddstr (i, 0, line);
    i++;
  }
  refresh();
}

static int getColorFromShape (Tshape shape) {
  // Returns the color ID corresponding to the given shape
  switch (shape) {
    case L    : return COL_L    ;
    case J    : return COL_J    ;
    case S    : return COL_S    ;
    case Z    : return COL_Z    ;
    case O    : return COL_O    ;
    case I    : return COL_I    ;
    case T    : return COL_T    ;
    case EMPTY: return COL_EMPTY;
  }

  return 0;
}

static void translateCoordinates (Tcoordinate x, Tcoordinate y, Tcoordinate x_origin, Tcoordinate y_origin, Tcoordinate *real_x, Tcoordinate *real_y) {
  // Translates a pair of coordinates from the mino coordinate system into the terminal coordinate system
  *real_x = x_origin + 2*x;
  *real_y = y_origin - y;
}
static void showMinoAtTerminalCoords (WINDOW *win, Tcoordinate x, Tcoordinate y) {
  // Displays a mino at (x,y) in the terminal
  mvwaddstr(win, y, x, mino_skin);
}

static void showNextQueue (Tnext_queue *next_queue) {
  // Shows the pieces of the next queue in the right spot

  // Clears the next window
  werase(win_next);

  // For each piece in the queue that needs to be displayed,
  // create the tetrimino associated with the shape and display it
  for (Tbyte j = 0; j < NEXT_DISPLAY_AMOUNT; j++) {
    Ttetrimino tmp_t = createTetrimino (getIthNextPiece (next_queue, j));
    Ttetrimino *t = &tmp_t;
    if (getTetriminoShape (t) != EMPTY) {
      Tcoordinate real_x, real_y;
      Tshape tmp_shape = getTetriminoShape (t);
      wattron (win_next, COLOR_PAIR(getColorFromShape (tmp_shape)));

      // Display each mino
      for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
        Tmino *tmp_mino = getIthMino (t, i);
        translateCoordinates (1 + getMinoXDiff (tmp_mino), 0 + getMinoYDiff (tmp_mino), 1, 1 + NEXT_DISPLAY_SPACING*j, &real_x, &real_y);
        showMinoAtTerminalCoords (win_next, real_x, real_y);
      }

      wattroff (win_next, COLOR_PAIR(getColorFromShape (tmp_shape)));
    }
  }
}
static void showHold (Tshape hold) {
  // Displays the current held piece

  // Resets the hold window
  werase (win_hold);

  // Create the tetrimino associated with the held shape, then display it
  Ttetrimino tmp_t = createTetrimino (hold);
  Ttetrimino *t = &tmp_t;
  if (getTetriminoShape (t) != EMPTY) {
    Tcoordinate real_x, real_y;
    Tshape tmp_shape = getTetriminoShape (t);
    wattron (win_hold, COLOR_PAIR(getColorFromShape (tmp_shape)));

    // Display each mino
    for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
      Tmino *tmp_mino = getIthMino (t, i);
      translateCoordinates (1 + getMinoXDiff (tmp_mino), 0 + getMinoYDiff (tmp_mino), 1, HOLD_WINDOW_HEIGHT-1, &real_x, &real_y);
      showMinoAtTerminalCoords (win_hold, real_x, real_y);
    }

    wattroff (win_hold, COLOR_PAIR(getColorFromShape (tmp_shape)));
  }
}
static void showActiveTetrimino (Ttetrimino *t) {
  // Displays the current active tetrimino in the matrix window
  if (getTetriminoShape (t) != EMPTY) {
    Tcoordinate real_x, real_y;
    Tshape tmp_shape = getTetriminoShape (t);
    wattron (win_matrix, COLOR_PAIR(getColorFromShape (tmp_shape)));

    // Display each mino
    for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
      Tmino *tmp_mino = getIthMino (t, i);
      translateCoordinates (getTetriminoX (t) + getMinoXDiff (tmp_mino), getTetriminoY (t) + getMinoYDiff (tmp_mino), 0, MATRIX_WINDOW_HEIGHT-1, &real_x, &real_y);
      showMinoAtTerminalCoords (win_matrix, real_x, real_y);
    }

    wattroff (win_matrix, COLOR_PAIR(getColorFromShape (tmp_shape)));
  }
}
static void showMatrix (Tmatrix *matrix) {
  // Displays the minos in the matrix
  Tcoordinate real_x, real_y;

  // For each mino in the matrix, compute its position the display it
  for (Tcoordinate i = 0; i < MATRIX_DISPLAY_WIDTH; i++) {
    for (Tcoordinate j = 0; j < MATRIX_DISPLAY_HEIGHT; j++) {
      Tshape tmp_shape = getMatrixShapeAtPos (matrix, i, j);
      wattron (win_matrix, COLOR_PAIR(getColorFromShape (tmp_shape)));
      translateCoordinates (i, j, 0, MATRIX_WINDOW_HEIGHT-1, &real_x, &real_y);
      showMinoAtTerminalCoords (win_matrix, real_x, real_y);
      wattroff (win_matrix, COLOR_PAIR(getColorFromShape (tmp_shape)));
    }
  }
}
static void showLinesCleared (Tline_counter lines) {
  // wborder (win_lines, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  wmove (win_lines, 0, 0);
  wprintw (win_lines, "%d", lines);
}

static void showBoard (Tboard *board) {
  // Global function to display all the board components
  showMatrix (getBoardMatrix (board));
  showActiveTetrimino (getBoardActiveTetrimino (board));
  showHold (getBoardHoldPiece (board));
  showNextQueue (getBoardNextQueue (board));
  showLinesCleared (getBoardLinesCleared (board));
}

// Input

static void initInput () {
  // Initialises the input system
  noecho ();
  cbreak();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
}
static Tmovement getInput () {
  // Gets input from the player
  Tmovement input = createMovementWord();
  char ch;

  while ((ch = getch ()) != ERR) {
    switch (ch) {
      case 'q' : addMovementToWord (&input, MV_LEFT); break;
      case 'd' : addMovementToWord (&input, MV_RIGHT); break;
      case 'l' : addMovementToWord (&input, MV_CW); break;
      case 'm' : addMovementToWord (&input, MV_CCW); break;
      case 's' : addMovementToWord (&input, MV_SD); break;
      case '*' : addMovementToWord (&input, MV_HD); break;
      case 'z' : addMovementToWord (&input, MV_HOLD); break;
    }
  }

  return input;
}
static void endInput() {
  // Function to end the input system
}
static void nop () {}

// Interface struct consructors

Tinterface_out getTerminalInterfaceOut () {
  // Construct the terminal interface output structure
  Tinterface_out IO_out;

  IO_out.initDisplayFunc = initDisplay;
  IO_out.displaySkinFunc = displaySkin;
  IO_out.resetScreenFunc = resetScreen;
  IO_out.updateScreenFunc = updateScreen;
  IO_out.endDisplayFunc = endDisplay;
  IO_out.showBoardFunc = showBoard;

  return IO_out;
}

Tinterface_in getTerminalInterfaceIn () {
  // Construct the terminal interface input structure
  Tinterface_in IO_in;

  IO_in.initInputFunc = initInput;
  IO_in.getInputFunc = getInput;
  IO_in.endInputFunc = endInput;
  IO_in.addNewBagFunc = nop;

  return IO_in;
}
