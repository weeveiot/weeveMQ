#include <stdio.h>
#include <string.h>

#include "gtest/gtest.h"

#include "wosCborCert.h"
#include "wosLog.h"
#include "wosMemory.h"
#include "wosMsgCommon.h"
#include "wosString.h"
#include "wosTypes.h"

#define LOG_TAG "TestUnitCborCert"

namespace
{

#define TEST_VERSION_0 (0x00)
#define TEST_SIGALGO_ECDSA 0
#define TEST_ISSUERID_WEEVE "WEEVE"
#define TEST_SUBJECTID_JOHN "JOHN DOE"
#define TEST_PUBKEY_INFO 100

/*
 * Test packing/parsing of Certificate TBS.
 * Step 1 - Pack using wosCborPackCertificateTbs() API.
 * Step 2 - Validate that input parameters to wosCborPackCertificateTbs() API
 *          must not modified.
 * Step 3 - Validate the encoded buffer matches to pre-calculated expected
 *          buffer.
 * Step 4 - Parse the above encoded buffer using wosCborUnpackCertificateTbs()
 *          API.
 * Step 5 - Match the parsed parameters to original parameters passed in Step 1.
 */
TEST(TestCborCert, TrivialCborCertTbs)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosCertTbsType_t tbsCert1;
    WosCertTbsType_t tbsCert2;
    WosBuffer_t pubKey;

    uint8_t pack[128] = {0}; /* larger than needed */
    WosBuffer_t packedBuffer = {pack, sizeof(pack)};
    WosBuffer_t expectedPackedBuffer;
    uint32_t packedLength;

    uint8_t pubKeyData[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    pubKey = {pubKeyData, sizeof(pubKeyData)};

    /* This is expected CBOR-packed buffer */
    /* [0, 0, "WEEVE", "JOHN DOE", 100, h'0123456789ABCDEF'] */
    uint8_t expectedPacked[] = {0x9F, 0x00, 0x00, 0x65, 0x57, 0x45, 0x45, 0x56,
                                0x45, 0x68, 0x4A, 0x4F, 0x48, 0x4E, 0x20, 0x44,
                                0x4F, 0x45, 0x18, 0x64, 0x48, 0x01, 0x23, 0x45,
                                0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFF};
    expectedPackedBuffer = {expectedPacked, sizeof(expectedPacked)};

    tbsCert1.version = TEST_VERSION_0;
    tbsCert1.signatureAlgorithm = TEST_SIGALGO_ECDSA;
    tbsCert1.issuerId = TEST_ISSUERID_WEEVE;
    tbsCert1.subjectId = TEST_SUBJECTID_JOHN;
    tbsCert1.subjectPubKeyInfo = TEST_PUBKEY_INFO;
    tbsCert1.pSubjectPubKey = &pubKey;

    ///// Step 1 - Pack
    msgResult = wosCborCalcCertificateTbs(&tbsCert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);
    EXPECT_EQ(packedBuffer.length, packedLength);

    ///// Step 2 - Check that input parameters are not modified
    EXPECT_EQ(tbsCert1.version, TEST_VERSION_0);
    EXPECT_EQ(tbsCert1.signatureAlgorithm, TEST_SIGALGO_ECDSA);
    EXPECT_EQ(wosStringComparison(tbsCert1.issuerId, TEST_ISSUERID_WEEVE), 0);
    EXPECT_EQ(wosStringComparison(tbsCert1.subjectId, TEST_SUBJECTID_JOHN), 0);
    EXPECT_EQ(tbsCert1.subjectPubKeyInfo, TEST_PUBKEY_INFO);
    EXPECT_EQ(tbsCert1.pSubjectPubKey->length, pubKey.length);
    EXPECT_EQ(wosMemComparison(tbsCert1.pSubjectPubKey->data, pubKey.data,
                               tbsCert1.pSubjectPubKey->length),
              0);

    ///// Step 3 - Check if expected CBor-packed buffer match
    EXPECT_NE(packedBuffer.data, (void *)0);
    EXPECT_EQ(expectedPackedBuffer.length, packedBuffer.length);
    EXPECT_EQ(wosMemComparison(packedBuffer.data, expectedPackedBuffer.data,
                               packedBuffer.length),
              0);

    ///// Step 4 - Parse
    msgResult = wosCborUnpackCertificateTbs(&packedBuffer, &tbsCert2);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);

