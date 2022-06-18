#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QDir>
#include <QDesktopWidget>
#include <QProcess>

#include "addfrienddialog.h"
#include "addfriendalert.h"
#include "addgroupalert.h"
#include "chooseimage.h"
#include "choosefile.h"
#include "itemwidget.h"
#include "tcpconnect.h"
#include "logindialog.h"
#include "information.h"
#include "json.hpp"
#include "voice.h"
#include "voicealert.h"
#include "updateinfo.h"

using namespace std;
using json=nlohmann::json;
#define MAX_SEND_LENGTH 6144//聊天框可发送的最大文本数据
typedef map<string,QListWidget*> mapChatWidget;

struct chatWidgetInfo{
    int m_account;  //聊天窗口对应的聊天号
    EChatType m_type;     //聊天窗口对应的类型 群聊/私聊
};
typedef list<chatWidgetInfo*> listChatWidgetInfo;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool getLoginStatus(){//返回用户登录状态
        return loginStatus;
    }
    bool isPressedWindow;//判断鼠标是否按在主界面
    QPoint last;//记录当前鼠标的位置
    void mousePressEvent(QMouseEvent *event);//鼠标点击
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动时，窗口跟随移动
    void mouseReleaseEvent(QMouseEvent *event);//鼠标释放，窗口跟随移动
    void Init();//连接服务器，显示登录界面
    void InitUI();//初始化界面样式
    int Login();//处理登录事务
    void SendMessage(QByteArray request);//向服务器发送数据
    void getFriendInfo(json responsejs);//将好友信息存储到g_currentUserFriendList中
    void Init_Friend_Info(FriendInfo friendInfo);//在主界面显示好友列表
    void Init_Group_Info(GroupInfo groupInfo);//在主界面显示群列表
    void getGroupInfo(json responsejs);// 记录当前用户的群组列表信息
    void getOfflineMessage(json responsejs);//登陆时获取离线信息

private slots:
//    void receiveLogin();//接受登陆界面的showmain信号并显示主界面
    void readSocketData();//接收服务器数据
    void disconnectSocket();//客户端断开连接
    void on_btn_hide_clicked();//点击最小化按钮
    void on_btn_close_clicked();//点击退出按钮
    void on_listWidget_info_itemClicked(QListWidgetItem *item);//点击主界面左侧列表项，即：好友列表
    void on_btn_send_clicked();//点击发送按钮，发送信息
    void on_btn_addFriend_clicked();//点击添加好友按钮
    void on_btn_img_clicked();//点击发送图片按钮
    void on_btn_file_clicked();//点击发送文件按钮
    void on_btn_update_clicked();//点击修改用户信息按钮
    void showMyImgMessage(QString img);//发送框内显示图片
    void showMyFileMessage(QString file);//发送框内显示文件
    void readImgData();
    void readFileData();
    void on_voiceButton_clicked(bool checked);//点击语音聊天按钮
    void getFriendId(int friendId);
    void getGroupId(int groupId);

private:
    Ui::MainWindow *ui;
    bool loginStatus;//记录用户登录状态，判断用户是否登录成功
    UserInfo _userInfo;//记录当前用户信息
    vector<FriendInfo> g_currentUserFriendList;// 记录当前登录用户的好友列表信息
    vector<GroupInfo> g_currentUserGroupList; // 记录当前登录用户的群组列表信息
    mapChatWidget chatWigetMap;     //聊天列表
    listChatWidgetInfo chatWidgetInfoList;
    ChooseImage * cImag;
    ChooseFile *cFile;
    QFile* img;
    qint64 imgSize;
    qint64 imgTotalSize;
//    string imgPath;
    QTcpSocket* tcpSocket;
    QTcpSocket* fileSocket;
    ImgInfo imgInfo;
    Voice *newVoice;
    AddFriendDialog* addFriendDlg;//添加好友的窗口
};
#endif // MAINWINDOW_H
