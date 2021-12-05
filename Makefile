CC = g++
debug_flags = -O3 -Wall -std=c++11 -g -lm
flags = -O3 -Wall -std=c++11 -lm

port = 58039  # Port in which our server if going to be running
ip = 120.0.0.1

# Compiles udp server
su: server/src/main.cpp
	$(CC) $(flags) -o ./server/bin/main ./server/src/main.cpp

# Compiles client
c: client/src/main.cpp
	$(CC) $(flags) -o ./client/bin/main ./client/src/main.cpp

# Runs udp server
rsu: su
	./server/bin/main

# Runs client
rc: c
	./client/bin/main -p $(port) -n $(ip)

# Cleans everything
clean:
	rm -f ./server/bin/* ./client/bin/*