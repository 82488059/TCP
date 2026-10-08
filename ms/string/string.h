#pragma once


#include <string>

namespace ms {
#ifdef _UNICODE
    typedef std::wstring mstring;
#else 
    typedef std::string mstring;
#endif
};

