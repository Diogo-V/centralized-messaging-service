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
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/* Const definitions */
#define PORT "58039"
#define LOCAL_IP "localhost"
#define MSG_MAX_SIZE 240
#define EXIT_CMD "exit"

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { fprintf(stderr, msg); exit(EXIT_FAILURE); }

/* If condition is false displays msg and returns with false bool value */
#define validate_(cond, msg) do { if (! (cond)) { cerr << (msg) << endl; return false; } } while(0);

/*-------------------------------------- Server global vars --------------------------------------*/

int
    fd_udp,  /* Holds user socket file descriptor */
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
    string pass;
    bool is_logged;
    string selected_group;
    user() { uid = ""; pass = ""; is_logged = false; selected_group = ""; }
} User;

User user;

/** Flag to represent the need to logout the user when unregister the user has been successful and this user is the
 the currently logged in user */
bool logouts = false;

/** No need to send message to server */
bool no_server = false;

/*----------------------------------------- Functions --------------------------------------------*/


/*
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
 * Receives message sent from server and decides based on the first 3 chars which action to take.
 *
 * @param cmd requested command
 * @param msg rest of the message without the command
 */
void selector(const string& msg) {

    vector<string> outputs;  /* Holds a list of strings with the outputs from our server */
    split(msg, outputs);  /* Splits msg by the spaces and returns an array with everything */

    if (outputs[0] == "RRG")  {  /* Receives status from REG (register user) */
        if (outputs[1] == "OK") cout << "User registered successfully" << endl;
        else if (outputs[1] == "DUP") cerr << "Failed. User has already registered" << endl;
        else if (outputs[1] == "NOK") cerr << "Failed. Too many users already registered " << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RUN") { /* Receives status from UNR (unregister user) */
        if (outputs[1] == "OK"){
            cout << "User unregistered successfully" << endl;

            /*Logouts the user if the user was logged in*/
            if (logouts) {user.is_logged = false; user.uid = ""; user.pass = ""; user.selected_group = ""; logouts = !logouts;}
        }
        else if (outputs[1] == "NOK") cerr << "Failed. Invalid user id or incorrect password." << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RLO") {  /* Receives status from LOG (login user) */
        if (outputs[1] == "OK") {
            cout << "User logged in successfully" << endl;
            user.is_logged = true; }
        else if (outputs[1] == "NOK") cerr << "Failed. Invalid user id or incorrect password." << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "ROU") {  /* Receives status from OUT (logout user) */
        if (outputs[1] == "OK") {
            cout << "User logged out successfully" << endl;
            user.is_logged = false; user.uid = ""; user.pass = ""; user.selected_group = "";
        }
        else if (outputs[1] == "NOK") cerr << "Failed. Invalid user id or incorrect password." << endl;
        else cerr << "Invalid status" << endl;


    } else if (outputs[0] == "RGL") {  /* Receives status from GLS (list of groups) */
        //TODO: @Sofia-Morgado -> corrigir este último espaço
        if (outputs[1] != "0") {
            for (auto i = outputs.begin() + 2; i != outputs.end(); ++i) {
                std::cout << *i << " ";
            }
        }

    } else if (outputs[0] == "RGS") {  /* Receives status from GSR (join group) */
        if (outputs[1] == "OK") cout << "User subscribed successfully." << endl;
        else if (outputs[1] == "NEW") cout << "New group created. User subscribed successfully." << endl;
        else if (outputs[1] == "E_USR") cerr << "Failed. Invalid user id." << endl;
        else if (outputs[1] == "E_GRP") cerr << "Failed. Invalid group id." << endl;
        else if (outputs[1] == "E_GNAME") cerr << "Failed. Invalid group name." << endl;
        else if (outputs[1] == "E_FULL") cerr << "Failed. Couldn't create new group." << endl;
        else if (outputs[1] == "NOK") cerr << "Failed. Unkown reasons." << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RGU") {  /* Receives status from GUR (unsub group) */
        if (outputs[1] == "OK") cout << "User unsubscribed successfully" << endl;
        else if (outputs[1] == "E_USR") cerr << "Failed. Invalid user id.";
        else if (outputs[1] == "E_GRP") cerr << "Failed. Invalid group id." << endl;
        else if (outputs[1] == "NOK") cerr << "Failed. Unkown reason." << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RGM") {  /* Receives status from GLM (lst usr groups) */
        if (outputs[1] != "0") {
            for (auto i = outputs.begin() + 2; i != outputs.end(); ++i) {
                std::cout << *i << " ";
            }
        }

    } else {
        cerr << "ERR" << endl;
    }

}

/**
 * Verifies if input string translates to a number.
 *
 * @param line string to be validated
 * @return boolean value
 */
bool isNumber(const string& line) {
    char* p;
    strtod(line.c_str(), &p);
    return *p == 0;
}

