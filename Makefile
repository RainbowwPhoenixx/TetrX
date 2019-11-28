object_dir = obj/
tetris_engine_dir = tetris_engine/
interface_dir = interface/

.PHONY: tetris_engine interface #Declare the targets that don't correspond to files

interface:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(interface_dir) all

tetris_engine:
	if ! [ -d $(object_dir) ]; then mkdir $(object_dir); fi
	$(MAKE) -C $(tetris_engine_dir) all

clean:
	-rm bin/*
	-rm obj/*
