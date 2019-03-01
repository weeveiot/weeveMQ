#include "gtest/gtest.h"

#include "wosMemory.h"
#include "wosString.h"

namespace
{
class TestWosString : public testing::Test
{
  protected:
    char testStringNbt[4] = "NBT";
    char testStringWeeve[6] = "weeve";
    char testStringFinal[9] = "NBTweeve";
};

TEST_F(TestWosString, TrivialAll)
{
    WosString_t firstString = testStringNbt, secondString = testStringWeeve;
    WosString_t myString;
    int32_t cmp = -1;
    size_t len = 0;

    myString = wosStringConcatenate(firstString, secondString);
    EXPECT_STREQ(myString, testStringFinal);

    len = wosStringLength(testStringFinal);
    EXPECT_EQ(len, 8);

    cmp = wosStringComparison(myString, testStringFinal);
    EXPECT_EQ(cmp, 0);

    wosMemFree(myString);
}

TEST_F(TestWosString, NegativeAll)
{
    WosString_t firstString = testStringNbt, secondString = testStringWeeve;
    WosString_t myString;
    int32_t cmp = 0;
    size_t len = 0;

    myString = wosStringConcatenate(NULL, secondString);
    EXPECT_EQ(myString, nullptr);
    myString = wosStringConcatenate(firstString, NULL);
    EXPECT_EQ(myString, nullptr);

    len = wosStringLength(NULL);
    EXPECT_EQ(len, 0);

    cmp = wosStringComparison(NULL, secondString);
    EXPECT_EQ(cmp, -1);
    cmp = wosStringComparison(firstString, NULL);
    EXPECT_EQ(cmp, 1);

    wosMemFree(myString);
}

} // namespace
