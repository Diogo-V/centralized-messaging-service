#include "helpers.h"


/*
 * Transforms a string with spaces in a vector with substring tokenized by the spaces.
 *
 * @param str string which is going to be separated
 * @param out vector with substrings
 */
void split(string const &str, vector<string> &out) {
    stringstream ss(str); string s; const char delim = (const char)* " ";
    while (getline(ss, s, delim)) out.push_back(s);
}


/**
 * Verifies if input string translates to a number.
 *
 * @param line string to be validated
 * @return boolean value
 */
bool isNumber(const string& line) { char* p; strtod(line.c_str(), &p); return *p == 0; }


/**
 * Verifies if input string translates to alphanumeric characters.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumeric(const string& line){
    uint8_t i = 0, len = line.length();

    while (isalnum(line[i])) i++;

    return i == len;
}


/**
 * Verifies if input string translates to alphanumeric characters plus '-' and '_'.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumericPlus(const string& line){
    uint8_t i = 0, len = line.length();
    while (isalnum(line[i]) || (line[i] == '-') || (line[i] == '_')) i++;
    return i == len;
}


/*
 * Gets user input command by reading until first space.
 *
 * @param str user input command
 *
 * @return requested command
 */
string get_command(const string& str) {
    stringstream ss(str); string s; char delim = ' '; string cmd;
    getline(ss, cmd, delim); return cmd;
}
