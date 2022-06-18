#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"

AddFriendDialog::AddFriendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFriendDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("RuiDog Find");
//    ui->widget_friendInfo->hide();
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint); // 最小化按钮
    setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint); // 帮助按钮
}

AddFriendDialog::~AddFriendDialog()
{
    delete ui;
}

//点击搜索好友按钮，根据好友账号进行搜索
void AddFriendDialog::on_btn_find_clicked(){
    qDebug()<<"on_btn_find_clicked in..";
    qDebug()<<"on_btn_find_clicked out..";
    emit sendFriendId(ui->lineEdit_account->text().toInt());//发送信号给主界面，主界面根据好友账号进行搜索
}

//点击添加好友按钮
void AddFriendDialog::on_btn_addFriend_clicked(){
    addFriendInfo.friendId=ui->lineEdit_friend_account->text().toInt();
    memset(addFriendInfo.requestInfo,'\0',sizeof(addFriendInfo.requestInfo));
    strncpy(addFriendInfo.requestInfo,ui->textEdit_reqInfo->toPlainText().toStdString().c_str(),ui->textEdit_reqInfo->toPlainText().toStdString().size());
    return accept();    //关闭对话框并发送accept()信号
}

//点击加入群组按钮
void AddFriendDialog::on_btn_addGroup_clicked(){
    addGroupInfo.groupId=ui->lineEdit_group_account->text().toInt();
    memset(addGroupInfo.requestInfo,'\0',sizeof(addGroupInfo.requestInfo));
    strncpy(addGroupInfo.requestInfo,ui->textEdit_reqInfo_2->toPlainText().toStdString().c_str(),ui->textEdit_reqInfo_2->toPlainText().toStdString().size());
    return accept();    //关闭对话框并发送accept()信号
}

//根据获取到的好友信息在添加好友窗口中显示
void AddFriendDialog::setAddFriendInfo(FriendInfo friendInfo){
    qDebug()<<"setAddFriendInfo(FriendInfo)";
    if(friendInfo.id==-1){
        QMessageBox::critical(this,tr("警告"),"当前用户不存在");
    }
    else{
        ui->lineEdit_friend_account->setText(QString::number(friendInfo.id));
        ui->lineEdit_friendName->setText(QString::fromStdString(friendInfo.name));
    }
}

//根据获取到的群组信息在添加群组窗口中显示
void AddFriendDialog::setAddGroupInfo(GroupInfo groupInfo){
    qDebug()<<"setAddGroupInfo(groupInfo)";
    if(groupInfo.m_account==-1){
        QMessageBox::critical(this,tr("警告"),"当前群组不存在");
    }
    else{
        ui->lineEdit_group_account->setText(QString::number(groupInfo.m_account));
        ui->lineEdit_groupName->setText(QString::fromStdString(groupInfo.m_groupName));
    }
}

//清除待添加好友信息
void AddFriendDialog::dialogClear(){
    ui->lineEdit_account->clear();
    ui->lineEdit_friendName->clear();
    ui->lineEdit_friend_account->clear();
    ui->textEdit_reqInfo->clear();
}

//获取当前弹窗界面所显示tab的index
int AddFriendDialog::getCurrentIndex(){
    return ui->tabWidget->currentIndex();
}

//点击查找群组按钮
void AddFriendDialog::on_btn_findGroup_clicked(){
    qDebug()<<"on_btn_findGroup_clicked in..";
    qDebug()<<"on_btn_findGroup_clicked out..";
    emit sendGroupId(ui->lineEdit_account_2->text().toInt());//向主界面发送信号，由主界面根据群组账号向业务服务器查询群组信息
}

//点击创建群组按钮
void AddFriendDialog::on_btn_createGroup_clicked(){
    createGroupInfo.m_groupName=ui->lineEdit_createName->text().toStdString();
    createGroupInfo.m_groupDesc=ui->textEdit_createDesc->toPlainText().toStdString();
    return accept();
}
