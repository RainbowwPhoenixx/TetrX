debug_flags = -g -D DEBUG -Wall -pedantic
object_dir = ../obj/
all_targets = general_library_debug.o type_shape_debug.o type_movement_debug.o type_mino_debug.o type_next_queue_debug.o type_matrix_debug.o type_tetrimino_debug.o type_board_debug.o

all:
	make $(all_targets)

general_library_debug.o: general_library.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_shape_debug.o: type_shape.c general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_movement_debug.o: type_movement.c general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_mino_debug.o: type_mino.c type_shape_debug.o general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_next_queue_debug.o: type_next_queue.c type_shape_debug.o general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_matrix_debug.o: type_matrix.c type_shape_debug.o general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_tetrimino_debug.o: type_tetrimino.c type_mino_debug.o type_shape_debug.o general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_board_debug.o: type_board.c type_tetrimino_debug.o type_mino_debug.o type_shape_debug.o general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@
