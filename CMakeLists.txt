cmake_minimum_required(VERSION 3.30)

project(PESyscall LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

add_executable(PESyscall main.cpp
        dbj2_hasher.h
        manual_resolver.cpp
        manual_resolver.h
        PESturctures.h
        HexDumper.cpp
        HexDumper.h
        pe_syscall.cpp
        pe_syscall.h
        hellsgate.s
        FuncStructs.h
        encryption.cpp
        encryption.h
)


if(MINGW)
    message(STATUS "MinGW detected. Adding specific compile and link flags for PESyscall.")

    target_compile_options(PESyscall PRIVATE
            -ffunction-sections
            -fdata-sections
            -Wno-write-strings
            -fmerge-all-constants
    )

    target_link_options(PESyscall PRIVATE
            -static-libgcc
            -static-libstdc++
            -s
            -Wl,--gc-sections
    )
else()
    message(WARNING "Not using MinGW. Specific flags for MinGW are not applied to PESyscall.")
endif()
