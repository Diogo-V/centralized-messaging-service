#ifndef PROJETO_RC_39_V2_API_H
#define PROJETO_RC_39_V2_API_H

#include <cstring>
#include "models/user.h"
#include "models/group.h"

#define USER_LIMIT 99999
#define MID_LIMIT 9999

using namespace std;

string register_user(unordered_map<string, User>* users, string& uid, string& pass);
string unregister_user(unordered_map<string, User>* users, unordered_map<string, Group>* groups, string& uid,
                       string& pass);
string login_user(unordered_map<string, User>* users, string& uid, string& pass);
string logout_user(unordered_map<string, User>* users, string& uid, string& pass);
string list_groups(unordered_map<string, Group>* groups);
string subscribe (unordered_map<string, Group>* groups, unordered_map<string, User>* users, string& uid, string& gid,
                  string& group_name);
string unsubscribe(unordered_map<string, Group>* groups, unordered_map<string, User>* users, string uid, string gid);
string groups_subscribed (unordered_map<string, Group>* groups, unordered_map<string, User>* users, string uid);
string users_subscribed (unordered_map<string, Group>* groups, unordered_map<string, User>* users, string gid);
string post_message (unordered_map<string, Group>* groups, unordered_map<string, User>* users, string uid, string gid,
                     string txt_size, string text, string filename = "", string filesize  = "");
string retrieve_message (unordered_map<string, Group>* groups, string& gid, string& mid, vector<Message>& out);

#endif
