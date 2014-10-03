all:

Collatz.log:
	git log > Collatz.log

Doxyfile:
	doxygen -g

html: Doxyfile Collatz.h Collatz.c++ RunCollatz.c++ TestCollatz.c++
	doxygen Doxyfile

Australian: Australian.c++
	g++ -pedantic -std=c++11 -Wall Australian.c++ -o Australian

clean:
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f RunCollatz
	rm -f RunCollatz.out
	rm -f RunCollatz.tmp
	rm -f TestCollatz
	rm -f TestCollatz.out
