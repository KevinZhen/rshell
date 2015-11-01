all: rshell
rshell:
	rm -rf bin
	mkdir bin
	g++ -Wall -Werror -pedantic src/main.cpp -o bin/rshell
