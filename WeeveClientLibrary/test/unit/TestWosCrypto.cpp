#include "gtest/gtest.h"

#include "wosCommon.h"
#include "wosCrypto.h"
#include "wosStorage.h"

namespace
{

class TestWosCrypto : public testing::Test
{

/* ========================================================================== */
/*                                 Test Data                                  */
/* ========================================================================== */

  protected:
    uint8_t alicePrivateKey[113] = {
        0x30, 0x6f, 0x03, 0x02, 0x07, 0x80, 0x02, 0x01, 0x20, 0x02, 0x20, 0x30,
        0xf5, 0xd7, 0xf0, 0xe4, 0x13, 0xc6, 0x95, 0x59, 0xd8, 0x9b, 0x28, 0x32,
        0x1f, 0x69, 0x74, 0x89, 0xe0, 0x12, 0x73, 0xcc, 0xd2, 0x6d, 0x96, 0x43,
        0x0e, 0x8f, 0x13, 0x64, 0x23, 0x5f, 0x7d, 0x02, 0x21, 0x00, 0x94, 0x82,
        0x20, 0x72, 0xf2, 0xb0, 0x20, 0xb9, 0x31, 0x2f, 0xba, 0xb2, 0x87, 0x60,
        0x58, 0x6c, 0x27, 0x66, 0x1e, 0xa0, 0x20, 0xf9, 0x43, 0xab, 0xa2, 0x15,
        0xd0, 0x3d, 0x33, 0x14, 0xad, 0x33, 0x02, 0x21, 0x00, 0xb1, 0x57, 0x8a,
        0xa2, 0x30, 0xdf, 0x04, 0x32, 0xca, 0xf3, 0xdd, 0x75, 0x37, 0xe5, 0x57,
        0xb4, 0x3b, 0x3e, 0xe5, 0x8f, 0x28, 0x5a, 0xa6, 0x0c, 0x97, 0x03, 0xcc,
        0x01, 0x7c, 0x07, 0x72, 0xe7};
    uint8_t alicePublicKey[65] = {
        0x04, 0x30, 0xf5, 0xd7, 0xf0, 0xe4, 0x13, 0xc6, 0x95, 0x59, 0xd8,
        0x9b, 0x28, 0x32, 0x1f, 0x69, 0x74, 0x89, 0xe0, 0x12, 0x73, 0xcc,
        0xd2, 0x6d, 0x96, 0x43, 0x0e, 0x8f, 0x13, 0x64, 0x23, 0x5f, 0x7d,
        0x94, 0x82, 0x20, 0x72, 0xf2, 0xb0, 0x20, 0xb9, 0x31, 0x2f, 0xba,
        0xb2, 0x87, 0x60, 0x58, 0x6c, 0x27, 0x66, 0x1e, 0xa0, 0x20, 0xf9,
        0x43, 0xab, 0xa2, 0x15, 0xd0, 0x3d, 0x33, 0x14, 0xad, 0x33};
    uint8_t bobPrivateKey[113] = {
        0x30, 0x6f, 0x03, 0x02, 0x07, 0x80, 0x02, 0x01, 0x20, 0x02, 0x20, 0x02,
        0x45, 0x91, 0x9b, 0x10, 0x20, 0xc6, 0x3a, 0x70, 0x99, 0x45, 0xcf, 0xf2,
        0x4a, 0x2a, 0x02, 0x6c, 0x84, 0x03, 0x68, 0x3d, 0x10, 0xfd, 0x91, 0xe1,
        0x83, 0x0a, 0xc9, 0x18, 0x68, 0xac, 0xc0, 0x02, 0x21, 0x00, 0xbc, 0x35,
        0xad, 0x73, 0x6d, 0xbf, 0x61, 0xa2, 0xc7, 0xe6, 0xef, 0xb9, 0x5d, 0xa3,
        0x4d, 0x44, 0xfa, 0x7e, 0x80, 0x43, 0x13, 0x3f, 0x09, 0x45, 0x54, 0x87,
        0x1e, 0x3c, 0xfd, 0x56, 0xdf, 0x21, 0x02, 0x21, 0x00, 0xfa, 0x96, 0x4a,
        0x08, 0x15, 0x5a, 0x1e, 0xdd, 0xf1, 0x2e, 0xa6, 0x4c, 0x73, 0x83, 0x19,
        0x50, 0x0f, 0xd4, 0xbf, 0xcc, 0x4b, 0x94, 0xa6, 0x0a, 0x51, 0xc2, 0xa7,
        0x69, 0x8d, 0xd4, 0x61, 0x1f};
    uint8_t bobPublicKey[65] = {
        0x04, 0x02, 0x45, 0x91, 0x9b, 0x10, 0x20, 0xc6, 0x3a, 0x70, 0x99,
        0x45, 0xcf, 0xf2, 0x4a, 0x2a, 0x02, 0x6c, 0x84, 0x03, 0x68, 0x3d,
        0x10, 0xfd, 0x91, 0xe1, 0x83, 0x0a, 0xc9, 0x18, 0x68, 0xac, 0xc0,
        0xbc, 0x35, 0xad, 0x73, 0x6d, 0xbf, 0x61, 0xa2, 0xc7, 0xe6, 0xef,
        0xb9, 0x5d, 0xa3, 0x4d, 0x44, 0xfa, 0x7e, 0x80, 0x43, 0x13, 0x3f,
        0x09, 0x45, 0x54, 0x87, 0x1e, 0x3c, 0xfd, 0x56, 0xdf, 0x21};
    uint8_t sharedSecret[32] = {0x74, 0x0e, 0xc3, 0x47, 0xb6, 0x49, 0x68, 0xfb,
                                0xaf, 0x0a, 0x27, 0x4c, 0x8e, 0xed, 0x5c, 0xd2,
                                0xce, 0x36, 0xd2, 0xf0, 0x62, 0x09, 0xbe, 0x8c,
                                0xa7, 0xcf, 0x6c, 0x42, 0xfd, 0x44, 0xf0, 0xd0};
    uint8_t testData[28] = {0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77,
                            0x6f, 0x72, 0x6c, 0x64, 0x2c, 0x20, 0x74,
                            0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
                            0x77, 0x65, 0x65, 0x76, 0x65, 0x2e, 0x0a};
    struct {
        unsigned char K[32];
        int keylen;
        unsigned char P[128];
        uint32_t ptlen;
        unsigned char A[128];
        uint32_t alen;
        unsigned char IV[128];
        uint32_t IVlen;
        unsigned char C[128];
        unsigned char T[16];
    } aesGcmTests[7] = {
        /* test case #0 - Brian Gladman modes/gcm.1 #13 */
        /* NULL PT & AAD */
        {
            /* key */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            32,
            /* plaintext */
            { 0 },
            0,
            /* AAD data */
            { 0 },
            0,
            /* IV */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00 },
            12,
            /* ciphertext  */
            { 0 },
            /* tag */
            { 0x53, 0x0f, 0x8a, 0xfb, 0xc7, 0x45, 0x36, 0xb9,
              0xa9, 0x63, 0xb4, 0xf1, 0xc4, 0xcb, 0x73, 0x8b }
        },

