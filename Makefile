object_dir = obj/
binaries_dir = bin/
tetris_engine_dir = tetris_engine/
interface_dir = interface/
gamemodes_dir = main_game/
bot_dir = bot/
export debug_flags = -g -D DEBUG -Wall -pedantic
export normal_flags = -O3
linking_flags = -lncurses -lpthread `sdl2-config --cflags --libs`
all_tetris_engine_debug_targets = $(object_dir)general_library_debug.o $(object_dir)type_shape_debug.o $(object_dir)type_movement_debug.o $(object_dir)type_mino_debug.o $(object_dir)type_next_queue_debug.o $(object_dir)type_matrix_debug.o $(object_dir)type_tetrimino_debug.o $(object_dir)type_board_debug.o $(object_dir)board_operations_debug.o
all_interface_debug_targets = $(object_dir)terminal_interface_debug.o
all_gamemodes_debug_targets = $(object_dir)singleplayer_mode_debug.o
all_bot_debug_targets = $(object_dir)bot_types_debug.o $(object_dir)tree_debug.o $(object_dir)move_queue_debug.o $(object_dir)node_queue_debug.o $(object_dir)bot_debug.o
all_tetris_engine_targets = $(object_dir)general_library.o $(object_dir)type_shape.o $(object_dir)type_movement.o $(object_dir)type_mino.o $(object_dir)type_next_queue.o $(object_dir)type_matrix.o $(object_dir)type_tetrimino.o $(object_dir)type_board.o $(object_dir)board_operations.o
all_interface_targets = $(object_dir)terminal_interface.o
all_gamemodes_targets = $(object_dir)singleplayer_mode.o
all_bot_targets = $(object_dir)bot_types.o $(object_dir)tree.o $(object_dir)move_queue.o $(object_dir)node_queue.o $(object_dir)bot.o


.PHONY: tetris_engine interface gamemodes bot clean view_log #Declare the targets that don't correspond to files

interface:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(interface_dir) all

tetris_engine:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(tetris_engine_dir) all

gamemodes:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(gamemodes_dir) all

bot:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(bot_dir) all

interface_debug:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(interface_dir) all_debug

tetris_engine_debug:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(tetris_engine_dir) all_debug

gamemodes_debug:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(gamemodes_dir) all_debug

bot_debug:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(bot_dir) all_debug

TetrX_debug: main_game/main.c interface_debug tetris_engine_debug gamemodes_debug bot_debug
	if ! [ -d $(binaries_dir) ]; then mkdir $(binaries_dir); fi
	gcc $(debug_flags) $(all_tetris_engine_debug_targets) $(all_interface_debug_targets) $(all_gamemodes_debug_targets) $(all_bot_debug_targets) $< -o $(binaries_dir)$@ $(linking_flags)

TetrX: main_game/main.c interface tetris_engine gamemodes bot
	if ! [ -d $(binaries_dir) ]; then mkdir $(binaries_dir); fi
	gcc $(normal_flags) $(all_tetris_engine_targets) $(all_interface_targets) $(all_gamemodes_targets) $(all_bot_targets) $< -o $(binaries_dir)$@ $(linking_flags)

view_log:
	gnome-terminal -- tail -f bot_debug.log

clean:
	-rm bin/*
	-rm obj/*
