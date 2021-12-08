#ifndef PROJETO_RC_39_V2_API_H
#define PROJETO_RC_39_V2_API_H

#include "models/user.h"

using namespace std;

string register_user(unordered_map<int, User>* users, string uid, string pass);

#endif //PROJETO_RC_39_V2_API_H
