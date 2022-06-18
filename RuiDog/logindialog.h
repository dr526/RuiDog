#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QPoint>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QHostAddress>

#include "tcpconnect.h"
#include "json.hpp"
#include "public.hpp"
#include "information.h"
#include "registerdialog.h"

using namespace std;
using json=nlohmann::json;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    bool isPressedDialog;//判断鼠标是否按在登录界面
    QPoint last;//记录当前鼠标的位置
    void mousePressEvent(QMouseEvent *event);//鼠标点击
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动时，窗口跟随移动
    void mouseReleaseEvent(QMouseEvent *event);//鼠标释放，窗口跟随移动
    QSystemTrayIcon* systemtrayicon;//系统托盘
    QMenu* menu;//系统托盘菜单
    QAction *menuShowAction;//托盘菜单中打开主界面的action
    QAction *menuCloseAction;//推盘菜单中退出的action
    UserInfo *getUserInfo(){
        return &userInfo;
    }

private slots:
    void on_btn_log_clicked(); //点击登陆按钮
//    void on_btn_clear_clicked(); //点击取消按钮
    void showMenu();//通过系统托盘菜单打开主界面
    void closeMenu();//通过系统托盘菜单退出软件
    void on_btn_min_clicked();//点击最小化按钮
    void on_btn_quit_clicked();//点击退出按钮
    void on_btn_register_clicked();//点击注册按钮
//    void onSocketErrorChange(QAbstractSocket::SocketError);//连接服务器失败
//    void onConnected();//连接服务器成功
//    void onSocketReadyRead();//读取服务器响应数据
private:
    Ui::LoginDialog *ui;
    UserInfo userInfo;//记录当前用户的账号信息
};

#endif // LOGINDIALOG_H
