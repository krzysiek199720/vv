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

int64 Vector2::compare(Vector2 v1, Vector2 v2)
{
    if(v1.x == v2.x && v1.y == v2.y)
        return 0;

    int64 v1P2 = v1.x * v1.x + v1.y * v1.y;
    int64 v2P2 = v2.x * v2.x + v2.y * v2.y;

    return v1P2 - v2P2;
}