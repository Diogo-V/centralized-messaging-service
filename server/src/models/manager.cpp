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
        FD_ZERO(this->getConnection()->getFDS());
        FD_SET(this->getConnection()->getSocketUDP(), this->getConnection()->getFDS());
        FD_SET(this->getConnection()->getSocketTCP(), this->getConnection()->getFDS());

        /* Blocks until one of the descriptors, previously set in are ready to by read. Returns number of file descriptors ready */
        uint8_t counter = select(this->getConnection()->getSocketTCP() + 1,this->getConnection()->getFDS(),
                                 (fd_set*) nullptr,(fd_set*) nullptr,(struct timeval *) nullptr);
        assert_(counter > 0, "Select threw an error")

        /* Cleans previous iteration so that it does not bug */
        this->getConnection()->cleanAddr();

        /* Checks if udp socket activated */
        if (FD_ISSET(this->getConnection()->getSocketUDP(), this->getConnection()->getFDS())) {

            /* Receives message from client */
            string request = this->getConnection()->receiveByUDP();

            /* Process client's message and decides what to do with it based on the passed code */
            string response = this->process_request(request);

            /* Sends response back to client */
            this->getConnection()->replyByUDP(response);

        /* Checks if tcp socket activated */
        } else if (FD_ISSET(this->getConnection()->getSocketTCP(), this->getConnection()->getFDS())) {

            string request = this->getConnection()->receiveByTCP();

            /* Process client's message and decides what to do with it based on the passed code */
            string response = this->process_request(request);

            /* Sends response back to client */
            this->getConnection()->replyByTCP(response);

            /* Closes file descriptor to avoid errors */
            close(this->getConnection()->getSocketTmpTCP());

        } else {
            assert_(false, "No correct file descriptor was activated in select")
        }

    }

}


/**
 * @brief Cleans and frees everything related to the Manager.
 */
