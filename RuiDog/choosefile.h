#ifndef CHOOSEFILE_H
#define CHOOSEFILE_H

#include <QWidget>
#include <QTcpSocket>
#include <QMouseEvent>
#include<QTextCodec>

#include "filemeta.h"
#include "tcpconnect.h"
#include "public.hpp"
#include "json.hpp"
#include "information.h"

using namespace std;
using json=nlohmann::json;

namespace Ui {
class ChooseFile;
}

class ChooseFile : public QWidget
{
    Q_OBJECT

public:
    explicit ChooseFile(QWidget *parent = nullptr);
    ~ChooseFile();
//    void setFriendInfo(User * Friend);
    bool requestSendFile(QString imgAbsolutePath);
    bool isExistFileByAbsolutePath(QString path);
    void uploadFile(FileMeta filemeta);
    void setFriendInfo(int userId,string userName,int friendId);
    void setGroupInfo(int userId,string userName,int groupId);

private slots:
    void on_chooseFile_clicked();
    void on_sendFile_clicked();
    void on_cancelButton_clicked();
    void readSocketData();//接收服务器数据
    void disconnectSocket();//客户端断开连接

signals:
    void readyShowMyFile(QString path);

protected:
    void mousePressEvent(QMouseEvent *event); //鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);  //鼠标移动事件

private:
    Ui::ChooseFile *ui;
    QTcpSocket *tcpSocket;
    QPoint m_point;
    int friendId;
    int userId;
    int groupId;
    string userName;
    string imgName;
    EChatType chatType;//聊天类型
//    User* friendInfo;
};

#endif // CHOOSEFILE_H
