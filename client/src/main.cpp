/*------------------------------------- Standard definitions -------------------------------------*/

#include "models/manager.h"


using namespace std;


/* Const definitions */
#define PORT "58011"
#define LOCAL_IP "localhost"
#define MSG_MAX_SIZE 300
#define EXIT_CMD "exit"


/*----------------------------------------- Functions --------------------------------------------*/


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

    char buffer[MSG_MAX_SIZE];  /* Holds user input */

    string ds_port{PORT};  /* Holds server port */
    string ds_ip{LOCAL_IP};  /* Holds server ip */

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

    /* Cleans everything related to the manager */
    manager.clean();

    return EXIT_SUCCESS;

}
