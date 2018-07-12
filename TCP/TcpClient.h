#pragma once
class CTcpClient
{
public:
    CTcpClient();
    ~CTcpClient();
public:
    int run();
    int start();
    int stop();
private:
    int status{ 0 };
};

