#ifndef PROJETO_RC_39_V2_API_H
#define PROJETO_RC_39_V2_API_H

#include "models/user.h"
#include <unordered_map>
#include <cstring>

using namespace std;

string register(unordered_map<int, User>* users, int uid, string pass);

#endif //PROJETO_RC_39_V2_API_H
