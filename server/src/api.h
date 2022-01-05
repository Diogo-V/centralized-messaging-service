#ifndef PROJETO_RC_39_V2_API_H
#define PROJETO_RC_39_V2_API_H

#include <cstring>
#include "models/user.h"
#include "models/group.h"

#define USER_LIMIT 99999

using namespace std;

string register_user(unordered_map<string, User>* users, string& uid, string& pass);
string unregister_user(unordered_map<string, User>* users, string& uid, string& pass);
string login_user(unordered_map<string, User>* users, string& uid, string& pass);
string logout_user(unordered_map<string, User>* users, string& uid, string& pass);
string list_groups(unordered_map<string, Group>* groups);
string subscribe (unordered_map<string, Group>* groups, unordered_map<string, User>* users, string uid, string gid, string gname, int* p_gid_counter);


#endif
