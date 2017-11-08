/******************************************************************************
   BigWorld Technology
   Copyright BigWorld Pty, Ltd.
   All Rights Reserved. Commercial in confidence.

   WARNING: This computer program is protected by copyright law and international
   treaties. Unauthorized use, reproduction or distribution of this program, or
   any portion of this program, may result in the imposition of civil and
   criminal penalties as provided by law.
******************************************************************************/

#include "unique_id.h"

#if defined(_WIN32)
    #include <objbase.h>
#endif

#include <Base/BaseTypes.h>

UniqueID UniqueID::s_zero_(0, 0, 0, 0);

UniqueID::UniqueID()
    : a_(0)
    , b_(0)
    , c_(0)
    , d_(0)
{
}

UniqueID::UniqueID(const std::string& s)
    : a_(0)
    , b_(0)
    , c_(0)
    , d_(0)
{
    if (!s.empty())
    {
        DAVA::uint32 data[4];
        if (fromString(s, &data[0]))
        {
            a_ = data[0];
            b_ = data[1];
            c_ = data[2];
            d_ = data[3];
        }
    }
}

UniqueID::operator std::string() const
{
    char buf[80];
    Snprintf(buf, sizeof(buf), "%08X.%08X.%08X.%08X", a_, b_, c_, d_);
    return std::string(buf);
}

bool UniqueID::operator==(const UniqueID& rhs) const
{
    return (a_ == rhs.a_) && (b_ == rhs.b_) && (c_ == rhs.c_) && (d_ == rhs.d_);
}

bool UniqueID::operator!=(const UniqueID& rhs) const
{
    return !(*this == rhs);
}

bool UniqueID::operator<(const UniqueID& rhs) const
{
    if (a_ < rhs.a_)
    {
        return true;
    }
    else if (a_ > rhs.a_)
    {
        return false;
    }

    if (b_ < rhs.b_)
    {
        return true;
    }
    else if (b_ > rhs.b_)
    {
        return false;
    }

    if (c_ < rhs.c_)
    {
        return true;
    }
    else if (c_ > rhs.c_)
    {
        return false;
    }

    if (d_ < rhs.d_)
    {
        return true;
    }
    else if (d_ > rhs.d_)
    {
        return false;
    }

    return false;
}

#if defined(_WIN32)
UniqueID UniqueID::generate()
{
    UniqueID n;
    CoCreateGuid(reinterpret_cast<GUID*>(&n));
    return n;
}

#endif

bool UniqueID::isUniqueID(const std::string& s)
{
    DAVA::uint32 data[4];
    return fromString(s, &data[0]);
}

bool UniqueID::fromString(const std::string& s, DAVA::uint32* data)
{
    if (s.empty())
    {
        return false;
    }

    std::string copyS(s);
    char* str = const_cast<char*>(copyS.c_str());
    for (int offset = 0; offset < 4; offset++)
    {
        char* initstr = str;
        data[offset] = strtoul(initstr, &str, 16);

        // strtoul will make these the same if it didn't read anything
        if (initstr == str)
        {
            return false;
        }

        str++;
    }

    return true;
}

// unique_id.cpp
