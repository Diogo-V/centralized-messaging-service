#ifndef PROJETO_RC_39_V2_USER_H
#define PROJETO_RC_39_V2_USER_H

#include <string>

using namespace std;

/**
 * @brief Represents a User.
 */
class User {
    private:
        /**
         * @brief User id
         */
        int _id;

        /**
         * @brief User password
         */
        string _password;

public:
    User(int id, string password){
        _id = id;
        _password = password;
    }

}


#endif //PROJETO_RC_39_V2_USER_H
