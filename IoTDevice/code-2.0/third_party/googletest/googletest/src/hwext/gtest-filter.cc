/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: CPPTest框架中通过Flag标识实现用例过滤
 */

#include "gtest/hwext/gtest-filter.h"
#include "gtest/hwext/gtest-tag.h"
#include "gtest/hwext/gtest-utils.h"

namespace testing {
  namespace ext {

    const char* const TestFilter::kStrictFilter = "strict_tags";
    enum IndexEnum {A = 1, B, C, D, E, F, G, H, I, J, K};

    TestFilter* TestFilter::instance() 
    {
        static TestFilter* instance_ = NULL;
        if (instance_ == NULL) {
            instance_ = new TestFilter();
            instance_->reset();
        }
        return instance_;
    }

    void TestFilter::printHelp() const 
    {
        printf("\nTest Filtering:\n");
        const std::vector<const TestFlag*>& sets = AllHextTagSets();
        // help message line indents
        const char* indents[] = { "  ", "      " };

        for (unsigned int i = 0; i < sets.size(); i++) {
            sets.at(i)->printHelp(indents);
        }

        printf("%sSelect tests by test level, may be a list seperated by ',' or ';'.\n", indents[1]);
    }

    map<const char*, string*>& TestFilter::getAllFilterFlagsKv() 
    {
        // no need to consider concurrence so for, because we do 
        // this before running tests
        if (flag_kvs_inited) {
            return filterFlagsKv;
        }

        flag_kvs_inited = true;
        const std::vector<const TestFlag*>& sets = AllHextTagSets();
        for (unsigned int i = 0; i < sets.size(); i++) {
            filterFlagsKv.insert(pair<const char*, string*>(sets.at(i)->naming(), new string("")));
        }

        // strict mode
        filterFlagsKv.insert(pair<const char*, string*>(kStrictFilter, new string("false")));
        return filterFlagsKv;
    }

    void TestFilter::postSetType (vector<string> vectemp) 
    {
        for (size_t i = 0; i < vectemp.size(); i++) {
            string curr = vectemp[i];
            if (curr == "Function") {
                vecType.push_back(A);
            } else if (curr == "Performance") {
                vecType.push_back(B);
            } else if (curr == "Power") {
                vecType.push_back(C);
            } else if (curr == "Reliability") {
                vecType.push_back(D);
            } else if (curr == "Security") {
                vecType.push_back(E);
            } else if (curr == "Global") {
                vecType.push_back(F);
            } else if (curr == "Compatibility") {
                vecType.push_back(G);
            } else if (curr == "User") {
                vecType.push_back(H);
            } else if (curr == "Standard") {
                vecType.push_back(I);
            } else if (curr == "Safety") {
                vecType.push_back(J);
            } else if (curr == "Resilience") {
                vecType.push_back(K);
            }
        }
    }

    void TestFilter::postSetSize (vector<string> vectemp) 
    {
        for (size_t i = 0; i < vectemp.size(); i++) {
            string curr = vectemp[i];
            if (curr == "SmallTest") {
                vecSize.push_back(A);
            } else if (curr == "MediumTest") {
                vecSize.push_back(B);
            } else if (curr == "LargeTest") {
                vecSize.push_back(C);
            }
        }
    }

    void TestFilter::postSetRank (vector<string> vectemp) 
    {
        for (size_t i = 0; i < vectemp.size(); i++) {
            string curr = vectemp[i];
            if (curr == "Level0") {
                vecRank.push_back(A);
            } else if (curr == "Level1") {
                vecRank.push_back(B);
            } else if (curr == "Level2") {
                vecRank.push_back(C);
            } else if (curr == "Level3") {
                vecRank.push_back(D);
            } else if (curr == "Level4") {
                vecRank.push_back(E);
            }
        }
    }

    bool TestFilter::postParsingArguments() 
    {
        if (ready || !flag_kvs_inited) {
            // only run setup logic once
            return true;
        }

        const char *kCandidateSeps = ",;|/";
        ready = true;
        bool error = false;
        map<const char*, string*>::iterator iter;

        for (iter = filterFlagsKv.begin(); iter != filterFlagsKv.end(); iter++) {
            const char *kstr = iter->first;
            const char *vstr = iter->second->c_str();
            int flag = TestFlag::None;

            if (compareStringsByIgnoreCase(kStrictFilter, kstr)) {
                strictMode = compareStringsByIgnoreCase("true", vstr) || compareStringsByIgnoreCase("t", vstr);
            } else if (flagForName(kstr, vstr, flag)) {
                string strname = string(kstr);
                string strval = string(vstr);
                vector<string> vectemp = SplitString(strval, kCandidateSeps);

                if (strname == "testsize") {
                    for (size_t i = 0; i < vectemp.size(); i++) {
                        string curr = vectemp[i];
                        if (curr == "Level0") {
                            vecTestLevel.push_back(1);
                        } else if (curr == "Level1") {
                            vecTestLevel.push_back(2);
                        } else if (curr == "Level2") {
                            vecTestLevel.push_back(4);
                        } else if (curr == "Level3") {
                            vecTestLevel.push_back(8);
                        } else if (curr == "Level4") {
                            vecTestLevel.push_back(16);
                        }
                    }
                } else if (strname == "type") {
                    this->postSetType(vectemp);
                } else if (strname == "size") {
                    this->postSetSize(vectemp);
                } else if (strname == "rank") {
                    this->postSetRank(vectemp);
                }
            } else {
                // illegal arguments
                error = true;
            }
                // free the newed strings
                delete iter->second;
        }

        filterFlagsKv.clear();
        return !error;     
    }

    bool  TestFilter::accept(int flags) const 
    {
        // both of flags and req_no should be accepted
        if (!ready) {
            return true;
        }
      
        int posType = 8;
        int posSize = 4;
        int level = (flags >> 24);
        int type = (flags >> posType);
        int size = (flags >> posSize);
        int rank = flags;
        bool flags_type = false;
        bool flags_size = false;
        bool flags_rank = false;
        bool flags_level = false;
        bool flags_accepted = false;
      
        if (!strictMode) {
            flags_type = IsElementInVector(vecType, type);
            flags_size = IsElementInVector(vecSize, size);
            flags_rank = IsElementInVector(vecRank, rank);
            flags_level = IsElementInVector(vecTestLevel, level);
            flags_accepted = (flags_type & flags_size & flags_rank) | (flags_level);
        }
        else {
            flags_accepted = ((flags&requiredFlags) == requiredFlags);
        }

        if (!flags_accepted) {
            return false;
        }

        return true;
    }

    void TestFilter::reset() 
    {
        filterFlagsKv.clear();
        requiredFlags = TestFlag::None;
        flag_kvs_inited = false;
        strictMode = false;
        ready = false;
    }

  } // namespace ext
} // namespace testing