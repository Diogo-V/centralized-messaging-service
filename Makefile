CC = g++
debug_flags = -O3 -Wall -std=c++11 -g -lm
flags = -O3 -Wall -std=c++11 -lm

# Compiles udp server and runs it
su: server/src/main.cpp
	$(CC) $(flags) -o ./server/bin/main ./server/src/main.cpp
	./server/bin/main

cmp: server/src/main.cpp
	$(CC) $(flags) -o ./server/bin/main ./server/src/main.cpp

rsu: cmp
	time ./server/bin/main

clean:
	rm -f ./server/bin/*