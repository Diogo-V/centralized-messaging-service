/*------------------------------------- Standard definitions -------------------------------------*/

#include "models/user.h"
#include "models/group.h"
#include "models/message.h"

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <vector>
#include <sstream>


using namespace std;

/* Const definitions */
#define PORT "58039"
#define MSG_MAX_SIZE 240
#define CMD_MAX_SIZE 3
#define PASS_MAX_SIZE 8

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { fprintf(stderr, msg); exit(EXIT_FAILURE); }

/*-------------------------------------- Server global vars --------------------------------------*/

int
    fd,  /* Holds server socket file descriptor */
    errcode;  /* Holds current error */
struct addrinfo
    hints,  /* Used to request info from DNS to get our "endpoint" */
    *res;  /* Stores result from getaddrinfo and uses it to setup our socket */

ssize_t n;  /* Holds number of bytes read/sent or -1 in case of error */
socklen_t addrlen;  /* Holds size of message sent from sender */
struct sockaddr_in addr;  /* Describes internet socket address. Holds sender info */
char buffer[MSG_MAX_SIZE];  /* Holds current message received in this socket */

bool isVerbose = false;  /* Is true if the server is set to verbose mode */

/*----------------------------------------- Functions --------------------------------------------*/

/**
 * Transforms a string with spaces in a vector with substring tokenized by the spaces.
 *
 * @param str string which is going to be separated
 * @param out vector with substrings
 */
void split(string const &str, vector<string> &out) {
    stringstream ss(str); string s; const char delim = (const char)* " ";
    while (getline(ss, s, delim)) out.push_back(s);
}


/**
 * Receives message sent from user and decides based on the first 3 chars which action to take.
 *
 * @param msg message sent by user
 */
string selector(char* msg, unordered_map<string, User>* users, unordered_map<string, Group>* groups) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    split(msg, inputs);  /* Splits msg by the spaces and returns an array with everything*/
    string status;

    if (inputs[0] == "REG") {  /* Registers user */
        /* receives status from call function*/
        cout << "REG!" << endl;

        return "REG" + status;

    } else if (inputs[0] == "UNR") {  /* Unregisters user */
        /* receives status from call function*/
        cout << "UNR" << endl;

        return "UNR" + status;

    } else if (inputs[0] == "LOG") {  /* Signs in user */
        /* receives status from call function*/
        cout << "LOG" << endl;

        return "LOG" + status;

    } else if (inputs[0] == "OUT") {  /* Logout user */
        /* receives status from call function*/
        cout << "OUT" << endl;

        return "OUT" + status;

    } else if (inputs[0] == "GLS") {  /* Requested list of existing groups */
        /* receives status from call function*/
        cout << "GLS" << endl;

        return "GLS" + status;

    } else if (inputs[0] == "GSR") {  /* Join group */
        /* receives status from call function*/
        cout << "GSR" << endl;

        return "GSR" + status;

    } else if (inputs[0] == "GUR") {  /* Unsubscribe to group */
        /* receives status from call function*/
        cout << "GUR" << endl;

        return "GUR" + status;

    } else if (inputs[0] == "GLM") {  /* Get list of user's groups */
        /* receives status from call function*/
        cout << "GLM" << endl;

        return "GLM" + status;

    } else {
        return "ERR";
    }

}


/**
 * Setups server loop.
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {

    /* Holds all users in our server*/
    unordered_map<string, User> users = {};

    /* Holds all groups in our server*/
    unordered_map<string, Group> groups = {};

    /* Creates udp soGroup for internet */
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
        bzero(&addr, sizeof(struct sockaddr_in));
        addrlen = sizeof(addr);
		n = recvfrom(fd, buffer, MSG_MAX_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
        assert_(n != -1, "Failed to receive message")

        /* Process client's message and decides what to do with it based on the passed code */
        string response = selector(buffer, &users, &groups).c_str();

		n = sendto(fd, response.c_str(), n, 0, (struct sockaddr*) &addr, addrlen);
        assert_(n != -1, "Failed to send message")

	}

	/* Closes sockets */
    freeaddrinfo(res);
    close(fd);

    return EXIT_SUCCESS;

}
