#pragma once
#include <map>


#include "Lock.h"

namespace ms {

    template<class T>
    class CLockMap
    {
    public:
        CLockMap() {}
        virtual ~CLockMap() { m_map.clear(); }

        CLock& operator[](T i)
        {
            return m_map[i];
        }
    private:
        std::map<T, CLock> m_map;
    };


};// ms