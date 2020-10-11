PROGRAMS = fbgame

fbgame: main.o
	gcc -o $@ $^

.c.o: 
	gcc -c $<

clean: 
	rm -f $(PROGRAMS) *.o
