#ifndef PROJETO_RC_39_V2_USER_H
#define PROJETO_RC_39_V2_USER_H

#include <unordered_map>
#include <string>
#include <list>


using namespace std;


/**
 * @brief Represents a User.
 */
class User {

    private:

        /**
         * @brief User's id
         */
        string _id;

        /**
         * @brief User's password
         */
        string _password;

        /**
         * @brief User's group
         */
         list<string> _group_ids;

        /**
         * @brief holds the current status of the users (logged in or not)
         */
        bool _status;

    public:

        /**
         * @brief User constructor
         *
         * @param id
         * @param password
         */
        explicit User(string& id, string& password);

        /**
         * @brief Gets user id
         *
         * @return user id as integer
         */
        string getUserId();

        /**
        * @brief Gets user password
        *
        * @return user password as string
        */
        string getUserPassword();

        /**
        *@brief Gets user's group
        *
        * @return group as unordered map
        */
        list<string> getUserGroups();

        /**
         * @brief Get's user status (logged in or not)
         * @return status as bool
         */
        bool getUserStatus();


        /**
         * @brief Changes user's status  (logged in or not)
         */
        void toggleStatus();

        /**
        * @brief add group to user's group list
        *
        * @param group group's pointer
        */
        void addGroup(string gId);

        /**
        * @brief remove group from user's group list
        *
        * @param gId group's Id
        */
        void removeGroup(string gId);

};


#endif //PROJETO_RC_39_V2_USER_H
