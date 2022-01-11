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
#include <fstream>

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


int fd_udp;  /* Holds server udp socket file descriptor */
int fd_tcp;  /* Holds server tcp socket file descriptor */
int errcode;  /* Holds current error */

struct addrinfo hints;  /* Used to request info from DNS to get our "endpoint" */
struct addrinfo *res;  /* Stores result from getaddrinfo and uses it to set up our socket */

ssize_t n;  /* Holds number of bytes read/sent or -1 in case of error */
socklen_t addrlen;  /* Holds size of message sent from sender */
struct sockaddr_in addr;  /* Describes internet socket address. Holds sender info */
char buffer[MSG_MAX_SIZE];  /* Holds current message received in this socket */

enum con_type {TCP, UDP, NO_CON};  /* Decides how to send message to server (by udp or tcp) */

typedef struct user {  /* Represents current client's user */
    string uid;
    string pass;
    bool is_logged;
    string selected_group;
    user() { uid = ""; pass = ""; is_logged = false; selected_group = ""; }
} User;

User user;

string ds_port{PORT};  /* Holds server port */
string ds_ip{LOCAL_IP};  /* Holds server ip */

/** Flag to represent the need to logout the user when unregister the user has been successful and this user is the
 the currently logged in user */
bool logouts = false;


/*----------------------------------------- Functions --------------------------------------------*/


/**
 * Verifies if input string translates to a number.
 *
 * @param line string to be validated
 * @return boolean value
 */
bool isNumber(const string& line) { char* p; strtod(line.c_str(), &p); return *p == 0; }


/**
 * Verifies if input string translates to alphanumeric characters.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumeric(const string& line){
    uint8_t i = 0, len = line.length();

    while (isalnum(line[i])) i++;

    return i == len;
}


/**
 * Verifies if input string translates to alphanumeric characters plus '-' and '_'.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumericPlus(const string& line){
    uint8_t i = 0, len = line.length();
    while (isalnum(line[i]) || (line[i] == '-') || (line[i] == '_')) i++;
    return i == len;
}

//TODO: comentar e colocar 5 segundos
int TimerON(int sd)
{
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    tmout.tv_sec=5; /* Wait for 15 sec for a reply from server. */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
                      (struct timeval *)&tmout,sizeof(struct timeval)));
}

