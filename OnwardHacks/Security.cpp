#include "Security.h"
#include "pch.h"
#include <string>

int count_char(std::string s, char c) {
    int count = 0;

    for (int i = 0; i < s.size(); i++)
        if (s[i] == c) count++;

    return count;
}



bool verify(std::string key) {
    if (count_char(key, 'F') == 4 &&
        count_char(key, 'C') == 2 &&
        count_char(key, '#') == 1 && 
        count_char(key, 'G') == 1 && 
        count_char(key, '`') == 1 &&
        count_char(key, '(') == 2 &&
        count_char(key, '%') == 2 &&
        count_char(key, 'U') == 2) {
        int count = 0;
        for (int i = 0; i < key.size(); i++) {
            if (isdigit(key[i])) {
                count = count + key[i] - '0';
            }
        }
        if (count == 63) {
            return true;
        }
    }
    return false;
}