    ///// Step 5 - Check if parsed values are correct
    EXPECT_EQ(tbsCert2.version, TEST_VERSION_0);
    EXPECT_EQ(tbsCert2.signatureAlgorithm, TEST_SIGALGO_ECDSA);
    EXPECT_EQ(wosStringComparison(tbsCert2.issuerId, TEST_ISSUERID_WEEVE), 0);
    EXPECT_EQ(wosStringComparison(tbsCert2.subjectId, TEST_SUBJECTID_JOHN), 0);
    EXPECT_EQ(tbsCert2.subjectPubKeyInfo, TEST_PUBKEY_INFO);
    EXPECT_EQ(tbsCert2.pSubjectPubKey->length, pubKey.length);
    EXPECT_EQ(wosMemComparison(tbsCert2.pSubjectPubKey->data, pubKey.data,
                               tbsCert2.pSubjectPubKey->length),
              0);

    ///// Step 6 - Free
    /* void */ wosCborFreeCertificateTbs(&tbsCert2);

    ///// Step 7 - Check if atleast members are set to NULL
    EXPECT_EQ(tbsCert2.version, 0);
    EXPECT_EQ(tbsCert2.signatureAlgorithm, 0);
    EXPECT_EQ(tbsCert2.issuerId, (void *)0);
    EXPECT_EQ(tbsCert2.subjectId, (void *)0);
    EXPECT_EQ(tbsCert2.subjectPubKeyInfo, 0);
    EXPECT_EQ(tbsCert2.pSubjectPubKey, (void *)0);
}

/*
 * Test packing/parsing of Certificate Container.
 * Step 1 - Pack using wosCborPackCertificateContainer() API.
 * Step 2 - Validate that input parameters to wosCborPackCertificateContainer()
 *          API must not modified.
 * Step 3 - Validate the encoded buffer matches to pre-calculated expected
 *          buffer.
 * Step 4 - Parse the above encoded buffer using
 *          wosCborUnpackCertificateContainer() API.
 * Step 5- Match the parsed parameters to original parameters passed in Step 1.
 */
