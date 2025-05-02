#include "pe_syscall.h"

#include <iostream>

#include "file_reader.h"
#include "HexDumper.h"

auto pe_syscall::m_get_proc_ssn( hash_type func_name ) -> DWORD
{
    const auto pe_file_buffer = file_reader( R"(C:\Windows\System32\ntdll.dll)" );
    if ( pe_file_buffer == nullptr )
    {
        return -3;
    }
    const auto func_addr = manual_resolver::m_GetProcAddress( pe_file_buffer, func_name, true );
    if ( !func_addr )
    {
        return -1;
    }
    const auto ssn = *reinterpret_cast<DWORD*>( static_cast<PBYTE>( func_addr ) + 4 );
    if ( ssn == 0 )
    {
        return -2;
    }
    return ssn;
}

auto pe_syscall::m_get_proc_address( hash_type func_name ) -> PVOID
{
    auto module_handle = manual_resolver::m_GetModuleHandle( "ntdll.dll"_djb2 );
    auto func_addr = manual_resolver::m_GetProcAddress( module_handle, func_name );
    if ( func_addr == nullptr )
    {
        return nullptr;
    }
    return func_addr;
}

auto pe_syscall::api_resolver( ) -> PVOID
{
    auto address = m_get_proc_address( func_name_ );
    if ( address == nullptr )
    {
        return nullptr;
    }
    if ( !hook_detector( address ))
    {
        return address;
    }

    std::cout << "!!!!!!!! Func hooked !!!!!!!!" << std::endl;

    utils::hex_dump( address , 16);
    std::cout << std::endl;

    auto ssn = m_get_proc_ssn( func_name_ );

    std::cout << "Func " << func_name_ << " Ssn " << ssn << std::endl;
    HellsGate( ssn );

    address = reinterpret_cast<void*>( HellDescent_stub );
    return address;
}

auto pe_syscall::hook_detector( PVOID func_addr ) -> bool
{
    if ( !func_addr )
    {
        return false;
    }

    if ( *static_cast<PBYTE>( func_addr ) == 0xe9 || *static_cast<PBYTE>( func_addr ) + 3 == 0xe9 )
    {
        return true;
    }

    return false;
}

auto pe_syscall::VxMoveMemory( PVOID dest, const PVOID src, SIZE_T len ) -> PVOID
{
    unsigned char *d = static_cast<unsigned char*>( dest );
    const unsigned char *s = static_cast<const unsigned char*>( src );

    PVOID original_dest = dest;

    if ( len == 0 )
    {
        return original_dest;
    }

    if ( d < s )
    {
            while ( len-- > 0 )
            *d++ = *s++;
    }
    else
    {
        const unsigned char *lasts = s + ( len - 1 );
        unsigned char *lastd = d + ( len - 1 );
        while ( len-- > 0 )
            *lastd-- = *lasts--;
    }

    return original_dest;
}

pe_syscall::pe_syscall( hash_type func_name )
    : func_name_( func_name )
{
    address = api_resolver( );
}