        /* test case #1 - Brian Gladman modes/gcm.2 #1 */
        /* NULL AAD */
        {
            /* key */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            32,
            /* plaintext */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            16,
            /* AAD data */
            { 0 },
            0,
            /* IV */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00 },
            12,
            /* ciphertext  */
            { 0xce, 0xa7, 0x40, 0x3d, 0x4d, 0x60, 0x6b, 0x6e,
              0x07, 0x4e, 0xc5, 0xd3, 0xba, 0xf3, 0x9d, 0x18 },
            /* tag */
            { 0xd0, 0xd1, 0xc8, 0xa7, 0x99, 0x99, 0x6b, 0xf0,
              0x26, 0x5b, 0x98, 0xb5, 0xd4, 0x8a, 0xb9, 0x19 }
        },

        /* test case #2 - Brian Gladman modes/gcm.2 #2 */
        /* NULL PT */
        {
            /* key */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            32,
            /* plaintext */
            { 0 },
            0,
            /* AAD data */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            16,
            /* IV */
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00 },
            12,
            /* ciphertext  */
            { 0 },
            /* tag */
            { 0x2d, 0x45, 0x55, 0x2d, 0x85, 0x75, 0x92, 0x2b,
              0x3c, 0xa3, 0xcc, 0x53, 0x84, 0x42, 0xfa, 0x26 }
        },

        /* test case #3 - Brian Gladman modes/gcm.1 #18 */
        /* IV longer than 12B */
        {
            /* key */
            { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
              0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
              0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
              0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 },
            32,
            /* plaintext */
            { 0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
              0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
              0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
              0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
              0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
              0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
              0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
              0xba, 0x63, 0x7b, 0x39 },
            60,
            /* AAD data */
            { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
              0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
              0xab, 0xad, 0xda, 0xd2 },
            20,
            /* IV */
            { 0x93, 0x13, 0x22, 0x5d, 0xf8, 0x84, 0x06, 0xe5,
              0x55, 0x90, 0x9c, 0x5a, 0xff, 0x52, 0x69, 0xaa,
              0x6a, 0x7a, 0x95, 0x38, 0x53, 0x4f, 0x7d, 0xa1,
              0xe4, 0xc3, 0x03, 0xd2, 0xa3, 0x18, 0xa7, 0x28,
              0xc3, 0xc0, 0xc9, 0x51, 0x56, 0x80, 0x95, 0x39,
              0xfc, 0xf0, 0xe2, 0x42, 0x9a, 0x6b, 0x52, 0x54,
              0x16, 0xae, 0xdb, 0xf5, 0xa0, 0xde, 0x6a, 0x57,
              0xa6, 0x37, 0xb3, 0x9b },
            60,
            /* ciphertext  */
            { 0x5a, 0x8d, 0xef, 0x2f, 0x0c, 0x9e, 0x53, 0xf1,
              0xf7, 0x5d, 0x78, 0x53, 0x65, 0x9e, 0x2a, 0x20,
              0xee, 0xb2, 0xb2, 0x2a, 0xaf, 0xde, 0x64, 0x19,
              0xa0, 0x58, 0xab, 0x4f, 0x6f, 0x74, 0x6b, 0xf4,
              0x0f, 0xc0, 0xc3, 0xb7, 0x80, 0xf2, 0x44, 0x45,
              0x2d, 0xa3, 0xeb, 0xf1, 0xc5, 0xd8, 0x2c, 0xde,
              0xa2, 0x41, 0x89, 0x97, 0x20, 0x0e, 0xf8, 0x2e,
              0x44, 0xae, 0x7e, 0x3f },
            /* tag */
            { 0xa4, 0x4a, 0x82, 0x66, 0xee, 0x1c, 0x8e, 0xb0,
              0xc8, 0xb5, 0xd4, 0xcf, 0x5a, 0xe9, 0xf1, 0x9a }
        },

        /* test case #4 - Brian Gladman modes/gcm.1 vector#16 */
        /* "Normal" case */
        {
            /* key */
            { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
              0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
              0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
              0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 },
            32,
            /* plaintext */
            { 0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
              0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
              0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
              0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
              0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
              0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
              0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
              0xba, 0x63, 0x7b, 0x39 },
            60,
            /* AAD data */
            { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
              0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
              0xab, 0xad, 0xda, 0xd2 },
            20,
            /* IV */
            { 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
              0xde, 0xca, 0xf8, 0x88 },
            12,
            /* ciphertext  */
            { 0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07,
              0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d,
              0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9,
              0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa,
              0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d,
              0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38,
              0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a,
              0xbc, 0xc9, 0xf6, 0x62 },
            /* tag */
            { 0x76, 0xfc, 0x6e, 0xce, 0x0f, 0x4e, 0x17, 0x68,
              0xcd, 0xdf, 0x88, 0x53, 0xbb, 0x2d, 0x55, 0x1b }
        },

        /* test case #5 - Brian Gladman modes/gcm.2 vector#5 */
        {
            /* key */
            { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
              0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
              0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
              0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f },
            32,
            /* plaintext */
            { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
              0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
              0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 },
            24,
            /* AAD data */
            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
              0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
              0x10, 0x11, 0x12, 0x13 },
            20,
            /* IV */
            { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
              0x18, 0x19, 0x1a, 0x1b },
            12,
            /* ciphertext  */
            { 0x59, 0x1b, 0x1f, 0xf2, 0x72, 0xb4, 0x32, 0x04,
              0x86, 0x8f, 0xfc, 0x7b, 0xc7, 0xd5, 0x21, 0x99,
              0x35, 0x26, 0xb6, 0xfa, 0x32, 0x24, 0x7c, 0x3c },
            /* tag */
            { 0x7d, 0xe1, 0x2a, 0x56, 0x70, 0xe5, 0x70, 0xd8,
              0xca, 0xe6, 0x24, 0xa1, 0x6d, 0xf0, 0x9c, 0x08 }
        },

        /* test case #6 - Brian Gladman modes/gcm.2 vector#9 */
        {
            /* key */
            { 0xf8, 0xd4, 0x76, 0xcf, 0xd6, 0x46, 0xea, 0x6c,
              0x23, 0x84, 0xcb, 0x1c, 0x27, 0xd6, 0x19, 0x5d,
              0xfe, 0xf1, 0xa9, 0xf3, 0x7b, 0x9c, 0x8d, 0x21,
              0xa7, 0x9c, 0x21, 0xf8, 0xcb, 0x90, 0xd2, 0x89 },
            32,
            /* plaintext */
            { 0x90, 0xae, 0x61, 0xcf, 0x7b, 0xae, 0xbd, 0x4c,
              0xad, 0xe4, 0x94, 0xc5, 0x4a, 0x29, 0xae, 0x70,
              0x26, 0x9a, 0xec, 0x71 },
            20,
            /* AAD data */
            { 0x7b, 0xd8, 0x59, 0xa2, 0x47, 0x96, 0x1a, 0x21,
              0x82, 0x3b, 0x38, 0x0e, 0x9f, 0xe8, 0xb6, 0x50,
              0x82, 0xba, 0x61, 0xd3 },
            20,
            /* IV */
            { 0xdb, 0xd1, 0xa3, 0x63, 0x60, 0x24, 0xb7, 0xb4,
              0x02, 0xda, 0x7d, 0x6f },
            12,
            /* ciphertext  */
            { 0xce, 0x20, 0x27, 0xb4, 0x7a, 0x84, 0x32, 0x52,
              0x01, 0x34, 0x65, 0x83, 0x4d, 0x75, 0xfd, 0x0f,
              0x07, 0x29, 0x75, 0x2e },
            /* tag */
            { 0xac, 0xd8, 0x83, 0x38, 0x37, 0xab, 0x0e, 0xde,
              0x84, 0xf4, 0x74, 0x8d, 0xa8, 0x89, 0x9c, 0x15 }
        },

        /* test case #TODO - Brian Gladman modes/gcm.TODO #TODO */
        // {
        //     /* key */
        //     {  },
        //     32,
        //     /* plaintext */
        //     {  },
        //     0,
        //     /* AAD data */
        //     {  },
        //     0,
        //     /* IV */
        //     {  },
        //     12,
        //     /* ciphertext  */
        //     {  },
        //     /* tag */
        //     {  }
        // },
    };

