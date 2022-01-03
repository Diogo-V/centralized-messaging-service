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
#include <dirent.h>

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

        /* receives status from call function*/
        cout << "OUT" << endl;

        return "ROU " + status + "\n";

    } else if (inputs[0] == "GLS") {  /* Requested list of existing groups */

        /* receives status from call function*/
        cout << "GLS" << endl;

        return "RGL " + status + "\n";

    } else if (inputs[0] == "GSR") {  /* Join group */

        /* receives status from call function*/
        cout << "GSR" << endl;

        return "RGS " + status + "\n";

    } else if (inputs[0] == "GUR") {  /* Unsubscribe to group */

        /* receives status from call function*/
        cout << "GUR" << endl;

        return "RGU " + status + "\n";

    } else if (inputs[0] == "GLM") {  /* Get list of user's groups */

        /* receives status from call function*/
        cout << "GLM" << endl;

        return "RGM " + status + "\n";

    } else {
        return "ERR";
    }

}


/**
 * @brief Function to handle Ctrl + C signal. Removes all files from the files directory and closes the server sockets
 *
 * @param sigtype signal type
 */
void termination_handler(int sigtype){
    struct dirent *entry;
    DIR *dp;
    /* Gets the current directory of the project*/
    char *project_directory = get_current_dir_name();
    string filepath;
    string files_directory = string(project_directory) + "/server/files";

    /* Opens the files directory */
    dp = opendir(files_directory.c_str());

    assert_(dp, "Failed to open the files directory");

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
    close(fd);

    /* Ends program */
    exit(EXIT_SUCCESS);
}



/**
 * Setups server loop.
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {
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

    /*Creating space for the struct and filling the struct. We are setting the handler to ignore the signal*/
    memset(&sa_2,0,sizeof sa_2);
    sa_2.sa_handler=SIG_IGN;

    /* Setting the sa_1 to the SIGPIPE and sa_1 to SIGINT*/
    if(sigaction(SIGPIPE,&sa_2,NULL)==-1 || sigaction(SIGINT, &sa_1, NULL) == -1)
        exit(1);

    // TODO: @Diogo-V -> Implement TCP connection and put a selector

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
