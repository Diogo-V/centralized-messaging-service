#ifndef PROJETO_RC_39_V2_MESSAGE_H
#define PROJETO_RC_39_V2_MESSAGE_H

#include <string>

using namespace std;

/**
 * @brief Represents a Message
 */
class Message{
    private:
        /**
         * @brief message's id
         */
        string _id;

        /**
         * brief message's text
         */
        string _text;

        //optional: file;

    public:
        //TODO: comentar
        explicit Message(string id, string text);

        string getMessageId();

        string getMessageText();

};

#endif
