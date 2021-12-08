#ifndef PROJETO_RC_39_V2_USER_H
#define PROJETO_RC_39_V2_USER_H

#include "group.h"
#include <cstring>
#include <unordered_map>


/**
 * @brief Represents a User.
 */
class User {

    private:

        /**
         * @brief User's id
         */
        int _id;

        /**
         * @brief User's password
         */
        string _password;

        /**
         * @brief User's group
         */
        unordered_map<int, Group*> _groups;

        /*
         * @brief holds the current active group
         */
        int _gid;

    public:

        /**
         * @brief User constructor
         *
         * @param id
         * @param password
         */
        User(int id, string password);

        /**
         * @brief Gets user id
         *
         * @return user id as integer
         */
        int getUserId();

        /*
         * @brief Gets user's active group id
         */
        int getUserGid();

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
        unordered_map<int, Group*> getUserGroups();

        /**
         * @brief Changes active group id
         *
         * @param id new group's id
         */
        void setUserGid(int id);

        /**
        * @brief add group to user's group list
        *
        * @param gId group's Id
        * @param group group's pointer
        */
        void addGroup(int gId, Group* group);

        /**
        * @brief remove group from user's group list
        *
        * @param gId group's Id
        */
        void removeGroup(int gId);

}


#endif //PROJETO_RC_39_V2_USER_H
