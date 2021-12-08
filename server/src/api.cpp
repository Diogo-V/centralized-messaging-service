#include "api.h"

/**
 * @brief Registers user
 *
 * @param unordered map of users
 * @param user id
 * @param user password
 *
 * @return status message
 */
string register(unordered_map<string, User>* users, string uid, string pass){
    User user;

    if (uid==00000) { /* verifies if user id is not 00000 */
        return "NOK";
    }
    else if(umap.find(uid) == umap.end()){ /* verifies if user isn't already registered */
        return "DUP";
    }
    else{ /* registers user */
        user.id=uid;
        user.password=pass;
        users.insert(make_pair(uid, user));
        return "OK";
    }
}

