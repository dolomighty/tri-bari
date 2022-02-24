
LIBS   := `pkg-config --libs   sdl` -lm   
CFLAGS := `pkg-config --cflags sdl` -O3 -Werror



.PHONY : all clean cl rebuild re

all : main

run : main
	./$^

main : main.o tri.o render.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

#DYN+=draw_scene_gl.h
#draw_scene_gl.h : scenes/scene.obj obj2c.sh
#	./obj2c.sh scenes/scene.obj > $@


clean cl :
	file * | awk '/ELF/ { gsub(/:.*/,"") ; print }' | xargs -r rm
	rm -f deps.inc $(DYN)

rebuild re : clean all





# 2016-09-08 12:08:27
# funziona, ma non capisco come:
# deps.inc non è un requisito di nessun target ... perche viene generato ?
# forse anche include si comporta un pò come un target
# cmq ... in caso aggiungere altre wildcard qui sotto. solo files esistenti!!
deps.inc : *.c
	gcc -M -MG $^ > deps.inc
-include deps.inc


