CC = g++
debug_flags = -O3 -Wall -std=c++11 -g -lm
flags = -O3 -Wall -std=c++11 -lm

port = 58011  # Port in which our server is going to run locally
ip = tejo.tecnico.ulisboa.pt

server_models = ./server/src/models/*.cpp ./server/src/models/*.h
server_source = ./server/src/*.cpp ./server/src/*.h

# COMPILE SERVER -> Compiles udp + tcp server and all its dependencies
cs: server/src/main.cpp
	$(CC) $(flags) -o ./server/bin/main $(server_source) $(server_models)

# COMPILE CLIENT -> Compiles client and all its dependencies
cc: client/src/main.cpp
	$(CC) $(flags) -o ./client/bin/main ./client/src/main.cpp

# RUN SERVER -> Runs server with udp and tcp connections
rs: cs
	./server/bin/main

# RUN CLIENT REMOTE -> Runs client with input ip and port
rcr: cc
	./client/bin/main -p $(port) -n $(ip)

# RUN CLIENT LOCAL -> Runs client with local configurations
rcl: cc
	./client/bin/main

# Cleans everything
clean:
	rm -f ./server/bin/* ./client/bin/*