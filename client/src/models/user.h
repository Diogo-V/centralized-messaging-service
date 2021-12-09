#ifndef PROJETO_RC_39_V2_USER_H
#define PROJETO_RC_39_V2_USER_H

#include <string>

using namespace std;

class User {

    private:

        /**
         * @brief User's id.
         */
        string _id;

        /**
         * @brief Tells us if the user is currently logged in in the server.
         */
        bool _is_logged_in;

        /**
         * @brief Holds the id of the currently selected group.
         */
        string _selected_group;


    public:

        explicit User();

};

#endif //PROJETO_RC_39_V2_USER_H
