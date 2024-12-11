

CC = gcc
CFLAGS ?= -Wall -O3 -fPIC

SRC  = $(notdir $(wildcard *.c)) 
OBJS = global.o graph.o metrics.o parser.o paths.o
EXEC = pathopt
LIB = pathopt.so


all: ${EXEC} ${LIB}

$(EXEC): ${OBJS}
	$(CC) main.c $(OBJS) -o $(EXEC) $(CFLAGS)

%.o : %.c
	$(CC) -c $< -o $@ ${CFLAGS}

pathopt.so: ${OBJS}
	${CC} -shared -o ${LIB} -fPIC $(OBJS)

clean:
	rm -f *.o *.d *~ *.a *.so *.s ${EXEC}

test: $(EXEC)
	./pathopt ../examples/simple-example.txt

testpy: $(LIB)
	./pathopt.py ../examples/simple-example.txt
