#ifndef PROJETO_RC_39_V2_MANAGER_H
#define PROJETO_RC_39_V2_MANAGER_H

#include "../misc/helpers.h"
#include "user.h"
#include "connect.h"

#include <string>
#include <iostream>
#include <vector>
#include <sstream>


using namespace std;


/**
 * Holds and performs set of commands that can be sent to our server.
 */
class Manager {

    private:
        
        /**
         * @brief Current client's user representation.
         */
        User *_user;

        /**
         * @brief Connection to the server.
         */
         Connect _connect;

    public:
        
        /**
         * @brief Manager class constructor.
         *
         * @param connect connection to our server
         * @param user current client's information
         */
        explicit Manager(Connect& connect, User& user);

        /**
         * @brief Gets manager's user.
         *
         * @return user
         */
        User* getUser();

        /**
         * @brief Gets manager's connection to the server.
         *
         * @return connection model
         */
        Connect getConnection();

        /**
         * @brief Cleans and frees everything related to the Manager.
         */
        void clean();

        /**
         * @brief Mounts and sends a register command and analyses response from server.
         *
         * @param input user input command
         */
        void doRegister(const string& input);

        /**
         * @brief Mounts and sends an unregister command and analyses response from server.
         *
         * @param input user input command
         */
        void doUnregister(const string& input);

        /**
         * @brief Mounts and sends a login command and analyses response from server.
         *
         * @param input user input command
         */
        void doLogin(const string& input);

        /**
         * @brief Mounts and sends a logout command and analyses response from server.
         *
         * @param input user input command
         */
        void doLogout(const string& input);

        /**
         * @brief Mounts and performs a show UID command.
         *
         * @param input user input command
         */
        void doShowUID(const string& input);

        /**
         * @brief Mounts and performs an exit command by also logging out the user from the server.
         *
         * @param input user input command
         */
        void doExit(const string& input);

        /**
         * @brief Mounts and sends a list groups command and analyses response from server.
         *
         * @param input user input command
         */
        void doListGroups(const string& input);

        /**
         * @brief Mounts and sends a subscribe command and analyses response from server.
         *
         * @param input user input command
         */
        void doSubscribe(const string& input);

        /**
         * @brief Mounts and sends an unsubscribe command and analyses response from server.
         *
         * @param input user input command
         */
        void doUnsubscribe(const string& input);

        /**
         * @brief Mounts and sends a my groups command and analyses response from server.
         *
         * @param input user input command
         */
        void doMyGroups(const string& input);

        /**
         * @brief Mounts and performs a select command.
         *
         * @param input user input command
         */
        void doSelect(const string& input);

        /**
         * @brief Mounts and performs a show GID command.
         *
         * @param input user input command
         */
        void doShowGID(const string& input);

        /**
         * @brief Mounts and sends an user list command and analyses response from server.
         *
         * @param input user input command
         */
        void doUserList(const string& input);

        /**
         * @brief Mounts and sends a post command and analyses response from server.
         *
         * @param input user input command
         */
        void doPost(const string& input);

        /**
         * @brief Mounts and sends a retrieve command and analyses response from server.
         *
         * @param input user input command
         */
        void doRetrieve(const string& input);

};

#endif
