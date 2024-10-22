#include "manager.h"


/**
 * @brief Manager class constructor.
 *
 * @param connect connection to our server
 */
Manager::Manager(Connect& connect, User& user) : _connect(connect), _user(&user) {}


/**
 * @brief Gets manager's user.
 *
 * @return user
 */
User* Manager::getUser() {
    return this->_user;
}


/**
 * @brief Gets manager's connection to the server.
 *
 * @return connection model
 */
Connect& Manager::getConnection() {
    return this->_connect;
}


/**
 * @brief Cleans everything related to the Manager.
 */
void Manager::clean() {
    this->getConnection().clean();
}


/**
 * @brief Mounts and sends a register command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doRegister(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command */
    validate_(inputs.size() == 3, "User ID and/or password not inputted")
    validate_(inputs[1].size() == 5, "User ID must have 5 figures")
    validate_(isNumber(inputs[1]), "User ID must be a number")
    validate_(inputs[2].size() == 8, "User password must have 8 alphanumerical characters")
    validate_(isAlphaNumeric(inputs[2]), "User password must have only alphanumerical characters")

    /* Transforms user input into a valid command to be sent to the server */
    req = "REG " + inputs[1] + " " + inputs[2] + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs the user of the result */
    if (strcmp(outputs[1].c_str(), "OK") == 0) cout << "User " + inputs[1] + " registered successfully" << endl;
    else if (strcmp(outputs[1].c_str(), "DUP") == 0) cerr << "User " + inputs[1] + " already registered" << endl;
    else if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Too many users already registered" << endl;
    else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and sends an unregister command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doUnregister(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command */
    validate_(inputs.size() == 3, "User ID and/or password not inputted")
    validate_(inputs[1].size() == 5, "User ID must have 5 figures")
    validate_(isNumber(inputs[1]), "User ID must be a number")
    validate_(inputs[2].size() == 8, "User password must have 8 alphanumerical characters")
    validate_(isAlphaNumeric(inputs[2]), "User password must have only alphanumerical characters")

    /* Transforms user input into a valid command to be sent to the server */
    req = "UNR " + inputs[1] + " " + inputs[2] + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs the user of the result */
    if (strcmp(outputs[1].c_str(), "OK") == 0) {
        cout << "User " + inputs[1] + " unregistered successfully" << endl;
        /*Logouts the user if the user was logged in*/
        if (this->getUser()->getLoggedStatus()) this->getUser()->resetUser();
    }
    else if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Unregister failed" << endl;
    else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and sends a login command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doLogin(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits message by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 3, "User ID and/or password not inputted")
    validate_(inputs[1].size() == 5, "User ID must have 5 figures")
    validate_(isNumber(inputs[1]), "User ID must be a number")
    validate_(inputs[2].size() == 8, "User password must have 8 alphanumerical characters")
    validate_(isAlphaNumeric(inputs[2]), "User password must have only alphanumerical characters")
    validate_(!this->getUser()->getLoggedStatus(), "User must logout first before log in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "LOG " + inputs[1] + " " + inputs[2] + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs the user of the result */
    if (strcmp(outputs[1].c_str(), "OK") == 0) {
        cout << "Login user " + inputs[1] + " successful" << endl;
        this->getUser()->setLoggedStatus(true);
        this->getUser()->setUserID( inputs[1]);
        this->getUser()->setUserPassword(inputs[2]);
    }
    else if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Login error" << endl;
    else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and sends a logout command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doLogout(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 1, "Too many arguments")
    validate_(this->getUser()->getLoggedStatus(), "User must be logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "OUT " + this->getUser()->getUserID() + " " + this->getUser()->getUserPassword() + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs the user of the result */
    if (strcmp(outputs[1].c_str(), "OK") == 0) {
        cout << "Logout user " + this->getUser()->getUserID() + " successful" << endl;
        this->getUser()->resetUser();
    }
    else if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Logout error" << endl;
    else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and performs a show UID command.
 *
 * @param input user input command
 */
void Manager::doShowUID(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 1, "Too many arguments")

    if (this->getUser()->getUserID().empty()){
        cout << "No user logged in" << endl;
    } else {
        cout << "User " + this->getUser()->getUserID() + " logged in" << endl;
    }

}


/**
 * @brief Mounts and performs an exit command by also logging out the user from the server.
 *
 * @param input user input command
 */
void Manager::doExit(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 1, "Too many arguments")

    /* Still needs to log out user if he is logged in */
    if (this->getUser()->getLoggedStatus()) {

        /* Transforms user input into a valid command to be sent to the server */
        req = "OUT " + this->getUser()->getUserID() + " " + this->getUser()->getUserPassword() + "\n";

        /* Sends request to server by UDP and gets response */
        this->getConnection().sendByUDP(req);
        string response = this->getConnection().receivesByUDP();

        /* Splits response to be analysed */
        vector<string> outputs;
        split(response, outputs);

        /* Analyses response and informs the user of the result */
        if (strcmp(outputs[1].c_str(), "OK") == 0) {
            cout << "Logout user " + this->getUser()->getUserID() + " successful" << endl;
            this->getUser()->resetUser();
        }
        else if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Logout error" << endl;
        else cerr << "Invalid status" << endl;

    }

    cout << "Exit successful" << endl;

}


/**
 * @brief Mounts and sends a list groups command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doListGroups(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 1, "Too many arguments")

    /* Transforms user input into a valid command to be sent to the server */
    req = "GLS\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be shown */
    vector<string> outputs;
    split(response, outputs);

    /* Prints response to the user */
    if (strcmp(outputs[1].c_str(), "0") != 0) {

        int number_groups = atoi(outputs[1].c_str());

        cout << "There are " + outputs[1] + " groups:" << endl;
        for (auto i = outputs.begin() + 2; number_groups > 0; i++){
            cout << "Group " + *i; ++i;
            cout << ": " + *i; ++i;
            cout << " Last MSG: " + *i << endl;
            number_groups --;
        }
    }
    else {
        cout << "No groups found" << endl;
    }

}


