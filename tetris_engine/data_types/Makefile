object_dir = ../../obj/
all_targets = general_library.o type_shape.o type_movement.o type_mino.o type_next_queue.o type_matrix.o type_tetrimino.o type_board.o
all_debug_targets = $(all_targets:.o=_debug.o)

# Tell make to not delete the files
.PRECIOUS: $(object_dir)%.o $(object_dir)%_debug.o
#Cancel implicit rule
%.o: %.c
#Debug version
all_debug: $(all_debug_targets) ;
%_debug.o: $(object_dir)%_debug.o ;
$(object_dir)%_debug.o: %.c
	gcc $(debug_flags) -c $< -o $@

#Release version
all: $(all_targets) ;
%.o: $(object_dir)%.o ;
$(object_dir)%.o: %.c
	gcc $(normal_flags) -c $< -o $@
