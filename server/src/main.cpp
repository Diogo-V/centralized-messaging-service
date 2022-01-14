/*------------------------------------- Standard definitions -------------------------------------*/

#include "models/user.h"
#include "models/group.h"
#include "models/message.h"
#include "models/manager.h"
#include "models/connect.h"
#include "misc/helpers.h"

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <csignal>
#include <cstring>
#include <dirent.h>
#include <memory>


using namespace std;


/* Const definitions */
#define PORT "58039"


/*-------------------------------------- Server global vars --------------------------------------*/


bool isVerbose = false;  /* Is true if the server is set to verbose mode */

/* Creates unique pointer to manager */
unique_ptr<Manager> manager;


/*----------------------------------------- Functions --------------------------------------------*/


/**
 * @brief Function to handle Ctrl + C signal. Removes all files from the files directory and closes the server sockets
 *
 * @param sig_type type of signal
 */
void termination_handler(int sig_type) {

    struct dirent *entry;
    DIR *dp;

    /* Gets the current directory of the project*/
    string filepath;
    string files_directory = string(get_current_dir_name()) + "/server/files";

    /* Opens the files directory */
    dp = opendir(files_directory.c_str());
    assert_(dp, "Failed to open the files directory");

    /* Goes over each file and removes it */
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
            continue;

        filepath = files_directory +  "/" + entry->d_name;
        /* Removes file*/
        assert_((remove(filepath.c_str()) == 0), "Failed to delete file");
    }

    /* Closes the directory*/
    closedir(dp);

    manager->clean();  /* Cleans manager's memory */

    /* Ends program */
    exit(EXIT_SUCCESS);
}


/**
 * @brief Initializes signal interrupters treatment (for SIGINT and SIGIGN)
 */
void initialize_interrupters() {

    struct sigaction sa_1{}, sa_2{};
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
    if (sigaction(SIGPIPE,&sa_2,nullptr) == -1 || sigaction(SIGINT, &sa_1, nullptr) == -1)
        exit(EXIT_FAILURE);
}


/**
 * Setups server loop.
 *
 * @param argc number of arguments passed
 * @param argv array of arguments
 * @return 0 if success and 1 if error
 */
int main(int argc, char const *argv[]) {

    string ds_port{PORT};  /* Holds server port */

    /* Initializes signal interrupters treatment */
    initialize_interrupters();

    /* Goes over all the flags and setups port and verbose mode */
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-p") == 0) { string s(argv[i + 1]); ds_port = s; }
        else if (strcmp(argv[i], "-v") == 0) { isVerbose = true; }
    }

    /* Create structures that will allow us to run the server */
    unordered_map<string, User> users;
    unordered_map<string, Group> groups;
    Connect connect(ds_port);

    /* Creates manager that will control our server */
    manager = make_unique<Manager>(&users, &groups, connect, isVerbose);

    /* Inits main server loop */
    manager->start_server();

}
