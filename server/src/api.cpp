#include "api.h"

#include <utility>
#include <iostream>
#include <list>
#include <cstdio>

using namespace std;


/**
 * @brief Registers user
 *
 * @param users unordered map of users
 * @param uid user id (must have only 5 digits)
 * @param pass user password
 *
 * @return status message
 */
string register_user(unordered_map<string, User>* users, string& uid, string& pass) {

    /* Verifies if user limit has been reached or that the user id is zero */
    if (users->size() == USER_LIMIT || uid == "00000") {
        return "NOK";

    /* Verifies if user isn't already registered */
    } else if (users->count(uid) > 0) {
        return "DUP";

    /* Since everything went alright, registers user */
    } else {
        User user(uid, pass);
        users->insert(make_pair(uid, user));
        return "OK";
    }

}


/**
 * @brief Unregisters user
 *
 * @param users unordered map of users
 * @param groups unordered map of groups
 * @param uid user id
 * @param pass user password
 *
 * @return status message
 */
string unregister_user(unordered_map<string, User>* users, unordered_map<string, Group>* groups, string& uid, string& pass) {

    /* Verifies if there are users registered, if the user is already registered and if password is correct */
    if (users->empty() || users->count(uid) == 0 || users->at(uid).getUserPassword() != pass) {
        return "NOK";
    } else {
        // Removes user from list of users and from all subscribed groups
        users->erase(uid);
        for (auto& ele: *groups) ele.second.unsubscribeUser(uid);
        return "OK";
    }

}


/**
 * @brief user logs in
 *
 * @param users structure that holds all users in the server
 * @param uid user id
 * @param pass user password
 * @return status message
 */
string login_user(unordered_map<string, User>* users, string& uid, string& pass) {

    /* verifies if there are users registered. This is for safety measure*/
    if(users->empty()) {
        return "NOK";
    }
    /* Verifies if user is registered, if he is not logged in and if password is correct*/
    else if (users->count(uid) == 0 || users->at(uid).getUserStatus() || users->at(uid).getUserPassword() != pass) {
            return "NOK";
    } else {
        users->at(uid).toggleStatus();  /* Sets user status to true */
        return "OK";
    }

}


/**
 * @brief user logs out
 *
 * @param users structure that holds all users in the server
 * @param uid user id
 * @param pass user password
 * @return status message
 */
string logout_user(unordered_map<string, User>* users, string& uid, string& pass) {
    /* verifies if there are users registered. This is for safety measure*/
    if(users->empty()) {
        return "NOK";
    }

    /* Verifies if user is registered, if he is logged in and if password is correct*/
    else if (users->count(uid) == 0 || !users->at(uid).getUserStatus() || users->at(uid).getUserPassword() != pass) {
        return "NOK";
    } else {
        users->at(uid).toggleStatus();  /* Sets user status to false */
        return "OK";
    }

}


/**
 * @brief Lists groups
 *
 * @param groups unordered map of groups
 *
 * @return list of group IDs and names
 */
string list_groups(unordered_map<string, Group>* groups) {
    string group, list, mid;

    /* verifies if there are groups created. This is for safety measure*/
    if(groups->empty()) {
        return "";
    }

    for (auto & itr : *groups) {
        mid = itr.second.getMid() == 0 ? "0000" : to_string(itr.second.getMid()) ;
        group = itr.first + " " + itr.second.getName() + " " + mid + " ";
        list.append(group);
    }

    return list;
}

//TODO: @Sofia-Morgado -> caso o user já esteja subscrito, devia dar erro

/**
 * Subscribe user to group. If gid equal 0, also creates group.
 *
 * @param groups structure that holds all groups in the server
 * @param users structure that holds all users in the server
 * @param uid user's id
 * @param gid group's id
 * @param group_name group's name
 *
 * @return status message
 */
string subscribe(unordered_map<string, Group>* groups, unordered_map<string, User>* users, string& uid, string& gid, string& group_name) {

    char new_gid[3];

    //FIXME: @Sofia-Morgado -> visto que isto é um erro de não existirem grupos ou users, o erro é NOK ou E_USR?
    /* Verifies if there are users registered or if there are groups to subscribe. This is for safety measure s*/
    if(users->empty() || (groups->empty() && gid != "00")) {
        return "NOK";

    /* UID doesn't exist or isn't logged in*/
    } else if ((users->count(uid) == 0) || !(users->at(uid).getUserStatus())) {
        return "E_USR";

    /* Want to create a new group, but there are already 99 groups*/
    } else if (gid == "00" && groups->size() == 99) {
        return "E_FULL";

    /* Group doesn't exist*/
    } else if (gid != "00" && groups->count(gid) == 0) {
        return "E_GRP";

    /* Group name is incorrect */
    } else if (gid != "00" && groups->at(gid).getName() != group_name) {
        return "E_GNAME";

    /* Group already exists and user has already subscribed*/
    } else if (gid == "00" && groups->count(gid) != 0 && groups->at(gid).getUsers().count(uid) != 0) {
        return "OK";

    /* Everything is fine */
    } else {

        if (gid == "00") {

            /* Formats group id to hold 2 chars */
            sprintf(new_gid, "%02lu", groups->size() + 1);

            /* Create new group*/
            Group group(new_gid, group_name);
            groups->insert(make_pair(new_gid, group));

        } else {
            strcpy(new_gid, gid.c_str());
        }

        /* Subscribes user to new group. Add user to group subscribers and the group to user's group */
        groups->at(new_gid).subscribeUser(&users->at(uid));
        users->at(uid).addGroup(new_gid);

        return "OK";
    }

}


