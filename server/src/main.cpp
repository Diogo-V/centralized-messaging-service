/*------------------------------------- Standard definitions -------------------------------------*/

#include "models/user.h"
#include "models/group.h"
#include "models/message.h"
#include "api.h"

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <csignal>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <dirent.h>

using namespace std;

/* Const definitions */
#define PORT "58039"
#define MSG_MAX_SIZE 240
#define TCP_N_CONNECTIONS 5

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { cerr << (msg) << endl; exit(EXIT_FAILURE); }

/* If server is verbose, output message to the screen */
#define verbose_(cond, msg) if((cond)) { cout << (msg) << endl; }


/*-------------------------------------- Server global vars --------------------------------------*/


int fd_udp;  /* Holds server udp socket file descriptor */
int fd_tcp;  /* Holds server tcp socket file descriptor */
int errcode;  /* Holds current error */
fd_set fds;  /* Holds file descriptors in select */

struct addrinfo hints;  /* Used to request info from DNS to get our "endpoint" */
struct addrinfo *res;  /* Stores result from getaddrinfo and uses it to set up our socket */

ssize_t n, nw, nr;  /* Holds number of bytes read/sent or -1 in case of error */
socklen_t addrlen;  /* Holds size of message sent from sender */
struct sockaddr_in addr;  /* Describes internet socket address. Holds sender info */
char buffer[MSG_MAX_SIZE];  /* Holds current message received in this socket */

bool isVerbose = false;  /* Is true if the server is set to verbose mode */
string ds_port{PORT};  /* Holds server port */

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
 * Also sends a report back to client.
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
        return "RRG " + status + "\n";

    } else if (inputs[0] == "UNR") {  /* Unregisters user */
        status = unregister_user(&users, inputs[1], inputs[2]);
        return "RUN " + status + "\n";

    } else if (inputs[0] == "LOG") {  /* Signs in user */
        status = login_user(&users, inputs[1], inputs[2]);
        return "RLO " + status + "\n";

    } else if (inputs[0] == "OUT") {  /* Logout user */
        status = logout_user(&users, inputs[1] , inputs[2]);
        return "ROU " + status + "\n";

    } else if (inputs[0] == "GLS") {  /* Requested list of existing groups */
        status = list_groups(&groups);
        return "RGL " + to_string(groups.size()) + " " + status + "\n";

    } else if (inputs[0] == "GSR") {  /* Join group */
        status = subscribe(&groups, &users, inputs[1], inputs[2], inputs[3]);
        return "RGS " + status + "\n";

    } else if (inputs[0] == "GUR") {  /* Unsubscribe to group */
        status = unsubscribe(&groups, &users, inputs[1], inputs[2]);
        return "RGU " + status + "\n";

    } else if (inputs[0] == "GLM") {  /* Get list of user's groups */
        status = groups_subscribed(&groups, &users, inputs[1]);
        return "RGM " + status + "\n";

    } else if (inputs[0] == "ULS"){ /* Get list of users subscribed to this group */
        status = users_subscribed(&groups, &users, inputs[1]);
        return "RUL " + status + "\n";

    } else if (inputs[0] == "PST") { /* Receives a text and optionally also a file*/
        status = post_message(&groups, &users, inputs[1], inputs[2], inputs[3], inputs[4]);
        return "RPT " + status + "\n";

    } else if (inputs[0] == "RTV") {
        status = retrieve_message(&groups, &users, inputs[1], inputs[2], inputs[3]);
        printf("%s", status.c_str());
        return "RRT " + status + "\n";

    } else {
            cout << inputs[0] << endl;
            return "ERR";
    }

}


/**
 * @brief Function to handle Ctrl + C signal. Removes all files from the files directory and closes the server sockets
 *
 * @param sig_type type of signal
 */
void termination_handler(int sig_type) {

    struct dirent *entry;
    DIR *dp;

    /* Gets the current directory of the project*/
    char *project_directory = get_current_dir_name();
    string filepath;
    string files_directory = string(project_directory) + "/server/files";

    /* Opens the files directory */
    dp = opendir(files_directory.c_str());
    assert_(dp, "Failed to open the files directory");

    /* Goes over each file and removes it */
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 ||strcmp(entry->d_name, "..") == 0 )
            continue;

        filepath = files_directory +  "/" + entry->d_name;
        /* Removes file*/
        assert_((remove(filepath.c_str()) == 0), "Failed to delete file");
    }

    /* Closes the directory*/
    closedir(dp);

    /* Closes the server socket*/
    freeaddrinfo(res);
    close(fd_udp);
    close(fd_tcp);

    /* Ends program */
    exit(EXIT_SUCCESS);
}


/**
 * @brief Initializes signal interrupters treatment (for SIGINT and SIGIGN)
 */
void initialize_interrupters() {

    struct sigaction sa_1, sa_2;
    sigset_t block_mask;

    /*Creating space for the sa and filling the struct. We are setting the handler to termination handlers function,
     * that will close all the sockets and erase files.*/
    memset(&sa_1,0,sizeof sa_1);
    /* Blocks signal quit (CTRL+Q) and signal stop (CTRL+Z) while handling signal interruption (CTRL + C). Only here
     * for safety*/
    //TODO: @Sofia-Morgado devia colocar mais sinais a bloquear?
    sigaddset (&block_mask, SIGTSTP);
    sigaddset (&block_mask, SIGQUIT);
    sa_1.sa_mask = block_mask;
    sa_1.sa_flags = 0;
    sa_1.sa_handler = termination_handler;

    /* Creating space for the struct and filling the struct. We are setting the handler to ignore the signal */
    memset(&sa_2,0,sizeof sa_2);
    sa_2.sa_handler = SIG_IGN;

    /* Setting the sa_1 to the SIGPIPE and sa_1 to SIGINT*/
    if(sigaction(SIGPIPE,&sa_2,nullptr) == -1 || sigaction(SIGINT, &sa_1, nullptr) == -1)
        exit(EXIT_FAILURE);
}


