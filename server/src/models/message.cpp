#include "message.h"

using namespace std;

Message::Message(string id, string uid,  string text){
    _id = id;
    _text = text;
    _uid = uid;
}

string Message::getMessageId(){
    return _id;
};

string Message::getMessageText(){
    return _text;
};

string Message::getMessageUid(){
    return _uid;
}