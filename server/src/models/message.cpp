#include "message.h"

using namespace std;

Message::Message(string id, string text){
    _id = id;
    _text = text;
}

string Message::getMessageId(){
    return _id;
};

string Message::getMessageText(){
    return _text;
};