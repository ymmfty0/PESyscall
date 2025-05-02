//
// Created by ymfyt on 02.05.2025.
//

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <cstddef>

namespace encryption
{
    void xor_crypt( unsigned char *data, unsigned long long data_len,
                    const unsigned char *key, unsigned long long key_len );
}


#endif //ENCRYPTION_H
