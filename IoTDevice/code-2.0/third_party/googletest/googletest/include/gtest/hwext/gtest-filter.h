// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#ifndef GTEST_INCLUDE_GTEST_GTEST_FILTER_H_
#define GTEST_INCLUDE_GTEST_GTEST_FILTER_H_

#include <string>
#include <map>
#include <vector>

namespace testing {
  namespace ext {

    using ::std::string;
    using ::std::map;
    using ::std::vector;

    class TestFilter {
    public:
        map<const char*, string*>& getAllFilterFlagsKv();
        void printHelp() const;
        bool postParsingArguments();
        bool accept(int flags) const;
        void reset();
        static TestFilter* instance();

    private:
        TestFilter() {};
        static const char* const kStrictFilter;
        int requiredFlags;
        // strcit filter requires the entirely same test flags and require no
        bool strictMode;
        bool flag_kvs_inited; // teels iff the filter kvs has been parsed
        bool ready; // teels iff the filter are ready to be used
        void postSetType(vector<string> vectemp);
        void postSetSize(vector<string> vectemp);
        void postSetRank(vector<string> vectemp);
        map<const char*, string*> filterFlagsKv;
        vector<int> vecTestLevel;
        vector<int> vecType;
        vector<int> vecSize;
        vector<int> vecRank;
    };

  } // namespace ext
} // namespace testing

#endif  // GTEST_INCLUDE_GTEST_GTEST_FILTER_H_