/* ========================================================================== */
/*                                  Common                                    */
/* ========================================================================== */

    void *pStorageContext = NULL;
    WosString_t eccPubKeyStorageId = "eccKeyPublic";
    WosString_t eccPrivKeyStorageId = "eccKeyPrivate";
    WosString_t symKeyStorageId = "symKey";

    virtual void clearTestData()
    {
        /* ignored */ wosStorageDelete(pStorageContext, eccPubKeyStorageId);
        /* ignored */ wosStorageDelete(pStorageContext, eccPrivKeyStorageId);
        /* ignored */ wosStorageDelete(pStorageContext, symKeyStorageId);
    }

    virtual void SetUp()
    {
        WosCryptoError_t cryptoError;
        WosCryptoConfig_t cryptoConfig;

        WosStorageConfig_t *config = NULL;

        /* Crypto */
        cryptoError = wosCryptoInitialize(&cryptoConfig);
        EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);
        /* Storage */
        WosStorageError_t storageError;
        storageError = wosStorageInitialize(&pStorageContext, config);
        ASSERT_EQ(storageError, WOS_STORAGE_SUCCESS);
        clearTestData();
    }

    virtual void TearDown()
    {
        WosCryptoError_t cryptoError;
        /* Storage */
        WosStorageError_t storageError;
        clearTestData();
        storageError = wosStorageTerminate(pStorageContext);
        EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
        /* Crypto */
        cryptoError = wosCryptoTerminate();
        EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);
    }
};

