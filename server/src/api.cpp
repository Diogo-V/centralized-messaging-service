#include "api.h"

#include <utility>

/**
 * @brief Registers user
 *
 * @param unordered map of users
 * @param user id
 * @param user password
 *
 * @return status message
 */
string register_user(unordered_map<string, User>* users, string uid, string pass){

    if(users->size()==USER_LIMIT){ /* verifies if user limit has been reached */
        return "NOK";
    }
    else if(uid=="00000"){ /* verifies if user id is not 0 */
        return "NOK";
    }
    else if(users->find(uid)==users->end()){ /* verifies if user isn't already registered */
        return "DUP";
    }
    else{ /* registers user */
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
 * @return
 */
string login_user(unordered_map<string, User>* users, string uid, string pass) {

    auto it = users->find(uid);

    /* verifies if user is registered*/
    if(it == users->end()){
            return "NOK";
    }

    /*verifies is password is equal*/
    else if (it->second.getUserPassword() != pass){
        return "NOK";
    }

    /* user logged in*/
    else{
        return "OK";
    }
}

string list_groups();