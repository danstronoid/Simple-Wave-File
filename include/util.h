#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

/*
    Simple Wave File
    Author: Daniel Schwartz

    -- util --

    helper functions for working with the non null-terminated
    4-letter strings found in headers
*/

#include <iostream>
#include <string>

bool wordCompare(char *charArray, string word) {
    for (int i = 0; i < 4; ++i) {
        if (charArray[i] != word[i]) {
            return false;
        }
    }
    return true;
}

void printWord(char *text) {
    for (int i = 0; i < 4; ++i) {
        cout << text[i];
    }
    cout << endl;
}

#endif // UTILITY_H_INCLUDED
