#include "api.h"

#include <utility>


/**
 * @brief Registers user
 *
 * @param unordered map of users
 * @param user user id (must have only 5 digits)
 * @param user password
 *
 * @return status message
 */
string register_user(unordered_map<string, User>* users, string& uid, string& pass){

    /* Verifies if user limit has been reached or that the user id is zero */
    if (users->size() == USER_LIMIT || uid == "00000") {
        return "NOK";

    /* Verifies if user isn't already registered */
    } else if (users->find(uid) == users->end()) {
        return "DUP";

    /* Since everything went alright, registers user */
    } else {
        User user(uid, pass);
        users->insert(make_pair(uid, user));
        return "OK";
    }

}


/**
 * @brief Unregisters user
 *
 * @param unordered map of users
 * @param user id
 * @param user password
 *
 * @return status message
 */
string unregister_user(unordered_map<string, User>* users, string uid, string pass){

    if(users->find(uid)!=users->end()){ /* verifies if user is already registered */
        return "NOK";
    }
    else if(users->at(uid).getUserPassword()!=pass){ /* verifies if user password is correct */
        return "NOK";
    }
    else{ /* unregisters user */
        users->erase(uid);
        return "OK";
    }

}

/**
 * @brief user logs in
 *
 * @param users structure that holds all users in the server
 * @param uid user id
 * @param pass user password
 * @return status message
 */
string login_user(unordered_map<string, User>* users, string uid, string pass) {

    auto it = users->find(uid);

    /* verifies if user is registered*/
    if(it == users->end()){
            return "NOK";
    }

    /* verifies if user is already logged in*/
    else if (it->second.getUserStatus()){
        return "NOK";
    }

    /*verifies is password is equal*/
    else if (it->second.getUserPassword() != pass){
        return "NOK";
    }

    /* user logged in*/
    else{
        /* set user status to true*/
        it->second.setUserStatus();
        return "OK";
    }

}

// TODO: fazer logged out

/**
 * @brief Lists groups
 *
 * @param unordered map of groups
 *
 * @return list of group IDs and names
 */
string list_groups(unordered_map<string, Group>* groups){
    unordered_map<string, Group>:: iterator itr;
    string group, list;

    for (itr = groups->begin(); itr != groups->end(); itr++){
        group = itr->first + " \\ " + itr->second.getGroupId() + "\n";
        list.append(group);
    }

    return list;
};