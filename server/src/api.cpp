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
string register_user(unordered_map<string, User>* users, string& uid, string& pass) {

    /* Verifies if user limit has been reached or that the user id is zero */
    if (users->size() == USER_LIMIT || uid == "00000") {
        return "NOK";

    /* Verifies if user isn't already registered */
    } else if (users->count(uid) > 0) {
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
string unregister_user(unordered_map<string, User>* users, string& uid, string& pass){

    /* verifies if user is already registered or if the password is not correct */
    if(users->count(uid) > 0 || users->at(uid).getUserPassword() != pass) {
        return "NOK";
    } else {
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
string login_user(unordered_map<string, User>* users, string& uid, string& pass) {

    auto it = users->find(uid);

    /* Verifies if user is registered, if he is not logged in and if password is correct*/
    if (it == users->end() || it->second.getUserStatus() || it->second.getUserPassword() != pass) {
            return "NOK";
    } else {
        it->second.toggleStatus();  /* Sets user status to true */
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
string list_groups(unordered_map<string, Group>* groups) {
    string group, list;

    for (auto & itr : *groups) {
        group = itr.first + " \\ " + itr.second.getName() + "\n";
        list.append(group);
    }

    return list;
}
