object_dir = obj/
binaries_dir = bin/
tetris_engine_dir = tetris_engine/
interface_dir = interface/
gamemodes_dir = main_game/
bot_dir = bot/
export debug_flags = -g -D DEBUG -Wall -pedantic
export normal_flags = -O3
linking_flags = -lncurses -lpthread `sdl2-config --cflags --libs`
all_tetris_engine_targets := $(addprefix $(object_dir), general_library.o type_shape.o type_movement.o type_mino.o type_next_queue.o type_matrix.o type_tetrimino.o type_board.o board_operations.o)
all_interface_targets := $(addprefix $(object_dir), terminal_interface.o)
all_gamemodes_targets := $(addprefix $(object_dir), singleplayer_mode.o)
all_bot_targets := $(addprefix $(object_dir), bot_types.o tree.o move_queue.o bot.o)
all_tetris_engine_debug_targets := $(all_tetris_engine_targets:.o=_debug.o)
all_interface_debug_targets := $(all_interface_targets:.o=_debug.o)
all_gamemodes_debug_targets := $(all_gamemodes_targets:.o=_debug.o)
all_bot_debug_targets := $(all_bot_targets:.o=_debug.o)


.PHONY: tetris_engine interface gamemodes bot clean view_log MKOBJ_DIR MKBIN_DIR #Declare the targets that don't correspond to files

interface: MKOBJ_DIR
	$(MAKE) -C $(interface_dir) all

tetris_engine: MKOBJ_DIR
	$(MAKE) -C $(tetris_engine_dir) all

gamemodes: MKOBJ_DIR
	$(MAKE) -C $(gamemodes_dir) all

bot: MKOBJ_DIR
	$(MAKE) -C $(bot_dir) all

interface_debug: MKOBJ_DIR
	$(MAKE) -C $(interface_dir) all_debug

tetris_engine_debug: MKOBJ_DIR
	$(MAKE) -C $(tetris_engine_dir) all_debug

gamemodes_debug: MKOBJ_DIR
	$(MAKE) -C $(gamemodes_dir) all_debug

bot_debug: MKOBJ_DIR
	$(MAKE) -C $(bot_dir) all_debug

TetrX_debug: main_game/main.c interface_debug tetris_engine_debug gamemodes_debug bot_debug | MKBIN_DIR
	gcc $(debug_flags) $(all_tetris_engine_debug_targets) $(all_interface_debug_targets) $(all_gamemodes_debug_targets) $(all_bot_debug_targets) $< -o $(binaries_dir)$@ $(linking_flags)

TetrX: main_game/main.c interface tetris_engine gamemodes bot | MKBIN_DIR
	gcc $(normal_flags) $(all_tetris_engine_targets) $(all_interface_targets) $(all_gamemodes_targets) $(all_bot_targets) $< -o $(binaries_dir)$@ $(linking_flags)

MKOBJ_DIR:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi

MKBIN_DIR:
	if ! [ -d $(binaries_dir) ]; then mkdir $(binaries_dir); fi

view_log:
	gnome-terminal -- tail -f bot_debug.log

clean:
	-rm bin/*
	-rm obj/*

.DEFAULT_GOAL := TetrX
