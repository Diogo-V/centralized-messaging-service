#include "user.h"


/**
 * @brief Gets user's identifier.
 * 
 * @return user's identifier
 */
string User::getUserID() {
    return this->_uid;
}


/**
 * @brief Gets user's password.
 * 
 * @return user's password
 */
string User::getUserPassword() {
    return this->_pass;
}


/**
 * @brief Gets user's logged status.
 * 
 * @return user's status
 */
bool User::getLoggedStatus() const {
    return this->_is_logged;
}


/**
 * @brief Gets user's currently selected group.
 * 
 * @return user's selected group's id
 */
string User::getSelectedGroupID() {
    return this->_selected_group_id;
}


/**
 * @brief Sets user's identifier.
 * 
 * @param uid new user id
 */
void User::setUserID(const string& uid) {
    this->_uid = uid;
}


/**
 * @brief Sets user's password.
 * 
 * @param password new password
 */
void User::setUserPassword(const string& password) {
    this->_pass = password;
}


/**
 * @brief Sets user's identifier.
 *
 * @param state new user logged state
 */
void User::setLoggedStatus(bool state) {
    this->_is_logged = state;
}


/**
 * @brief Sets user's identifier.
 * 
 * @param group_id new selected group's id
 */
void User::setUserSelectedGroupID(const string& group_id) {
    this->_selected_group_id = group_id;
}


/**
 * @brief Resets user info.
 */
void User::resetUser() {
    this->setUserSelectedGroupID("");
    this->setUserPassword("");
    this->setUserID("");
    this->setLoggedStatus(false);
}
