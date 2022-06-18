#ifndef ADDFRIENDALERT_H
#define ADDFRIENDALERT_H

#include <QDialog>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class AddFriendalert;
}

class AddFriendalert : public QDialog
{
    Q_OBJECT

public:
    explicit AddFriendalert(QWidget *parent = nullptr);
    ~AddFriendalert();
    void setAddRequest(QString addRequest);//设置添加好友弹出窗显示的请求信息

private slots:
    void on_agree_clicked();//同意添加
    void on_refuse_clicked();//拒绝添加
    void on_toolButton_clicked();//关闭该窗口

signals:
    void refreshFriendList();

private:
    Ui::AddFriendalert *ui;
};

#endif // ADDFRIENDALERT_H
