#include "HexDumper.h"

void utils::hex_dump( const void *data, size_t size )
{
    const auto ptr = static_cast<const unsigned char*>( data );
    constexpr unsigned int bytes_per_line = 16;

    for ( size_t j = 0; j < size; j += bytes_per_line )
    {
        std::cout << std::setw( 4 ) << std::setfill( '0' ) << std::hex << j << " ";

        std::cout << ": ";

        for ( size_t i = 0; i < bytes_per_line; ++i )
        {
            if ( j + i < size )
                std::cout << std::setw( 2 )
                        << std::setfill( '0' )
                        << std::hex
                        << std::uppercase
                        << static_cast<int>( ptr[j + i] ) << " ";
            else
                std::cout << "   ";
        }
        std::cout << "| ";

        for (size_t i = 0; i < bytes_per_line; ++i)
        {
            if (j + i < size)
            {
                unsigned char c = ptr[j + i];
                if (std::isprint(c))
                    std::cout << c;
                else
                    std::cout << '.';
            }
        }
    }
}
