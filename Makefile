object_dir = obj/
binaries_dir = bin/
tetris_engine_dir = tetris_engine/
interface_dir = interface/
gamemodes_dir = main_game/
export debug_flags = -g -D DEBUG -Wall -pedantic
export normal_flags = -O3
all_tetris_engine_debug_targets = $(object_dir)general_library_debug.o $(object_dir)type_shape_debug.o $(object_dir)type_movement_debug.o $(object_dir)type_mino_debug.o $(object_dir)type_next_queue_debug.o $(object_dir)type_matrix_debug.o $(object_dir)type_tetrimino_debug.o $(object_dir)type_board_debug.o $(object_dir)board_operations_debug.o
all_interface_debug_targets = $(object_dir)terminal_interface_debug.o
all_gamemodes_debug_targets = $(object_dir)singleplayer_mode_debug.o
all_tetris_engine_targets = $(object_dir)general_library.o $(object_dir)type_shape.o $(object_dir)type_movement.o $(object_dir)type_mino.o $(object_dir)type_next_queue.o $(object_dir)type_matrix.o $(object_dir)type_tetrimino.o $(object_dir)type_board.o $(object_dir)board_operations.o
all_interface_targets = $(object_dir)terminal_interface.o
all_gamemodes_targets = $(object_dir)singleplayer_mode.o


.PHONY: tetris_engine interface clean #Declare the targets that don't correspond to files

interface:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(interface_dir) all

tetris_engine:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(tetris_engine_dir) all

gamemodes:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(gamemodes_dir) all

interface_debug:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(interface_dir) all_debug

tetris_engine_debug:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(tetris_engine_dir) all_debug

gamemodes_debug:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(gamemodes_dir) all_debug


TetrX_debug: main_game/main.c interface_debug tetris_engine_debug gamemodes_debug
	if ! [ -d $(binaries_dir) ]; then mkdir $(binaries_dir); fi
	gcc $(debug_flags) $(all_tetris_engine_debug_targets) $(all_interface_debug_targets) $(all_gamemodes_debug_targets) $< -o $(binaries_dir)$@ -lncurses

TetrX: main_game/main.c interface tetris_engine gamemodes
	if ! [ -d $(binaries_dir) ]; then mkdir $(binaries_dir); fi
	gcc $(normal_flags) $(all_tetris_engine_targets) $(all_interface_targets) $(all_gamemodes_targets) $< -o $(binaries_dir)$@ -lncurses


clean:
	-rm bin/*
	-rm obj/*
