#include "gtest/gtest.h"

#include "wosCommon.h"
#include "wosLog.h"
#include "wosMemory.h"
#include "wosStorage.h"

namespace
{

class TestWosStorage : public testing::Test
{
  protected:
    uint8_t testData[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    void *pStorageContext = NULL;
    WosString_t keyStorageIdOne = (WosString_t) "keyOne";
    WosString_t keyStorageIdTwo = (WosString_t) "keyTwo";

    virtual void clearTestData()
    {
        WosStorageError_t storageError;
        storageError = wosStorageDelete(pStorageContext, keyStorageIdOne);
        storageError = wosStorageDelete(pStorageContext, keyStorageIdTwo);
    }

    virtual void SetUp()
    {
        WosStorageConfig_t config;
        config.hostname = (char *)"127.0.0.1";
        config.port = 6379;
        config.dbId = 0;

        WosStorageError_t storageError;
        storageError = wosStorageInitialize(&pStorageContext, &config);
        ASSERT_EQ(storageError, WOS_STORAGE_SUCCESS);

        clearTestData();
    }

    virtual void TearDown()
    {
        WosStorageError_t storageError;

        clearTestData();

        storageError = wosStorageTerminate(pStorageContext);
        EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
    }
};

TEST_F(TestWosStorage, TrivialAll)
{
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosBuffer_t buffer = {.data = testData, .length = sizeof(testData)};
    WosBuffer_t *pBuffer = NULL;
    int32_t compareResult = -1;

    /* Write */
    storageError = wosStorageWrite(pStorageContext, keyStorageIdOne, &buffer);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);

    /* Read */
    storageError = wosStorageRead(pStorageContext, keyStorageIdOne, &pBuffer);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
    EXPECT_EQ(pBuffer->length, buffer.length);
    compareResult =
        wosMemComparison(buffer.data, pBuffer->data, pBuffer->length);
    EXPECT_EQ(compareResult, 0);
    WOS_FREE_BUF_AND_DATA(pBuffer);

    /* Delete */
    storageError = wosStorageDelete(pStorageContext, keyStorageIdOne);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
}

TEST_F(TestWosStorage, NegativeAll)
{
    WosStorageError_t storageError = WOS_STORAGE_SUCCESS;
    WosBuffer_t buffer = {.data = testData, .length = sizeof(testData)};
    WosBuffer_t *pBuffer = NULL;
    int32_t compareResult = -1;

    /* Write */
    // TODO Storage using StdC-File allows NULL context
    // storageError = wosStorageWrite(NULL, keyStorageIdOne, &buffer);
    // EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);
    storageError = wosStorageWrite(pStorageContext, NULL, &buffer);
    EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);
    buffer = {.data = NULL, .length = sizeof(testData)};
    storageError = wosStorageWrite(pStorageContext, keyStorageIdOne, &buffer);
    EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);
    buffer = {.data = testData, .length = 0};
    storageError = wosStorageWrite(pStorageContext, keyStorageIdOne, &buffer);
    EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);
    buffer = {.data = testData, .length = sizeof(testData)};
    storageError = wosStorageWrite(pStorageContext, keyStorageIdOne, NULL);
    EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);

    /* Read */
    // TODO Storage using StdC-File allows NULL context
    // storageError = wosStorageRead(NULL, keyStorageIdOne, &pBuffer);
    // EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);
    storageError = wosStorageRead(pStorageContext, NULL, &pBuffer);
    EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);

    /* Delete */
    // TODO Storage using StdC-File allows NULL context
    // storageError = wosStorageDelete(NULL, keyStorageIdOne);
    // EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);
    storageError = wosStorageDelete(pStorageContext, NULL);
    EXPECT_EQ(storageError, WOS_STORAGE_ERROR_BAD_PARAMS);
}

} /* namespace */
