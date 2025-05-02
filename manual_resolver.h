//
// Created by ymfyt on 19.04.2025.
//

#ifndef MANUAL_RESOLVER_H
#define MANUAL_RESOLVER_H


#include <Windows.h>
#include "dbj2_hasher.h"
#include "PESturctures.h"

using hashing::hash_type;

namespace manual_resolver
{
    static auto RvaToOffset(PIMAGE_NT_HEADERS NtHeaders,DWORD rva ) -> DWORD;
    auto m_GetModuleHandle( const hash_type module_name ) -> HMODULE;
    auto m_GetProcAddress( PVOID h_module, const hash_type func_name , bool local = false) -> PVOID;
    inline auto get_peb( ) -> PPEB;
}

#endif //MANUAL_RESOLVER_H
