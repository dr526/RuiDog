#ifndef ADDGROUPALERT_H
#define ADDGROUPALERT_H

#include <QDialog>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class AddGroupalert;
}

class AddGroupalert : public QDialog
{
    Q_OBJECT

public:
    explicit AddGroupalert(QWidget *parent = nullptr);
    ~AddGroupalert();
    void setAddRequest(QString addRequest);
private slots:
    void on_agree_clicked();
    void on_refuse_clicked();
    void on_toolButton_clicked();
signals:
    void refreshFriendList();
private:
    Ui::AddGroupalert *ui;
};

#endif // ADDGROUPALERT_H
