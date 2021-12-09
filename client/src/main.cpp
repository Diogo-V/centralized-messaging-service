/*------------------------------------- Standard definitions -------------------------------------*/

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
#define LOCAL_IP "localhost"
#define MSG_MAX_SIZE 240
#define EXIT_CMD "exit"

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { fprintf(stderr, msg); exit(EXIT_FAILURE); }

/* If condition if false displays msg and returns with false bool value */
#define validate_(cond, msg) do { if (! (cond)) { fprintf(stderr, msg); return false; } } while(0);

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

typedef struct user {  /* Represents current client's user */
    string uid;
    bool is_logged;
    user() { uid = ""; is_logged = false; }
} User;

User user;  /* Holds current user */

/*----------------------------------------- Functions --------------------------------------------*/

/**
 * Receives message sent from server and decides based on the first 3 chars which action to take.
 *
 * @param cmd requested command
 * @param msg rest of the message without the command
 */
void selector(const string& cmd, string msg) {

    if(outputs[0] == "RRG")  {  /* Receives status from REG (register user) */
        if (outputs[1] == "OK") cout << "User registered successfully" << endl;
        else if (outputs[1] == "DUP") cerr << "Failed. User has already registered" << endl;
        else if (outputs[1] == "NOK") cerr << "Failed. Too many users already registered " << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RUN") { /* Receives status from UNR (unregister user) */
        if (outputs[1] == "OK") cout << "User unregistered successfully" << endl;
        else if (outputs[1] == "NOK") cerr << "Failed. Invalid user id or incorrect password." << endl;
        else cerr << "Invalid status" << endl;

    } else if (strcmp(cmd.c_str(), "RLO") == 0) {  /* Receives status from LOG (login user) */
        if (msg == "OK") cout << "User logged in successfully" << endl;
        else if (msg == "NOK") cerr << "Failed. Invalid user id or incorrect password." << endl;
        else cerr << "Invalid status" << endl;

    } else if (strcmp(cmd.c_str(), "ROU") == 0) {  /* Receives status from OUT (logout user) */
        if (msg == "OK") cout << "User logged out successfully" << endl;
        else if (msg == "NOK") cerr << "Failed. Invalid user id or incorrect password." << endl;
        else cerr << "Invalid status" << endl;

    } else if (strcmp(cmd.c_str(), "RGL") == 0) {  /* Receives status from GLS (list of groups) */
        //TODO: recebe grupos

    } else if (strcmp(cmd.c_str(), "RGS") == 0) {  /* Receives status from GSR (join group) */
        if (msg == "OK") cout << "User subscribed successfully." << endl;
        else if (msg == "NEW") cout << "New group created. User subscribed successfully." << endl;
        else if (msg == "E_USR") cerr << "Failed. Invalid user id." << endl;
        else if (msg == "E_GRP") cerr << "Failed. Invalid group id." << endl;
        else if (msg == "E_GNAME") cerr << "Failed. Invalid group name." << endl;
        else if (msg == "E_FULL") cerr << "Failed. Couldn't create new group." << endl;
        else if (msg == "NOK") cerr << "Failed. Unkown reasons." << endl;
        else cerr << "Invalid status" << endl;

    } else if (strcmp(cmd.c_str(), "RGU") == 0) {  /* Receives status from GUR (unsub group) */
        if (msg == "OK") cout << "User unsubscribed successfully" << endl;
        else if (msg == "E_USR") cerr << "Failed. Invalid user id.";
        else if (msg == "E_GRP") cerr << "Failed. Invalid group id." << endl;
        else if (msg == "NOK") cerr << "Failed. Unkown reason." << endl;
        else cerr << "Invalid status" << endl;

    } else if (strcmp(cmd.c_str(), "RGM") == 0) {  /* Receives status from GLM (lst usr groups) */
        //TODO: mostra grupos que está subscrito
        // else cerr << "Invalid status" << endl;

    } else {
        cerr << "ERR" << endl;
    }

}


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
 * Verifies if input string translates to a number.
 *
 * @param line string to be validated
 * @return boolean value
 */