/**
 * Verifies if input string translates to alpanumeric characters.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumeric(const string& line){
    uint8_t i = 0, len = line.length();

    while (isalnum(line[i])) i++;

    return i == len;
}

/**
 * Verifies if input string translates to alpanumeric characters plus '-' and '_'.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumericPlus(const string& line){
    uint8_t i = 0, len = line.length();

    while (isalnum(line[i]) || (line[i] == '-') || (line[i] == '_')) i++;

    return i == len;
}

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
        validate_(inputs[2].size() == 8, "User password should have 8 alpanumerical characters")
        validate_(isAlphaNumeric(inputs[2]), "User password should have only alpanumerical characters")


        /* Transforms user input into a valid command to be sent to the server */
        out = "REG " + inputs[1] + " " + inputs[2] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "unr" || inputs[0] == "unregister") {

        /* Verifies if the user input a valid command */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")
        validate_(inputs[2].size() == 8, "User password should have 8 alpanumerical characters")
        validate_(isAlphaNumeric(inputs[2]), "User password should have only alpanumerical characters")

        if (user.uid == inputs[1] && user.is_logged) logouts = true;

        /* Transforms user input into a valid command to be sent to the server */
        out = "UNR " + inputs[1] + " " + inputs[2] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "login") {
        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")
        validate_(inputs[2].size() == 8, "User password should have 8 alpanumerical characters")
        validate_(isAlphaNumeric(inputs[2]), "User password should have only alpanumerical characters")
        validate_(!user.is_logged, "Client is already logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "LOG " + inputs[1] + " " + inputs[2] + "\n";

        user.uid = inputs[1];  /* Sets tmp_user's uid */
        user.pass = inputs[2];  /* Saves tmp_user's password locally */

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "logout") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(user.is_logged, "Client needs to be logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "OUT " + user.uid + " " + user.pass + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "showuid" || inputs[0] == "su"){
        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(user.is_logged, "No client logged in")

        cout << user.uid << endl;

        /* There is no need to send message to server */
        no_server = true;

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "exit"){
        /* Closes client socket */
        freeaddrinfo(res);
        close(fd_udp);

        //TODO: close tcp connections;
        //FIXME: quando há exit antes do logout, o user que está logged int, no servidor fica como logged in e vai dar erro

        return EXIT_SUCCESS;

    } else if (inputs[0] == "groups" || inputs[0] == "gl") {

        /* Transforms user input into a valid command to be sent to the server */
        out = "GLS\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "subscribe" || inputs[0] == "s") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 3, "User did not input group ID and/or group name")
        validate_(inputs[1].size() <= 2, "Group ID is a 2 digit-number")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(inputs[2].size() <= 24, "Group name limited to 24 characters")
        validate_(isAlphaNumericPlus(inputs[2]), "Group name should have only alpanumerical characters plus '-' and "
                                                 "'_'")
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GSR " + user.uid + " " + inputs[1] + " " + inputs[2] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "unsubscribe" || inputs[0] == "u") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 2, "User did not input group ID")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(inputs[1].size() <= 2, "Group ID is a 2 digit-number")
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GUR " + user.uid + " " + inputs[1] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "my_groups" || inputs[0] == "mgl") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GLM " + user.uid + " " + inputs[1] + "\n";

        return true;  /* Since everything was ok, we return true */

    } else if (inputs[0] == "select" || inputs[0] == "sag") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 2, "User did not input group ID")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(user.is_logged, "Client is not logged in")

        /* Saves selected group locally */
        user.selected_group = inputs[1];

        return true;  /* Since everything was ok, we return true */

    }

    /* TODO: implement rest of TCP */

    cout << "Invalid command" << endl;
    return false;  /* Since no command was chosen, the user did not input a valid command */

}


/**
 * Setups our socket "fd_udp". Uses our main function's arguments to process flags.
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
    fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(fd_udp != -1, "Could not create socket")

    /* Inits UDP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    /* Uses its URL to consult DNS and get the server to which we want to send messages */
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

    /* Initializes and setups fd_udp to be a valid socket */
    init_socket(argc, argv);


    /* Gets the command that the user input */
    cin.getline(buffer, MSG_MAX_SIZE);

    while (strcmp(buffer, EXIT_CMD) != 0) {
        string req{};  /* Holds the request message that is going to be sent to the server */

        /* Verify if message has correct formatting. If not, displays error to user and continues */
        /* Also populates "req" with a valid request */
        if (! preprocessing(buffer, req)) {
            memset(buffer, 0, MSG_MAX_SIZE);  /* Cleans buffer */

            /* Gets the new command that the user input. This replaces the previous command */
            cin.getline(buffer, MSG_MAX_SIZE);

            continue;
        }

        //FIXME: Perguntar se isto é uma boa ideia;
        /*In case of a command that not requires to send message to the server */
        if(no_server){
            no_server = false;

            /* Gets the new command that the user input. This replaces the previous command */
            cin.getline(buffer, MSG_MAX_SIZE);

            continue;
        }

        memset(buffer, 0, MSG_MAX_SIZE);  /* Cleans buffer before receiving response */

        /* Sends message to server */
        n = sendto(fd_udp, req.c_str(), req.length(), 0, res->ai_addr, res->ai_addrlen);
        assert_(n != -1, "Failed to send message")

        /* Gets server response and processes it */
        bzero(&addr, sizeof(struct sockaddr_in));
        addrlen = sizeof(addr);
        n = recvfrom(fd_udp, buffer, MSG_MAX_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
        assert_(n != -1, "Failed to receive message")

        /* Removes \n at the end of the buffer. Makes things easier down the line */
        buffer[strlen(buffer) - 1] = '\0';

        /* Based on the message sent by the server, display a message to the user */
        selector(buffer);

        /* Gets the new command that the user input. This replaces the previous command */
        cin.getline(buffer, MSG_MAX_SIZE);

    }


    /* Closes client socket */
    freeaddrinfo(res);
    close(fd_udp);

    return EXIT_SUCCESS;

}
