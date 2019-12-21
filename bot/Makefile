object_dir = ../obj/
all_debug_targets = bot_debug.o tree_debug.o
all_targets = tree.o bot.o

all_debug:
	make $(all_debug_targets)

%_debug.o: %.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@

all:
	make $(all_targets)

%.o: %.c
	gcc $(normal_flags) -c $< -o $(object_dir)$@
