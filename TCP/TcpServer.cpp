#include "stdafx.h"
#include "TcpServer.h"


CTcpServer::CTcpServer()
{
}


CTcpServer::~CTcpServer()
{
}


int CTcpServer::run()
{
    switch (status)
    {
    case 0:
        start();
        break;

    case 1:
        stop();
        break;

    case 2:
        stop();
        break;

    default:
        stop();
        break;
    }
    return 0;
}

int CTcpServer::start()
{

    return 0;
}

int CTcpServer::stop()
{

    return 0;
}