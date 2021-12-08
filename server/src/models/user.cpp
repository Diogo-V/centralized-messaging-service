#include "user.h"


using namespace std;


/**
 * @brief User constructor
 *
 * @param id
 * @param password
 */
User::User(int id, string password){
    _id = id;
    _password = password;
    _gid = 0;
}


/**
 * @brief Gets user id
 *
 * @return user id as integer
 */
int User::getUserId(){
    return _id;
}


/**
 * @brief gets users active group id
 *
 * @return group id
 */
int User::getUserGid() {
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
unordered_map<int, Group*> User::getUserGroups(){
    return _groups;
}


/**
 * @brief Changes user's active group
 * @param id group's id
 */
void User::setUserGid(int id) {
    _gid = id;
}


/**
 * @brief add group to user's group list
 *
 * @param gId group's Id
 * @param group group's pointer
 */
void User::addGroup(int gId, Group* group){
    _groups.insert({gId, group});
}


/**
 * @brief remove group from user's group list
 *
 * @param gId group's Id
 */
void User::removeGroup(int gId){
    _groups.erase(gId);
}
