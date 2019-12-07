debug_flags = -g -D DEBUG -Wall -pedantic
object_dir = ../obj/
all_targets = singleplayer_mode_debug.o

all:
	make $(all_targets)

singleplayer_mode_debug.o: singleplayer_mode.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@
