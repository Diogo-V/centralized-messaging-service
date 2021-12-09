#ifndef PROJETO_RC_39_V2_API_H
#define PROJETO_RC_39_V2_API_H

#include "models/user.h"
#define USER_LIMIT 99999

using namespace std;

string register_user(unordered_map<int, User>* users, string uid, string pass);
string unregister_user(unordered_map<string, User>* users, string uid, string pass);
string login_user(unordered_map<string, User>* users, string uid, string pass);
string list_groups();

#endif //PROJETO_RC_39_V2_API_H
