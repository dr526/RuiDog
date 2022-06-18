#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QBitmap>
#include <QPainter>

#include "json.hpp"
#include "public.hpp"
#include "tcpconnect.h"
#include "information.h"

using namespace std;
using json=nlohmann::json;

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();
    bool isPressedDialog;//判断鼠标是否按在登录界面
    QPoint last;//记录当前鼠标的位置
    void mousePressEvent(QMouseEvent *event);//鼠标点击
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动时，窗口跟随移动
    void mouseReleaseEvent(QMouseEvent *event);//鼠标释放，窗口跟随移动
    bool getRegisterStatus(){
        return registerStatus;
    }
    UserInfo *getUserInfo(){
        return &userInfo;
    }

private slots:
    void on_btn_register_clicked();//点击注册按钮
    void on_btn_cancel_clicked();//点击取消按钮
private:
    Ui::RegisterDialog *ui;
    bool registerStatus;//记录注册状态
    UserInfo userInfo;
};

#endif // REGISTERDIALOG_H
