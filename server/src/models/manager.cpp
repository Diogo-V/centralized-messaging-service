#include "manager.h"


/**
 * @brief Manager class constructor.
 *
 * @param users map of user currently in the server
 * @param groups map of groups registered in the server
 * @param connect module for connecting with clients
 * @param isVerbose checks if server is being ran in verbose mode
 */
Manager::Manager(unordered_map<string, User>* users, unordered_map<string, Group>* groups,
                 Connect& connect, bool isVerbose) : _connect(connect) {
    this->_users = users;
    this->_groups = groups;
    this->_isVerbose = isVerbose;
}


/**
 * @brief Starts main server loop.
 */
void Manager::start_server() {

    /* Inits server connection loop */
    while (true) {

        /* Adds file descriptors to watcher */
        FD_ZERO(this->getConnection().getFDS());
        FD_SET(this->getConnection().getSocketUDP(), this->getConnection().getFDS());
        FD_SET(this->getConnection().getSocketTCP(), this->getConnection().getFDS());

        /* Blocks until one of the descriptors, previously set in are ready to by read. Returns number of file descriptors ready */
        uint8_t counter = select(this->getConnection().getSocketTCP() + 1,this->getConnection().getFDS(),
                                 (fd_set*) nullptr,(fd_set*) nullptr,(struct timeval *) nullptr);
        assert_(counter > 0, "Select threw an error");

        /* Cleans previous iteration so that it does not bug */
        this->getConnection().cleanAddr();

        /* Checks if udp socket activated */
        if (FD_ISSET(this->getConnection().getSocketUDP(), this->getConnection().getFDS())) {

            /* Receives message from client */
            string request = this->getConnection().receiveByUDP();

            /* Process client's message and decides what to do with it based on the passed code */
            string response = this->process_request(request);

            /* Sends response back t client */
            this->getConnection().replyByUDP(response);

        /* Checks if tcp socket activated */
        } else if (FD_ISSET(this->getConnection().getSocketTCP(), this->getConnection().getFDS())) {



            /* Process client's message and decides what to do with it based on the passed code */
            string response = selector(in_buffer);

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

        memset(in_buffer, 0, MSG_MAX_SIZE);  /* Cleans in_buffer for new iteration */

    }

}


/**
 * @brief Cleans and frees everything related to the Manager.
 */
void Manager::clean() {
    this->getConnection().clean();
}


/**
 * @brief Receives user request, process it and creates a request to be sent back.
 *
 * @param request user request
 *
 * @return server's response
 */
string Manager::process_request(const string& request) {

    /* Extracts requested command */
    string cmd = get_command(request);

    /* Verifies if the user requested a valid command */
    if (cmd == "REG") this->doRegister(request);
    else if (cmd == "UNR") this->doUnregister(request);
    else if (cmd == "LOG") this->doLogin(request);
    else if (cmd == "OUT") this->doLogout(request);
    else if (cmd == "GLS") this->doListGroups(request);
    else if (cmd == "GSR") this->doSubscribe(request);
    else if (cmd == "GUR") this->doUnsubscribe(request);
    else if (cmd == "GLM") this->doMyGroups(request);
    else if (cmd == "ULS") this->doUserList(request);
    else if (cmd == "PST") this->doPost(request);
    else if (cmd == "RTV") this->doRetrieve(request);
    else cout << "Invalid command" << endl;

}


/**
 * @brief Gets server's users.
 *
 * @return server's users.
 */
unordered_map<string, User>* Manager::getUsers() {
    return this->_users;
}


/**
 * @brief Gets server's groups.
 *
 * @return server's groups.
 */
unordered_map<string, Group>* Manager::getGroups() {
    return this->_groups;
}


/**
 * @brief Gets server connection module.
 *
 * @return server's connection module
 */
Connect Manager::getConnection() {
    return this->_connect;
}


/**
 * @brief Checks if the server is being executed in verbose mode.
 *
 * @return verbose mode
 */
bool Manager::getVerbose() const {
    return this->_isVerbose;
}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
string Manager::doRegister(const string& input) {


    /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection().getClientIP() +
        " | PORT: " + this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = register_user(this->getUsers(), inputs[1], inputs[2]);
    return "RRG " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doUnregister(const string& input) {

    /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection().getClientIP() +
                                 " | PORT: " + this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = unregister_user(this->getUsers(), this->getGroups(), inputs[1], inputs[2]);
    return "RUN " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doLogin(const string& input) {

     /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection().getClientIP() +
                                 " | PORT: " + this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = login_user(this->getUsers(), inputs[1], inputs[2]);
    return "RLO " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doLogout(const string& input) {

     /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection().getClientIP() +
                                 " | PORT: " + this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = logout_user(this->getUsers(), inputs[1] , inputs[2]);
    return "ROU " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doListGroups(const string& input) {

     /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "IP: " + this->getConnection().getClientIP() + " | PORT: " +
        this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = list_groups(this->getGroups());
    return "RGL " + to_string(this->getGroups()->size()) + " " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doSubscribe(const string& input) {

     /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " GID: " + inputs[2] + " | IP: " +
        this->getConnection().getClientIP() + " | PORT: " + this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = subscribe(this->getGroups(), this->getUsers(), inputs[1], inputs[2], inputs[3]);
    return "RGS " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doUnsubscribe(const string& input) {

     /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " GID: " + inputs[2] + " | IP: " +
        this->getConnection().getClientIP() + " | PORT: " + this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = unsubscribe(this->getGroups(), this->getUsers(), inputs[1], inputs[2]);
    return "RGU " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doMyGroups(const string& input) {

     /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection().getClientIP() +
        " | PORT: " + this->getConnection().getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = groups_subscribed(this->getGroups(), this->getUsers(), inputs[1]);
    return "RGM " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doUserList(const string& input) {

    /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " GID: " + inputs[2] + " | IP: " +
        this->getConnection().getClientIP() + " | PORT: " + this->getConnection().getClientPort())

    string status = users_subscribed(this->getGroups(), this->getUsers(), inputs[1]);
    return "RUL " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doPost(const string& input) {

    /* If server is in verbose mode, we log the client's information */
//    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection().getClientIP() +
//                                 " | PORT: " + this->getConnection().getClientPort())

/* Splits msg by the spaces and returns an array with everything */
//    split(msg, inputs);
//
//    char text[TEXT_MAX_SIZE];  /* Will hold user input text */
//    char file_name[FILENAME_MAX_SIZE]; /* Will hold the input file */
//    string file;  /* Will hold the input file */
//    memset(text, 0, TEXT_MAX_SIZE);
//    memset(file_name, 0, FILENAME_MAX_SIZE);
//    int checker, file_size, pointer;  /* Used to check if the user did not input a file */
//    bool file_flag = false;
//
//    sscanf(msg.c_str(), R"(%*s %*s %*s %*s "%240[^"]" %n)", text, &checker);
//
//
//    if (checker == 0 || msg[checker] != '\0'){
//        printf("Chegou 3\n");
//        assert_(sscanf(msg.c_str(), R"(%*s %*s %*s %*s "%*240[^"]" %s %d %n)", file_name, &file_size, &pointer) == 1, "Invalid format\n")
//        file_flag = true;
//
//        printf("Chegou 1\n");
//
//        /* Gets the current directory of the project*/
//        char *project_directory = get_current_dir_name();
//        string new_file_path = string(project_directory) + "/server/files/" + file_name ;
//
//        /* Creates a new file*/
//        ofstream file(string(new_file_path), ofstream::out | ofstream::binary);
//
//        printf("Chegou 2\n");
//
//        file.write(&msg[pointer], file_size);
//
//        printf("Chegou aqui\n");
//
//        file.close();
//
//    }
//
//    printf("também chegou aqui\n");
//
//    /* receives status from call function*/
//    status = post_message(&groups, &users, inputs[1], inputs[2], inputs[3], text);
//
//    return "RPT " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doRetrieve(const string& input) {

    //    verbose_(this->getVerbose(), "UID: " + inputs[1] + " GID: " + inputs[2] + " | IP: " +
    //        this->getConnection().getClientIP() + " | PORT: " + this->getConnection().getClientPort())

    /* Splits msg by the spaces and returns an array with everything */
//    split(msg, inputs);
//
//    verbose_(isVerbose, "UID: " + inputs[1] + "GID: " + inputs[2] + "MID: " + inputs[3] + " | IP: " + ip + " | PORT: " + port)
//
//    /* receives status from call function*/
//    status = retrieve_message(&groups, inputs[2], inputs[3]);
//
//    return "RRT " + status + "\n";

}
