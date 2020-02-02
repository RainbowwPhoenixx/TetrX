link_flags = -lncurnses `sdl2-config --cflags --libs`
object_dir = ../obj/
all_targets = terminal_interface.o
all_debug_targets = $(all_targets:.o=_debug.o)

# Tell make to not delete the files
.PRECIOUS: $(object_dir)%.o $(object_dir)%_debug.o
#Cancel implicit rule
%.o: %.c
#Debug version
all_debug: $(all_debug_targets) ;
%_debug.o: $(object_dir)%_debug.o ;
$(object_dir)%_debug.o: %.c
	gcc $(debug_flags) -c $< -o $@ $(link_flags)

#Release version
all: $(all_targets) ;
%.o: $(object_dir)%.o ;
$(object_dir)%.o: %.c
	gcc $(normal_flags) -c $< -o $@ $(link_flags)
