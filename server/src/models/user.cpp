#include "user.h"


using namespace std;


/**
 * @brief User constructor
 *
 * @param id
 * @param password
 */
User::User(string id, string password){
    _id = id;
    _password = password;
}


/**
 * @brief Gets user id
 *
 * @return user id as integer
 */
string User::getUserId(){
    return _id;
}


/**
 * @brief gets users active group id
 *
 * @return group id
 */
string User::getUserGid() {
    return _gid;
}


/**
 * @brief Gets user password
 *
 * @return user password as string
 */
string User::getUserPassword(){
    return _password;
}


/**
 *@brief Gets user's group
 *
 * @return group as unordered map
 */
list<string> User::getUserGroups(){
    return _group_ids;
}

//TODO:comentar isto!
bool User::getUserStatus() {
    return _status;
}

/**
 * @brief Changes user's active group
 * @param id group's id
 */
void User::setUserGid(string id) {
    _gid = id;
}


/**
 * @brief add group to user's group list
 *
 * @param gId group's Id
 */
void User::addGroup(string gId){
    _group_ids.push_back(gId);
}


/**
 * @brief remove group from user's group list
 *
 * @param gId group's Id
 */
void User::removeGroup(string gId){
    _group_ids.remove(gId);
}
