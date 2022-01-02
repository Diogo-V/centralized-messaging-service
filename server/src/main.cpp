/*------------------------------------- Standard definitions -------------------------------------*/

#include "models/user.h"
#include "models/group.h"
#include "models/message.h"
#include "api.h"

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <csignal>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <vector>
#include <sstream>


using namespace std;

/* Const definitions */
#define PORT "58039"
#define MSG_MAX_SIZE 240

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { cerr << (msg) << endl; exit(EXIT_FAILURE); }

/* If server is verbose, output message to the screen */
#define verbose_(cond, msg) if((cond)) { cout << (msg) << endl; }

/*-------------------------------------- Server global vars --------------------------------------*/

int
    fd,  /* Holds server socket file descriptor */
    errcode;  /* Holds current error */
struct addrinfo
    hints,  /* Used to request info from DNS to get our "endpoint" */
    *res;  /* Stores result from getaddrinfo and uses it to set up our socket */

ssize_t n;  /* Holds number of bytes read/sent or -1 in case of error */
socklen_t addrlen;  /* Holds size of message sent from sender */
struct sockaddr_in addr;  /* Describes internet socket address. Holds sender info */
char buffer[MSG_MAX_SIZE];  /* Holds current message received in this socket */

bool isVerbose = false;  /* Is true if the server is set to verbose mode */

unordered_map<string, User> users;  /* Holds all users in our server. Key is user's id*/
unordered_map<string, Group> groups;  /* Holds all groups in our server. Key is group's id */


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
string selector(const char* msg) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    split(msg, inputs);  /* Splits msg by the spaces and returns an array with everything*/
    string status{};

    // TODO: Ask teacher about the verbose flag (what are we supposed to output to the screen?)
    string ip(inet_ntoa(addr.sin_addr));
    string port{to_string(ntohs(addr.sin_port))};
    verbose_(isVerbose, "COMMAND: " + inputs[0] + " | IP: " + ip + " | PORT: " + port)

    if (inputs[0] == "REG") {  /* Registers user */

        status = register_user(&users, inputs[1], inputs[2]);
        return "RRG " + status + "\n";  /* Sends report to client */

    } else if (inputs[0] == "UNR") {  /* Unregisters user */

        /* receives status from call function*/
        status = unregister_user(&users, inputs[1], inputs[2]);

        return "RUN " + status + "\n";  /* Sends report to client */

    } else if (inputs[0] == "LOG") {  /* Signs in user */

        /* receives status from call function*/
        status = login_user(&users, inputs[1], inputs[2]);

        return "RLO " + status + "\n";  /* Sends report to client */

    } else if (inputs[0] == "OUT") {  /* Logout user */

        /* receives status from call function*/
        cout << "OUT" << endl;

        return "ROU " + status + "\n";  /* Sends report to client */

    } else if (inputs[0] == "GLS") {  /* Requested list of existing groups */

        /* receives status from call function*/
        cout << "GLS" << endl;

        return "RGL " + status + "\n";  /* Sends report to client */

    } else if (inputs[0] == "GSR") {  /* Join group */

        /* receives status from call function*/
        cout << "GSR" << endl;

        return "RGS " + status + "\n";  /* Sends report to client */

    } else if (inputs[0] == "GUR") {  /* Unsubscribe to group */

        /* receives status from call function*/
        cout << "GUR" << endl;

        return "RGU " + status + "\n";  /* Sends report to client */

    } else if (inputs[0] == "GLM") {  /* Get list of user's groups */

        /* receives status from call function*/
        cout << "GLM" << endl;

        return "RGM " + status + "\n";  /* Sends report to client */

    } else {
        return "ERR";
    }

}

void termination_handler(){}



/**
 * Setups server loop.
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {

    /* TODO: @Sofia-Morgado -> finish ctrl+c interrupt handling
    struct sigaction new_action;
    new_action.sa_handler = termination_handler; // set callback function
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, NULL)
    */

    /* Goes over all the flags and setups port and ip address */
    string ds_port{PORT};  /* Holds server port */
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-p") == 0) { string s(argv[i + 1]); ds_port = s; }
        else if (strcmp(argv[i], "-v") == 0) { isVerbose = true; }
    }

    /* Creates udp soGroup for internet */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(fd != -1, "Could not create socket")

    /* Inits UDP server's struct to access the DNS */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	/* Uses its URL to consult DNS and get a UDP server's IP address */
	errcode = getaddrinfo(nullptr, ds_port.c_str(), &hints, &res);
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

        /* Removes \n at the end of the buffer. Makes things easier down the line */
        buffer[strlen(buffer) - 1] = '\0';

        /* Process client's message and decides what to do with it based on the passed code */
        string response = selector(buffer);

        /* Sends response back t client */
        n = sendto(fd, response.c_str(), response.size(), 0, (struct sockaddr*) &addr, addrlen);
        assert_(n != -1, "Failed to send message")

        memset(buffer, 0, MSG_MAX_SIZE);  /* Cleans buffer for new iteration */

	}

	/* Closes sockets */
    freeaddrinfo(res);
    close(fd);

    return EXIT_SUCCESS;

}
