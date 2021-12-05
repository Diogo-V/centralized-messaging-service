CC = g++
debug_flags = -O3 -Wall -std=c++11 -g -lm
flags = -O3 -Wall -std=c++11 -lm

# Compiles udp server
su: server/src/main.cpp
	$(CC) $(flags) -o ./server/bin/main ./server/src/main.cpp

# Runs udp server
rsu: su
	./server/bin/main

# Cleans everything
clean:
	rm -f ./server/bin/* ./client/bin/*