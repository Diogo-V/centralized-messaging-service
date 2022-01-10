#ifndef PROJETO_RC_39_V2_MESSAGE_H
#define PROJETO_RC_39_V2_MESSAGE_H

#include <string>


using namespace std;


/**
 * @brief Represents a Message
 */
class Message {

    private:

        /**
         * @brief message's id
         */
        string _id;

        /**
         * @brief message's text
         */
        string _text;

        /**
         * @brief author identifier
         */
        string _uid;

        //optional: file;

    public:

        /**
         * Message constructor.
         *
         * @param id message's id
         * @param uid message's author
         * @param text message's text
         */
        explicit Message(const string& id, const string& uid,  const string& text);

        /**
         * @brief Returns message's id.
         *
         * @return string with 4 chars containing the id
         */
        string getMessageId();

        /**
         * @brief Gets message's contents.
         *
         * @return message's text
         */
        string getMessageText();

        /**
         * @brief Gets message's author identifier.
         *
         * @return user's id
         */
        string getMessageUid();

};

#endif
