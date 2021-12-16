#ifndef PROJETO_RC_39_V2_GROUP_H
#define PROJETO_RC_39_V2_GROUP_H

#include "message.h"
#include "user.h"
#include <list>


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
         * @brief message identifier counter
         */
        uint32_t _mid;

        /**
         * @brief hashmap of users subscribed to this group
         */
        unordered_map<string, User*> _users;

        /**
         * @brief Group's message
         */
        list<Message> _messages;

    public:

        /**
         * @brief Group Constructor
         *
         * @param name
         * @param user
         */
        explicit Group(string id, string name);

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
        * @brief increments group's message identifier counter
        */
        void incrementMid();

        /**
        * @brief add user to this group
        * @param user's pointer
        */
        void subscribeUser(User* user);

        /**
         * @brief removes user from this group

         * @param userId
         */
        void unsubscribeUser(string userId);

        /**
         * @brief post new message
         * @param m message to be posted
         */
        void postMessage(Message m);

        /**
         * @brief retrieves messages with mid values higher than mid
         */
        list<Message> retrieveMessages(uint32_t mid);

};


#endif //PROJETO_RC_39_V2_GROUP_H
