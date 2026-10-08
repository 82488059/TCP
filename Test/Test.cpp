// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <winsock2.h>

#include "tcp/TcpServer.h"


#pragma comment(lib,"ws2_32.lib")

ms::CTcpServer tcp;

int AcceptCall(SOCKET s, ms::CPeerIoData* io)
{
    std::cout << "client connect socket is: " << s << std::endl;
    tcp.RegRecv(s);
    return 0;
}
int RecvCall(SOCKET s, ms::CPeerIoData* io)
{
    std::cout << "socket: " << s << " recv data: " << io->m_buffer.GetSize() << std::endl;
    tcp.PostSend(s, io->m_buffer.Data(), io->m_buffer.GetSize());
    return 0;
}
 
int SendCom(SOCKET s, ms::CPeerIoData* io)
{
    std::cout << "socket: " << s << " send: " << io->m_buffer.GetSize() << "complete." << std::endl;
    return 0;
}

int ShoutdownCall(SOCKET s, ms::CPeerIoData* io)
{
    std::cout << "socket close: " << s << std::endl;
    return 0;
}


int main()
{
    int nResult = 0;
    WSADATA wsaData;
    nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (RESULT_OK != tcp.Init("192.168.1.99", 6688))
    {
        return -1;
    }

    tcp.SetCallback(AcceptCall, RecvCall, nullptr, SendCom, ShoutdownCall);
    std::cout << "Start Tcp Server." << nResult << std::endl;
    nResult = tcp.Start();
    std::cout << "Start status is: " << nResult << ", 0 is OK!" << std::endl;
    
    system("pause");
    
    tcp.Stop();
    
    return 0;
}

