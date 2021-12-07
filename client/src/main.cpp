/*------------------------------------- Standard definitions -------------------------------------*/

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>

using namespace std;

/* Const definitions */
#define PORT "58039"
#define LOCAL_IP "localhost"
#define MSG_MAX_SIZE 240
#define EXIT_CMD "exit"

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { fprintf(stderr, msg); exit(EXIT_FAILURE); }

/*-------------------------------------- Server global vars --------------------------------------*/

int
    fd,  /* Holds user socket file descriptor */
    errcode;  /* Holds current error */
struct addrinfo
    hints,  /* Used to request info from DNS to get our "endpoint" */
    *res;  /* Stores result from getaddrinfo and uses it to setup our socket */

ssize_t n;  /* Holds number of bytes read/sent or -1 in case of error */
socklen_t addrlen;  /* Holds size of message sent from sender */
struct sockaddr_in addr;  /* Describes internet socket address. Holds sender info */
char buffer[MSG_MAX_SIZE];  /* Holds current message received in this socket */

/*----------------------------------------- Functions --------------------------------------------*/

/**
 * Receives message sent from server and decides based on the first 3 chars which action to take.
 *
 * @param cmd requested command
 * @param msg rest of the message without the command
 */
void selector(const string& cmd, string msg) {

    if (strcmp(cmd.c_str(), "RRG") == 0) {  /* Receives status from REG (register user) */

    } else if (strcmp(cmd.c_str(), "RUN") == 0) {  /* Receives status from UNR (unregister user) */

    } else if (strcmp(cmd.c_str(), "RLO") == 0) {  /* Receives status from LOG (login user) */

    } else if (strcmp(cmd.c_str(), "ROU") == 0) {  /* Receives status from OUT (logout user) */

    } else if (strcmp(cmd.c_str(), "RGL") == 0) {  /* Receives status from GLS (list of groups) */

    } else if (strcmp(cmd.c_str(), "RGS") == 0) {  /* Receives status from GSR (join group) */

    } else if (strcmp(cmd.c_str(), "RGU") == 0) {  /* Receives status from GUR (unsub group) */

    } else if (strcmp(cmd.c_str(), "RGM") == 0) {  /* Receives status from GLM (lst usr groups) */

    } else {
        // reply ERR
    }

}


/**
 * Parses message from user and returns a new one that the server will accept.
 *
 * @param msg message that the user input
 * @return message that the server will recognize
 */
char* parser(char* msg) {
    // to be done
}


/**
 * Setups our socket "fd". Uses our main function's arguments to process flags.
 *
 * @param argc number of arguments passed to main function
 * @param argv array of arguments passed to main function
 */
void init_socket(int argc, char const *argv[]) {

    string ds_port{PORT};  /* Holds server port */
    string ds_ip{LOCAL_IP};  /* Holds server ip */

    /* Goes over all the flags and setups port and ip address */
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-p") == 0) { std::string s(argv[i + 1]); ds_port = s; }
        else if (strcmp(argv[i], "-n") == 0) { std::string s(argv[i + 1]); ds_ip = s; }
    }

    /* Creates udp socket for internet */
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(fd != -1, "Could not create socket")

    /* Inits UDP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    /* Uses it's URL to consult DNS and get the server to which we want to send messages */
    errcode = getaddrinfo(ds_ip.c_str(), ds_port.c_str(), &hints, &res);
    assert_(errcode == 0, "Failed getaddrinfo call")

}


/**
 * Setups client loop.
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {

    /* Initializes and setups fd to be a valid socket */
    init_socket(argc, argv);

    /* Gets the command that the user input */
    std::cin.getline (buffer, MSG_MAX_SIZE);

    do {

        /* TODO: Verify if message has correct formatting */

        /* TODO: Format message for it to be recognized by the server */

        /* TODO: Send message to server */

        /* TODO: Get answer from server */

        /* TODO: Based on the message sent by the server, display a message to the user */

        /* Gets the new command that the user input. This replaces the previous command */
        cin.getline(buffer, MSG_MAX_SIZE);

    } while (strcmp(buffer, EXIT_CMD) == 0);

    /* Sends message to server */
    n = sendto(fd, "Hello server!!!\n",16, 0, res->ai_addr, res->ai_addrlen);
    assert_(n != -1, "Failed to send message")

    /* Gets server response and processes it */
    bzero(&addr, sizeof(struct sockaddr_in));
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
    assert_(n != -1, "Failed to receive message")

    write(1, "echo: ", 6);
    write(1, buffer, n);

    /* Closes client socket */
    freeaddrinfo(res);
    close(fd);

    return EXIT_SUCCESS;
}