/* ========================================================================== */
/*                         wosCryptoEccGenerateKey                            */
/* ========================================================================== */

TEST_F(TestWosCrypto, TrivialEccGen)
{
    WosCryptoError_t cryptoError;
    WosCryptoEccOptions_t eccOptions;

    cryptoError = wosCryptoEccGenerateKey(
        &eccOptions, pStorageContext, eccPrivKeyStorageId, eccPubKeyStorageId);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

    /* Now there are 2 keys stored: eccPubKeyStorageId, eccPrivKeyStorageId.
     * As there is no predefined key generation, it is not possible to make any
     * assumptions. */
}

TEST_F(TestWosCrypto, NegativeEccGen)
{
    WosCryptoError_t cryptoError;
    WosCryptoEccOptions_t eccOptions;

    // TODO Not yet implemented
    // cryptoError = wosCryptoEccGenerateKey(
    //     NULL, pStorageContext, eccPrivKeyStorageId, eccPubKeyStorageId);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    // TODO Storage using StdC-File allows NULL context
    // cryptoError = wosCryptoEccGenerateKey(
    //     &eccOptions, NULL, eccPrivKeyStorageId, eccPubKeyStorageId);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_STORAGE);

    cryptoError = wosCryptoEccGenerateKey(&eccOptions, pStorageContext, NULL,
                                          eccPubKeyStorageId);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError = wosCryptoEccGenerateKey(&eccOptions, pStorageContext,
                                          eccPrivKeyStorageId, NULL);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
}

/* ========================================================================== */
/*                         wosCryptoGetRandomBytes                            */
/* ========================================================================== */

