#ifndef TCPCONNECT_H
#define TCPCONNECT_H

#include<QTcpSocket>

#define SERVER_ADDR "192.168.43.201" //这是我的服务器地址，需要修改为你启动服务器的地址
#define SERVER_PORT 6000


class TcpConnect
{
public:
    bool network; //是否能正常连接主机
    QTcpSocket * tcpClient = NULL;//连接网络
    QString serverIp;
    qint16 serverPort;
    TcpConnect();
    static TcpConnect* instance();//获取单例对象的接口函数
};

#endif // TCPCONNECT_H
