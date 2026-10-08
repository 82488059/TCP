#pragma once
#include <memory>

namespace ms {

    class CBuffer
    {
    public:
        CBuffer() {}
        virtual ~CBuffer()
        {
            if (m_buffer)
            {
                delete[] m_buffer;
                m_buffer = NULL;
            }
        }
        void ClearData()
        {
            memset(m_buffer, 0, m_nMaxSize);
        }
        char* Data()
        {
            return m_buffer;
        }

        int Write(const char* buf, long size)
        {
            if (size > m_nMaxSize)
            {
                Resize(size);
            }
            memcpy(m_buffer, buf, size);
            return size;
        }
        void Resize(long size)
        {
            if (size >= m_nMaxSize)
            {
                if (m_buffer)
                {
                    delete[] m_buffer;
                }
                m_buffer = new char[size];
                m_nMaxSize = size;
            }
            memset(m_buffer, 0, m_nMaxSize);
        }
        void SetSize(long nSize)
        {
            m_nSize = nSize;
        }

        long GetSize()
        {
            return m_nSize;
        }
        void SetData(const char* pData, long nSize)
        {
            //if (m_nMaxSize < nSize)
            //{
            //    Resize(nSize);
            //}
            memcpy(m_buffer, pData, nSize);
        }

    private:
        char* m_buffer{ NULL };
        long m_nMaxSize{ 0 };
        long m_pSize{ 0 };
        long m_nSize{ 0 };

        CBuffer(const CBuffer&) = delete;
        CBuffer operator=(const CBuffer&) = delete;
    };

    typedef std::shared_ptr<CBuffer> spBuffer;
};// ms