TEST_F(TestWosCrypto, TrivialGetRandom)
{
    WosCryptoError_t cryptoError;
    const size_t RANDOM_BYTES = 80;
    uint8_t dataArray[RANDOM_BYTES];
    WosBuffer_t buffer;
    buffer = {.data = dataArray, .length = RANDOM_BYTES};

    cryptoError = wosCryptoGetRandomBytes(&buffer);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

    /* Not much that we can test here. =( */
}

TEST_F(TestWosCrypto, NegativeGetRandom)
{
    WosCryptoError_t cryptoError;
    WosBuffer_t buffer = {NULL, 0};

    cryptoError = wosCryptoGetRandomBytes(NULL);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError = wosCryptoGetRandomBytes(&buffer);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
}

/* ========================================================================== */
/*                         wosCryptoEccSign                                   */
/*                         wosCryptoEccSignKeyBuffer                          */
/*                         wosCryptoEccVerify                                 */
/*                         wosCryptoEccVerifyKeyBuffer                        */
/* ========================================================================== */

TEST_F(TestWosCrypto, TrivialEccSign)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    WosBuffer_t privateKey, publicKey, data;
    WosBuffer_t *pSignature = NULL, *pSignature2 = NULL;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    /* Store predefined keys */
    privateKey = {.data = alicePrivateKey, .length = sizeof(alicePrivateKey)};
    storageError =
        wosStorageWrite(pStorageContext, eccPrivKeyStorageId, &privateKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
    publicKey = {.data = alicePublicKey, .length = sizeof(alicePublicKey)};
    storageError =
        wosStorageWrite(pStorageContext, eccPubKeyStorageId, &publicKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
    data = {.data = testData, .length = sizeof(testData)};

    /* Sign */
    cryptoError = wosCryptoEccSign(&eccOptions, pStorageContext,
                                   eccPrivKeyStorageId, &data, &pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

    /* Sign 2 */
    cryptoError = wosCryptoEccSignKeyBuffer(&eccOptions, &privateKey, &data,
                                            &pSignature2);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

    /* We can make no assumptions on the signature because it includes
     * randomness. */

    /* Verify */
    cryptoError = wosCryptoEccVerify(&eccOptions, pStorageContext,
                                     eccPubKeyStorageId, &data, pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SIGNATURE_MATCH);
    cryptoError = wosCryptoEccVerify(&eccOptions, pStorageContext,
                                     eccPubKeyStorageId, &data, pSignature2);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SIGNATURE_MATCH);

    /* Verify 2 */
    cryptoError =
        wosCryptoEccVerifyKeyBuffer(&eccOptions, &publicKey, &data, pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SIGNATURE_MATCH);
    cryptoError = wosCryptoEccVerifyKeyBuffer(&eccOptions, &publicKey, &data,
                                              pSignature2);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SIGNATURE_MATCH);

    /* Clear */
    WOS_FREE_BUF_AND_DATA(pSignature);
    WOS_FREE_BUF_AND_DATA(pSignature2);
}

