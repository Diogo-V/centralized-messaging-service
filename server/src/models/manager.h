#ifndef PROJETO_RC_39_V2_MANAGER_H
#define PROJETO_RC_39_V2_MANAGER_H

#include "user.h"
#include "../misc/helpers.h"
#include "group.h"
#include "connect.h"
#include "../api.h"

#include <string>


using namespace std;


/**
 * Manages main server routines.
 */
class Manager {

    private:

        /**
         * @brief Holds all users in our server. Key is user's id.
         */
        unordered_map<string, User> *_users;

        /**
         * @brief Holds all groups in our server. Key is group's id.
         */
        unordered_map<string, Group> *_groups;

        /**
         * @brief Allows connecting with a client.
         */
        Connect _connect;

        /**
         * @brief Is true if the server is set to verbose mode.
         */
        bool _isVerbose;

    public:

        /**
         * @brief Manager class constructor.
         *
         * @param users map of user currently in the server
         * @param groups map of groups registered in the server
         * @param connect module for connecting with clients
         * @param isVerbose checks if server is being ran in verbose mode
         */
        explicit Manager(unordered_map<string, User>* users, unordered_map<string, Group>* groups,
                         Connect& connect, bool isVerbose);

        /**
         * @brief Gets server's users.
         *
         * @return server's users.
         */
        unordered_map<string, User>* getUsers();

        /**
         * @brief Gets server's groups.
         *
         * @return server's groups.
         */
        unordered_map<string, Group>* getGroups();

        /**
         * @brief Gets server connection module.
         *
         * @return server's connection module
         */
        Connect* getConnection();

        /**
         * @brief Checks if the server is being executed in verbose mode.
         *
         * @return verbose mode
         */
        bool getVerbose() const;

        /**
         * @brief Starts main server loop.
         */
        void start_server();

        /**
         * @brief Cleans and frees everything related to the Manager.
         */
        void clean();

        /**
         * @brief Receives user request, process it and creates a request to be sent back.
         *
         * @param request user request
         *
         * @return server's response
         */
        string process_request(const string& request);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doRegister(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doUnregister(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doLogin(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doLogout(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doListGroups(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doSubscribe(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doUnsubscribe(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doMyGroups(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doUserList(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doPost(const string& input);

        /**
         * @brief Receives request from client, processes it and returns a response.
         *
         * @param input user input command
         *
         * @return response to be sent back to the client
         */
         string doRetrieve(const string& input);

};

#endif
