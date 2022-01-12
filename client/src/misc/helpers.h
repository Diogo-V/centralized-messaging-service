#ifndef PROJETO_RC_39_V2_HELPERS_H
#define PROJETO_RC_39_V2_HELPERS_H

#include <string>
#include <vector>
#include <sstream>

/* If condition is false displays msg and returns with false bool value */
#define validate_(cond, msg) do { if (! (cond)) { cerr << (msg) << endl; return; } } while(0);

/* If condition is false displays msg and interrupts execution */
#define assert_(cond, msg) if(! (cond)) { fprintf(stderr, msg); exit(EXIT_FAILURE); }


using namespace std;


/*
 * Transforms a string with spaces in a vector with substring tokenized by the spaces.
 *
 * @param str string which is going to be separated
 * @param out vector with substrings
 */
void split(string const &str, vector<string> &out);

/**
 * Verifies if input string translates to a number.
 *
 * @param line string to be validated
 * @return boolean value
 */
bool isNumber(const string& line);

/**
 * Verifies if input string translates to alphanumeric characters.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumeric(const string& line);

/**
 * Verifies if input string translates to alphanumeric characters plus '-' and '_'.
 * @param line string to be validated
 * @return boolean value
 */
bool isAlphaNumericPlus(const string& line);


#endif