TEST_F(TestWosCrypto, NegativeEccSign)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    WosBuffer_t privateKey, publicKey, data;
    WosBuffer_t *pSignature = NULL, *pSignature2 = NULL;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    /* Store predefined keys */
    privateKey = {.data = alicePrivateKey, .length = sizeof(alicePrivateKey)};
    storageError =
        wosStorageWrite(pStorageContext, eccPrivKeyStorageId, &privateKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
    publicKey = {.data = alicePublicKey, .length = sizeof(alicePublicKey)};
    storageError =
        wosStorageWrite(pStorageContext, eccPubKeyStorageId, &publicKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);
    data = {.data = testData, .length = sizeof(testData)};

    /* Sign */
    // TODO Not yet implemented
    // cryptoError = wosCryptoEccSign(NULL, pStorageContext,
    //                                eccPrivKeyStorageId, &data, &pSignature);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    // TODO Storage using StdC-File allows NULL context
    // cryptoError = wosCryptoEccSign(&eccOptions, NULL,
    //                                eccPrivKeyStorageId, &data, &pSignature);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_STORAGE);

    cryptoError = wosCryptoEccSign(&eccOptions, pStorageContext, NULL, &data,
                                   &pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError = wosCryptoEccSign(&eccOptions, pStorageContext,
                                   eccPrivKeyStorageId, NULL, &pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    /* Sign 2 */
    // TODO Not yet implemented
    // cryptoError = wosCryptoEccSignKeyBuffer(NULL, &privateKey, &data,
    //                                         &pSignature2);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

    cryptoError =
        wosCryptoEccSignKeyBuffer(&eccOptions, NULL, &data, &pSignature2);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError =
        wosCryptoEccSignKeyBuffer(&eccOptions, &privateKey, NULL, &pSignature2);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    /* Verify */
    // TODO Not yet implemented
    // cryptoError = wosCryptoEccVerify(NULL, pStorageContext,
    //                                  eccPubKeyStorageId, &data, pSignature);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    // TODO Storage using StdC-File allows NULL context
    // cryptoError = wosCryptoEccVerify(&eccOptions, NULL,
    //                                  eccPubKeyStorageId, &data, pSignature);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_STORAGE);

    cryptoError = wosCryptoEccVerify(&eccOptions, pStorageContext, NULL, &data,
                                     pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError = wosCryptoEccVerify(&eccOptions, pStorageContext,
                                     eccPubKeyStorageId, NULL, pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError = wosCryptoEccVerify(&eccOptions, pStorageContext,
                                     eccPubKeyStorageId, &data, NULL);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    /* Verify 2 */
    // TODO Not yet implemented
    // cryptoError =
    //     wosCryptoEccVerifyKeyBuffer(NULL, &publicKey, &data, pSignature);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError =
        wosCryptoEccVerifyKeyBuffer(&eccOptions, NULL, &data, pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError =
        wosCryptoEccVerifyKeyBuffer(&eccOptions, &publicKey, NULL, pSignature);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError =
        wosCryptoEccVerifyKeyBuffer(&eccOptions, &publicKey, &data, NULL);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    /* Clear */
}

/* ========================================================================== */
/*                         wosCryptoEccReadPubKey                             */
/* ========================================================================== */

TEST_F(TestWosCrypto, TrivialEccReadPubKey)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    WosBuffer_t publicKey;
    WosBuffer_t *pExportedPubKey = NULL;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    /* Write */
    publicKey = {.data = alicePublicKey, .length = sizeof(alicePublicKey)};
    storageError =
        wosStorageWrite(pStorageContext, eccPubKeyStorageId, &publicKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);

    /* Read Public Key */
    cryptoError = wosCryptoEccReadPubKey(&eccOptions, pStorageContext,
                                         eccPubKeyStorageId, &pExportedPubKey);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);
    WOS_FREE_BUF_AND_DATA(pExportedPubKey);
}

TEST_F(TestWosCrypto, NegativeEccReadPubKey)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    WosBuffer_t *pExportedPubKey = NULL;

    // TODO Not yet implemented
    // cryptoError = wosCryptoEccReadPubKey(NULL, pStorageContext,
    //                                      eccPubKeyStorageId,
    //                                      &pExportedPubKey);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    // TODO Storage using StdC-File allows NULL context
    // cryptoError = wosCryptoEccReadPubKey(&eccOptions, NULL,
    //                                      eccPubKeyStorageId,
    //                                      &pExportedPubKey);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_STORAGE);

    cryptoError = wosCryptoEccReadPubKey(&eccOptions, pStorageContext, NULL,
                                         &pExportedPubKey);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
}

/* ========================================================================== */
/*                         wosCryptoDeriveSymKey                              */
/* ========================================================================== */

TEST_F(TestWosCrypto, TrivialEccDerive)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    WosBuffer_t devicePrivateKey, serverPublicKey;
    WosBuffer_t *pSharedSecret = NULL;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    int ret;

    /* Store predefined key */
    devicePrivateKey = {.data = alicePrivateKey,
                        .length = sizeof(alicePrivateKey)};
    storageError =
        wosStorageWrite(pStorageContext, eccPrivKeyStorageId, &devicePrivateKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);

    /* Prepare the other party's key */
    serverPublicKey = {.data = bobPublicKey, .length = sizeof(bobPublicKey)};

    /* Derive Key */
    cryptoError =
        wosCryptoDeriveSymKey(&eccOptions, pStorageContext, &serverPublicKey,
                              eccPrivKeyStorageId, symKeyStorageId);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

    /* Directly read symmetric key from storage and check against expected. */
    storageError =
        wosStorageRead(pStorageContext, symKeyStorageId, &pSharedSecret);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);

    /* Check shared secret */
    ret = wosMemComparison(pSharedSecret->data, sharedSecret,
                           sizeof(sharedSecret));
    EXPECT_EQ(ret, 0);

    /* Clear */
    WOS_FREE_BUF_AND_DATA(pSharedSecret);
}

