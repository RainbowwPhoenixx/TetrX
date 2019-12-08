object_dir = ../obj/
all_debug_targets = singleplayer_mode_debug.o
all_targets = singleplayer_mode.o

all_debug:
	make $(all_debug_targets)

%_debug.o: %.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@

all:
	make $(all_targets)

%.o: %.c
	gcc $(normal_flags) -c $< -o $(object_dir)$@
