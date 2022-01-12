#ifndef PROJETO_RC_39_V2_MANAGER_H
#define PROJETO_RC_39_V2_MANAGER_H

#include "../misc/helpers.h"
#include "user.h"

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
        User _user;

    public:
        
        /**
         * @brief Manager class constructor.
         */
        explicit Manager();

        /**
         * @brief Gets manager's user.
         *
         * @return user
         */
        User getUser();

        /**
         * @brief Mounts and sends a register command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doRegister(const string& input, const int& fd_udp);

        /**
         * @brief Mounts and sends an unregister command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doUnregister(const string& input, const int& fd_udp);

        /**
         * @brief Mounts and sends a login command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doLogin(const string& input, const int& fd_udp);

        /**
         * @brief Mounts and sends a logout command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doLogout(const string& input, const int& fd_udp);

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
         * @param fd_udp file descriptor that has an udp connection
         */
        void doExit(const string& input, const int& fd_udp);

        /**
         * @brief Mounts and sends a list groups command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doListGroups(const string& input, const int& fd_udp);

        /**
         * @brief Mounts and sends a subscribe command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doSubscribe(const string& input, const int& fd_udp);

        /**
         * @brief Mounts and sends an unsubscribe command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doUnsubscribe(const string& input, const int& fd_udp);

        /**
         * @brief Mounts and sends a my groups command and analyses response from server.
         *
         * @param input user input command
         * @param fd_udp file descriptor that has an udp connection
         */
        void doMyGroups(const string& input, const int& fd_udp);

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
         * @param fd_tcp file descriptor that has an tcp connection
         */
        void doUserList(const string& input, const int& fd_tcp);

        /**
         * @brief Mounts and sends a post command and analyses response from server.
         *
         * @param input user input command
         * @param fd_tcp file descriptor that has an tcp connection
         */
        void doPost(const string& input, const int& fd_tcp);

        /**
         * @brief Mounts and sends a retrieve command and analyses response from server.
         *
         * @param input user input command
         * @param fd_tcp file descriptor that has an tcp connection
         */
        void doRetrieve(const string& input, const int& fd_tcp);

};

#endif //PROJETO_RC_39_V2_MANAGER_H
