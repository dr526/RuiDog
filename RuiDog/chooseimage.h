#ifndef CHOOSEIMAGE_H
#define CHOOSEIMAGE_H

#include <QWidget>
#include <synchapi.h>
#include<QTextCodec>

#include "information.h"
#include "filemeta.h"
#include "tcpconnect.h"
#include "public.hpp"
#include "json.hpp"

using namespace std;
using json=nlohmann::json;


namespace Ui {
class ChooseImage;
}

class ChooseImage : public QWidget
{
    Q_OBJECT

public:
    explicit ChooseImage(QWidget *parent = nullptr);
    ~ChooseImage();
    void setFriendInfo(int userId,string userName,int friendId);//设置待发送好友信息
    void setGroupInfo(int userId,string userName,int groupId);//设置待发送群组信息
    bool requestSendImg(QString imgAbsolutePath);//先根据路径判断图片是否存在，存在则生成格式化文件名并调用uploadImg
    bool isExistFileByAbsolutePath(QString path);//判断形参路径对应的图片是否存在
    void uploadImg(FileMeta filemeta);//上传文件
private slots:
    void on_chooseImg_clicked();//点击选择图片按钮
    void on_cancelButton_clicked();//点击取消按钮
    void on_sendImg_clicked();//点击发送图片按钮
    void readSocketData();//接收服务器数据
    void disconnectSocket();//客户端断开连接
signals:
    void readyShowMyIMG(QString path);//该信号是在图片发送完毕后，通知主界面在聊天窗口显示该图片
private:
    Ui::ChooseImage *ui;
    int friendId;//好友账号
    int userId;//本用户账号
    int groupId;//群组账号
    string userName;//本用户名
    string imgName;//图片名
    EChatType chatType;//聊天类型
    QTcpSocket * tcpSocket;//用于图片发送
};

#endif // CHOOSEIMAGE_H
