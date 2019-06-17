CFLAGS=-std=c11 -Wall -g -static
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

ccpr: $(OBJS)
		mkdir dist
		$(CC) -o dist/ccpr $(OBJS) $(LDFLAGS)

$(OBJS): src/ccpr.h

test: clean ccpr
		dist/ccpr --test
		test/test.sh

clean:
		rm -f ccpr src/*.o *~ tmp/* dist/*