TEST_F(TestWosCrypto, NegativeEccDerive)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    WosBuffer_t devicePrivateKey, serverPublicKey;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    /* Store predefined key */
    devicePrivateKey = {.data = alicePrivateKey,
                        .length = sizeof(alicePrivateKey)};
    storageError =
        wosStorageWrite(pStorageContext, eccPrivKeyStorageId, &devicePrivateKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);

    /* Prepare the other party's key */
    serverPublicKey = {.data = bobPublicKey, .length = sizeof(bobPublicKey)};

    // TODO Not yet implemented
    // cryptoError =
    //     wosCryptoDeriveSymKey(NULL, pStorageContext, &serverPublicKey,
    //                           eccPrivKeyStorageId, symKeyStorageId);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    // TODO Storage using StdC-File allows NULL context
    // cryptoError =
    //     wosCryptoDeriveSymKey(&eccOptions, NULL, &serverPublicKey,
    //                           eccPrivKeyStorageId, symKeyStorageId);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_STORAGE);

    cryptoError = wosCryptoDeriveSymKey(&eccOptions, pStorageContext, NULL,
                                        eccPrivKeyStorageId, symKeyStorageId);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError = wosCryptoDeriveSymKey(
        &eccOptions, pStorageContext, &serverPublicKey, NULL, symKeyStorageId);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    cryptoError =
        wosCryptoDeriveSymKey(&eccOptions, pStorageContext, &serverPublicKey,
                              eccPrivKeyStorageId, NULL);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    /* Clear */
}

/* ========================================================================== */
/*                         wosCryptoAeEncrypt                                 */
/*                         wosCryptoAeDecrypt                                 */
/* ========================================================================== */

TEST_F(TestWosCrypto, TrivialAe)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoAeOptions_t aeOptions;
    WosBuffer_t *pPlainText = NULL, *pCipherText = NULL, *pIv = NULL,
                *pTag = NULL;
    WosBuffer_t plainText, aad, cipherText, tag, iv;
    WosBuffer_t symKey;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    int ret;
    unsigned int i;

    for (i = 0; i < (int)(sizeof(aesGcmTests) / sizeof(aesGcmTests[0])); ++i) {
        /* Store predefined key */
        symKey = {.data = aesGcmTests[i].K, .length = aesGcmTests[i].keylen};
        storageError =
            wosStorageWrite(pStorageContext, symKeyStorageId, &symKey);
        EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);

        /***** Encrypt *****/
        plainText = {.data = aesGcmTests[i].P, .length = aesGcmTests[i].ptlen};
        aad = {.data = aesGcmTests[i].A, .length = aesGcmTests[i].alen};
        iv = {.data = aesGcmTests[i].IV, .length = aesGcmTests[i].IVlen};
        pIv = &iv;

        cryptoError =
            wosCryptoAeEncrypt(&aeOptions, pStorageContext, symKeyStorageId,
                               &plainText, &aad, &pIv, &pCipherText, &pTag);
        EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

        EXPECT_EQ(pCipherText->length, aesGcmTests[i].ptlen);
        if (pCipherText->length != 0) {
            /* fields of aesGcmTests[i] are never NULL */
            ret = wosMemComparison(pCipherText->data, aesGcmTests[i].C,
                                   pCipherText->length);
            EXPECT_EQ(ret, 0);
        }

        EXPECT_EQ(pTag->length, WOS_CRYPTO_AE_AES_BLOCK_LENGTH);
        ret = wosMemComparison(pTag->data, aesGcmTests[i].T, pTag->length);
        EXPECT_EQ(ret, 0);

        WOS_FREE_BUF_AND_DATA(pCipherText);
        WOS_FREE_BUF_AND_DATA(pTag);
        /* Keeping IV and AAD for next call */

        /***** Decrypt *****/
        cipherText = {.data = aesGcmTests[i].C, .length = aesGcmTests[i].ptlen};
        tag = {.data = aesGcmTests[i].T,
               .length = WOS_CRYPTO_AE_AES_BLOCK_LENGTH};

        cryptoError =
            wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                               &cipherText, &aad, pIv, &tag, &pPlainText);
        EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS);

        EXPECT_EQ(pPlainText->length, aesGcmTests[i].ptlen);
        if (pPlainText->length != 0) {
            /* fields of aesGcmTests[i] are never NULL */
            ret = wosMemComparison(pPlainText->data, aesGcmTests[i].P,
                                   pPlainText->length);
            EXPECT_EQ(ret, 0);
        }

        WOS_FREE_BUF_AND_DATA(pPlainText);
    }
}