/**
 * @brief Mounts and sends a subscribe command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doSubscribe(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 3, "Group ID and/or name not inputted")
    validate_(inputs[1].size() <= 2, "Group ID must have 2 figures")
    validate_(isNumber(inputs[1]), "Group ID must be a number")
    validate_(inputs[2].size() <= 24, "Group name must be limited to 24 alphanumerical characters plus '-' and '_'")
    validate_(isAlphaNumericPlus(inputs[2]), "Group name must have only alphanumerical characters plus '-' and '_'")
    validate_(this->getUser()->getLoggedStatus(), "No user logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "GSR " + this->getUser()->getUserID() + " " + inputs[1] + " " + inputs[2] + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs the user of the result */
    if (strcmp(outputs[1].c_str(), "OK") == 0) cout << "User " + this->getUser()->getUserID() + " subscribed to group " + inputs[1] + " successfully" << endl;
    else if (strcmp(outputs[1].c_str(), "NEW") == 0) cout << "User " + this->getUser()->getUserID() + " subscribed to new group successfully" << endl;
    else if (strcmp(outputs[1].c_str(), "E_USR") == 0) cerr << "Invalid user ID" << endl;
    else if (strcmp(outputs[1].c_str(), "E_GRP") == 0) cerr << "Invalid group ID" << endl;
    else if (strcmp(outputs[1].c_str(), "E_GNAME") == 0) cerr << "Invalid group name" << endl;
    else if (strcmp(outputs[1].c_str(), "E_FULL") == 0) cerr << "Too many groups already created" << endl;
    else if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Subscribe error" << endl;
    else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and sends an unsubscribe command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doUnsubscribe(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 2, "Group ID not inputted")
    validate_(isNumber(inputs[1]), "Group ID must be a number")
    validate_(inputs[1].size() <= 2, "Group ID must have 2 figures")
    validate_(this->getUser()->getLoggedStatus(), "No user logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "GUR " + this->getUser()->getUserID() + " " + inputs[1] + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs the user of the result */
    if (strcmp(outputs[1].c_str(), "OK") == 0) cout << "User " + this->getUser()->getUserID() + " unsubscribed from group " + inputs[1] + " successfully" << endl;
    else if (strcmp(outputs[1].c_str(), "E_USR") == 0) cerr << "Invalid user ID";
    else if (strcmp(outputs[1].c_str(), "E_GRP") == 0) cerr << "Invalid group ID" << endl;
    else if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Unsubscribe error" << endl;
    else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and sends a my groups command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doMyGroups(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 1, "Too many arguments")
    validate_(this->getUser()->getLoggedStatus(), "No user logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "GLM " + this->getUser()->getUserID() + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByUDP(req);
    string response = this->getConnection().receivesByUDP();

    /* Splits response to be shown */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and shows it to the user */
    if (strcmp(outputs[1].c_str(), "0") != 0) {
        int number_groups = atoi(outputs[1].c_str());

        cout << "User " << this->getUser()->getUserID() << " is member of the following group :" << endl;
        for (auto i = outputs.begin() + 2; number_groups > 0; i++){
            cout << "Group " + *i; ++i;
            cout << ": " + *i; ++i;
            cout << " Last MSG: " + *i << endl;
            number_groups --;
        }
    }
    else {
        cout << "User " << this->getUser()->getUserID() << " isn't member of any group" << endl;
    }


}


/**
 * @brief Mounts and performs a select command.
 *
 * @param input user input command
 */
void Manager::doSelect(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 2, "User did not input group ID")
    validate_(isNumber(inputs[1]), "Group ID is not a number")
    validate_(inputs[1].size() <= 2, "Group ID isn't a 2 digit-number")

    /* Saves selected group locally */
    this->getUser()->setUserSelectedGroupID(inputs[1]);
    
}


