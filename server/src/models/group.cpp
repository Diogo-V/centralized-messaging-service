#include "group.h"

using namespace std;


/**
 * @brief Group constructor.
 *
 * @param name
 * @param user
 */
Group::Group(string& id, string& name) {
    _id = id;
    _name = name;
}


/**
 * @brief get group's name.
 *
 * @return group's name
 */
string Group::getName() {
    return _name;
}


/**
 * @brief Gets group's id.
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
unordered_map<string, User*> Group::getUsers() {
    return _users;
}


/**
 * @brief Get group's message identifier counter.
 *
 * @return message identifier counter
 */
uint32_t Group::getMid() {
    return this->_messages.size();
}


/**
 * @brief add user to this group.
 *
 * @param user user that wants to subscribe to this group
 */
void Group::subscribeUser(User* user) {
    _users.insert({user->getUserId(), user});
}


/**
 * @brief removes user from this group.
 *
 * @param user_id
 */
void Group::unsubscribeUser(string& user_id) {
    _users.erase(user_id);
}


/**
 * @brief post new message to this group
 *
 * @param message new message
 */
void Group::postMessage(Message& message) {
    _messages.push_back(message);
}


/**
 * Retrieve up to 20 messages, starting from the message with identifier mid.
 *
 * @param mid message's identifier
 *
 * @return vector with message
 */
vector<Message> Group::retrieveMessages(uint32_t mid) {

    /* Number of messages from the selected message to the last one*/
    uint32_t interval = this->getMid() - mid + 1;

    /* If the number is less than 20, display all message. If not, we select the last message to
     * be retrieved as mid plus 20 */
    uint32_t end = interval < 20 ? 0 : (this->getMid() - (mid + 19));
    vector<Message> result(_messages.begin() + mid - 1, _messages.end() - end);

    return result;
}