TEST(TestCborCert, TrivialCborCertContainer)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosCertContainerType_t cert1;
    WosCertContainerType_t cert2;

    WosBuffer_t encodedTbs;
    WosBuffer_t signature;

    uint8_t pack[128] = {0};
    WosBuffer_t packedBuffer = {pack, sizeof(pack)};
    WosBuffer_t expectedPackedBuffer;
    uint32_t packedLength;

    /* Same as the expected of the test above */
    uint8_t encodedTbsData[30] = {
        0x9F, 0x00, 0x00, 0x65, 0x57, 0x45, 0x45, 0x56, 0x45, 0x68,
        0x4A, 0x4F, 0x48, 0x4E, 0x20, 0x44, 0x4F, 0x45, 0x18, 0x64,
        0x48, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFF};
    encodedTbs = {encodedTbsData, sizeof(encodedTbsData)};

    /* A correct signature is not needed at this point. */
    uint8_t signatureData[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    signature = {signatureData, sizeof(signatureData)};

    /* [0,
     * h'9F0000655745455645684A4F484E20444F451864480123456789ABCDEFFF',
     * h'0123456789ABCDEF'] */
    uint8_t expectedPacked[] = {
        0x9F, 0x00, 0x58, 0x1E, 0x9F, 0x00, 0x00, 0x65, 0x57, 0x45, 0x45,
        0x56, 0x45, 0x68, 0x4A, 0x4F, 0x48, 0x4E, 0x20, 0x44, 0x4F, 0x45,
        0x18, 0x64, 0x48, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFF, 0x48, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFF};
    expectedPackedBuffer = {expectedPacked, sizeof(expectedPacked)};

    cert1.version = TEST_VERSION_0;
    cert1.pEncodedTbs = &encodedTbs;
    cert1.pSignature = &signature;

    ///// Step 1 - Pack
    msgResult = wosCborCalcCertificateContainer(&cert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);
    EXPECT_EQ(packedBuffer.length, packedLength);

    ///// Step 2 - Check that input parameters are not modified
    EXPECT_EQ(cert1.version, TEST_VERSION_0);
    EXPECT_EQ(cert1.pEncodedTbs->length, encodedTbs.length);
    EXPECT_EQ(wosMemComparison(cert1.pEncodedTbs->data, encodedTbs.data,
                               cert1.pEncodedTbs->length),
              0);
    EXPECT_EQ(cert1.pSignature->length, signature.length);
    EXPECT_EQ(wosMemComparison(cert1.pSignature->data, signature.data,
                               cert1.pSignature->length),
              0);

    ///// Step 3 - Check if expected CBor-packed buffer match
    EXPECT_NE(packedBuffer.data, (void *)0);
    EXPECT_EQ(expectedPackedBuffer.length, packedBuffer.length);
    EXPECT_EQ(wosMemComparison(packedBuffer.data, expectedPackedBuffer.data,
                               packedBuffer.length),
              0);

    ///// Step 4 - Parse
    msgResult = wosCborUnpackCertificateContainer(&packedBuffer, &cert2);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);

    ///// Step 5 - Check if parsed values are correct
    EXPECT_EQ(cert2.version, TEST_VERSION_0);
    EXPECT_EQ(cert2.pEncodedTbs->length, encodedTbs.length);
    EXPECT_EQ(wosMemComparison(cert2.pEncodedTbs->data, encodedTbs.data,
                               cert2.pEncodedTbs->length),
              0);
    EXPECT_EQ(cert2.pSignature->length, signature.length);
    EXPECT_EQ(wosMemComparison(cert2.pSignature->data, signature.data,
                               cert2.pSignature->length),
              0);

    ///// Step 6 - Free
    /* void */ wosCborFreeCertificateContainer(&cert2);

    ///// Step 7 - Check if atleast members are set to NULL
    EXPECT_EQ(cert2.version, 0);
    EXPECT_EQ(cert2.pEncodedTbs, (void *)0);
    EXPECT_EQ(cert2.pSignature, (void *)0);
}

