#include "tcpconnect.h"

TcpConnect::TcpConnect()
{
    this->serverIp=QString(SERVER_ADDR);
    this->serverPort=qint16(SERVER_PORT);
}

TcpConnect *TcpConnect::instance(){
    static TcpConnect tcpConnect;
    return &tcpConnect;
}
