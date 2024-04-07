// Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

#ifndef GTEST_INCLUDE_GTEST_GTEST_TAG_H_
#define GTEST_INCLUDE_GTEST_GTEST_TAG_H_

#include <string.h>
#include <map>
#include <vector>

namespace testing {
  namespace ext {

    using ::std::string;
    using ::std::map;
    using ::std::pair;
    using ::std::vector;

    enum TestTypeFlag {
        Function = 1 << 8, Performance = 2 << 8, Power = 3 << 8, Reliability = 4 << 8,
        Security = 5 << 8, Global = 6 << 8, Compatibility = 7 << 8, User = 8 << 8,
        Standard = 9 << 8, Safety = 10 << 8, Resilience = 11 << 8
    };

    enum TestSizeFlag {
        SmallTest = 1 << 4, MediumTest = 2 << 4, LargeTest = 3 << 4
    };

    enum TestRankFlag {
        Level0 = 1, Level1 = 2, Level2 = 3, Level3 = 4, Level4 = 5
    };

    // base class of tag flag::bitwise integers
    class TestFlag {
    public:
        static const int None = 0;

    private:
        const char* const name;
        const char* const desc;
        const int mask;
        map<int, const char*> elementMap;
        int eleCount;
    protected:
        TestFlag(const char*  n, const char* d, int m);
        void element(const char* desc, int hex);
    public:
        bool verify(const int hex, char* err) const;
        const char* naming() const { return name; }
        const char* description() const { return desc; }
        bool eleForName(const char* name, int& result) const;
        void printHelp(const char** indents) const;
    };

    // test size scope
    class  TestSizeSet : public TestFlag {
    public:
        TestSizeSet();
        static const int Level0 = 1  << 24;
        static const int Level1 = 2  << 24;
        static const int Level2 = 4  << 24;
        static const int Level3 = 8  << 24;
        static const int Level4 = 16 << 24;
    };

    extern const TestSizeSet TestSize;

    // test type scope
    class  TypeSet : public TestFlag {
    public:
        TypeSet();
        static const int function = Function;
        static const int performance = Performance;
        static const int power = Power;
        static const int reliability = Reliability;
        static const int security = Security;
        static const int global = Global;
        static const int compatibility = Compatibility;
        static const int user = User;
        static const int standard = Standard;
        static const int safety = Safety;
        static const int resilience = Resilience;
    };
    
    // test size scope
    class  SizeSet : public TestFlag {
    public:
        SizeSet();
        static const int smallTest = SmallTest;
        static const int mediumTest = MediumTest;
        static const int largeTest = LargeTest;
    };

    // test rank scope
    class  RankSet : public TestFlag {
    public:
        RankSet();
        static const int level0 = Level0;
        static const int level1 = Level1;
        static const int level2 = Level2;
        static const int level3 = Level3;
        static const int level4 = Level4;
    };

    // get each instance of all the TestFlag implementions
    const vector<const TestFlag*>& AllHextTagSets();
    // verify the test flagset, returns false if the flagset is illegal
    bool CheckFlagsLegality(int flags);
    // convert name string to test flag value
    bool flagForName(const char* set_name, const char* ele_name, int& result);

  } // namespace ext
} // namespace testing

#endif  // GTEST_INCLUDE_GTEST_GTEST_TAG_H_
