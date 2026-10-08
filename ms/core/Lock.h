#pragma once
namespace ms {

class CLock
{
public:
    CLock()
    {
        InitializeCriticalSection(&m_cs);
    }

    virtual ~CLock()
    {
        DeleteCriticalSection(&m_cs);
    }
    void Lock()
    {
        EnterCriticalSection(&m_cs);
    }

    void Unlock()
    {
        LeaveCriticalSection(&m_cs);
    }

private:
    CRITICAL_SECTION m_cs;
};

};// ms