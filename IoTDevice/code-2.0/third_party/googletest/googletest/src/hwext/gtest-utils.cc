// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#include <string.h>
#include <string>
#include "gtest/hwext/gtest-utils.h"

namespace testing {

    /*
     * Function:     compareStringsByIgnoreCase
     * Description:  Ignore case to compare two strings
     * Input:        one: first string
     *               two: second string
     * Output:       N/A
     * Return:       true(equal),flase(not equal)
     * Others:       N/A
     */
    bool compareStringsByIgnoreCase(const char* one, const char* two) {
        if (one == NULL && two == NULL) {
            return true;
        }

        if (one == NULL || two == NULL) {
            return false;
        }

        if (strcmp(one, two) == 0) {
            return true;
        }

        const int len_one = strlen(one);
        const int len_two = strlen(two);

        if (len_one != len_two) {
            return false;
        }

        if (len_one == 0 && len_two == 0) {
            return true;
        }

        for (int i = 0; i < len_one; i++) {
            if (tolower(one[i]) != tolower(two[i])) {
                return false;
            }
        }

        return true;
    }

    bool IsElementInVector(vector<int> vec, int element){
        vector<int>::iterator it = find(vec.begin(), vec.end(), element);
        if (it != vec.end()) {
            return true;
        }
        return false;
    }

    vector<string> SplitString(const string& str, const string& delim) {
        vector<string> result;
        if (str != "") {
            int len = str.length();
            char *src = new char[len + 1];
            memset(src, 0, len + 1);
            strcpy(src, str.c_str());
            src[len] = '\0';

            char *tokenptr = strtok(src, delim.c_str());
            while (tokenptr != NULL)
            {
                string tk = tokenptr;
                result.emplace_back(tk);
                tokenptr = strtok(NULL, delim.c_str());
            }
            delete[] src;
        }

        return result;
    }

} // namespace testing
