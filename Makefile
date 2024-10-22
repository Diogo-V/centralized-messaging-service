CC = g++
debug_flags = -Wall -std=c++14 -g -lm
compile_flags = -Wall -std=c++14 -g -lm

port = 58040  # Port in which our server is going to run (tejo's port)
ip_tecnico = tejo.tecnico.ulisboa.pt
ip = localhost  # Ip of our server

server_models = ./server/src/models/*.cpp ./server/src/models/*.h
server_source = ./server/src/*.cpp ./server/src/*.h
server_helpers = ./server/src/misc/*.cpp  ./server/src/misc/*.h

client_models = ./client/src/models/*.cpp ./client/src/models/*.h
client_source = ./client/src/*.cpp
client_helpers = ./client/src/misc/*.cpp ./client/src/misc/*.h

# COMPILE SERVER -> Compiles udp + tcp server and all its dependencies
cs: server/src/main.cpp
	$(CC) $(compile_flags) -o ./server/bin/main $(server_source) $(server_models) $(server_helpers)

# COMPILE CLIENT -> Compiles client and all its dependencies
cc: client/src/main.cpp
	$(CC) $(compile_flags) -o ./client/bin/main $(client_source) $(client_models) $(client_helpers)

# RUN SERVER -> Runs server with udp and tcp connections
rs: cs
	./server/bin/main

# RUN SERVER REMOTE VERBOSE -> Runs server with udp and tcp connections remotely
rsrv: cs
	./server/bin/main -p $(port) -v

# RUN CLIENT REMOTE -> Runs client with input ip and port
rcr: cc
	./client/bin/main -p $(port) -n $(ip)

# RUN CLIENT REMOTE IST -> Runs client with input ip and port
rct: cc
	./client/bin/main -p $(port) -n $(ip_tecnico)

# RUN CLIENT LOCAL -> Runs client with local configurations
rcl: cc
	./client/bin/main

# TEST -> Runs both client and server and tests the outputs of each one with a set of instructions
test: cs cc
	./test/test.sh

# Cleans everything
clean:
	rm -f ./server/bin/* ./client/bin/*