#include "message.h"


using namespace std;


/**
 * Message constructor.
 *
 * @param id message's id
 * @param uid message's author
 * @param text message's text
 */
Message::Message(const string& id, const string& uid,  const string& text, string& filename, string& filesize) {
    _id = id;
    _text = text;
    _uid = uid;
    _filename = filename;
    _filesize = filesize;
}


/**
 * @brief Returns message's id.
 *
 * @return string with 4 chars containing the id
 */
string& Message::getMessageId() {
    return this->_id;
}


/**
 * @brief Gets message's contents.
 *
 * @return message's text
 */
string& Message::getMessageText() {
    return this->_text;
}


/**
 * @brief Gets message's author identifier.
 *
 * @return user's id
 */
string& Message::getMessageUid() {
    return this->_uid;
}

/**
 * @brief Gets message's filename.
 *
 * @return user's id
 */
string& Message::getMessageFileName(){
    return this->_filename;
}

/**
* @brief Gets message's file's size.
*
* @return user's id
*/
string& Message::getMessageFileSize(){
    return this->_filesize;
}