void Manager::clean() {
    this->getConnection()->clean();
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
    if (cmd == "REG") return this->doRegister(request);
    else if (cmd == "UNR") return this->doUnregister(request);
    else if (cmd == "LOG") return this->doLogin(request);
    else if (cmd == "OUT") return this->doLogout(request);
    else if (cmd == "GLS") return this->doListGroups(request);
    else if (cmd == "GSR") return this->doSubscribe(request);
    else if (cmd == "GUR") return this->doUnsubscribe(request);
    else if (cmd == "GLM") return this->doMyGroups(request);
    else if (cmd == "ULS") return this->doUserList(request);
    else if (cmd == "PST") return this->doPost(request);
    else if (cmd == "RTV") return this->doRetrieve(request);
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
Connect* Manager::getConnection() {
    return &this->_connect;
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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection()->getClientIP() +
        " | PORT: " + this->getConnection()->getClientPort())

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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection()->getClientIP() +
                                 " | PORT: " + this->getConnection()->getClientPort())

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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection()->getClientIP() +
                                 " | PORT: " + this->getConnection()->getClientPort())

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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection()->getClientIP() +
                                 " | PORT: " + this->getConnection()->getClientPort())

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
    verbose_(this->getVerbose(), "IP: " + this->getConnection()->getClientIP() + " | PORT: " +
        this->getConnection()->getClientPort())

    /* Calls api to process command and send back a status to be then sent to the client */
    string status = list_groups(this->getGroups());
    if (this->getGroups()->empty()) {
        return "RGL " + to_string(this->getGroups()->size()) + status + "\n";
    }
    else{
        return "RGL " + to_string(this->getGroups()->size()) + " " + status + "\n";
    }

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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | GID: " + inputs[2] + " | IP: " +
        this->getConnection()->getClientIP() + " | PORT: " + this->getConnection()->getClientPort())

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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | GID: " + inputs[2] + " | IP: " +
        this->getConnection()->getClientIP() + " | PORT: " + this->getConnection()->getClientPort())

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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | IP: " + this->getConnection()->getClientIP() +
        " | PORT: " + this->getConnection()->getClientPort())

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
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | GID: " + inputs[2] + " | IP: " +
        this->getConnection()->getClientIP() + " | PORT: " + this->getConnection()->getClientPort())

    string status = users_subscribed(this->getGroups(), inputs[1].c_str());
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

    /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | GID: " + inputs[2] + " | IP: " +
        this->getConnection()->getClientIP() + " | PORT: " + this->getConnection()->getClientPort())

    char text[TEXT_MAX_SIZE];  /* Will hold user input text */
    char file_name[FILENAME_MAX_SIZE]; /* Will hold the input file */
    string file;  /* Will hold the input file */
    memset(text, 0, TEXT_MAX_SIZE);
    memset(file_name, 0, FILENAME_MAX_SIZE);
    int checker = 0;  /* Used to check if the user did not input a file */
    int file_size;
    string status;

    /* Gets text and checks if  */
    sscanf(input.c_str(), R"(%*s %*s %*s %*s "%240[^"]" %n)", text, &checker);

    /* Checks if user input any files and acts accordingly */
    if (checker == 0 || input[checker] != '\0') {
        sscanf(input.c_str(), R"(%*s %*s %*s %*s "%240[^"]" %s %d)", text, file_name, &file_size);
        status = post_message(this->getGroups(), this->getUsers(), inputs[1], inputs[2], inputs[3], text,
                              file_name, to_string(file_size));
        this->getConnection()->receiveByTCPWithFile(file_name, file_size);
    } else {
        status = post_message(this->getGroups(), this->getUsers(), inputs[1], inputs[2], inputs[3], text);
    }

    return "RPT " + status + "\n";

}


/**
 * @brief Receives request from client, processes it and returns a response.
 *
 * @param input user input command
 *
 * @return response to be sent back to the client
 */
 string Manager::doRetrieve(const string& input) {

    /* Splits input by the spaces and returns an array with everything */
    vector<string> inputs;
    split(input, inputs);

    /* Auxiliary buffer */
    char buffer[MAX_REQUEST_SIZE];
    memset(buffer, 0, MAX_REQUEST_SIZE);

    /* If server is in verbose mode, we log the client's information */
    verbose_(this->getVerbose(), "UID: " + inputs[1] + " | GID: " + inputs[2] + " | IP: " +
        this->getConnection()->getClientIP() + " | PORT: " + this->getConnection()->getClientPort())

    /* Gets status and vector of messages upon success to be worked on this function */
    vector<Message> result;
    string status = retrieve_message(this->getGroups(), inputs[2], inputs[3], result);

    /* If we had some kind of error, ignores loop */
    if (status != "OK") return "RRT " + status + "\n";

    /* Inits output string */
    string res = "RRT " + status + " " + to_string(result.size()) + "\n";

    this->getConnection()->replyByTCP(res);  // Sends current request
    res = "";  // Clears response to not conflict with the rest of the commands

    /* Mounts string to be sent to the user by reading every message and transforming it into
     * a valid response */
    for (auto itr: result) {

        /* Creates main request information */
        res = itr.getMessageId() + " " + itr.getMessageUid() + " " + to_string(itr.getMessageText().length())
                + " \"" + itr.getMessageText() + "\"";

        /* We do this to have a way of alerting the client an attached file */
        if (! itr.getMessageFileName().empty()) res += " ";
        else res += "\n";

        this->getConnection()->replyByTCP(res);  // Sends current request

        if (! itr.getMessageFileName().empty()) {  /* Checks if file is associated */

            /* Appends information related to the input file */
            res = "/ " + itr.getMessageFileName() + " " + itr.getMessageFileSize() + " " + "\n";

            this->getConnection()->replyByTCP(res);  // Sends current request

            /* Opens file to be read */
            string file_path = string(get_current_dir_name()) + "/server/files/" + itr.getMessageFileName();
            ifstream file(file_path, ifstream::in | ifstream::binary);

            /* Sends file to client */
            this->getConnection()->replyByTCPWithFile(file, stoi(itr.getMessageFileSize()));

            file.close();

        }

        res = "";  // Resets response

    }

    res += "\n";

}
