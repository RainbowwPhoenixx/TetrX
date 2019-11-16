debug_flags = -g -D DEBUG -Wall -pedantic
object_dir = ../obj/

all:
	make general_library_debug.o type_shape_debug.o type_mino_debug.o type_tetrimino_debug.o

general_library_debug.o: general_library.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_shape_debug.o: type_shape.c general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_mino_debug.o: type_mino.c type_shape_debug.o general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@

type_tetrimino_debug.o: type_tetrimino.c type_mino_debug.o type_shape_debug.o general_library_debug.o
	gcc $(debug_flags) -c $< -o $(object_dir)$@
