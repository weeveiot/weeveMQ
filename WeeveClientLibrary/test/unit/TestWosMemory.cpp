#include "gtest/gtest.h"

#include "wosMemory.h"

#define LENGTH 3
#define CHAR_VALUE '3'

namespace
{

TEST(TestWosMemory, TrivialAll)
{
    uint8_t data[LENGTH] = {0x01, 0x02, 0x03};
    uint8_t *pFixedBuffer = data;
    uint8_t *pDynamicBuffer = NULL;
    int32_t compareResult;
    uint8_t compareTimeResult;

    pDynamicBuffer = (uint8_t *)wosMemAlloc(LENGTH * sizeof(uint8_t));
    EXPECT_NE(pDynamicBuffer, nullptr);

    pDynamicBuffer = (uint8_t *)wosMemSet(pDynamicBuffer, CHAR_VALUE,
                                          LENGTH * sizeof(uint8_t));
    EXPECT_NE(pDynamicBuffer, nullptr);
    for (uint32_t i = 0; i < LENGTH; ++i) {
        EXPECT_EQ(pDynamicBuffer[i], CHAR_VALUE);
    }

    wosMemCopy(pDynamicBuffer, pFixedBuffer, LENGTH);
    for (uint32_t i = 0; i < LENGTH; ++i) {
        EXPECT_EQ(pFixedBuffer[i], pDynamicBuffer[i]);
    }

    compareResult = wosMemComparison(pFixedBuffer, pDynamicBuffer, LENGTH);
    EXPECT_EQ(compareResult, 0);
    compareTimeResult =
        wosMemComparisonConstTime(pFixedBuffer, pDynamicBuffer, LENGTH);
    EXPECT_EQ(compareTimeResult, 0);

    data[0] = 0xFF;
    compareResult = wosMemComparison(pFixedBuffer, pDynamicBuffer, LENGTH);
    EXPECT_NE(compareResult, 0);
    compareTimeResult =
        wosMemComparisonConstTime(pFixedBuffer, pDynamicBuffer, LENGTH);
    EXPECT_NE(compareTimeResult, 0);

    wosMemFree(pDynamicBuffer);
}

TEST(TestWosMemory, NegativeAll)
{
    uint8_t array[LENGTH];
    void *p;
    int32_t compareResult;
    uint8_t compareTimeResult;

    p = wosMemAlloc(0);
    EXPECT_EQ(p, nullptr);

    p = (uint8_t *)wosMemSet(NULL, CHAR_VALUE, LENGTH * sizeof(uint8_t));
    EXPECT_EQ(p, nullptr);

    p = wosMemCopy(NULL, array, LENGTH);
    EXPECT_EQ(p, nullptr);
    p = wosMemCopy(array, NULL, LENGTH);
    EXPECT_EQ(p, nullptr);

    compareResult = wosMemComparison(NULL, array, LENGTH);
    EXPECT_EQ(compareResult, -1);
    compareResult = wosMemComparison(array, NULL, LENGTH);
    EXPECT_EQ(compareResult, 1);

    compareTimeResult = wosMemComparisonConstTime(NULL, array, LENGTH);
    EXPECT_EQ(compareTimeResult, 1);
    compareTimeResult = wosMemComparisonConstTime(array, NULL, LENGTH);
    EXPECT_EQ(compareTimeResult, 1);
}

} // namespace
