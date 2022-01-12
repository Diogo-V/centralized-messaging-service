#include "manager.h"


/**
 * @brief Manager class constructor.
 */
Manager::Manager() {
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
void Manager::doRegister(const string& input, const int& fd_udp) {

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
void Manager::doUnregister(const string& input, const int& fd_udp) {

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
void Manager::doLogin(const string& input, const int& fd_udp) {

    vector<string> inputs;  /* Holds a list of strings with the inputs from our user */
    string req;  /* Holds request that is going to be sent to the server */

    /* Splits msg by the spaces and returns an array with everything */
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

    con = UDP;  /* Sets connection type to be used by the client to connect to the server */

    this->getUser().setUserID( inputs[1]);  /* Sets user's _uid */
    this->getUser().setUserPassword(inputs[2]);  /* Saves user's password locally */

}


/**
 * @brief Mounts and sends a logout command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doLogout(const string& input, const int& fd_udp) {
    
}


/**
 * @brief Mounts and performs a show UID command.
 *
 * @param input user input command
 */
void Manager::doShowUID(const string& input) {
    
}


/**
 * @brief Mounts and performs an exit command by also logging out the user from the server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doExit(const string& input, const int& fd_udp) {
    
}


/**
 * @brief Mounts and sends a list groups command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doListGroups(const string& input, const int& fd_udp) {
    
}


/**
 * @brief Mounts and sends a subscribe command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doSubscribe(const string& input, const int& fd_udp) {
    
}


/**
 * @brief Mounts and sends an unsubscribe command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doUnsubscribe(const string& input, const int& fd_udp) {
    
}


/**
 * @brief Mounts and sends a my groups command and analyses response from server.
 *
 * @param input user input command
 * @param fd_udp file descriptor that has an udp connection
 */
void Manager::doMyGroups(const string& input, const int& fd_udp) {
    
}


/**
 * @brief Mounts and performs a select command.
 *
 * @param input user input command
 */
void Manager::doSelect(const string& input) {
    
}


/**
 * @brief Mounts and performs a show GID command.
 *
 * @param input user input command
 */
void Manager::doShowGID(const string& input) {
    
}


/**
 * @brief Mounts and sends an user list command and analyses response from server.
 *
 * @param input user input command
 * @param fd_tcp file descriptor that has an tcp connection
 */
void Manager::doUserList(const string& input, const int& fd_tcp) {
    
}


/**
 * @brief Mounts and sends a post command and analyses response from server.
 *
 * @param input user input command
 * @param fd_tcp file descriptor that has an tcp connection
 */
void Manager::doPost(const string& input, const int& fd_tcp) {
    
}


/**
 * @brief Mounts and sends a retrieve command and analyses response from server.
 *
 * @param input user input command
 * @param fd_tcp file descriptor that has an tcp connection
 */
void Manager::doRetrieve(const string& input, const int& fd_tcp) {
    
}
