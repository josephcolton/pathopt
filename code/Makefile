CC =  /usr/bin/gcc
CFLAGS  = -Wall -g -std=c99
CFLAGS += -O3 -fopenmp -lm
CFLAGS += -D_POSIX_C_SOURCE=200112L

SRC  = $(notdir $(wildcard *.c)) 
OBJS = $(addsuffix .o, $(basename $(SRC)))
EXEC = pathopt

all: ${EXEC}

${EXEC}: ${OBJS}
	${CC} $^ -o $@ ${CFLAGS}

%.o : %.c
	$(CC) -c $< -o $@ ${CFLAGS}

clean:
	rm -f *.o *.d *~ *.a *.so *.s ${EXEC}

test:
	OMP_NUM_THREADS=8 ./pathopt ../examples/simple-example.txt