/**
 * @brief Setups our socket "fd_udp"
 */
void init_udp_socket() {

    /* Creates udp subgroup for internet */
    fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    assert_(fd_udp != -1, "Could not create udp socket")

    /* Inits UDP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    /* Uses its URL to consult DNS and get a UDP server's IP address */
    errcode = getaddrinfo(nullptr, ds_port.c_str(), &hints, &res);
    assert_(errcode == 0, "Failed getaddrinfo call for udp socket")

    /* Binds sockets to our specified port and tells our SO that this channel if for this program */
    int err = bind(fd_udp, res->ai_addr, res->ai_addrlen);
    assert_(err == 0, "Failed to bind udp socket")

}


/**
 * @brief Setups our socket "fd_tcp".
 */
void init_tcp_socket() {

    /* Creates tcp subgroup for internet */
    fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    assert_(fd_tcp != -1, "Could not create tcp socket")

    /* Inits TCP server's struct to access the DNS */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* Uses its URL to consult DNS and get a TCP server's IP address */
    errcode = getaddrinfo(nullptr, ds_port.c_str(), &hints, &res);
    assert_(errcode == 0, "Failed getaddrinfo call for tcp")

    /* Binds sockets to our specified port and tells our SO that this channel if for this program */
    int err = bind(fd_tcp, res->ai_addr, res->ai_addrlen);
    assert_(err == 0, "Failed to bind tcp socket")

    /* Prepares socket to receive connections */
    assert_(listen(fd_tcp,TCP_N_CONNECTIONS) != -1, "Could not prepare tcp socket")

}


/**
 * Setups server loop.
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {

    /* Initializes signal interrupters treatment */
    initialize_interrupters();

    /* Goes over all the flags and setups port and verbose mode */
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-p") == 0) { string s(argv[i + 1]); ds_port = s; }
        else if (strcmp(argv[i], "-v") == 0) { isVerbose = true; }
    }

    /* Initializes both sockets */
    init_udp_socket();
    init_tcp_socket();

    /* Inits server connection loop */
	while (true) {

        FD_ZERO(&fds);  // Clears all file descriptors
        FD_SET(fd_udp, &fds);  // Adds socket to selector
        FD_SET(fd_tcp, &fds);  // Adds socket to selector

        /* Blocks until one of the descriptors, previously set in are ready to by read. Returns number of file descriptors ready */
        uint8_t counter = select(fd_tcp + 1,&fds,(fd_set*) nullptr,(fd_set*) nullptr,(struct timeval *) nullptr);
        assert_(counter > 0, "Select threw an error")

        /* Cleans previous iteration so that it does not bug */
        bzero(&addr, sizeof(struct sockaddr_in));
        addrlen = sizeof(addr);

        if (FD_ISSET(fd_udp, &fds)) {  /* Checks if udp socket activated */

            /* Receives message from client */
            n = recvfrom(fd_udp, buffer, MSG_MAX_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
            assert_(n != -1, "Failed to receive message")

            /* Removes \n at the end of the buffer. Makes things easier down the line */
            buffer[strlen(buffer) - 1] = '\0';

            /* Process client's message and decides what to do with it based on the passed code */
            string response = selector(buffer);

            /* Sends response back t client */
            n = sendto(fd_udp, response.c_str(), response.size(), 0, (struct sockaddr*) &addr, addrlen);
            assert_(n != -1, "Failed to send message")

        } else if (FD_ISSET(fd_tcp, &fds)) {  /* Checks if tcp socket activated */

            n = 0;  /* Clears previous connection's number of bytes before proceeding */

            /* Creates temporary socket to connect to client. Keeps main channel active */
            int tmp_fd = accept(fd_tcp,(struct sockaddr*) &addr, &addrlen);
            assert_(tmp_fd != -1, "Could not create temporary tcp socket")

            /* Keeps on reading until everything has been read from the client */
            do {
                nr = read(tmp_fd, buffer, MSG_MAX_SIZE);
                assert_(nr != -1, "Failed to read from temporary socket")
                if (nr == 0) break;  /* If a client closes a socket, we need to ignore */
                n += nr;
            } while (n < MSG_MAX_SIZE);
            if (nr == 0) break;  /* If a client closes a socket, we need to ignore */

            /* Removes \n at the end of the buffer. Makes things easier down the line */
            buffer[strlen(buffer) - 1] = '\0';

            /* Process client's message and decides what to do with it based on the passed code */
            string response = selector(buffer);

            /* Keeps sending messages to client until everything is sent */
            char* ptr = &response[0];
            while (n > 0) {
                assert_((nw = write(tmp_fd, ptr, n)) > 0, "Could not send message to client")
                n -= nw; ptr += nw;
            }

            close(tmp_fd);  /* Closes file descriptor to avoid errors */

        } else {
            assert_(false, "No correct file descriptor was activated in select")
        }

        memset(buffer, 0, MSG_MAX_SIZE);  /* Cleans buffer for new iteration */

	}

}
