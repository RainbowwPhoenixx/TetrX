object_dir = ../../obj/
all_debug_targets = general_library_debug.o type_shape_debug.o type_movement_debug.o type_mino_debug.o type_next_queue_debug.o type_matrix_debug.o type_tetrimino_debug.o type_board_debug.o
all_targets = general_library.o type_shape.o type_movement.o type_mino.o type_next_queue.o type_matrix.o type_tetrimino.o type_board.o

all_debug:
	make $(all_debug_targets)

%_debug.o: %.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@

all:
	make $(all_targets)

%.o: %.c
	gcc $(normal_flags) -c $< -o $(object_dir)$@
