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

  Tcoordinate_diff (*initial_mino_pos)[2];

  switch (tetrimino_shape) {
    case L: initial_mino_pos = L_initial_mino_positions[0]; break;
    case J: initial_mino_pos = J_initial_mino_positions[0]; break;
    case S: initial_mino_pos = S_initial_mino_positions[0]; break;
    case Z: initial_mino_pos = Z_initial_mino_positions[0]; break;
    case O: initial_mino_pos = O_initial_mino_positions[0]; break;
    case I: initial_mino_pos = I_initial_mino_positions[0]; break;
    case T: initial_mino_pos = T_initial_mino_positions[0]; break;
    case EMPTY: break;
  }

  for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
    Tmino tmp_mino = createMino (initial_mino_pos[i][0], initial_mino_pos[i][1]);
    setIthMino (&t, i, tmp_mino);
  }

  return t;
}
