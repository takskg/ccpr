ccpr: src/main.c

test: clean ccpr
		test/test.sh

clean:
		rm -f ccpr *.o *~ tmp/*