TEST_F(TestWosCrypto, NegativeAe)
{
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoAeOptions_t aeOptions;
    WosBuffer_t *pPlainText = NULL, *pCipherText = NULL, *pIv = NULL,
                *pTag = NULL;
    WosBuffer_t plainText, aad, cipherText, tag, iv;
    WosBuffer_t symKey;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    int i = 4; /* The first "Normal" case, with both PT and AAD */

    /* Store predefined key */
    symKey = {.data = aesGcmTests[i].K, .length = aesGcmTests[i].keylen};
    storageError = wosStorageWrite(pStorageContext, symKeyStorageId, &symKey);
    EXPECT_EQ(storageError, WOS_STORAGE_SUCCESS);

    /***** Encrypt *****/
    plainText = {.data = aesGcmTests[i].P, .length = aesGcmTests[i].ptlen};
    aad = {.data = aesGcmTests[i].A, .length = aesGcmTests[i].alen};
    iv = {.data = aesGcmTests[i].IV, .length = aesGcmTests[i].IVlen};
    pIv = &iv;

    // TODO Not yet implemented
    // cryptoError =
    //     wosCryptoAeEncrypt(NULL, pStorageContext, symKeyStorageId,
    //                        &plainText, &aad, &pIv, &pCipherText, &pTag);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    // TODO Storage using StdC-File allows NULL context
    // cryptoError =
    //     wosCryptoAeEncrypt(&aeOptions, NULL, symKeyStorageId,
    //                        &plainText, &aad, &pIv, &pCipherText, &pTag);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_STORAGE);

    cryptoError =
        wosCryptoAeEncrypt(&aeOptions, pStorageContext, NULL, &plainText, &aad,
                           &pIv, &pCipherText, &pTag);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    /* PlainText */
    plainText = {.data = NULL, .length = aesGcmTests[i].ptlen};
    cryptoError =
        wosCryptoAeEncrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &plainText, &aad, &pIv, &pCipherText, &pTag);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    plainText = {.data = aesGcmTests[i].P, .length = 0};
    cryptoError =
        wosCryptoAeEncrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &plainText, &aad, &pIv, &pCipherText, &pTag);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS); // Valid, and must always work
    plainText = {.data = aesGcmTests[i].P, .length = aesGcmTests[i].ptlen};
    WOS_FREE_BUF_AND_DATA(pCipherText);
    WOS_FREE_BUF_AND_DATA(pTag);

    /* Aad */
    aad = {.data = NULL, .length = aesGcmTests[i].alen};
    cryptoError =
        wosCryptoAeEncrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &plainText, &aad, &pIv, &pCipherText, &pTag);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    aad = {.data = aesGcmTests[i].A, .length = 0};
    cryptoError =
        wosCryptoAeEncrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &plainText, &aad, &pIv, &pCipherText, &pTag);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_SUCCESS); // Valid, and must always work
    aad = {.data = aesGcmTests[i].A, .length = aesGcmTests[i].alen};
    WOS_FREE_BUF_AND_DATA(pCipherText);
    WOS_FREE_BUF_AND_DATA(pTag);

    /***** Decrypt *****/
    cipherText = {.data = aesGcmTests[i].C, .length = aesGcmTests[i].ptlen};
    tag = {.data = aesGcmTests[i].T, .length = WOS_CRYPTO_AE_AES_BLOCK_LENGTH};

    // TODO Not yet implemented
    // cryptoError =
    //     wosCryptoAeDecrypt(NULL, pStorageContext, symKeyStorageId,
    //                        &cipherText, &aad, &iv, &tag, &pPlainText);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    // TODO Storage using StdC-File allows NULL context
    // cryptoError =
    //     wosCryptoAeDecrypt(&aeOptions, NULL, symKeyStorageId,
    //                        &cipherText, &aad, &iv, &tag, &pPlainText);
    // EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_STORAGE);

    cryptoError = wosCryptoAeDecrypt(&aeOptions, pStorageContext, NULL,
                                     &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);

    /* Cipher Text */
    cipherText = {.data = NULL, .length = aesGcmTests[i].ptlen};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    cipherText = {.data = aesGcmTests[i].C, .length = 0};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR); // Invalid combination of values
    cipherText = {.data = aesGcmTests[i].C, .length = aesGcmTests[i].ptlen};

    /* Aad */
    aad = {.data = NULL, .length = aesGcmTests[i].alen};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    aad = {.data = aesGcmTests[i].A, .length = 0};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR); // Invalid combination of values
    aad = {.data = aesGcmTests[i].A, .length = aesGcmTests[i].alen};

    /* pIv */
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, NULL, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    iv = {.data = NULL, .length = aesGcmTests[i].IVlen};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    iv = {.data = aesGcmTests[i].IV, .length = 0};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    iv = {.data = aesGcmTests[i].IV, .length = aesGcmTests[i].IVlen};

    /* Tag */
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, NULL, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    tag = {.data = NULL, .length = WOS_CRYPTO_AE_AES_BLOCK_LENGTH};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    tag = {.data = aesGcmTests[i].T, .length = 0};
    cryptoError =
        wosCryptoAeDecrypt(&aeOptions, pStorageContext, symKeyStorageId,
                           &cipherText, &aad, &iv, &tag, &pPlainText);
    EXPECT_EQ(cryptoError, WOS_CRYPTO_ERROR_BAD_PARAMS);
    tag = {.data = aesGcmTests[i].T, .length = WOS_CRYPTO_AE_AES_BLOCK_LENGTH};
}

} // namespace