TEST(TestCborCert, NegativeCborCertTbs)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosCertTbsType_t tbsCert1;
    WosCertTbsType_t tbsCert2;
    WosBuffer_t pubKey;

    uint8_t pack[128] = {0}; /* larger than needed */
    WosBuffer_t packedBuffer = {pack, sizeof(pack)};
    WosBuffer_t expectedPackedBuffer;
    uint32_t packedLength;

    uint8_t pubKeyData[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    pubKey = {pubKeyData, sizeof(pubKeyData)};

    /* This is expected CBOR-packed buffer */
    /* [0, 0, "WEEVE", "JOHN DOE", 100, h'0123456789ABCDEF'] */
    uint8_t expectedPacked[] = {0x9F, 0x00, 0x00, 0x65, 0x57, 0x45, 0x45, 0x56,
                                0x45, 0x68, 0x4A, 0x4F, 0x48, 0x4E, 0x20, 0x44,
                                0x4F, 0x45, 0x18, 0x64, 0x48, 0x01, 0x23, 0x45,
                                0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFF};
    expectedPackedBuffer = {expectedPacked, sizeof(expectedPacked)};

    tbsCert1.version = TEST_VERSION_0;
    tbsCert1.signatureAlgorithm = TEST_SIGALGO_ECDSA;
    tbsCert1.issuerId = TEST_ISSUERID_WEEVE;
    tbsCert1.subjectId = TEST_SUBJECTID_JOHN;
    tbsCert1.subjectPubKeyInfo = TEST_PUBKEY_INFO;
    tbsCert1.pSubjectPubKey = &pubKey;

    ////////// Pack with invalid values
    msgResult = wosCborPackCertificateTbs(NULL, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    msgResult = wosCborPackCertificateTbs(&tbsCert1, NULL);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    // Packed Buffer
    packedBuffer = {NULL, sizeof(pack)};
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, 0};
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, sizeof(pack)};
    // Issuer
    tbsCert1.issuerId = NULL;
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    tbsCert1.issuerId = TEST_ISSUERID_WEEVE;
    // SubjectId
    tbsCert1.subjectId = NULL;
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    tbsCert1.subjectId = TEST_SUBJECTID_JOHN;
    // Subject Public Key
    tbsCert1.pSubjectPubKey = NULL;
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    tbsCert1.pSubjectPubKey = &pubKey;
    pubKey = {NULL, sizeof(pubKeyData)};
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    pubKey = {pubKeyData, 0};
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    pubKey = {pubKeyData, sizeof(pubKeyData)};

    ////////// Pack with wrong values
    tbsCert1.issuerId = TEST_SUBJECTID_JOHN; // Switched values
    tbsCert1.subjectId = TEST_ISSUERID_WEEVE;
    msgResult = wosCborPackCertificateTbs(&tbsCert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);
    EXPECT_NE(packedBuffer.data, (void *)0);
    EXPECT_NE(wosMemComparison(packedBuffer.data, expectedPackedBuffer.data,
                               expectedPackedBuffer.length),
              0);
    tbsCert1.issuerId = TEST_ISSUERID_WEEVE;
    tbsCert1.subjectId = TEST_SUBJECTID_JOHN;

    ////////// Calculate with invalid structs
    msgResult = wosCborCalcCertificateTbs(&tbsCert1, NULL);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    msgResult = wosCborCalcCertificateTbs(NULL, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    // Issuer
    tbsCert1.issuerId = NULL;
    msgResult = wosCborCalcCertificateTbs(&tbsCert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    tbsCert1.issuerId = TEST_ISSUERID_WEEVE;
    // SubjectId
    tbsCert1.subjectId = NULL;
    msgResult = wosCborCalcCertificateTbs(&tbsCert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    tbsCert1.subjectId = TEST_SUBJECTID_JOHN;
    // Subject Public Key
    tbsCert1.pSubjectPubKey = NULL;
    msgResult = wosCborCalcCertificateTbs(&tbsCert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    tbsCert1.pSubjectPubKey = &pubKey;
    pubKey = {NULL, sizeof(pubKeyData)};
    msgResult = wosCborCalcCertificateTbs(&tbsCert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    pubKey = {pubKeyData, 0};
    msgResult = wosCborCalcCertificateTbs(&tbsCert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    pubKey = {pubKeyData, sizeof(pubKeyData)};

    ////////// Parse invalid values
    msgResult = wosCborUnpackCertificateTbs(&packedBuffer, NULL);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    msgResult = wosCborUnpackCertificateTbs(NULL, &tbsCert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {NULL, sizeof(pack)};
    msgResult = wosCborUnpackCertificateTbs(&packedBuffer, &tbsCert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, 0};
    msgResult = wosCborUnpackCertificateTbs(&packedBuffer, &tbsCert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, sizeof(pack)};

    ////////// Parse wrong values
    pack[0] = 0x00;
    msgResult = wosCborUnpackCertificateTbs(&packedBuffer, &tbsCert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_FORMAT);
}

TEST(TestCborCert, NegativeCborCertContainer)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosCertContainerType_t cert1;
    WosCertContainerType_t cert2;

    WosBuffer_t encodedTbs;
    WosBuffer_t signature;

    uint8_t pack[128] = {0};
    WosBuffer_t packedBuffer = {pack, sizeof(pack)};
    WosBuffer_t expectedPackedBuffer;
    uint32_t packedLength;

    /* Same as the expected of the test above */
    uint8_t encodedTbsData[30] = {
        0x9F, 0x00, 0x00, 0x65, 0x57, 0x45, 0x45, 0x56, 0x45, 0x68,
        0x4A, 0x4F, 0x48, 0x4E, 0x20, 0x44, 0x4F, 0x45, 0x18, 0x64,
        0x48, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFF};
    encodedTbs = {encodedTbsData, sizeof(encodedTbsData)};

    /* A correct signature is not needed at this point. */
    uint8_t signatureData[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    signature = {signatureData, sizeof(signatureData)};

    /* [0,
     * h'9F0000655745455645684A4F484E20444F451864480123456789ABCDEFFF',
     * h'0123456789ABCDEF'] */
    uint8_t expectedPacked[] = {
        0x9F, 0x00, 0x58, 0x1E, 0x9F, 0x00, 0x00, 0x65, 0x57, 0x45, 0x45,
        0x56, 0x45, 0x68, 0x4A, 0x4F, 0x48, 0x4E, 0x20, 0x44, 0x4F, 0x45,
        0x18, 0x64, 0x48, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFF, 0x48, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFF};
    expectedPackedBuffer = {expectedPacked, sizeof(expectedPacked)};

    cert1.version = TEST_VERSION_0;
    cert1.pEncodedTbs = &encodedTbs;
    cert1.pSignature = &signature;

    ////////// Pack with invalid values
    msgResult = wosCborPackCertificateContainer(NULL, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    msgResult = wosCborPackCertificateContainer(&cert1, NULL);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    // Packed Buffer
    packedBuffer = {NULL, sizeof(pack)};
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, 0};
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, sizeof(pack)};
    // encodedTbs
    encodedTbs = {NULL, sizeof(encodedTbsData)};
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    encodedTbs = {encodedTbsData, 0};
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    encodedTbs = {encodedTbsData, sizeof(encodedTbsData)};
    // signature
    signature = {NULL, sizeof(signatureData)};
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    signature = {signatureData, 0};
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    signature = {signatureData, sizeof(signatureData)};

    ////////// Pack with wrong values
    cert1.pEncodedTbs = &signature;
    cert1.pSignature = &encodedTbs;
    msgResult = wosCborPackCertificateContainer(&cert1, &packedBuffer);
    EXPECT_EQ(msgResult, WOS_MSG_SUCCESS);
    EXPECT_NE(packedBuffer.data, (void *)0);
    EXPECT_NE(wosMemComparison(packedBuffer.data, expectedPackedBuffer.data,
                               packedBuffer.length),
              0);
    cert1.pEncodedTbs = &encodedTbs;
    cert1.pSignature = &signature;

    ////////// Calculate with invalid structs
    msgResult = wosCborCalcCertificateContainer(&cert1, NULL);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    msgResult = wosCborCalcCertificateContainer(NULL, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    // encodedTbs
    encodedTbs = {NULL, sizeof(encodedTbsData)};
    msgResult = wosCborCalcCertificateContainer(&cert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    encodedTbs = {encodedTbsData, 0};
    msgResult = wosCborCalcCertificateContainer(&cert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    encodedTbs = {encodedTbsData, sizeof(encodedTbsData)};
    // signature
    signature = {NULL, sizeof(signatureData)};
    msgResult = wosCborCalcCertificateContainer(&cert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    signature = {signatureData, 0};
    msgResult = wosCborCalcCertificateContainer(&cert1, &packedLength);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    signature = {signatureData, sizeof(signatureData)};

    ////////// Parse invalid values
    msgResult = wosCborUnpackCertificateContainer(&packedBuffer, NULL);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    msgResult = wosCborUnpackCertificateContainer(NULL, &cert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {NULL, sizeof(pack)};
    msgResult = wosCborUnpackCertificateContainer(&packedBuffer, &cert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, 0};
    msgResult = wosCborUnpackCertificateContainer(&packedBuffer, &cert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_PARAMS);
    packedBuffer = {pack, sizeof(pack)};

    ////////// Parse wrong values
    pack[0] = 0x00;
    msgResult = wosCborUnpackCertificateContainer(&packedBuffer, &cert2);
    EXPECT_EQ(msgResult, WOS_MSG_ERROR_BAD_FORMAT);
}

} // namespace
