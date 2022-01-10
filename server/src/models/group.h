#ifndef PROJETO_RC_39_V2_GROUP_H
#define PROJETO_RC_39_V2_GROUP_H

#include "message.h"
#include "user.h"
#include <vector>


/**
 * @brief Represents a Group.
 */
class Group {

    private:

        /**
         * @brief group's id
         */
        string _id;

        /**
         * @brief group's name
         */
        string _name;

        /**
         * @brief hashmap of users subscribed to this group
         */
        unordered_map<string, User*> _users;

        /**
         * @brief Group's message
         */
        vector<Message> _messages;

    public:

        /**
         * @brief Group Constructor
         *
         * @param id group's identifier
         * @param name group's name
         */
        explicit Group(const string& id, const string& name);

        /**
        * @brief Get group's name
        *
        * @return group's name
        */
        string getName();

        /**
         * @brief Get group's id
         *
         * @return group's id
         */
        string getGroupId();

        /**
        * @brief Get group's users
        *
        * @return users as an unordered map
        */
        unordered_map<string, User*> getUsers();

        /**
        * @brief Get group's message identifier counter
        *
        * @return message identifier counter
        */
        uint32_t getMid();

        /**
        * @brief add user to this group
        * @param user's pointer
        */
        void subscribeUser(User *user);

        /**
         * @brief removes user from this group

         * @param user_id
         */
        void unsubscribeUser(const string& user_id);

        /**
         * @brief post new message
         * @param m message to be posted
         */
        void postMessage(const Message& m);

        /**
        * Retrieve up to 20 messages, starting from the message with identifier mid
        * @param mid message's identifier
        * @return vector with message
        */
        vector<Message> retrieveMessages(const uint32_t& mid);

};


#endif //PROJETO_RC_39_V2_GROUP_H
