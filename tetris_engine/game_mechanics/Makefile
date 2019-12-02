debug_flags = -g -D DEBUG -Wall -pedantic
object_dir = ../../obj/
all_targets = board_operations_debug.o

all:
	make $(all_targets)

board_operations_debug.o: board_operations.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@
