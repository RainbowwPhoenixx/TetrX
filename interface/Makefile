# debug_flags = -g -D DEBUG -Wall -pedantic
# normal_flags = -O3
link_flags = -lncurnses `sdl2-config --cflags --libs`
object_dir = ../obj/
all_debug_targets = terminal_interface_debug.o
all_targets = terminal_interface.o

all_debug:
	make $(all_debug_targets)

%_debug.o: %.c
	gcc $(debug_flags) -c $< -o $(object_dir)$@ $(link_flags)

all:
	make $(all_targets)

%.o: %.c
	gcc $(normal_flags) -c $< -o $(object_dir)$@ $(link_flags)
