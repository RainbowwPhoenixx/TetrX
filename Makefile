object_dir = obj/
tetris_engine_dir = tetris_engine/

.PHONY: tetris_engine #Declare the targets that don't correspond to files

tetris_engine:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(tetris_engine_dir) all

clean:
	-rm bin/*
	-rm obj/*
