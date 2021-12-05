#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <cstring>

using namespace std;

#define PORT "58001"
#define MAX_INPUT_SIZE 256

int fd, newfd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];


void registration(char* name, char* password){

}


void parser(char* msg) {

	char name_2[MAX_INPUT_SIZE];
	char name_1[MAX_INPUT_SIZE];
	string token = fgets(msg, 3, stdin);

}


int main(int argc, char const *argv[]) {
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) exit(1);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	errcode = getaddrinfo(nullptr, PORT, &hints, &res);
	if (errcode != 0) exit(1);

	n = bind(fd, res->ai_addr, res->ai_addrlen);
	if (n == -1) exit(1);

	while(true) {

		addrlen = sizeof(addr);
		n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
		if (n==-1) exit(1);

		write(1, "received: ", 10); write(1, buffer, n);
		n = sendto(fd, buffer, n, 0, (struct sockaddr*) &addr, addrlen);
		if(n==-1) exit(1);

	}

	freeaddrinfo(res);
	close(fd);

	return 0;
}