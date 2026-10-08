#pragma once
#include <WinSock2.h>
namespace ms {

    class CCompletionPort
    {
    public:
        // 创建
        static CCompletionPort* Create(int nProcessors)
        {
            CCompletionPort* pCompPort = new CCompletionPort;
            // 要创建不关联的I/O完成端口，请将FileHandle参数设置为INVALID_HANDLE_VALUE，
            // 将ExistingCompletionPort参数设置为NULL，
            // 并将CompletionKey参数设置为零（在这种情况下将被忽略）。
            // 对于新的I/O完成端口，将NumberOfConcurrentThreads参数设置为所需的并发值，
            // 对于默认值（系统中的处理器数量），请将其设置为零。
            pCompPort->m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE
                , NULL, 0, nProcessors);
            if (NULL == pCompPort->m_hIOCompletionPort)
            {
                delete pCompPort;
                pCompPort = NULL;
            }
            return pCompPort;
        }
        // 绑定
        // 将现有的I/O完成端口与文件句柄相关联。
        BOOL Bind(HANDLE FileHandle, ULONG_PTR CompletionKey)
        {
            BOOL bResult = FALSE;
            // ExistingCompletionPort
            // 现有I/O完成端口 如果此参数指定了现有的I/O完成端口，则该函数将其与FileHandle参数指定的句柄相关联。
            // 如果成功，该函数将返回现有I/O完成端口的句柄；它不会创建新的I/O完成端口。
            // CompletionKey[输入]
            // 每个句柄用户定义的完成密钥，包含在指定文件句柄的每个I/O完成数据包中。
            // NumberOfConcurrentThreads [输入]
            // 操作系统可以允许同时处理I/O完成端口的I/O完成数据包的最大线程数。
            // 如果ExistingCompletionPort参数不为NULL，则忽略此参数。
            HANDLE hcp = CreateIoCompletionPort(FileHandle
                , m_hIOCompletionPort, CompletionKey, 0);
            if (hcp == m_hIOCompletionPort)
            {
                bResult = TRUE;
            }
            return bResult;
        }

        BOOL GetQueuedStatus(DWORD& lpNumberOfBytesTransferred, ULONG_PTR& lpCompletionKey
            , LPOVERLAPPED* lpOverlapped, DWORD dwMilliseconds = INFINITE)
        {
            BOOL bResult = GetQueuedCompletionStatus(m_hIOCompletionPort
                , &lpNumberOfBytesTransferred, &lpCompletionKey
                , lpOverlapped, dwMilliseconds);
            return bResult;
        }
        BOOL PostQueuedStatus(DWORD dwNumberOfBytesTransferred,
            ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped)
        {
            BOOL bResult = PostQueuedCompletionStatus(m_hIOCompletionPort
                , dwNumberOfBytesTransferred, dwCompletionKey, lpOverlapped);
            return bResult;
        }

        virtual ~CCompletionPort() {}

    private:
        CCompletionPort() {}

    private:
        HANDLE m_hIOCompletionPort{ NULL };
    };// CCompletionPort

};// ms