/**
 * Unsubscribe user from the group.
 *
 * @param groups structure that holds all groups in the server
 * @param users structure that holds all users in the server
 * @param uid user's id
 * @param gid group's id
 * @return status message
 */
string unsubscribe(unordered_map<string, Group>* groups, unordered_map<string, User>* users, string uid, string gid) {
    /*Verifies if the user exists */
    if(!users->empty() && users->count(uid) == 0){
        return "E_USR";

    /*Verifies if the group exists */
    } else if (!groups->empty() && groups->count(gid) == 0){
        return "E_GRP";

    } else {
        /*Unsubscribe user */
        groups->at(gid).unsubscribeUser(uid);
        users->at(uid).removeGroup(gid);

        return "OK";
    }
}


/**
 * Sends a list of the groups that the user is subscribed
 * @param groups structure that holds all groups in the server
 * @param users structure that holds all users in the server
 * @param uid user's id
 * @return number of groups and list of groups
 */
string groups_subscribed(unordered_map<string, Group>* groups, unordered_map<string, User>* users, string uid){
    string out, group, mid;
    list<string> user_groups;

    /*Verifies if the user exists */
    if(!users->empty() && users->count(uid) == 0) {
        return "E_USR";

    } else {
        user_groups = users->at(uid).getUserGroups();

        //TODO: @Sofia-Morgado-> isto vai ter consequências no código
        for (auto & itr : user_groups ) {
            mid = groups->at(itr).getMid() == 0 ? "0000" : to_string(groups->at(itr).getMid()) ;
            group = groups->at(itr).getGroupId() + " " + groups->at(itr).getName() + " " + mid + " ";
            out.append(group);
        }

        return to_string(user_groups.size()) + " " + out + "\n";
    }

}


/**
 * Sends a list of the users subscribed to this group
 * @param groups structure that holds all groups in the server
 * @param users structure that holds all users in the server
 * @param gid group's is
 * @return status message and list of users (if applicable)
 */
string users_subscribed(unordered_map<string, Group>* groups, unordered_map<string, User>* users, string gid){
    string out, user;
    unordered_map<string, User*> users_subscribed;

    /*Verifies if the group exists */
    if(!groups->empty() && groups->count(gid) == 0) {
        cout << groups->count(gid) << endl;
        return "NOK";

    } else {
        /* Gets all users subscribed to the group*/
        users_subscribed = groups->at(gid).getUsers();

        /* For each user, gets its id */
        for (auto & itr : users_subscribed ) {
            user = itr.first + "\n";
            out.append(user);
        }

        return "OK " + groups->at(gid).getName() + "\n" + out;
    }

}


/**
 * Post a new message and optionally also a file in the selected group
 * @param groups structure that holds all the groups in the server
 * @param users structure that holds all the users in the server
 * @param uid user's id
 * @param gid group's id
 * @param tsize text size
 * @param text text
 * @return status message
 */
 //TODO: @Sofia-Morgado-> tratar do file transfer
string post_message(unordered_map<string, Group>* groups, unordered_map<string, User>* users, string uid, string gid, string tsize, string text, string filename, string filesize) {

    char mid[5];

    /*Verifies if the user exists */
    if(!users->empty() && users->count(uid) == 0) {
        return "NOK";

    /*Verifies if the group exists */
    } else if (!groups->empty() && groups->count(gid) == 0) {
        return "NOK";

    /*Verifies if it's possible to post a new message*/
    } else if (groups->at(gid).getMid() == MID_LIMIT) {
        return "NOK";
    }

    /* Formats message id to hold 4 chars */
    sprintf(mid, "%04u", groups->at(gid).getMid() + 1);

    /* Create a new message and post it on the group*/
    Message m(mid, uid, text, filename, filesize);
    groups->at(gid).postMessage(m);

    /* Returns message identifier*/
    return mid;
}


//TODO: files
string retrieve_message(unordered_map<string, Group>* groups, string gid, string mid){
    string out;
    vector<Message> messages;

    //TODO: @Sofia-Morgago penso que esta verificação não é necessária
    /*Verifies if the user exists */
    //if(!users->empty() && users->count(_uid) == 0){
    //    return "NOK";

    /*Verifies if the group exists */
    if (!groups->empty() && groups->count(gid) == 0) {
        return "NOK";
    }

    messages = groups->at(gid).retrieveMessages(stoi(mid));

    /* No messages available*/
    if (messages.empty()){
        return "EOF";
    }

    out = to_string(messages.size()) + "\n";

    for (auto itr: messages){
        out += itr.getMessageId() + " " + itr.getMessageUid() + " " + to_string(itr.getMessageText().size()) + " \"" +
                itr.getMessageText() + "\" \n";
    }

    /* Returns message identifier*/
    return "OK " + out;
}