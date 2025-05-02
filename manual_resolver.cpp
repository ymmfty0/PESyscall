#include "manual_resolver.h"

#include <algorithm>
#include <iostream>
#include <ostream>

namespace manual_resolver
{
    static auto RvaToOffset(PIMAGE_NT_HEADERS NtHeaders, DWORD rva) -> DWORD {

        if (rva == 0) {
            return 0;
        }

        PIMAGE_SECTION_HEADER SectionHeader = IMAGE_FIRST_SECTION(NtHeaders);
        for (size_t i = 0; i < NtHeaders->FileHeader.NumberOfSections; ++i) {
            DWORD SectionStart = SectionHeader->VirtualAddress;
            DWORD SectionVirtualSize = SectionHeader->Misc.VirtualSize;
            if (SectionVirtualSize == 0) {
                SectionVirtualSize = SectionHeader->SizeOfRawData;
            }
            DWORD SectionEnd = SectionStart + SectionVirtualSize;

            if (rva >= SectionStart && rva < SectionEnd) {
                DWORD OffsetInSection = rva - SectionStart;

                if (OffsetInSection < SectionHeader->SizeOfRawData) {
                    return SectionHeader->PointerToRawData + OffsetInSection;
                } else {
                    return 0;
                }
            }

            SectionHeader++;
        }

        return 0;
    }

    auto m_GetModuleHandle( const hash_type module_name ) -> HMODULE
    {
        if ( !module_name )
        {
            return nullptr;
        }

        auto peb = get_peb( );
        HMODULE module = nullptr;

        if ( !peb || !peb->LoaderData )
        {
            return nullptr;
        }

        PPEB_LDR_DATA ldr_data = peb->LoaderData;
        PLIST_ENTRY list_head = &ldr_data->InMemoryOrderModuleList;

        PLIST_ENTRY current_link = list_head->Flink;
        do
        {
            const auto entry = CONTAINING_RECORD(
                current_link,
                LDR_DATA_TABLE_ENTRY,
                InMemoryOrderLinks
            );
            if ( entry->BaseDllName.Buffer != nullptr )
            {
                auto temp = std::wstring{ entry->BaseDllName.Buffer };
                std::transform( temp.begin( ), temp.end( ), temp.begin( ), ::tolower );

                auto hash_w = hashing::djb2::hash( temp.c_str( ) );
                if ( module_name == hash_w )
                {
                    return static_cast<HMODULE>( entry->DllBase );
                }
            }

            current_link = current_link->Flink;
        }
        while ( current_link != list_head );
        return module;
    }


    inline PVOID CalculateAddress( PVOID base_address, DWORD rva, PIMAGE_NT_HEADERS nt_header, bool local )
    {
        DWORD offset = local ? RvaToOffset( nt_header, rva ) : rva;
        return static_cast<PBYTE>( base_address ) + offset;
    }

    auto m_GetProcAddress( PVOID h_module, const hash_type func_name, bool local ) -> PVOID
    {
        const auto base_address = h_module;
        if ( !base_address )
        {
            return nullptr;
        }

        const auto dos_header = static_cast<PIMAGE_DOS_HEADER>( base_address );;
        if ( dos_header->e_magic != IMAGE_DOS_SIGNATURE )
        {
            std::cerr << "e_magic error" << std::endl;
            return nullptr;
        }

        const auto nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(
            static_cast<PBYTE>( base_address ) + dos_header->e_lfanew );;

        if ( nt_header->Signature != IMAGE_NT_SIGNATURE )
        {
            std::cerr << "nt signature error" << std::endl;
            return nullptr;
        }

        if ( nt_header->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_EXPORT ||
             nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
             nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0 )
        {
            return nullptr;
        }

        const auto export_data_directory = nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

        const auto export_directory = static_cast<PIMAGE_EXPORT_DIRECTORY>(
            CalculateAddress( base_address, export_data_directory.VirtualAddress, nt_header, local ) );
        if ( !export_directory ) { return nullptr; }

        const auto address_function = static_cast<PDWORD>( CalculateAddress(
            base_address, export_directory->AddressOfFunctions, nt_header, local ) );
        const auto address_of_names = static_cast<PDWORD>( CalculateAddress(
            base_address, export_directory->AddressOfNames, nt_header, local ) );
        const auto address_of_ordinals = static_cast<PWORD>( CalculateAddress(
            base_address, export_directory->AddressOfNameOrdinals, nt_header, local ) );

        if ( !address_function || !address_of_names || !address_of_ordinals )
        {
            return nullptr;
        }

        const DWORD NumberOfNames = export_directory->NumberOfNames;

        PVOID func_addr = nullptr;

        for ( DWORD i = 0; i < NumberOfNames; ++i )
        {
            DWORD nameRva = address_of_names[i];
            if ( nameRva == 0 )
            {
                continue;
            }

            const char *func_name_in_dll = static_cast<const char*>(
                CalculateAddress( base_address, nameRva, nt_header, local ) );

            if ( !func_name_in_dll )
                continue;

            if ( func_name == hashing::djb2::hash( func_name_in_dll ) )
            {
                {
                    WORD ordinal = address_of_ordinals[i];

                    if ( ordinal >= export_directory->NumberOfFunctions )
                    {
                        continue;
                    }

                    DWORD function_rva = address_function[ordinal];
                    if ( function_rva == 0 )
                    {
                        continue;
                    }

                    DWORD exportStart = export_data_directory.VirtualAddress;
                    DWORD exportEnd = exportStart + export_data_directory.Size;
                    if ( function_rva >= exportStart && function_rva < exportEnd )
                    {
                        continue;
                    }

                    func_addr = CalculateAddress( base_address, function_rva, nt_header, local );
                    break;
                }
            }
        }
        return func_addr;
    }

    auto get_peb( ) -> PPEB
    {
#ifdef _WIN64
        return reinterpret_cast<PEB*>( __readgsqword( 0x60 ) );
#else
        return reinterpret_cast<PEB*>(__readfsdword(0x30));
#endif
    }
}