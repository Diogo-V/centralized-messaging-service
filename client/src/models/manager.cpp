#include "manager.h"


/**
 * @brief Manager class constructor.
 *
 * @param connect connection to our server
 */
Manager::Manager(Connect& connect) : _connect(connect) {
    User user;
    this->_user = user;
}


/**
 * @brief Gets manager's user.
 *
 * @return user
 */
User Manager::getUser() {
    return this->_user;
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
    validate_(inputs.size() == 3, "User did not input user ID and/or password")
    validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
    validate_(isNumber(inputs[1]), "User ID is not a number")
    validate_(inputs[2].size() == 8, "User password should have 8 alphanumerical characters")
    validate_(isAlphaNumeric(inputs[2]), "User password should have only alphanumerical characters")

    /* Transforms user input into a valid command to be sent to the server */
    req = "REG " + inputs[1] + " " + inputs[2] + "\n";



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
    validate_(inputs.size() == 3, "User did not input user ID and/or password")
    validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
    validate_(isNumber(inputs[1]), "User ID is not a number")
    validate_(inputs[2].size() == 8, "User password should have 8 alphanumerical characters")
    validate_(isAlphaNumeric(inputs[2]), "User password should have only alphanumerical characters")

    // Updates login status
    if (this->getUser().getUserID() == inputs[1] && this->getUser().getLoggedStatus())
        bool logouts = true;

    /* Transforms user input into a valid command to be sent to the server */
    req = "UNR " + inputs[1] + " " + inputs[2] + "\n";

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
    validate_(inputs.size() == 3, "User did not input user ID and/or password")
    validate_(inputs[1].size() == 5, "User ID should have 5 numbers")
    validate_(isNumber(inputs[1]), "User ID is not a number")
    validate_(inputs[2].size() == 8, "User password should have 8 alphanumerical characters")
    validate_(isAlphaNumeric(inputs[2]), "User password should have only alphanumerical characters")
    validate_(!this->getUser().getLoggedStatus(), "Client is already logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "LOG " + inputs[1] + " " + inputs[2] + "\n";



    /* Updates user's info */
    this->getUser().setUserID( inputs[1]);
    this->getUser().setUserPassword(inputs[2]);

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
    validate_(this->getUser().getLoggedStatus(), "Client needs to be logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "OUT " + this->getUser().getUserID() + " " + this->getUser().getUserPassword() + "\n";

//    con = UDP;  /* Sets connection type to be used by the client to connect to the server */

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
    validate_(this->getUser().getLoggedStatus(), "No client logged in")

    cout << this->getUser().getUserID() << endl;

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
    if (this->getUser().getLoggedStatus()) {
        req = "OUT " + this->getUser().getUserID() + " " + this->getUser().getUserPassword() + "\n";
//        con = UDP;
    }

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

//    con = UDP;  /* Sets connection type to be used by the client to connect to the server */

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
    validate_(inputs.size() == 3, "User did not input group ID and/or group name")
    validate_(inputs[1].size() <= 2, "Group ID isn't 2 digit-number")
    validate_(isNumber(inputs[1]), "Group ID is not a number")
    validate_(inputs[2].size() <= 24, "Group name limited to 24 characters")
    validate_(isAlphaNumericPlus(inputs[2]), "Group name should have only alphanumerical characters plus '-' and "
                                             "'_'")
    validate_(this->getUser().getLoggedStatus(), "Client is not logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "GSR " + this->getUser().getUserID() + " " + inputs[1] + " " + inputs[2] + "\n";

//    con = UDP;  /* Sets connection type to be used by the client to connect to the server */
    
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
    validate_(inputs.size() == 2, "User did not input group ID")
    validate_(isNumber(inputs[1]), "Group ID is not a number")
    validate_(inputs[1].size() <= 2, "Group ID isn't a 2 digit-number")
    validate_(this->getUser().getLoggedStatus(), "Client is not logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "GUR " + this->getUser().getUserID() + " " + inputs[1] + "\n";

//    con = UDP;  /* Sets connection type to be used by the client to connect to the server */
    
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
    validate_(this->getUser().getLoggedStatus(), "Client is not logged in")

    /* Transforms user input into a valid command to be sent to the server */
    req = "GLM " + this->getUser().getUserID() + "\n";

//    con = UDP;  /* Sets connection type to be used by the client to connect to the server */
    
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
    validate_(this->getUser().getLoggedStatus(), "Client is not logged in")

    /* Saves selected group locally */
    this->getUser().setUserSelectedGroupID(inputs[1]);
    
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
    validate_(this->getUser().getLoggedStatus(), "Client is not logged in")
    validate_(!this->getUser().getSelectedGroupID().empty(), "No selected group")

    /* Shows the client its currently selected group */
    cout << this->getUser().getSelectedGroupID() << endl;
    
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
    validate_(this->getUser().getLoggedStatus(), "Client is not logged in")
    validate_(!this->getUser().getSelectedGroupID().empty(), "No selected group")

    /* Transforms user input into a valid command to be sent to the server */
    req = "ULS " + this->getUser().getSelectedGroupID() +  "\n";

//    con = TCP;  /* Sets connection type to be used by the client to connect to the server */
    
}


/**
 * @brief Mounts and sends a post command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doPost(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    // TODO: @Sofia-Morgado -> Copy your implementation
    
}


/**
 * @brief Mounts and sends a retrieve command and analyses response from server.
 *
 * @param input user input command
 */
void Manager::doRetrieve(const string& input) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    // TODO: @Sofia-Morgado -> Copy your implementation
    
}
