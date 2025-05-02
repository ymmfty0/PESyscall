//
// Created by ymfyt on 01.05.2025.
//

#ifndef FILE_READER_H
#define FILE_READER_H

#include <fstream>

struct file_reader
{
    explicit file_reader( const char *file_name )
    {
        std::ifstream input_file( file_name, std::ios::binary );

        input_file.seekg( 0, std::ios::end );
        auto length = input_file.tellg( );
        input_file.seekg( 0, std::ios::beg );

        if ( length <= 0 )
        {
            throw std::runtime_error( "File is empty" );
        }

        file_addr = malloc( length );
        if ( !file_addr )
        {
            throw std::bad_alloc( );
        }

        if ( !input_file.read( static_cast<char*>( file_addr ), length ) )
        {
            free( file_addr );
            file_addr = nullptr;
            throw std::runtime_error( "Cannot read file" );
        }
    };

    file_reader(const file_reader&) = delete;
    file_reader& operator=(const file_reader&) = delete;

    file_reader( file_reader&& other ) noexcept
        : file_addr( other.file_addr )
    {
        other.file_addr = nullptr;
    }

    file_reader& operator=( file_reader&& other ) noexcept
    {
        if ( file_addr )
        {
            free( file_addr );
            file_addr = other.file_addr;
            other.file_addr = nullptr;
        }
        return *this;
    }

    operator void*( ) const noexcept
    {
        return file_addr;
    }

    ~file_reader( ) noexcept
    {
        free( file_addr );
    }

private:
    void *file_addr = nullptr;
};


#endif //FILE_READER_H
