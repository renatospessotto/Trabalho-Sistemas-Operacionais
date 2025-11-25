CC = gcc
CFLAGS = -Wall -pthread -lncurses -Wno-deprecated-declarations

OBJS = main.o fruta.o etapas.o interface.o input.o

fabrica: $(OBJS)
	$(CC) -o fabrica $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o fabrica
