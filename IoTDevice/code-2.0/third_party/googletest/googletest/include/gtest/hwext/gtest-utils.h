// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#ifndef GTEST_INCLUDE_GTEST_UTILS_H_
#define GTEST_INCLUDE_GTEST_UTILS_H_

#include <string>
#include <vector>
#include <algorithm>

namespace testing {

    using ::std::string;
    using ::std::vector;

    bool compareStringsByIgnoreCase(const char* one, const char* two);
    bool IsElementInVector(vector<int> vec, int element);
    vector<string> SplitString(const string& str, const string& delim);

} // namespace testing

#endif  // GTEST_INCLUDE_GTEST_UTILS_H_
