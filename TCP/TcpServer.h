#pragma once
class CTcpServer
{
public:
    CTcpServer();
    virtual ~CTcpServer();


public:
    int run();
    int start();
    int stop();

private:

    int status{ 0 };
};

