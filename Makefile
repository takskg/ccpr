ccpr: src/main.c
		gcc -o dist/ccpr src/main.c

test: clean ccpr
		dist/ccpr --test
		test/test.sh

clean:
		rm -f ccpr *.o *~ tmp/*