/**
 * @brief Mounts and performs a show GID command.
 *
 * @param input user input command
 */
void Manager::doShowGID(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 1, "Too many arguments")
    validate_(this->getUser()->getLoggedStatus(), "No user logged in")

    /* Shows the client its currently selected group */
    if (this->getUser()->getSelectedGroupID().empty()){
        cout << "No group selected" << endl;
    } else {
        cout << "Group " + this->getUser()->getSelectedGroupID() + " selected" << endl;
    }
    
}


/**
 * @brief Mounts and sends an user list command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doUserList(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(inputs.size() == 1, "Too many arguments")
    validate_(this->getUser()->getLoggedStatus(), "No user logged in")
    //TODO: remover isto (Sofia)
    validate_(!this->getUser()->getSelectedGroupID().empty(), "No group selected")

    /* Transforms user input into a valid command to be sent to the server */
    req = "ULS " + this->getUser()->getSelectedGroupID() +  "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().init_socket_tcp();
    this->getConnection().sendByTCP(req);
    string response = this->getConnection().receivesByTCP();
    this->getConnection().closeTCP();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs the user of the result */
    if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Group does not exist." << endl;
    else if (strcmp(outputs[1].c_str(), "OK") == 0) {
        cout << "User list for group " << outputs[2] << " : "<< endl;
        for (auto i = outputs.begin() + 3; i != outputs.end(); ++i) cout << *i << endl;
        cout << "End of users list" << endl;
    } else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and sends a post command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doPost(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */
    string response;  /* Holds server's response */

    char text[TEXT_MAX_SIZE];  /* Will hold user input text */
    char file_name[FILENAME_MAX_SIZE]; /* Will hold the input file */
    memset(text, 0, TEXT_MAX_SIZE);
    memset(file_name, 0, FILENAME_MAX_SIZE);
    int hasFile = 0; /* Used to check if the user did not input a file */

    /* Opens TCP connection with the server */
    this->getConnection().init_socket_tcp();

    /* Extracts user's message from input */
    assert_(sscanf(input.c_str(), R"(%*s "%240[^"]" %n)", text, &hasFile) == 1, "Text is limited to 240 characters\n")

    /* Verifies if the user input a valid command and that this command can be issued */
    validate_(this->getUser()->getLoggedStatus(), "No user logged in")
    validate_(!this->getUser()->getSelectedGroupID().empty(), "No group selected")
    validate_(strlen(file_name) <= FILENAME_MAX_SIZE, "File name up to 24 characters, including the dot and the file type")

    if (hasFile == 0 || input[hasFile] != '\0') {  /* User input a file */

        /* Extracts file name from user's input */
        assert_(sscanf(input.c_str(), R"(%*s "%*240[^"]" %s)", file_name) == 1, "Invalid format\n")

        /* Gets the current directory of the project*/
        char *project_directory = get_current_dir_name();
        string file_path = string(project_directory) + "/client/bin/" + file_name ;

        ifstream file(string(file_path), ifstream::in | ifstream::binary | ifstream::ate);
        file.seekg(0, ios::end);
        int file_length = file.tellg();  /* Sends request size */
        file.seekg(0, ios::beg);

        /* Transforms user input into a valid command to be sent to the server */
        req = "PST " + this->getUser()->getUserID() + " " + this->getUser()->getSelectedGroupID() + " " +
                to_string(strlen(text)) + " " + "\"" + text + "\"" + " " + string(file_name) + " "
                + to_string(file_length) + "\n";

        /* Sends initial information that will allow the server to receive also a file */
        this->getConnection().sendByTCP(req);

        /* Then sends file's data */
        this->getConnection().sendByTCPWithFile(file, file_length);

        /* Gets response from server */
        response = this->getConnection().receivesByTCP();

        /* Close file*/
        file.close();

    } else {  /* User did not input a file */

        string len = to_string(strlen(text));

        /* Transforms user input into a valid command to be sent to the server */
        req = "PST " + this->getUser()->getUserID() + " " + this->getUser()->getSelectedGroupID() + " " + len + " " + "\"" + text + "\"\n";

        /* Since we don't have any files, we can just send it as a normal request */
        this->getConnection().sendByTCP(req);
        response = this->getConnection().receivesByTCP();

    }

    this->getConnection().closeTCP();  // Closes previous connection

    vector<string> outputs;  /* Holds a list of strings with the outputs from our server */
    split(response, outputs);  /* Splits msg by the spaces and returns an array with everything */

    /* Analyses response and informs the user of the result */
    if (outputs[1] == "NOK") cerr << "Failed. Message couldn't be posted" << endl;
    else if (isNumber(outputs[1])) cout << outputs[1] << endl;
    else cerr << "Invalid status" << endl;

}


/**
 * @brief Mounts and sends a retrieve command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doRetrieve(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
    split(input, inputs);

    /* Opens TCP connection with the server */
    this->getConnection().init_socket_tcp();

    req = "RTV " + this->getUser()->getUserID() + " " + this->getUser()->getSelectedGroupID() + " " + inputs[1] + "\n";

    /* Sends request to server by UDP and gets response */
    this->getConnection().sendByTCP(req);

    string response = this->getConnection().receivesByTCPWithFile();

    /* Splits response to be analysed */
    vector<string> outputs;
    split(response, outputs);

    /* Analyses response and informs user of the result */
    if (strcmp(outputs[1].c_str(), "NOK") == 0) cerr << "Failed. Message couldn't be retrieved" << endl;
    else if (strcmp(outputs[1].c_str(), "EOF") == 0) cout << "No messages available" << endl;
    else {
        cout << response;
    }
    
}
