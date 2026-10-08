#pragma once
#include <memory>
#include <queue>
#include <mutex>

namespace ms {

template <class T>
class ObjectPool
{
public:
    ObjectPool(int maxObj = 10240):m_max(maxObj) {}
    virtual ~ObjectPool() 
    {
        while(!m_pool.empty())
            m_pool.pop(); 
    }

    std::shared_ptr<T> Take()
    {
        std::lock_guard<std::mutex> lg(m_mtx);
        if (m_pool.empty())
        {
            auto pop = std::make_shared<T>();
            return pop;
        }
        else
        {
            auto pop = m_pool.front();
            m_pool.pop();
            --m_size;
            return pop;
        }
    }

    int Size() const
    {
        return m_size;
    }

    void Put(std::shared_ptr<T> object)
    {
        std::lock_guard<std::mutex> lg(m_mtx);
        if (m_size < m_max)
        {
            m_pool.push(object);
            ++m_size;
        }
    }
private:
    int m_max{ 10240 };
    int m_size{ 0 };
    // lock
    std::mutex m_mtx;
    // pool
    std::queue<std::shared_ptr<T>> m_pool;
};// ObjectPool<T>


};// ms