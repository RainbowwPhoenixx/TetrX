#include "type_tetrimino.h"

#define TETRIMINO_SPAWN_X 4
#define TETRIMINO_SPAWN_Y 20

static Tcoordinate_diff L_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}}, // Initial pos for the L
  {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
  {{-1,  0}, { 0,  0}, { 1,  0}, {-1, -1}},
  {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}},
};
static Tcoordinate_diff J_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}}, // Initial pos for the J
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
  {{-1,  0}, { 0,  0}, { 1,  0}, { 1, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, {-1, -1}},
};
static Tcoordinate_diff S_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}}, // Initial pos for the S
  {{ 0,  1}, { 0,  0}, { 1,  0}, { 1, -1}},
  {{ 1,  0}, { 0,  0}, { 0, -1}, {-1, -1}},
  {{ 0, -1}, { 0,  0}, {-1,  0}, {-1,  1}},
};
static Tcoordinate_diff Z_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{ 0,  0}, { 1,  0}, { 0,  1}, {-1,  1}}, // Initial pos for the Z
  {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},
  {{ 0,  0}, {-1,  0}, { 0, -1}, { 1, -1}},
  {{ 0,  1}, { 0,  0}, {-1,  0}, {-1, -1}},
};
static Tcoordinate_diff O_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}, // Initial pos for the O
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
};
static Tcoordinate_diff I_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}}, // Initial pos for the I
  {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
  {{-1, -1}, { 0, -1}, { 1, -1}, { 2, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 0,  2}},
};
static Tcoordinate_diff T_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}}, // Initial pos for the T
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  0}},
  {{-1,  0}, { 0,  0}, { 1,  0}, { 0, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, {-1,  0}},
};

static void setTetriminoMinos (Ttetrimino *t, Tbyte rotation_state) {
  Tcoordinate_diff (*initial_mino_pos)[2];

  switch (getTetriminoShape (t)) {
    case L: initial_mino_pos = L_initial_mino_positions[rotation_state]; break;
    case J: initial_mino_pos = J_initial_mino_positions[rotation_state]; break;
    case S: initial_mino_pos = S_initial_mino_positions[rotation_state]; break;
    case Z: initial_mino_pos = Z_initial_mino_positions[rotation_state]; break;
    case O: initial_mino_pos = O_initial_mino_positions[rotation_state]; break;
    case I: initial_mino_pos = I_initial_mino_positions[rotation_state]; break;
    case T: initial_mino_pos = T_initial_mino_positions[rotation_state]; break;
    case EMPTY: break;
  }

  for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
    Tmino tmp_mino = createMino (initial_mino_pos[i][0], initial_mino_pos[i][1]);
    setIthMino (t, i, tmp_mino);
  }
}

// Accessors
Tshape getTetriminoShape (Ttetrimino *t) {
  return t->shape;
}
void setTetriminoShape (Ttetrimino *t, Tshape new_shape) {
  t->shape = new_shape;
}

Tmino *getIthMino (Ttetrimino *t, int i) {
  return &(t->minos[i]);
}
void setIthMino (Ttetrimino *t, int i, Tmino new_mino) {
  t->minos[i] = new_mino;
}

Tcoordinate getTetriminoX (Ttetrimino *t) {
  return t->x;
}
void setTetriminoX (Ttetrimino *t, Tcoordinate new_x) {
  t->x = new_x;
}

Tcoordinate getTetriminoY (Ttetrimino *t) {
  return t->y;
}
void setTetriminoY (Ttetrimino *t, Tcoordinate new_y) {
  t->y = new_y;
}

static Trotation_state getTetriminoRotationState (Ttetrimino *t) {
  return t->rotation_state;
}
static void setTetriminoRotationState (Ttetrimino *t, Trotation_state rot_state) {
  t->rotation_state = rot_state;
}

// Constructor
Ttetrimino createTetrimino (Tshape tetrimino_shape) {
  Ttetrimino t;

  setTetriminoShape (&t, tetrimino_shape);

  if (tetrimino_shape == EMPTY) {
    setTetriminoX (&t, 0);
    setTetriminoY (&t, 0);
    return t;
  }

  setTetriminoX (&t, TETRIMINO_SPAWN_X);
  setTetriminoY (&t, TETRIMINO_SPAWN_Y);

  setTetriminoMinos (&t, 0);
  setTetriminoRotationState (&t, R0);

  return t;
}

// Useful functions

// Translation
void moveTetriminoRight (Ttetrimino *t) {
  setTetriminoX (t, getTetriminoX (t) + 1);
}
void moveTetriminoLeft (Ttetrimino *t) {
  setTetriminoX (t, getTetriminoX (t) - 1);
}
void moveTetriminoUp (Ttetrimino *t) {
  setTetriminoY (t, getTetriminoY (t) + 1);
}
void moveTetriminoDown (Ttetrimino *t) {
  setTetriminoY (t, getTetriminoY (t) - 1);
}

// Rotation
void moveTetriminoCW (Ttetrimino *t) {
  setTetriminoRotationState (t, (getTetriminoRotationState (t)+1) % 4);
  setTetriminoMinos (t, getTetriminoRotationState (t));
}
void moveTetriminoCCW (Ttetrimino *t) {
  setTetriminoRotationState (t, (getTetriminoRotationState (t)-1) % 4);
  setTetriminoMinos (t, getTetriminoRotationState (t));
}

// Copy
void copyTetrimino (Ttetrimino *dest, Ttetrimino *src) {
  setTetriminoShape (dest, getTetriminoShape (src));
  setTetriminoRotationState (dest, getTetriminoRotationState (src));
  setTetriminoX (dest, getTetriminoX (src));
  setTetriminoY (dest, getTetriminoY (src));

  for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
    copyMino (getIthMino (dest, i), getIthMino (src, i));
  }
}
