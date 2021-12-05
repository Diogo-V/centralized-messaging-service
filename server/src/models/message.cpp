#include "message.h"

using namespace std;

Message::Message(int id, string text){
    _id = id;
    _text = text;
}

int Message::getMessageId(){
    return _id;
};

string Message::getMessageText(){
    return _text;
};