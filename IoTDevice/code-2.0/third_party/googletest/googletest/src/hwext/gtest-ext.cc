/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: CPPTest框架中ext入口
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include "gtest/hwext/gtest-ext.h"


namespace testing {
  namespace ext {
    #define GTEST_EXT_DEBUG 0

    TestDefManager* TestDefManager::instance() 
    {
        static TestDefManager* instance_ = NULL;
        if (instance_ == NULL) {
            instance_ = new TestDefManager();
        }
        return instance_;
    }

    const TestDefManager* TestDefManager::cinstance() 
    {
        return instance();
    }

    // c'tor, all members immutable
    TestDefInfo::TestDefInfo(const char* tcn, const char* n, int fs, TestDefType tdf) :\
        test_case_name(tcn), name(n), flags(fs), def_type(tdf) {};

    bool TestDefManager::regist(const char* test_case_name, const char* test_name, int test_flags, TestDefType tdf) 
    {
        TestDefManager::testDefInfos.push_back(new TestDefInfo(test_case_name, test_name, test_flags, tdf));
        return true;
    }

    int TestDefManager::queryFlagsFor(const TestInfo* test, int def_value) const 
    {
        const TestDefInfo* def = findDefFor(test);
        return def == NULL ? def_value : def->flags;
    }

    const TestDefInfo* TestDefManager::findDefFor(const TestInfo* test) const 
    {
        // search by matching test definition information
        NamingMatchType case_name_mt = AEqualsB;
        NamingMatchType test_name_mt = AEqualsB;

        for (unsigned int i = 0; i < testDefInfos.size(); i++)
        {
            const TestDefInfo* info = testDefInfos.at(i);
            switch (info->def_type) {
                case Plain:
                case Fixtured:
                    case_name_mt = AEqualsB;
                    test_name_mt = AEqualsB;
                    break;
                case Typed:
                    case_name_mt = AStartsWithB;
                    test_name_mt = AEqualsB;
                    break;
                case PatternTyped:
                    case_name_mt = AContainsB;
                    test_name_mt = AEqualsB;
                    break;
                case Parameterized:
                    case_name_mt = AEndsWithB;
                    test_name_mt = AStartsWithB;
                    break;
                default:
                    break;
            }

            const bool matched = matchNaming(test->test_case_name(), info->test_case_name, case_name_mt) 
              && matchNaming(test->name(), info->name, test_name_mt);
            if (matched) {
                return info;
            }
        }

        #if GTEST_EXT_DEBUG
        printf("cannot find test definition for: %s.%s\n", test->test_case_name(), test->name());
        #endif
        return NULL;
    }

    bool TestDefManager::matchNaming(const char * const a, const char * const b, NamingMatchType mt) const 
    {
        const char sep = TestDefInfo::kNamingSepchar;
        const int len_a = strlen(a);
        const int len_b = strlen(b);
        int i;
        switch (mt) {
        case AEqualsB:
            // a=b
            return strcmp(a, b) == 0;
        case AStartsWithB:
            // a=b/xxx
            return strstr(a, b) == a && a[len_b] == sep;
        case AContainsB:
            // a=xxx/b/yyy
            for (i = 1; i < len_a - len_b; i++) {
                if (a[i - 1] == sep&&a[i + len_b] == sep&&strstr(a + i, b) == a + i) {
                    return true;
                }
            }
            return false;
        case AEndsWithB:
            // a=xxx/b
            return len_a > len_b&&a[len_a - len_b - 1] == sep&&strcmp(a + len_a - len_b, b) == 0;
        default:
            fprintf(stderr, "Illegal NamingMatchType: %d", mt);
            return false;
        }
    }

    int TestDefManager::getLevel(const std::string testcasename, const std::string testname) const 
    {
        NamingMatchType case_name_mt = AEqualsB;
        NamingMatchType test_name_mt = AEqualsB;
        int levelShift = 24;
        int levelList[5] = {1, 2, 4, 8, 16};
        enum indexEnum {I0 = 0, I1, I2, I3, I4};

        for (unsigned int i = 0; i < testDefInfos.size(); i++) {
            const TestDefInfo* info = testDefInfos.at(i);
            switch (info->def_type) {
                case Plain:
                case Fixtured:
                    case_name_mt = AEqualsB;
                    test_name_mt = AEqualsB;
                    break;
                case Typed:
                    case_name_mt = AStartsWithB;
                    test_name_mt = AEqualsB;
                    break;
                case PatternTyped:
                    case_name_mt = AContainsB;
                    test_name_mt = AEqualsB;
                    break;
                case Parameterized:
                    case_name_mt = AEndsWithB;
                    test_name_mt = AStartsWithB;
                    break;
                default:
                    break;
            }

            const bool matched = matchNaming(testcasename.c_str(), info->test_case_name, case_name_mt) 
              && matchNaming(testname.c_str(), info->name, test_name_mt);
            if (matched) {
                int level = (info->flags >> levelShift);
                if (level == levelList[I0]) return I0;
                if (level == levelList[I1]) return I1;
                if (level == levelList[I2]) return I2;
                if (level == levelList[I3]) return I3;
                if (level == levelList[I4]) return I4;
            }
        }
        return -1;
    }

    int* TestDefManager::getTestFlags (const std::string testcasename, const std::string testname) const 
    {
        NamingMatchType case_name_mt = AEqualsB;
        NamingMatchType test_name_mt = AEqualsB;
        static int flagList[3] = {-1, -1, -1};
        int posType = 8;
        int posSize = 4;
        int indexType = 0;
        int indexSize = 1;
        int indexRank = 2;

        for (unsigned int i = 0; i < testDefInfos.size(); i++) {
            const TestDefInfo* info = testDefInfos.at(i);
            switch (info->def_type) {
                case Plain:
                case Fixtured:
                    case_name_mt = AEqualsB;
                    test_name_mt = AEqualsB;
                    break;
                case Typed:
                    case_name_mt = AStartsWithB;
                    test_name_mt = AEqualsB;
                    break;
                case PatternTyped:
                    case_name_mt = AContainsB;
                    test_name_mt = AEqualsB;
                    break;
                case Parameterized:
                    case_name_mt = AEndsWithB;
                    test_name_mt = AStartsWithB;
                    break;
                default:
                    break;
            }
            
            const bool matched = matchNaming(testcasename.c_str(), info->test_case_name, case_name_mt) 
              && matchNaming(testname.c_str(), info->name, test_name_mt);
            if (matched) {
                // get the three flag
                int type = (info->flags >> posType);
                int size = (info->flags >> posSize);
                int rank =  info->flags;

                // add three flag value in the flag list
                flagList[indexType] = type;
                flagList[indexSize] = size;
                flagList[indexRank] = rank;
            }
        }
        return flagList;
    }

  } //namespace ext
} //namespace testing
