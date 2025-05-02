#ifndef PE_SYSCALL_H
#define PE_SYSCALL_H

#include "manual_resolver.h"

using hashing::hash_type;

class pe_syscall
{
public:
    explicit pe_syscall(hash_type func_name);
    static auto VxMoveMemory(PVOID dest, const PVOID src, SIZE_T len)-> PVOID;

    operator void*()  const noexcept
    {
        return address;
    }

private:

    hash_type func_name_;

    // -- Resolve api
    auto api_resolver() -> PVOID;

    // -- Get address or ssn
    static auto m_get_proc_ssn( hash_type func_name ) -> DWORD;
    static auto m_get_proc_address( hash_type func_name ) -> PVOID;

    // -- Check hoos via JMP
    static auto hook_detector(PVOID func_addr) -> bool;


private:
    void* address;
};

// --HellGate ( TartrausGate Impl )
extern "C" VOID HellsGate( WORD wSystemCall );
extern "C" VOID HellDescent_stub( );

#endif //PE_SYSCALL_H