int TimerOFF(int sd)
{
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
                      (struct timeval *)&tmout,sizeof(struct timeval)));
}

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

    } else if (outputs[0] == "RUL") {
        if (outputs[1] == "NOK") cerr << "Failed. Group doesn't exist." << endl;
        else if (outputs[1] == "OK") {
            for (auto i = outputs.begin() + 2; i != outputs.end() - 1; ++i) {
                cout << *i << " ";
            }
            std::cout << *(outputs.end() - 1);
        } else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RPT") {
        if (outputs[1] == "NOK") cerr << "Failed. Message couldn't be posted" << endl;
        else if (isNumber(outputs[1])) cout << outputs[1] << endl;
        else cerr << "Invalid status" << endl;

    } else if (outputs[0] == "RRT"){
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


/**
 * Verifies if a message that the user input is valid. Also populates "req" with the request that is
 * going to be sent to the server.
 *
 * @param msg input from user
 * @param out server request
 * @return bool values
 */
bool preprocessing(const string& msg, string& out, con_type& con) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string cmd = get_command(msg);

    /* Verifies if the user requested a valid command */
    if (cmd == "reg") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")
        validate_(inputs[2].size() == 8, "User password should have 8 alphanumerical characters")
        validate_(isAlphaNumeric(inputs[2]), "User password should have only alphanumerical characters")

        /* Transforms user input into a valid command to be sent to the server */
        out = "REG " + inputs[1] + " " + inputs[2] + "\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "unr" || cmd == "unregister") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")
        validate_(inputs[2].size() == 8, "User password should have 8 alphanumerical characters")
        validate_(isAlphaNumeric(inputs[2]), "User password should have only alphanumerical characters")

        if (user.uid == inputs[1] && user.is_logged) logouts = true;

        /* Transforms user input into a valid command to be sent to the server */
        out = "UNR " + inputs[1] + " " + inputs[2] + "\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "login") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 3, "User did not input user ID and/or password")
        validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
        validate_(isNumber(inputs[1]), "User ID is not a number")
        validate_(inputs[2].size() == 8, "User password should have 8 alphanumerical characters")
        validate_(isAlphaNumeric(inputs[2]), "User password should have only alphanumerical characters")
        validate_(!user.is_logged, "Client is already logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "LOG " + inputs[1] + " " + inputs[2] + "\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        user.uid = inputs[1];  /* Sets tmp_user's uid */
        user.pass = inputs[2];  /* Saves tmp_user's password locally */

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "logout") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 1, "Too many arguments")
        validate_(user.is_logged, "Client needs to be logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "OUT " + user.uid + " " + user.pass + "\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "showuid" || cmd == "su") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 1, "Too many arguments")
        validate_(user.is_logged, "No client logged in")

        cout << user.uid << endl;

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "exit") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Closes client socket */
        freeaddrinfo(res);
        close(fd_udp);
        close(fd_tcp);

        //FIXME: quando há exit antes do logout, o user que está logged int, no servidor fica como logged in e vai dar erro

        return EXIT_SUCCESS;

    } else if (cmd == "groups" || cmd == "gl") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 1, "Too many arguments")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GLS\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "subscribe" || cmd == "s") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 3, "User did not input group ID and/or group name")
        validate_(inputs[1].size() <= 2, "Group ID isn't 2 digit-number")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(inputs[2].size() <= 24, "Group name limited to 24 characters")
        validate_(isAlphaNumericPlus(inputs[2]), "Group name should have only alphanumerical characters plus '-' and "
                                                 "'_'")
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GSR " + user.uid + " " + inputs[1] + " " + inputs[2] + "\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "unsubscribe" || cmd == "u") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 2, "User did not input group ID")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(inputs[1].size() <= 2, "Group ID isn't a 2 digit-number")
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GUR " + user.uid + " " + inputs[1] + "\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "my_groups" || cmd == "mgl") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 1, "Too many arguments")
        validate_(user.is_logged, "Client is not logged in")

        /* Transforms user input into a valid command to be sent to the server */
        out = "GLM " + user.uid + "\n";

        con = UDP;  /* Sets connection type to be used by the client to connect to the server */

        return true;  /* Since everything was ok, we return true */

    //FIXME: @Sofia-Morgado -> não está no enunciado, mas deviamos de alguma forma verificar se o grupo a selecionar    existe
    } else if (cmd == "select" || cmd == "sag") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 2, "User did not input group ID")
        validate_(isNumber(inputs[1]), "Group ID is not a number")
        validate_(inputs[1].size() <= 2, "Group ID isn't a 2 digit-number")
        validate_(user.is_logged, "Client is not logged in")

        /* Saves selected group locally */
        user.selected_group = inputs[1];

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "showgid" || cmd == "sg") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 1, "Too many arguments")
        validate_(user.is_logged, "Client is not logged in")
        validate_(!user.selected_group.empty(), "No selected group")

        cout << user.selected_group << endl;

        return true;  /* Since everything was ok, we return true */

    } else if (cmd == "ulist" || cmd == "ul") {

        /* Splits msg by the spaces and returns an array with everything */
        split(msg, inputs);

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 1, "Too many arguments")
        validate_(user.is_logged, "Client is not logged in")
        validate_(!user.selected_group.empty(), "No selected group")

        /* Transforms user input into a valid command to be sent to the server */
        out = "ULS " + user.selected_group +  "\n";

        con = TCP;  /* Sets connection type to be used by the client to connect to the server */

        return true;

    } else if (cmd == "post") {

        char text[MSG_MAX_SIZE];  /* Will hold user input text */
        string file;  /* Will hold the input file */
        memset(text, 0, MSG_MAX_SIZE);
        char c;  /* Used to check if the user did not input a file */

        assert_(sscanf(msg.c_str(), R"(%*s "%240[^"]" %c)", text, &c) == 1, "Invalid format")
//        assert_(sscanf(msg.c_str(), R"(%*s "%*s" %[^\n])", file) == 1, "Invalid format")

        /*for (auto x = msg.end(); x >= msg.begin(); x--) {
            file.append(msg[x]);
        }*/


        cout << file << endl;

        inputs[1] = text;
        if (c != '\n') inputs[2] = file;

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() >= 2, "Invalid number of arguments")
        validate_(user.is_logged, "Client is not logged in")
        validate_(!user.selected_group.empty(), "No selected group")
        validate_((inputs[1].length() - 2) <= 240, "Text is limited to 240 characters")

        string len = to_string(strlen(text));

        /* Transforms user input into a valid command to be sent to the server */
        out = "PST " + user.uid + " " + user.selected_group + " " + len + " " + "\"" + text + "\"\n";

        cout << out << endl;

        /*if (inputs.size() == 3) {
            ifstream file(inputs[2], ifstream::ate | ifstream::binary);
            out += " " + inputs[2] + " " + to_string(file.tellg());
        } else { out += "\n"; }*/

        con = TCP;  /* Sets connection type to be used by the client to connect to the server */

        return true;

    } else if (cmd == "retrieve" || cmd == "r") {

        /* Verifies if the user input a valid command and that this command can be issued */
        validate_(inputs.size() == 2, "Invalid number of arguments")
        validate_(user.is_logged, "Client is not logged in")
        validate_(!user.selected_group.empty(), "No selected group")

        out = "RTV " + user.uid + " " + user.selected_group + " " + inputs[1] + "\n";

        con = TCP;
        return true;
    }

    /* TODO: implement rest of TCP */

    cout << "Invalid command" << endl;
    return false;  /* Since no command was chosen, the user did not input a valid command */

}


