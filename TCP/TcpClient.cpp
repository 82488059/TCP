#include "stdafx.h"
#include "TcpClient.h"


CTcpClient::CTcpClient()
{
}


CTcpClient::~CTcpClient()
{
}

int CTcpClient::run()
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

int CTcpClient::start()
{

    return 0;
}
int CTcpClient::stop()
{

    return 0;
}