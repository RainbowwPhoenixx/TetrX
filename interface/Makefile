debug_flags = -g -D DEBUG -Wall -pedantic
object_dir = ../obj/
all_targets = terminal_interface_debug.o

all:
	make $(all_targets)

terminal_interface_debug.o: terminal_interface.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@ -lncurses