/**
 * Setups our socket "fd_udp".
 */
void init_socket_udp() {

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
 * @brief Setups our socket "fd_tcp".
 */
void init_socket_tcp() {

    /* Creates tcp subgroup for internet */
    fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    assert_(fd_tcp != -1, "Could not create tcp socket")

    /* Inits TCP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* Uses its URL to consult DNS and get a TCP server's IP address */
    errcode = getaddrinfo(ds_ip.c_str(), ds_port.c_str(), &hints, &res);
    assert_(errcode == 0, "Failed getaddrinfo call for tcp")



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

    /* Initializes and setups fd_udp to be a valid socket */
    init_socket_udp();


    /* Gets the command that the user input */
    cin.getline(buffer, MSG_MAX_SIZE);

    while (strcmp(buffer, EXIT_CMD) != 0) {

        string req{};  /* Holds the request message that is going to be sent to the server */
        con_type con = NO_CON;  /* Holds the protocol used to perform this request to the server */

        /* Verify if message has correct formatting. If not, displays error to user and continues */
        /* Also populates "req" with a valid request and con with how to connect to the server */
        if (! preprocessing(buffer, req, con)) {
            memset(buffer, 0, MSG_MAX_SIZE);  /* Cleans buffer to prevent infinite loop */
            cin.getline(buffer, MSG_MAX_SIZE);
            continue;
        }

        memset(buffer, 0, MSG_MAX_SIZE);  /* Cleans buffer before receiving response */

        if (con == UDP) {  /* Connects to server by UDP */

            /* Sends message to server */
            n = sendto(fd_udp, req.c_str(), req.length(), 0, res->ai_addr, res->ai_addrlen);
            assert_(n != -1, "Failed to send message with UDP")

            /* Gets server response and processes it */
            bzero(&addr, sizeof(struct sockaddr_in));
            addrlen = sizeof(addr);

            int tries = 3;
            bool try_again = false;

            //FIXME: @Sofia-Morgado -> melhorar isto
            do {
                TimerON(fd_udp);
                n = recvfrom(fd_udp, buffer, MSG_MAX_SIZE, 0, (struct sockaddr *) &addr, &addrlen);
                //TODO: fazer recvfrom novamente quando dá timeout
                //assert_(n != -1, "Time out")
                TimerOFF(fd_udp);

                if (n == -1){
                    try_again = true;
                    tries --;
                } else {
                    try_again = false;
                }

                if (try_again && tries == 0){
                    cout << "Time out" << endl;
                    exit(EXIT_FAILURE);
                }

            } while (try_again && tries > 0);


        } else if (con == TCP) {  /* Connects to server by TCP */

            /* Initializes and setups fd_udp to be a valid socket */
            init_socket_tcp();

            /* Creates connection between server and client */
            assert_(connect(fd_tcp, res->ai_addr, res->ai_addrlen) != -1, "Could not connect to sever")

            uint16_t nw;  /* Used to keep track of how many bytes we have sent to the server */
            n = (ssize_t) req.length();  /* Sends request size */

            /* Keeps sending messages to sever until everything is sent */
            char* ptr = &req[0];
            while (n > 0) {
                assert_((nw = write(fd_tcp, ptr, MSG_MAX_SIZE)) > 0, "Could not send message to server")
                n -= nw; ptr += nw;
            }

            /* Keeps on reading until everything has been read from the server */
            while ((n = read(fd_tcp,buffer, MSG_MAX_SIZE)) != 0) {
                assert_(n != -1, "Failed to retrieve response from server")
            }

            /* Closes TCP connection */
            close(fd_tcp);

        }

        if (con == UDP || con == TCP){
            /* Removes \n at the end of the buffer. Makes things easier down the line */
            buffer[strlen(buffer) - 1] = '\0';

            /* Based on the message sent by the server, display a message to the user */
            selector(buffer);
        }

        /* Gets the new command that the user input. This replaces the previous command */
        cin.getline(buffer, MSG_MAX_SIZE);

    }


    /* Closes client socket */
    freeaddrinfo(res);
    close(fd_udp);
    close(fd_tcp);

    return EXIT_SUCCESS;

}
