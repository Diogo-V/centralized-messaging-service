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

    if (uid == "00000") { /* verifies if user id is not 00000 */
        return "NOK";
    }
    else if(users->find(uid) == users->end()){ /* verifies if user isn't already registered */
        return "DUP";
    }
    else{ /* registers user */
        User user(uid, pass);
        users->insert(make_pair(uid, user));
        return "OK";
    }
}

