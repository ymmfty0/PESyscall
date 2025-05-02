//
// Created by ymfyt on 02.05.2025.
//

#include "encryption.h"

namespace encryption
{
    void xor_crypt(unsigned char* data, unsigned long long data_len,
                  const unsigned char* key, unsigned long long key_len) {

        if (key_len == 0 || data_len == 0) {
            return;
        }

        for (unsigned long long i = 0; i < data_len; ++i) {
            data[i] ^= key[i % key_len];
        }
    }

}
