#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include "information.h"

namespace Ui {
class AddFriendDialog;
}

class AddFriendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFriendDialog(QWidget *parent = nullptr);
    ~AddFriendDialog();
    AddFriendInfo getAddFriendInfo(){
        return addFriendInfo;
    }
    AddGroupInfo getAddGroupInfo(){
        return addGroupInfo;
    }
    CreateGroupInfo getCreateGroupInfo(){
        return createGroupInfo;
    }
    void setAddFriendInfo(FriendInfo friendInfo);//设置待添加好友信息
    void setAddGroupInfo(GroupInfo groupInfo);//设置待加入群组信息
    void dialogClear();//清除对话框中所有数据
    int getCurrentIndex();//获取当前活跃页
private slots:
    void on_btn_find_clicked();//点击查找按钮，根据账号查找用户
    void on_btn_addFriend_clicked();//点击添加好友按钮
    void on_btn_addGroup_clicked();//点击添加群组按钮
    void on_btn_findGroup_clicked();//点击查找群组按钮，根据账号查找群组
    void on_btn_createGroup_clicked();//点击创建群组按钮

signals:
    void sendFriendId(int);//向主界面发送信号，由主界面根据好友账号向业务服务器查询好友信息
    void sendGroupId(int);//向主界面发送信号，由主界面根据群组账号向业务服务器查询群组信息

private:
    Ui::AddFriendDialog *ui;
    AddFriendInfo addFriendInfo;//添加好友的请求信息，包含发送端账号、本用户账号、请求信息
    AddGroupInfo addGroupInfo;//添加群组的请求信息，包含群组账号、发送端账号、请求信息
    CreateGroupInfo createGroupInfo;//待创建群组的信息，包含群组账号、群描述
};

#endif // ADDFRIENDDIALOG_H
