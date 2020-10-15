PROGRAMS = fbgame
CC = gcc -std=gnu99

all: $(PROGRAMS)

fbgame: main.o
	$(CC) -o $@ $^

.c.o: 
	$(CC) -c $<

clean: 
	rm -f $(PROGRAMS) *.o
