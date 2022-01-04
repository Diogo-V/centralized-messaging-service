#include "api.h"

#include <utility>
#include <iostream>

using namespace std;
/**
 * @brief Registers user
 *
 * @param users unordered map of users
 * @param uid user id (must have only 5 digits)
 * @param pass user password
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
 * @param users unordered map of users
 * @param uid user id
 * @param pass user password
 *
 * @return status message
 */
string unregister_user(unordered_map<string, User>* users, string& uid, string& pass){
    /* verifies if there are users registered. This is for safety measure*/
    if(users->empty()){
        return "NOK";
    }
    /* verifies if user isn't already registered or if the password is not correct */
    else if(users->count(uid) == 0 || users->at(uid).getUserPassword() != pass) {
        cout << users->at(uid).getUserPassword() << endl;
        return "NOK";
    } else {
        users->erase(uid);
        //TODO: @Sofia-Morgado -> desinscrever de todos os grupos
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
    /* verifies if there are users registered. This is for safety measure*/
    if(users->empty()) {
        return "NOK";
    }

    /* Verifies if user is registered, if he is not logged in and if password is correct*/
    else if (users->count(uid) == 0 || users->at(uid).getUserStatus() || users->at(uid).getUserPassword() != pass) {
            return "NOK";
    } else {
        users->at(uid).toggleStatus();  /* Sets user status to true */
        return "OK";
    }

}

/**
 * @brief user logs out
 *
 * @param users structure that holds all users in the server
 * @param uid user id
 * @param pass user password
 * @return status message
 */
string logout_user(unordered_map<string, User>* users, string& uid, string& pass) {
    /* verifies if there are users registered. This is for safety measure*/
    if(users->empty()) {
        return "NOK";
    }

    /* Verifies if user is registered, if he is logged in and if password is correct*/
    else if (users->count(uid) == 0 || !users->at(uid).getUserStatus() || users->at(uid).getUserPassword() != pass) {
        return "NOK";
    } else {
        users->at(uid).toggleStatus();  /* Sets user status to false */
        return "OK";
    }

}


/**
 * @brief Lists groups
 *
 * @param groups unordered map of groups
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
