/*------------------------------------- Standard definitions -------------------------------------*/

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>

using namespace std;

/* Const definitions */
#define PORT "58001"
#define MSG_MAX_SIZE 240

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { fprintf(stderr, msg); exit(EXIT_FAILURE); }

/*-------------------------------------- Server global vars --------------------------------------*/

int fd,  /* Holds server socket file descriptor */
    errcode;  /* Holds current error */
ssize_t n;  /* Holds number of bytes read/sent or -1 in case of error */
socklen_t addrlen;  /* Holds size of message sent from sender */
struct addrinfo hints,  /* Used to request info from DNS to get "endpoint" */
                *res;  /* Stores result from getaddrinfo and uses it to setup our socket */
struct sockaddr_in addr;  /* Describes internet socket address. Holds sender info */
char buffer[MSG_MAX_SIZE];  /* Holds current message received in this socket */

/*----------------------------------------- Functions --------------------------------------------*/

void selector(char* msg) {

    char token[3];

    /* Reads first 3 letters. scanf stops on first space */
    scanf("%s", token);

    if (strcmp(token, "REG") == 0) {

    } else if (strcmp(token, "RRG") == 0) {

    } else if (strcmp(token, "UNR") == 0) {

    } else if (strcmp(token, "RUN") == 0) {

    } else if (strcmp(token, "LOG") == 0) {

    } else if (strcmp(token, "RLO") == 0) {

    } else if (strcmp(token, "OUT") == 0) {

    } else if (strcmp(token, "ROU") == 0) {

    } else if (strcmp(token, "GLS") == 0) {

    } else if (strcmp(token, "RGL") == 0) {

    } else if (strcmp(token, "GSR") == 0) {

    } else if (strcmp(token, "GUR") == 0) {

    } else if (strcmp(token, "RGU") == 0) {

    } else if (strcmp(token, "GLM") == 0) {

    } else if (strcmp(token, "RGM") == 0) {

    } else if (strcmp(token, "RGS") == 0) {

    } else {
        // reply ERR
    }

}


/**
 * Main program loop
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {

    /* Creates udp socket in internet */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(fd != -1, "Could not create socket")

    /* Inits UDP server's struct to access the DNS */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	/* Uses it's URL to consult DNS and get a UDP server's IP address */
	errcode = getaddrinfo(nullptr, PORT, &hints, &res);
    assert_(errcode == 0, "Failed getaddrinfo call")

    /* Binds sockets to our specified port and tells our SO that this channel if for this server */
	int err = bind(fd, res->ai_addr, res->ai_addrlen);
    assert_(err == 0, "Failed to bind socket")

    /* Inits server connection loop */
	while (true) {

	    /* Receives message from client */
		addrlen = sizeof(addr);
		n = recvfrom(fd, buffer, MSG_MAX_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
        assert_(n != -1, "Failed to receive message")

        /* Process client's message and decides what to do with it based on the passed code */
        selector(buffer);

        /* Returns confirmation to client */
		write(1, "received: ", 10); write(1, buffer, n);
		n = sendto(fd, buffer, n, 0, (struct sockaddr*) &addr, addrlen);
        assert_(n != -1, "Could not send message")

	}

}
