/*------------------------------------- Standard definitions -------------------------------------*/

#include "models/manager.h"

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
#define PORT "58011"
#define LOCAL_IP "localhost"
#define MSG_MAX_SIZE 300
#define EXIT_CMD "exit"


/*-------------------------------------- Server global vars --------------------------------------*/


int fd_udp;  /* Holds server udp socket file descriptor */
int fd_tcp;  /* Holds server tcp socket file descriptor */
int errcode;  /* Holds current error */

struct addrinfo hints;  /* Used to request info from DNS to get our "endpoint" */
struct addrinfo *res;  /* Stores result from getaddrinfo and uses it to set up our socket */

ssize_t n;  /* Holds number of bytes read/sent or -1 in case of error */
socklen_t addrlen;  /* Holds size of message sent from sender */
struct sockaddr_in addr;  /* Describes internet socket address. Holds sender info */
char buffer[MSG_MAX_SIZE];  /* Holds user input */

string ds_port{PORT};  /* Holds server port */
string ds_ip{LOCAL_IP};  /* Holds server ip */

/** Flag to represent the need to logout the user when unregister the user has been successful and this user is the
 the currently logged in user */
bool logouts = false;


/*----------------------------------------- Functions --------------------------------------------*/


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
        if (outputs[1] != "0") {
            for (auto i = outputs.begin() + 2; i != outputs.end() - 1; ++i) {
                std::cout << *i << " ";
            }
            std::cout << *(outputs.end() - 1);
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
            for (auto i = outputs.begin() + 2; i != outputs.end() - 1; ++i) {
                std::cout << *i << " ";
            }
            std::cout << *(outputs.end() - 1);
        }

    } else if (outputs[0] == "RUL") { /* Receives status from GLM (lst users subscribed to this group) */
        if (outputs[1] == "NOK") cerr << "Failed. Group doesn't exist." << endl;
        else if (outputs[1] == "OK") {
            for (auto i = outputs.begin() + 2; i != outputs.end() - 1; ++i) {
                cout << *i << " ";
            }
            std::cout << *(outputs.end() - 1);
        } else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RPT") { /* Receives status from PST (post message) */
        if (outputs[1] == "NOK") cerr << "Failed. Message couldn't be posted" << endl;
        else if (isNumber(outputs[1])) cout << outputs[1] << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RRT"){ /* Receives status from RTV (retrieve message) */
        if (outputs[1] == "NOK") cerr << "Failed. Couldn't retrieve messages" << endl;
        else if (outputs[1] == "EOF") cout << "No messages available" << endl;
        else if (outputs[1] == "OK"){
            for (auto i = outputs.begin() + 2; i != outputs.end() - 1; ++i) {
                cout << *i << " ";
            }
            std::cout << *(outputs.end() - 1);
        } else cerr << "Invalid status" << endl;

    } else {
        cerr << "ERR" << endl;
    }

}


/*
 * Gets user input command by reading until first space.
 *
 * @param str user input command
 *
 * @return requested command
 */
string get_command(const string& str) {
    stringstream ss(str); string s; char delim = ' '; string cmd;
    getline(ss, cmd, delim); return cmd;
}


/**
 * Tells manager to process user's input. It will verify if it is valid, rearranges it to be sent to
 * the server, sends our request and analyses response.
 *
 * @param msg input from user
 */
void manageUserInput(const string& msg, Manager& manager) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string cmd = get_command(msg);

    /* Verifies if the user requested a valid command */
    if (cmd == "reg") manager.doRegister(msg);
    else if (cmd == "unr" || cmd == "unregister") manager.doUnregister(msg);
    else if (cmd == "login") manager.doLogin(msg);
    else if (cmd == "logout") manager.doLogout(msg);
    else if (cmd == "showuid" || cmd == "su") manager.doShowUID(msg);
    else if (cmd == "exit") manager.doExit(msg);
    else if (cmd == "groups" || cmd == "gl") manager.doListGroups(msg);
    else if (cmd == "subscribe" || cmd == "s") manager.doSubscribe(msg);
    else if (cmd == "unsubscribe" || cmd == "u") manager.doUnsubscribe(msg);
    else if (cmd == "my_groups" || cmd == "mgl") manager.doMyGroups(msg);
    else if (cmd == "select" || cmd == "sag") manager.doSelect(msg);
    else if (cmd == "showgid" || cmd == "sg") manager.doShowGID(msg);
    else if (cmd == "ulist" || cmd == "ul") manager.doUserList(msg);
    else if (cmd == "post") manager.doPost(msg);
    else if (cmd == "retrieve" || cmd == "r") manager.doRetrieve(msg);
    else cout << "Invalid command" << endl;

}


/**
 * Setups client loop.
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {

    /* Goes over all the flags and setups port and ip address to connect to server */
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-p") == 0) { string s(argv[i + 1]); ds_port = s; }
        else if (strcmp(argv[i], "-n") == 0) { string s(argv[i + 1]); ds_ip = s; }
    }

    /* Creates connection with server and manager to execute commands */
    Connect connect(ds_ip, ds_port);
    Manager manager(connect);

    do {

        memset(buffer, 0, MSG_MAX_SIZE);  /* Cleans buffer before receiving user input */
        cin.getline(buffer, MSG_MAX_SIZE);  /* Gets the command that the user input */

        string req{};  /* Holds the request message that is going to be sent to the server */

        manageUserInput(req, manager);  /* Processes user's input */

   } while (strcmp(buffer, EXIT_CMD) != 0);

    /* Closes client socket */
    freeaddrinfo(res);
    close(fd_udp);
    close(fd_tcp);

    return EXIT_SUCCESS;

}
