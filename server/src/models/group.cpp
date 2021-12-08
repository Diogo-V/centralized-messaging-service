#include "group.h"

using namespace std;


/**
 * @brief Group constructor
 *
 * @param name
 * @param user
 */
Group::Group(string id, string name){
    _id = id;
    _name = name;
    _mid = 1;
    // message
}


/**
 * @brief get group's name
 *
 * @return group's name
 */
string Group::getName(){
    return _name;
}


/**
 * @brief Gets group's id
 *
 * @return group's id
 */
string Group::getGroupId() {
    return _id;
}


/**
 * @brief Get group's users
 *
 * @return users as an unordered map
 */
unordered_map<string, User*> Group::getUsers(){
    return _users;
}


/**
 * @brief Get group's message identifier counter
 *
 * @return message identifier counter
 */
int Group::getMid(){
    return _mid;
}


/**
 * @brief increments group's message identifier counter
 */
void Group::incrementMid(){
    _mid ++;
}


/**
 * @brief add user to this group
 * @param user
 */
void Group::subscribeUser(User* user){
    _users.insert({user->getUserId(), user});
}


/**
 * @brief removes user from this group
 * @param userId
 */
void Group::unsubscribeUser(string userId){
    _users.erase(userId);
}


/**
 * @brief post new message to this group
 *
 * @param m new message
 */
void Group::postMessage(Message m){
        _messages.push_back(m);
}


list<Message> Group::retrieveMessages(int mid) {
    return {};
}


