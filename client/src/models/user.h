#ifndef PROJETO_RC_39_V2_USER_H
#define PROJETO_RC_39_V2_USER_H

#include <string>


using namespace std;


/**
 * Represents an user in our client side application.
 */
class User {

    private:

        /**
         * @brief Holds user's identifier
         */
        string _uid;

        /**
         * @brief Holds user's password
         */
        string _pass;

        /**
         * @brief Holds user's login state
         */
        bool _is_logged;

        /**
         * @brief Holds user's currently selected group's id
         */
        string _selected_group_id;

    public:

        /**
         * @brief Gets user's identifier.
         *
         * @return user's identifier
         */
        string getUserID();

        /**
         * @brief Gets user's password.
         *
         * @return user's password
         */
        string getUserPassword();

        /**
         * @brief Gets user's logged status.
         *
         * @return user's status
         */
        bool getLoggedStatus() const;

        /**
         * @brief Gets user's currently selected group.
         *
         * @return user's selected group's id
         */
        string getSelectedGroupID();

        /**
         * @brief Sets user's identifier.
         *
         * @param uid new user id
         */
        void setUserID(const string& uid);

        /**
         * @brief Sets user's password.
         *
         * @param password new password
         */
        void setUserPassword(const string& password);

        /**
         * @brief Sets user's identifier.
         *
         * @param state new user logged state
         */
        void setLoggedStatus(bool state);

        /**
         * @brief Sets user's identifier.
         *
         * @param group_id new selected group's id
         */
        void setUserSelectedGroupID(const string& group_id);

        /**
         * @brief Resets user info.
         */
        void resetUser();

};

#endif
