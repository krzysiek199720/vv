#include "main.h"


bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
bool hasSupportedExtension(std::string const & value)
{
    for(uint32 i = 0; i < SFECount; ++i)
    {
        if(ends_with(value, supportedFileExtensions[i]))
            return true;
    }
    return false;
}