bool isNumber(const string& line) { char* p; strtod(line.c_str(), &p); return *p == 0; }


/**
 * Verifies if a message that the user input is valid. Also populates "req" with the request that is
 * going to be sent to the server.
 *
 * @param msg input from user
 * @param out server request
 * @return bool values
 */
bool preprocessing(const string& msg, string& out) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    split(msg, inputs);  /* Splits msg by the spaces and returns an array with everything */

    /* Verifies if the user requested a valid command */
    if (inputs[0] == "reg") {

        /* Verifies if the user input a valid command */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")

        /* TODO: corrigir o loop infinito das macros */

        /* Transforms user input into a valid command to be sent to the server */
        out = "REG " + inputs[1] + " " + inputs[2] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "unr" || inputs[0] == "unregister") {

        /* Verifies if the user input a valid command */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")

        /* Transforms user input into a valid command to be sent to the server */
        out = "UNR " + inputs[1] + " " + inputs[2] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "login") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")
        validate_(!user.is_logged, "Client is already logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "LOG " + inputs[1] + " " + inputs[2] + "\n";

        user.uid = inputs[1];  /* Sets user's uid */

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "logout") {

        /* TODO: implement login functionality */

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "groups" || inputs[0] == "gl") {

        /* Transforms user input into a valid command to be sent to the server */
        out = "GLS\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "subscribe" || inputs[0] == "s") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 3, "User did not input group ID and/or group name")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GSR " + user.uid + " " + inputs[1] + " " + inputs[2] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "unsubscribe" || inputs[0] == "u") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 2, "User did not input group ID")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GUR " + user.uid + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "my_groups" || inputs[0] == "mgl") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GLM " + user.uid + " " + inputs[1] + "\n";

        return true;  /* Since everything was ok, we return true */

    }

    return false;  /* Since no command was chosen, the user did not input a valid command */

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
        if (strcmp(argv[i], "-p") == 0) { string s(argv[i + 1]); ds_port = s; }
        else if (strcmp(argv[i], "-n") == 0) { string s(argv[i + 1]); ds_ip = s; }
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
    cin.getline(buffer, MSG_MAX_SIZE);

    while (strcmp(buffer, EXIT_CMD) != 0) {

        string req{};  /* Holds the request message that is going to be sent to the server */

        /* Verify if message has correct formatting. If not, displays error to user and continues */
        /* Also populates "req" with a valid request */
        if (! preprocessing(buffer, req)) {
            cout << "Incorrect message format!" << endl;
            continue;
        }

        /* Sends message to server */
        n = sendto(fd, req.c_str(), req.length(), 0, res->ai_addr, res->ai_addrlen);
        assert_(n != -1, "Failed to send message")

        /* Gets server response and processes it */
        bzero(&addr, sizeof(struct sockaddr_in));
        addrlen = sizeof(addr);
        n = recvfrom(fd, buffer, MSG_MAX_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
        assert_(n != -1, "Failed to receive message")

        /* Based on the message sent by the server, display a message to the user */
        selector(buffer);
        write(1, "Server mandou a mensagem com sucesso\n", 30);

        /* Gets the new command that the user input. This replaces the previous command */
        cin.getline(buffer, MSG_MAX_SIZE);

    }

    /* Sends message to server */
    /* n = sendto(fd, "REG server!!!\n",14, 0, res->ai_addr, res->ai_addrlen);
    assert_(n != -1, "Failed to send message")*/

    /* Gets server response and processes it */
    /*bzero(&addr, sizeof(struct sockaddr_in));
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, MSG_MAX_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
    assert_(n != -1, "Failed to receive message")*/



    /* Closes client socket */
    freeaddrinfo(res);
    close(fd);

    return EXIT_SUCCESS;
}
