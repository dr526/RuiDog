#ifndef UPDATEINFO_H
#define UPDATEINFO_H

#include <QDialog>
using namespace std;

namespace Ui {
class UpdateInfo;
}

class UpdateInfo : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateInfo(int userId, string password, QWidget *parent = nullptr);
    ~UpdateInfo();
    string getPassword()const{
        return this->_password;
    }

private slots:
    void on_btn_update_clicked();//点击修改按钮
    void on_btn_cancel_clicked();//点击取消按钮

private:
    Ui::UpdateInfo *ui;
    int _userId;
    string _password;
};

#endif // UPDATEINFO_H
