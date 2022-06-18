#include "updateinfo.h"
#include "ui_updateinfo.h"

UpdateInfo::UpdateInfo(int userId, string password, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateInfo)
{
    ui->setupUi(this);
    this->_userId=userId;
    this->_password=password;
    ui->account->setText(QString::number(userId));
    ui->account->setReadOnly(true);
    ui->password->setText(QString::fromStdString(password));
}

UpdateInfo::~UpdateInfo()
{
    delete ui;
}

void UpdateInfo::on_btn_update_clicked(){//点击修改按钮
    this->_password=ui->password->text().toStdString();
    return accept();//关闭窗口并向主窗口发送accept信号
}
void UpdateInfo::on_btn_cancel_clicked(){//点击取消按钮
    this->close();
}
