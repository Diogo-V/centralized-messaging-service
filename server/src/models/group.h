#ifndef PROJETO_RC_39_V2_GROUP_H
#define PROJETO_RC_39_V2_GROUP_H
#include "user.h"
#include "message.h"
#include <list>
#include <unordered_map>

using namespace std;

/**
 * @brief Represents a Group.
 */
class Group{
    private:
        /**
         * @brief group's id
         */
        int _id;

        /**
         * @brief group's name
         */
        string _name;

        /**
         * @brief message identifier counter
         */
        int _mid;

        /**
         * @brief hashmap of users subscribed to this group
         */
        unordered_map<int, User*> _users;

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
        Group(int id, string name, User* user);

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
        int getGroupId();

        /**
        * @brief Get group's users
        *
        * @return users as an unordered map
        */
        unordered_map<int, User*> getUsers();

        /**
        * @brief Get group's message identifier counter
        *
        * @return message identifier counter
        */
        int getMid();

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
        void unsubscribeUser(int userId);

        /**
         * @brief post new message
         * @param m message to be posted
         */
        void postMessage(Message m);

        /**
         * @brief retrieves messages wtih mid values higher than mid
         */
        list<Message> retrieveMessages(int mid);


};

#endif //PROJETO_RC_39_V2_GROUP_H